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
#include <boost/python.hpp>
using namespace boost::python;

#include <ost/log.hh>
#include <ost/log_sink.hh>
#include <ost/dyn_cast.hh>

using namespace ost;


namespace {

struct WrappedLogSink : public LogSink, wrapper<LogSink>
{
    virtual void LogMessage(const String& message , int severity) {
      call<void>(this->get_override("LogMessage").ptr(),message,severity);
    }
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

void pop_verb_file() 
{
  Logger::Instance().PopFile();
}

void pop_log_sink()
{
  Logger::Instance().PopSink();
}

void push_verb_file(const String& filename) 
{
  Logger::Instance().PushFile(filename);
}

void push_log_sink_1(LogSinkPtr& sink)
{
  Logger::Instance().PushSink(sink);
}

void push_log_sink_2(WrappedLogSinkPtr& sink)
{
  LogSinkPtr lsp = dyn_cast<LogSink>(sink);
  Logger::Instance().PushSink(lsp);
}

void push_log_sink_3(ObservableLogSinkPtr& sink)
{
  LogSinkPtr lsp = dyn_cast<LogSink>(sink);
  Logger::Instance().PushSink(lsp);
}

void push_log_sink_4(FileLogSinkPtr& sink)
{
  LogSinkPtr lsp = dyn_cast<LogSink>(sink);
  Logger::Instance().PushSink(lsp);
}

void push_log_sink_5(NullLogSinkPtr& sink)
{
  LogSinkPtr lsp = dyn_cast<LogSink>(sink);
  Logger::Instance().PushSink(lsp);
}

void add_observer_1(ObservableLogSinkPtr& obs_sink, WrappedLogSinkPtr& sink)
{
  LogSinkPtr lsp = dyn_cast<LogSink>(sink);
  obs_sink->AddObserver(lsp);
}

void add_observer_2(ObservableLogSinkPtr& obs_sink, FileLogSinkPtr& sink)
{
  LogSinkPtr lsp = dyn_cast<LogSink>(sink);
  obs_sink->AddObserver(lsp);
}

void add_observer_3(ObservableLogSinkPtr& obs_sink, NullLogSinkPtr& sink)
{
  LogSinkPtr lsp = dyn_cast<LogSink>(sink);
  obs_sink->AddObserver(lsp);
}

void remove_observer_1(ObservableLogSinkPtr& obs_sink, WrappedLogSinkPtr& sink)
{
  LogSinkPtr lsp = dyn_cast<LogSink>(sink);
  obs_sink->RemoveObserver(lsp);
}

void remove_observer_2(ObservableLogSinkPtr& obs_sink, FileLogSinkPtr& sink)
{
  LogSinkPtr lsp = dyn_cast<LogSink>(sink);
  obs_sink->RemoveObserver(lsp);
}

void remove_observer_3(ObservableLogSinkPtr& obs_sink, NullLogSinkPtr& sink)
{
  LogSinkPtr lsp = dyn_cast<LogSink>(sink);
  obs_sink->RemoveObserver(lsp);
}

void log_error(const String& m) {LOGN_ERROR(m);}
void log_message(const String& m) {LOGN_MESSAGE(m);}
void log_verbose(const String& m) {LOGN_VERBOSE(m);}
void log_debug(const String& m) {LOGN_DEBUG(m);}

}

void export_Logger()
{
  class_<WrappedLogSink, WrappedLogSinkPtr, LogSink, boost::noncopyable>("LogSink",init<>())
    .def("LogMessage",pure_virtual(&WrappedLogSink::LogMessage))
  ;

  class_<ObservableLogSink, ObservableLogSinkPtr, bases<LogSink>, boost::noncopyable >("ObservableLogSink", init<>())
    .def("LogMessage",&ObservableLogSink::LogMessage)
    .def("AddObserver",&ObservableLogSink::AddObserver)
    .def("AddObserver",add_observer_1)
    .def("AddObserver",add_observer_2)
    .def("AddObserver",add_observer_3)
    .def("RemoveObserver",&ObservableLogSink::RemoveObserver)
    .def("RemoveObserver",remove_observer_1)
    .def("RemoveObserver",remove_observer_2)
    .def("RemoveObserver",remove_observer_3)
  ;

  class_<FileLogSink, FileLogSinkPtr, bases<LogSink>, boost::noncopyable >("FileLogSink", init<const String&>())
    .def("LogMessage",&FileLogSink::LogMessage)
  ;

  class_<NullLogSink, NullLogSinkPtr, bases<LogSink>, boost::noncopyable >("NullLogSink", init<>())
    .def("LogMessage",&NullLogSink::LogMessage)
  ;

  def("PushVerbosityLevel",push_verb);
  def("PopVerbosityLevel",pop_verb);
  def("PushVerbosityFile",push_verb_file);
  def("PopVerbosityFile",pop_verb_file);
  def("PushLogSink",push_log_sink_1);
  def("PushLogSink",push_log_sink_2);
  def("PushLogSink",push_log_sink_3);
  def("PushLogSink",push_log_sink_4);
  def("PushLogSink",push_log_sink_5);
  def("PopLogSink",pop_log_sink);
  def("LogError",log_error);
  def("LogMessage",log_message);
  def("LogVerbose",log_verbose);
  def("LogDebug",log_debug);
}
