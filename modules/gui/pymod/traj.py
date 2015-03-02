#-------------------------------------------------------------------------------
# This file is part of the OpenStructure project <www.openstructure.org>
#
# Copyright (C) 2008-2015 by the OpenStructure authors
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
#-------------------------------------------------------------------------------
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from ost import *

class TrajWidget(QWidget):
  def __init__(self, traj=None, render_mode=gfx.SIMPLE, sel='', parent=None):
    QWidget.__init__(self, parent, Qt.Tool)
    self.render_mode=render_mode
    vb=QVBoxLayout()
    hb=QHBoxLayout()
    hb2=QHBoxLayout() 
    self.selection=sel
    self.callback=None   
    self._slider=QSlider(self)
    self._slider.setOrientation(Qt.Horizontal)
    self._speed_slider=QSlider(self)
    self._speed_slider.setOrientation(Qt.Horizontal)
    self._play=QToolButton(self)
    self._repeat=QCheckBox(self)
    self._frame=QLabel(self)
    self._frameNo=QLabel(self)
    self._frameEnd=QLabel(self)
    self._repeat.setText('Repeat')
    self._slider.setTracking(True)
    self._play.setText('Play')
    self._play.setCheckable(True)
    self._frame.setText('Frame: ')
    self._frameNo.setNum(0)
    self._frameNo.setAlignment(Qt.AlignRight)
    self._frameEnd.setText('/ '+ str(traj.GetFrameCount()-1))
    self._frameEnd.setAlignment(Qt.AlignLeft)
    self._speed_slider.setTracking(True)
    self._speed_slider.setRange(-1000,-1)
    self._speed_slider.value=-100
    hb.addWidget(self._play)
    hb.addWidget(self._repeat)
    hb2.addWidget(self._frame)
    hb2.addWidget(self._frameNo)
    hb2.addWidget(self._frameEnd)
    hb2.addWidget(self._speed_slider)
    self.setLayout(vb)
    vb.addLayout(hb)
    vb.addWidget(self._slider)
    vb.addLayout(hb2)
    self.traj=traj
    self.time=1
    QObject.connect(self._play, SIGNAL('toggled(bool)'), 
                    self._TogglePlay)
    QObject.connect(self._slider, SIGNAL('valueChanged(int)'), 
                    self._SliderValueChanged)
    QObject.connect(self._speed_slider,SIGNAL('valueChanged(int)'),self._SpeedSliderValChanged)

  def _SpeedSliderValChanged(self,speed_pos):
    self.time=-speed_pos
    if self._play.isChecked():
      self._TogglePlay(False)
      self._TogglePlay(True)
  
  def _SetTime(self,t):
    self.time=t
    self._speed_slider.setSliderPosition(-t)
    if self._play.isChecked():
      self._TogglePlay(False)
      self._TogglePlay(True)
  
  def _SliderValueChanged(self, pos):
    self.current_frame=pos
    self._traj.CopyFrame(self.current_frame)
    self.gfx_entity.UpdatePositions()

  def _GetCurrentFrame(self):
    return self._slider.sliderPosition()
    
  def _SetCurrentFrame(self, pos):
    if self._slider.maximum()<pos:
      if self._repeat.isChecked():
        pos=0
      else:
        pos=self._slider.maximum()
    self._slider.setSliderPosition(pos)
    self._frameNo.setNum(pos)
    
  current_frame=property(_GetCurrentFrame, _SetCurrentFrame)
  
  def timerEvent(self, event):
    self.gfx_entity.BlurSnapshot()
    self.current_frame+=1
    if self.callback:
      self.callback(self.gfx_entity)
    self.gfx_entity.UpdatePositions()
  
  def _TogglePlay(self, playing):
    if playing:
      self.timer_id_=self.startTimer(self.time)
    else:
      self.killTimer(self.timer_id_)

  def _SetTraj(self, traj):
    self._traj=traj
    if self._traj:
      ev=traj.GetEntity()
      self.gfx_entity=gfx.Entity("mol",self.render_mode,
                                 ev.Select(self.selection))
      gfx.Scene().Add(self.gfx_entity)
      gfx.Scene().CenterOn(self.gfx_entity)
      # enable the blur effect
      #self.gfx_entity.SetBlur(True)
      self._slider.setMinimum(0)
      self._slider.setMaximum(self._traj.GetFrameCount()-1)
      self._slider.setSliderPosition(0)
    else:
      if self._traj:
        del self._anim
      self._traj=None
  def _SetBlur(self, blur):
    self.gfx_entity.SetBlur(blur)

  def _GetBlur(self):
    return self.gfx_entity.GetBlur()

  def _GetTraj(self):
    return self._traj

  blur=property(_GetBlur, _SetBlur)
  traj=property(_GetTraj, _SetTraj)
