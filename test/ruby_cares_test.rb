require 'test_helper'
require 'pp'
require 'benchmark'
require 'socket'

class AresTest < Minitest::Test
  def test_no_timeout
    pp Ares.gethostbyname('www.google.com')
  end

  def test_timeout
    pp Ares.gethostbyname('www.google.com', 1)
  end

  def test_unicode
    pp Ares.gethostbyname('кто.рф')
  end
end
