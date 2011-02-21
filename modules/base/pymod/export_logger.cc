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
#include <boost/python.hpp>
using namespace boost::python;

#include <ost/log.hh>
#include <ost/log_sink.hh>
#include <ost/dyn_cast.hh>

using namespace ost;


namespace {

struct WrappedLogSink : public LogSink {
  WrappedLogSink(PyObject* self): self_(self)
  { }
  virtual void LogMessage(const String& message , int severity) 
  {
    call_method<void>(self_, "LogMessage", message, severity);
  }
  
  void LogMessageDefault(const String& message, int severity)
  {
  }
  PyObject* self_;
};



typedef boost::shared_ptr<WrappedLogSink> WrappedLogSinkPtr;

void push_verb(int n) 
{
 if(n<0){
   Logger::Instance().PushVerbosityLevel(0);
  }else{
   Logger::Instance().PushVerbosityLevel(n);
  }
}

void pop_verb() 
{
  Logger::Instance().PopVerbosityLevel();
}

int get_verb()
{
  return Logger::Instance().GetVerbosityLevel();
}


void push_log_sink(LogSinkPtr sink)
{
  Logger::Instance().PushSink(sink);
}

void pop_log_sink()
{
  Logger::Instance().PopSink();
}


void log_error(const String& m) {LOG_ERROR(m);}
void log_warning(const String& m) {LOG_WARNING(m);}
void log_script(const String& m) {LOG_SCRIPT(m);}
void log_info(const String& m) {LOG_INFO(m);}
void log_verbose(const String& m) {LOG_VERBOSE(m);}


void reset_sinks()
{
  Logger::Instance().ResetSinks();
}
}

void export_Logger()
{
  class_<LogSink, WrappedLogSinkPtr, 
         boost::noncopyable>("LogSink")
    .def("LogMessage", &WrappedLogSink::LogMessageDefault)
  ;

  class_<MultiLogSink, MultiLogSinkPtr, bases<LogSink>, 
         boost::noncopyable >("MultiLogSink", init<>())
    .def("AddSink",&MultiLogSink::AddSink)
    .def("RemoveSink",&MultiLogSink::RemoveSink)
    .def("LogMessage", &MultiLogSink::LogMessage)
  ;

  class_<FileLogSink, FileLogSinkPtr, bases<LogSink>, 
         boost::noncopyable >("FileLogSink", init<const String&>())
    .def("LogMessage", &FileLogSink::LogMessage)
  ;

  def("PushVerbosityLevel",push_verb);
  def("PopVerbosityLevel",pop_verb);
  def("GetVerbosityLevel",get_verb);
  def("PushLogSink",push_log_sink);
  def("PopLogSink",pop_log_sink);
  def("LogError",log_error);
  def("LogWarning",log_warning);
  def("LogInfo",log_info);
  def("LogScript", log_script);
  def("LogVerbose", log_verbose);
  
  // this relatively ugly construct is required to work around a problem with
  // the "ost" command-line interpreter. If we don't remove all the sinks from
  // the sink stack, we will get "Fatal Python error: PyEval_SaveThread: 
  // NULL tstate" upon exiting ost. I don't completely understand why, though.
  scope().attr("__dict__")["atexit"]=import("atexit");

  def("_reset_sinks", &reset_sinks);
  object r=scope().attr("_reset_sinks");
  scope().attr("atexit").attr("register")(r);
}
