#------------------------------------------------------------------------------
# This file is part of the OpenStructure project <www.openstructure.org>
#
# Copyright (C) 2008-2010 by the OpenStructure authors
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 3.0 of the License, or (at your option)
# any later version.
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#------------------------------------------------------------------------------
from _gui import *
import sip


## \brief Opens a DataViewer 
# \sa \ref fft_li.py "View Fourier Transform Example" \sa \ref modulate_image.py "Modulate Image Example"
def _close_event_override_(event):
  print "close event"
def _set_data_override_(data):
  print "set data"

def CreateDataViewer(ih,flag=False):
      viewer=GostyApp.Instance().CreateDataViewer(ih)
      if flag:
        viewer.image_=ih
        sip_viewer=viewer.qobject
        sip_viewer.closeEvent=_close_event_override_
        sip_viewer.setData=_set_data_override_
      return viewer

def ClearMessageWidget():
      gosty=GostyApp.Instance()
      gosty.message_widget.Clear()


from PyQt4.QtGui import *
from ost import gfx

def PickColor(default=gfx.WHITE):
  """
  Pops up a color chooser that lets' the user pick a color and returns the 
  selected color. If the user cancels the color chooser, None is returned.
  
  :rtype: :class:`~ost.gfx.Color`
  """
  dialog=QColorDialog()
  qt_color=QColor(int(min(255,default.Red()*256)),
                  int(min(255,default.Green()*256)),
                  int(min(255,default.Blue()*256)))
  qt_color=dialog.getColor(qt_color)
  if not qt_color.isValid():
    return None
  return gfx.Color(qt_color.red()/256.0, qt_color.green()/256.0,
                   qt_color.blue()/256.0)