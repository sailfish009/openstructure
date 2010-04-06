import ConfigParser
import string

class Config:
  def __init__(self, path):
    config = ConfigParser.ConfigParser()
    config.read(path)

    self.values = dict()
    for section in config.sections():
      section_dict = dict()
      for option in config.options(section):
        section_dict[str(option).upper()]=config.get(section, option)
      self.values[str(section).lower()]=section_dict
      
  def __getattr__(self, item):
    item = str(item).lower()
    if self.values.has_key(item):
      return self.values[item]
    raise AttributeError(item)
