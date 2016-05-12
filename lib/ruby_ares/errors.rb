module Ares
  class FileError < StandardError
    def initialize(*args)
      super "A configuration file could not be read."
    end
  end

  class NoMemory < StandardError
    def initialize(*args)
      super "No memory"
    end
  end

  class NotInitialized < StandardError
    def initialize(*args)
      super "Not initialized"
    end
  end

  # gethostbyname errors
  class NotImplemented < StandardError
    def initialize(*args)
      super "Not implemented"
    end
  end

  class BadName < StandardError
    def initialize(*args)
      super "Bad name"
    end
  end

  class NotFound < StandardError
    def initialize(*args)
      super "Not found"
    end
  end

  class Cancelled < StandardError
    def initialize(*args)
      super "Cancelled"
    end
  end

  class Destruction < StandardError
    def initialize(*args)
      super "Destruction"
    end
  end

  class ResolveError < StandardError
    def initialize(*args)
      super "Unknown resolve error"
    end
  end

  class InitializeError < StandardError
    def initialize(*args)
      super "Unknown initialization error"
    end
  end
end
