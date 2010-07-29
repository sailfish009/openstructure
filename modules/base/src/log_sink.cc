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
#include <ost/log.hh>
#include <ost/log_sink.hh>
#include <iostream>
namespace ost {

ObservableLogSink::ObservableLogSink(){}

bool ObservableLogSink::AddObserver(LogObserverPtr& observer){
  if((std::find( this->observers_.begin(), this->observers_.end(), observer )) == this->observers_.end())
  {
    this->observers_.push_back( observer );
    return true;
  }
  return false;
}

bool ObservableLogSink::RemoveObserver(LogObserverPtr& observer){
  std::vector<LogObserverPtr>::iterator found = std::find( this->observers_.begin(), this->observers_.end(), observer);
  if( found != this->observers_.end() ){
    this->observers_.erase(found);
    return true;
  }
  return false;
}

void ObservableLogSink::LogMessage(const String& message, int severity){
  std::vector<LogObserverPtr>::const_iterator observers_it = this->observers_.begin() ;
  while( observers_it != this->observers_.end() )
  {
    ( *observers_it )->LogMessage(message, severity);
    observers_it++;
  }
}

} // ns

