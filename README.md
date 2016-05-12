# ruby_ares

Bindings for c-ares non-blocking resolver library

## Installation

Add this line to your application's Gemfile:

```ruby
gem 'ruby_ares'
```

## Usage

```ruby
Ares.gethostbyname "www.google.com"

Ares.gethostbyname "www.google.com", 5
```


## License

The gem is available as open source under the terms of the [MIT License](http://opensource.org/licenses/MIT).

