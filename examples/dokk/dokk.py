import getopt
import sys

from PyQt4 import QtGui

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
      self._language=""
      
  def SetLevels(self,levels):
    self._current_index = -1
    if isinstance(levels, list):
      self.levels_ = levels
        
  def NextLevel(self):
    if self.HasNextLevel():
      self._current_index += 1
      self._LoadLevel()
  
  def PreviousLevel(self):
    if self.HasPreviousLevel():
      self._current_index -= 1
      self._LoadLevel()
  
  def HasNextLevel(self):
    if(self._current_index < len(self.levels_)-1):
      return True
    return False
  
  def HasPreviousLevel(self):
    if(self._current_index > 0):
      return True
    return False
  
  def Restart(self):
    self._current_index = -1
    self.NextLevel()
  
  def GetLevel(self):
    return self._current_level
  
  def GetLanguage(self):
    return self._language
  
  def SetLanguage(self, lang):
    lang_str = str(lang).upper()
    if len(lang_str) > 0:
      self._language = lang_str[0]
      print self._language
    else:
      self._language = ""
  
  def Start(self, args):
    try:
        opts, args = getopt.getopt(args[1:], "hfsl:", ["help", "fullscreen", "stereo", "language="])
    except getopt.GetoptError, err:
        print str(err) # will print something like "option -a not recognized"
        self._Usage()
        QtGui.QApplication.exit()
        sys.exit(2)
    fullscreen = False
    stereo = False
    language = ""
    for o, value in opts:
      if o in ("-h", "--help"):
        self._Usage()
        QtGui.QApplication.exit()
        sys.exit()
      elif o in ("-f", "--fullscreen"):
        fullscreen=True
      elif o in ("-s", "--stereo"):
        stereo=True
      elif o in ("-l", "--language"):
        language = value
      else:
        assert False, "unhandled option"
    self.SetLanguage(language)
    self.NextLevel()
    self.gl_win.Show(fullscreen)
    self.gl_win.SetStereo(stereo)

  def _LoadLevel(self):
    if self._current_level is not None:
      self._current_level.Close()
    self._current_level = level.Level(self.levels_[self._current_index])
    self.gl_win.SetLevel(self._current_level)

  def _Usage(self):
    print """
Usage: dokk [OPTIONS] 
     -h --help                    Display this usage message
     -f --fullscreen              Start dokk in fullscreen mode
     -s --stereo                  Enable stereo view
     -l <lang>, --language=<lang> Set language to <lang>
"""

