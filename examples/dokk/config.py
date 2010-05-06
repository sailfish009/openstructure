import ConfigParser
import string

class Config:
  def __init__(self, path):
    self._path = path
    self.UpdateValues()
    
  def UpdateValues(self):
    config = ConfigParser.ConfigParser()
    config.read(self._path)
    self._values = dict()
    for section in config.sections():
      section_dict = dict()
      for option in config.options(section):
        section_dict[str(option).upper()]=config.get(section, option)
      self._values[str(section).lower()]=section_dict
      
  def __getattr__(self, item):
    item = str(item).lower()
    if self._values.has_key(item):
      return self._values[item]
    raise AttributeError(item)

  def Get(self, item):
    return self.__getattr__(item)

class MutableConfig(Config):
  def __init__(self,path):
    Config.__init__(self, path)
  
  def Set(self, section, key, value):
    if not self._values.has_key(section):
      self._values[section] = dict()
    section_dict = self._values[section]
    section_dict[key] = value
    self.__save()
  
  def __save(self):
    config = ConfigParser.ConfigParser()
    file = open(self._path,"w")
    for section, values in self._values.iteritems():
      config.add_section(section)
      for k, v in values.iteritems():
        config.set(section, k, v)
    config.write(file)
    

class TopTen():
  def __init__(self,path):
    self._config = MutableConfig(path)
    
  
  def IsTopTen(self,score):
    self._config.UpdateValues()
    try:
      if score < float(self._config.Get(str(10))["SCORE"]):
        return True
    except AttributeError:
      return True
    return False
  
  def GetDiff(self, score):
    rank = self.GetRank(score) -1
    if(rank > 0):
      return abs(score - float(self._config.Get(str(rank))["SCORE"]))
    elif(rank == 0):
      return 0
    else:
      return abs(score - float(self._config.Get(str(10))["SCORE"]))
  
  def GetRank(self, score):
    if self.IsTopTen(score):
      for i in range(1,11):
        try:
          tt_score = float(self._config.Get(str(i))["SCORE"])
          if score < tt_score:
            return i
        except AttributeError:
          return i
    return -1
        
  def SetValue(self, name, score):
    rank = self.GetRank(score)
    if rank > 0:
      finish = False
      for i in range(rank,11):
        try:
          old_name = self._config.Get(str(i))["NAME"]
          old_score = self._config.Get(str(i))["SCORE"]
        except AttributeError:
          finish = True
        self._config.Set(str(i), "NAME", name)
        self._config.Set(str(i), "SCORE", score)
        if finish:
          break
        name = old_name
        score = old_score

  def GetData(self, rank):
    try:
      name = self._config.Get(str(rank))["NAME"]
      score = self._config.Get(str(rank))["SCORE"]
      return [name, score]
    except AttributeError:
      return None
