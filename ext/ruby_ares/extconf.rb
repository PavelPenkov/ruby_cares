require 'mkmf'

$CFLAGS =  '-std=c99 -g'

have_library 'cares'

create_makefile 'ruby_ares/ruby_ares'
