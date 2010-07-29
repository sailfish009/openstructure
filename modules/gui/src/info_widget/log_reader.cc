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
#include <ost/log.hh>

#include "info_widget.hh"
#include "log_reader.hh"

namespace ost { namespace gui {

LogReader::LogReader(QObject* parent) :
  QObject(parent) {
  ost::Logger& logger = ost::Logger::Instance();
  ObservableLogSinkPtr olsp = ObservableLogSinkPtr(new ObservableLogSink());
  LogSinkPtr lsp = LogSinkPtr(olsp);
  logger.SetSink(lsp);
  LogObserverPtr losp = LogObserverPtr(this);
  olsp->AddObserver(losp);
}

void LogReader::LogMessage(const String& message){
  if(this->parent()){
    if(InfoWidget* info_widget = qobject_cast<InfoWidget*>(this->parent())){
      info_widget->LogMessage(message.c_str());
    }
  }
}

LogReader::~LogReader() {}

}
} // ns
