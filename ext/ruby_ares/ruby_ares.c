#include <stdio.h>
#include <stdlib.h>
#include <ruby.h>
#include <ares.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>

static VALUE ruby_ares_module() {
  return rb_const_get(rb_cObject, rb_intern("Ares"));
}

struct resolve_result {
  int result;
  struct hostent *hostent;
} result;

void hostent_to_array(VALUE ary, struct hostent *hostent) {
  VALUE official_hostname = rb_str_new2(hostent->h_name);
  rb_ary_push(ary, official_hostname);

  VALUE alias_hostnames = rb_ary_new();
  int i;
  for(i = 0; hostent->h_aliases[i] != NULL; i++) {
    rb_ary_push(alias_hostnames, rb_str_new2(hostent->h_aliases[i]));
  }

  rb_ary_push(ary, alias_hostnames);

  rb_ary_push(ary, INT2NUM(hostent->h_addrtype));

  for(i = 0; hostent->h_addr_list[i] != NULL; i++) {
    int buf_size = hostent->h_addrtype == AF_INET ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN;
    char *buf = (char *) malloc(buf_size);
    ares_inet_ntop(hostent->h_addrtype, hostent->h_addr_list[i], buf, buf_size);
    VALUE addr_str = rb_str_new2(buf);
    free(buf);
    rb_ary_push(ary, addr_str);
  }
}

static VALUE translate_resolve_status(int status) {
  VALUE e;
  VALUE ares = ruby_ares_module();
  printf("Resolve status %d", status);
  switch(status) {
    case ARES_ENOTIMP:
      e = rb_const_get(ares, rb_intern("NotImplemented"));
      break;
    case ARES_EBADNAME:
      e = rb_const_get(ares, rb_intern("BadName"));
      break;
    case ARES_ENOTFOUND:
      e = rb_const_get(ares, rb_intern("NotFound"));
      break;
    case ARES_ENOMEM:
      e = rb_const_get(ares, rb_intern("NoMemory"));
      break;
    case ARES_ECANCELLED:
      e = rb_const_get(ares, rb_intern("Cancelled"));
      break;
    case ARES_EDESTRUCTION:
      e = rb_const_get(ares, rb_intern("Destruction"));
      break;
    default:
      e = rb_eStandardError;
  }
  return e;
}


void callback(void *arg, int status, int timeouts, struct hostent *hostent) {
  if (status != ARES_SUCCESS) {
    VALUE e = translate_resolve_status(status);
    rb_raise(e, "%s", "");
  }
  VALUE res = (VALUE) arg;
  hostent_to_array(res, hostent);
}

static VALUE translate_init_error(int err) {
  VALUE e;
  switch(err) {
    case ARES_EFILE:
      e = rb_const_get(ruby_ares_module(), rb_intern("FileError"));
      break;
    case ARES_ENOMEM:
      e = rb_const_get(ruby_ares_module(), rb_intern("NoMemory"));
      break;
    case ARES_ENOTINITIALIZED:
      e = rb_const_get(ruby_ares_module(), rb_intern("NotInitialized"));
      break;
    default:
      e = rb_eStandardError;
  }
  return e;
}

static VALUE ruby_ares_gethostbyname(int argc, VALUE *argv, VALUE self) {
  if (argc > 2 || argc == 0) {
    rb_raise(rb_eArgError, "wrong number of arguments: %d for 1..2", argc);
  }

  VALUE host_name = argv[0];
  int timeout = argc == 2 ? NUM2INT(argv[1]) : 30;

  VALUE e;
  ares_channel channel;
  int err;

  if(TYPE(host_name) != T_STRING) {
    rb_raise(rb_eTypeError, "not a valid value");
    return Qnil;
  }

  err = ares_init(&channel);
  if(err != ARES_SUCCESS) {
    VALUE e = translate_init_error(e);
    rb_raise(e, "%s", "");
  }

  VALUE result = rb_ary_new();

  char *hostname = StringValueCStr(host_name);

  ares_gethostbyname(channel, hostname, AF_INET6, &callback, (void*) result);

  int nfds, count;
  fd_set readers, writers;
  struct timeval tv, *tvp;

  for(;;) {
    FD_ZERO(&readers);
    FD_ZERO(&writers);
    nfds = ares_fds(channel, &readers, &writers);
    if (nfds == 0) break;
    tvp = ares_timeout(channel, NULL, &tv);
    count = select(nfds, &readers, &writers, NULL, tvp);
    ares_process(channel, &readers, &writers);
  }

  ares_destroy(channel);

  return result;
}

void Init_ruby_ares() {
  VALUE m_ruby_ares;

  m_ruby_ares = ruby_ares_module();
  rb_define_module_function(m_ruby_ares, "gethostbyname", ruby_ares_gethostbyname, -1);

  int err = ares_library_init(ARES_LIB_INIT_ALL);
  if(err != 0) {
    VALUE e = rb_const_get(m_ruby_ares, rb_intern("AresError"));
    rb_raise(e, "%s", ares_strerror(err));
  }
}
