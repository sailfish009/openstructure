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
#ifndef OST_LOG_SINK_HH
#define OST_LOG_SINK_HH

#include <ostream>
#include <iostream>
#include <stack>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <ost/module_config.hh>

namespace ost {

namespace {

  class DevNull: public std::streambuf {
  protected:
    virtual int_type overflow(int_type c) {return c;}
    virtual std::streamsize xsputn(const char* s, std::streamsize num) {return num;}
  };

} // anon ns

class DLLEXPORT_OST_BASE LogSink {
public:
  virtual void LogMessage(const String& message, int severity = 0)=0;
};

typedef boost::shared_ptr<LogSink> LogSinkPtr;

class DLLEXPORT_OST_BASE NullLogSink : public LogSink {
public:
  NullLogSink():null_(new DevNull()){}
  virtual void LogMessage(const String& message, int severity){null_ << message;}

private:
  std::ostream null_;
};


class DLLEXPORT_OST_BASE StdLogSink : public LogSink {
public:
  StdLogSink(std::ostream& stream):stream_(stream){}
  virtual void LogMessage(const String& message, int severity){
    stream_ << message;
  }

private:
  std::ostream& stream_;
};

class DLLEXPORT_OST_BASE LogObserver {
public:
  virtual void LogMessage(const String& message, int severity)=0;
};
typedef boost::shared_ptr<LogObserver> LogObserverPtr;

class DLLEXPORT_OST_BASE ObservableLogSink : public LogSink {
public:
  ObservableLogSink();
  bool AddObserver(LogObserverPtr& observer);
  bool RemoveObserver(LogObserverPtr& observer);
  void LogMessage(const String& message, int severity);
private:
  std::vector<LogObserverPtr> observers_;
};

typedef boost::shared_ptr<ObservableLogSink> ObservableLogSinkPtr;

}
#endif
