import glwin
import level

class Dokk(object):
  def __new__(type, *args):
    if not '_the_instance' in type.__dict__:
      type._the_instance = object.__new__(type)
    return type._the_instance
  
  def __init__(self):
    if not '_ready' in dir(self):
      self.gl_win=glwin.DokkGLWin(self)
      self._levels = list()
      self._current_index = -1
      self._current_level = None
      self._ready = True

  def SetLevels(self,levels):
    self._current_index = -1
    if isinstance(levels, list):
      self.levels_ = levels
        
  def NextLevel(self):
    self._current_index += 1
    if(self._current_index < len(self.levels_)):
      self._LoadLevel()
    else:
      self._current_index -=1
  
  def PreviousLevel(self):
    self._current_index -= 1
    if(self._current_index >= 0):
      self._LoadLevel()
    else:
      self._current_index +=1
  
  def GetLevel(self):
    return self._current_level
  
  def Start(self, args):
    self.NextLevel()
    self.gl_win.Show(fullscreen=('--fullscreen' in args))
    if '--stereo' in args:
      self.gl_win.SetStereo(1)

  def _LoadLevel(self):
    if self._current_level is not None:
      self._current_level.Close()
    self._current_level = level.Level(self.levels_[self._current_index])
    self.gl_win.SetLevel(self._current_level)
