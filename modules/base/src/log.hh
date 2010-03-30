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
#ifndef OST_LOG_HH
#define OST_LOG_HH

#include <ostream>
#include <stack>

#include <ost/module_config.hh>

namespace ost {

  // singleton
class DLLEXPORT_OST_BASE Logger {
public:
  enum LogLevel {
    QUIET=0,
    NORMAL,
    VERBOSE,
    DEBUG,
    DUMP,
    TRACE
  };
  
  void PushVerbosityLevel(int level);
  void PopVerbosityLevel();

  std::ostream& operator()(enum LogLevel);

  static Logger& Instance();

  int GetLogLevel() const {return level_;}
  
protected:
  Logger();
  Logger(const Logger&);
  Logger& operator=(const Logger&);

private:
  int level_;
  std::stack<int> level_stack_;
  std::ostream null_;
};

#define PUSH_VERBOSITY(n) ::ost::Logger::Instance().PushVerbosityLevel(n)
#define POP_VERBOSITY(n) ::ost::Logger::Instance().PopVerbosityLevel()

#define LOG_ERROR(m) ::ost::Logger::Instance()(::ost::Logger::QUIET) << m;
#define LOGN_ERROR(m) ::ost::Logger::Instance()(::ost::Logger::QUIET) << m << std::endl;

#define LOG_MESSAGE(m) if(::ost::Logger::Instance().GetLogLevel()>=::ost::Logger::NORMAL) {(::ost::Logger::Instance()(::ost::Logger::NORMAL)) << m ;}
#define LOGN_MESSAGE(m) if(::ost::Logger::Instance().GetLogLevel()>=::ost::Logger::NORMAL) {(::ost::Logger::Instance()(::ost::Logger::NORMAL)) << m << std::endl;}

#define LOG_VERBOSE(m) if(::ost::Logger::Instance().GetLogLevel()>=::ost::Logger::VERBOSE) {(::ost::Logger::Instance()(::ost::Logger::VERBOSE)) << m ;}
#define LOGN_VERBOSE(m) if(::ost::Logger::Instance().GetLogLevel()>=::ost::Logger::VERBOSE) {(::ost::Logger::Instance()(::ost::Logger::VERBOSE)) << m << std::endl;}

#define LOG_DEBUG(m) if(::ost::Logger::Instance().GetLogLevel()>=::ost::Logger::DEBUG) {(::ost::Logger::Instance()(::ost::Logger::DEBUG)) << m ;}
#define LOGN_DEBUG(m) if(::ost::Logger::Instance().GetLogLevel()>=::ost::Logger::DEBUG) {(::ost::Logger::Instance()(::ost::Logger::DEBUG)) << m << std::endl;}

#ifdef NDEBUG

#  define LOG_DUMP(m)
#  define LOGN_DUMP(m)

#  define LOG_TRACE(m)
#  define LOGN_TRACE(m)

#else

#  define LOG_DUMP(m) if(::ost::Logger::Instance().GetLogLevel()>=::ost::Logger::DUMP) {(::ost::Logger::Instance()(::ost::Logger::DUMP)) << m ;}
#  define LOGN_DUMP(m) if(::ost::Logger::Instance().GetLogLevel()>=::ost::Logger::DUMP) {(::ost::Logger::Instance()(::ost::Logger::DUMP)) << m << std::endl;}

#  define LOG_TRACE(m) if(::ost::Logger::Instance().GetLogLevel()>=::ost::Logger::TRACE) {(::ost::Logger::Instance()(::ost::Logger::TRACE)) << m ;}
#  define LOGN_TRACE(m) if(::ost::Logger::Instance().GetLogLevel()>=::ost::Logger::TRACE) {(::ost::Logger::Instance()(::ost::Logger::TRACE)) << m << std::endl;}

#endif

}

#endif
