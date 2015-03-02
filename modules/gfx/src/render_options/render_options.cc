//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2011 by the OpenStructure authors
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
#include <ost/gfx/input.hh>
#include <ost/gfx/entity.hh>

#include "render_options.hh"

namespace ost { namespace gfx {

RenderOptions::RenderOptions(){}

bool RenderOptions::AddObserver(EntityP entity)
{
  if((std::find( this->observers_.begin(), this->observers_.end(), entity )) == this->observers_.end())
  {
    this->observers_.push_back( entity );
    return true;
  }
  return false;
}

bool RenderOptions::RemoveObserver(EntityP entity)
{
  EntityPObservers::iterator found = std::find( this->observers_.begin(), this->observers_.end(), entity);
  if( found != this->observers_.end() ){

    this->observers_.erase(found);
    return true;
  }
  return false;
}

void RenderOptions::NotifyStateChange()
{
  InputEvent event = InputEvent(INPUT_DEVICE_MOUSE,INPUT_COMMAND_REBUILD,0);
  EntityPObservers::const_iterator observers_it = this->observers_.begin() ;
  while( observers_it != this->observers_.end() )
  {
    ( *observers_it )->OptionsChanged(this->GetRenderMode()) ;
    observers_it++;
  }
}

}} // ns
