//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2010 by the OpenStructure authors
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 3.0 of the License, or (at your option)
// any later version.
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//------------------------------------------------------------------------------

/*
  Author: Stefan Scheuber
*/

#include "spnav_input.hh"

#include <iostream>

#include <ost/io/io_exception.hh>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <spnav.h>

namespace ost { namespace gui {

SpnavInput* SpnavInput::spnav_=NULL;

SpnavInput::SpnavInput(QObject* parent): QThread(parent)
{
  qRegisterMetaType<geom::Mat4>("geom::Mat4");
  if(spnav_open()==-1) {
    throw(io::IOException("failed to connect to the space navigator daemon\n"));
  }
}

void SpnavInput::run(){

	spnav_event sev;

  while(spnav_wait_event(&sev)) {
    if(sev.type == SPNAV_EVENT_MOTION) {
      emit this->deviceTransformed(sev.motion.x,sev.motion.y,sev.motion.z, sev.motion.rx, sev.motion.ry, sev.motion.rz);
    } else {  /* SPNAV_EVENT_BUTTON */
      printf("got button %s event b(%d)\n", sev.button.press ? "press" : "release", sev.button.bnum);
      if(sev.button.press)
        emit this->deviceButtonPressed(sev.button.bnum);
    }
  }
}

SpnavInput::~SpnavInput(){
  spnav_close();
}

SpnavInput* SpnavInput::Instance() {
  if (!SpnavInput::spnav_) {
    SpnavInput::spnav_=new SpnavInput;
  }
  return SpnavInput::spnav_;
}

}} //ns
