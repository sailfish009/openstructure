//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2010 by the OpenStructure authors
// Copyright (C) 2003-2010 by the IPLT authors
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
  python interpreter

  Authors: Andreas Schenk, Ansgar Philippsen, Marco Biasini
*/

#ifndef PYTHON_INTERPRETER_HH
#define PYTHON_INTERPRETER_HH

#include <csignal>
#include <vector>

#include <QMutex>
#include <QMetaType>

#include <boost/python.hpp>

#include <ost/gui/module_config.hh>

#include <ost/gui/module_config.hh>
#include "python_interpreter_proxy.hh"
#include "output_redirector.hh"
#include "main_thread_runner.hh"

namespace ost { namespace gui {
namespace bp = boost::python;

enum InterpreterStatus{
  STATUS_OK,
  STATUS_ERROR
};


typedef enum {
   CODE_BLOCK_COMPLETE,
   CODE_BLOCK_ERROR,
   CODE_BLOCK_INCOMPLETE  
} CodeBlockStatus;

class DLLEXPORT_OST_GUI PythonInterpreter: public QObject
{
  Q_OBJECT
public:
  static PythonInterpreter& Instance(bool set_sigint_handler=true, 
                                     bool multitreaded=false);

  ~PythonInterpreter();
  bool IsRunning();
  bp::object RunInMainThread(const QString& widget);
  void AppendModulePath(const QString& entry);
  
  void AppendCommandlineArgument(const QString& arg);
  /// \brief set command line arguments
  /// 
  /// Sets sys.argv to the given list of arguments
  /// 
  /// The method internally uses RunCommand and is thus appended to the command
  /// list when execution is delayed due to an open ExecWait() ExecRelease() 
  /// block
  /// 
  /// \todo escape arguments
  void SetCommandLineArguments(const QList<QString>& args);
  
  bp::dict GetMainNamespace() const;
  bp::object GetMainModule() const;
  /// \brief Get code block status
  /// 
  /// Determines whether the command contains errors, is incomplete or ready
  /// to be run.
  CodeBlockStatus GetCodeBlockStatus(const QString& command);
  void RunInitRC();

  void RunScript(const String& script);
  
  /// \brief delay execution of Python commands
  /// 
  /// The execution of Python commands with RunCommand and methods that
  /// make use of RunCommand is delayed until ExecRelease() is called.
  /// ExecWait(), ExecRelease() calls can be nested.
  /// 
  /// \sa ExecRelease
  void ExecWait();
  /// \brief execute pending Python commands
  /// 
  /// \sa ExecWait
  void ExecRelease();
  
  /// \brief turn on output redirection
  void RedirectOutput();
  /// \brief whether the string can be interpreted as a simple expression
  /// 
  /// import and any kind of control structures don't count as simple 
  /// expressions and will return false.
  bool IsSimpleExpression(const QString& expr);

public slots:
  /// \brief execute python command
  bool RunCommand(const QString& command);
  void CallFunction(const bp::object& func,const bp::object& args);
  void Abort();
protected slots:
  void InitSlot(bool multitreaded);
signals:
  void Done(int status,const QString& output);
  void Exit();
  void InitSignal(bool multitreaded);
  void RunInMainThreadSignal(const QString& widget);
protected:
  void Init(bool multitreaded);
  PythonInterpreter();
  bp::object main_module_;
  bp::dict main_namespace_;
#ifndef _MSC_VER
  struct sigaction sig_act_;
#endif
  OutputRedirector  output_redirector_;
  MainThreadRunner main_thread_runner_;
  bool running_;
  QMutex mutex_;
  PyGILState_STATE gstate_;
  PyThreadState* main_thread_state_;
  PythonInterpreterProxy proxy_;
  bp::object compile_command_;
  int exec_wait_;
  std::vector<QString>      exec_queue_;
  bp::object parse_expr_cmd_;  
};

}} // ns

#endif
