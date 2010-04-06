import glwin

class Dokk(object):
  def __new__(type, *args):
    if not '_the_instance' in type.__dict__:
      type._the_instance = object.__new__(type)
    return type._the_instance
  
  def __init__(self):
    if not '_ready' in dir(self):
      self.gl_win=glwin.DokkGLWin(self)
      self._ready = True

  def SetLevel(self,level):
    self.gl_win.SetLevel(level)
    self.level_ = level
    
  def NextLevel(self):
    self.level_.Close()
  
  def GetLevel(self):
    return self.level_
  
  def Start(self, args):
    self.gl_win.Show(fullscreen=('--fullscreen' in args))


