module Ares
  class FileError < StandardError
    def initialize
      super "A configuration file could not be read."
    end
  end

  class NoMemory < StandardError
    def initialize
      super "No memory"
    end
  end

  class NotInitialized < StandardError
    def initialize
      super "Not initialized"
    end
  end

  # gethostbyname errors
  class NotImplemented < StandardError
  end

  class BadName < StandardError
  end

  class NotFound < StandardError
  end

  class Cancelled < StandardError
  end

  class Destruction < StandardError
  end
end
