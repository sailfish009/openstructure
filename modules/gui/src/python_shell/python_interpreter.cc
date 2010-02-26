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
  python interpreter

  Authors: Andreas Schenk, Ansgar Philippsen, Marco Biasini
*/

#include "python_interpreter.hh"

#include <iostream>
#include <csignal>
#include <string>
#include <fstream>

#include <boost/filesystem/convenience.hpp>

#include <QDebug>
#include <QApplication>
#include <QThread>
#include <QStringList>
#include <ost/log.hh>

namespace ost { namespace gui {

PythonInterpreter::PythonInterpreter():
  QObject(),
  main_module_(),
  main_namespace_(),
#ifndef _MSC_VER
  sig_act_(),
#endif
  output_redirector_(),
  main_thread_runner_(),
  running_(false),
  mutex_(),
  gstate_(),
  main_thread_state_(NULL),
  proxy_(this),
  compile_command_(),
  exec_wait_(false),
  exec_queue_(),
  parse_expr_cmd_()  
{
}

PythonInterpreter::~PythonInterpreter()
{
 PyGILState_Release(gstate_);
 PyEval_RestoreThread(main_thread_state_);
}

PythonInterpreter& PythonInterpreter::Instance(bool register_sighandler, 
                                               bool multithreaded)
{
  //todo handle register_sighandler;
  static bool initialized=false;
  static PythonInterpreter instance;
  static QThread thread;
  if(!initialized){
    initialized=true;
    if(multithreaded){
      instance.moveToThread(&thread);
      thread.start();
      instance.Init(true);
    }else{
      instance.Init(false);
    }
  }
  return instance;
}

void PythonInterpreter::Init(bool multithreaded)
{
  if(multithreaded){
    //initialize over a blocking queued connection to get the correct thread context
  /*  connect(this, SIGNAL(InitSignal(bool)),this, SLOT(InitSlot(bool)),Qt::BlockingQueuedConnection);
    emit InitSignal(true);*/
  }else{
    // direct initalization
    InitSlot(false);
  }
}

void PythonInterpreter::RedirectOutput()
{
  main_namespace_["sys"].attr("stderr") = output_redirector_;
  main_namespace_["sys"].attr("stdout") = output_redirector_;
}

void PythonInterpreter::InitSlot(bool multithreaded)
{
  if(multithreaded){
    //main_thread_runner_.moveToThread(QApplication::instance()->thread());
    //connect(this,SIGNAL(RunInMainThreadSignal(const QString&)),&main_thread_runner_,SLOT(Run(const QString&)),Qt::BlockingQueuedConnection);
  }else{
    connect(this,SIGNAL(RunInMainThreadSignal(const QString&)),&main_thread_runner_,SLOT(Run(const QString&)),Qt::DirectConnection);
  }
  Py_InitializeEx(1);
  PyEval_InitThreads();
  main_thread_state_ = PyEval_SaveThread();
  gstate_ = PyGILState_Ensure();
  //store signal handler
  #ifndef _MSC_VER
  sigaction(SIGINT,0,&sig_act_);
  #endif
  main_module_ = bp::import("__main__");
  main_module_.attr("in_gui_mode")=true;  
  main_namespace_ = bp::extract<bp::dict>(main_module_.attr("__dict__"));
  parse_expr_cmd_=bp::import("parser").attr("expr");  
  main_namespace_["OutputRedirector"] = bp::class_<OutputRedirector>("OutputRedirector", bp::init<>())
                                          .def("write", &OutputRedirector::Write);
  main_namespace_["PythonInterpreterProxy"] = bp::class_<PythonInterpreterProxy>("PythonInterpreterProxy", bp::init<PythonInterpreter*>())
                                          .def("Run", &PythonInterpreterProxy::Run);

  main_namespace_["os"]=bp::import("os");
  main_namespace_["__builtin__"]=bp::import("__builtin__");
  main_namespace_["keyword"]=bp::import("keyword");

  parse_expr_cmd_=bp::import("parser").attr("expr");  

  main_namespace_["sys"]=bp::import("sys");  

  main_module_.attr("Proxy")=proxy_;
  bp::object code=bp::import("code");
  compile_command_=code.attr("compile_command");

}

namespace {
#ifndef _MSC_VER
  class SigIntHandler {
  public:
    SigIntHandler(const struct sigaction& act) {
      sigaction(SIGINT,&act,&old_);
    }
    ~SigIntHandler() {
      sigaction(SIGINT,&old_,0);
    }
  private:
    struct sigaction old_;
  };
#endif
}
void PythonInterpreter::RunInitRC()
{
  if(!getenv("HOME")) return;
  String fname=String(getenv("HOME"))+"/.dngrc";
  try {
    if (boost::filesystem::exists(fname)) {
      try {
        bp::exec_file(bp::str(fname), main_module_.attr("__dict__"), 
                      main_module_.attr("__dict__"));      
      } catch(bp::error_already_set&) {
        PyErr_Print();
      }
    }
  } catch(std::exception&) {
    // silently ignore. needed for boost 1.33.1
  }  
}

void PythonInterpreter::ExecWait()
{
  ++exec_wait_;
}

void PythonInterpreter::ExecRelease()
{
  --exec_wait_;
  assert(exec_wait_>=0);
  if(exec_wait_==0) {
    for(std::vector<QString>::const_iterator it=exec_queue_.begin();
        it!=exec_queue_.end();++it) {
      if(!RunCommand(*it)) break;        
    }
    exec_queue_.clear();
  }
}

void PythonInterpreter::RunScript(const String& fname)
{
  std::ifstream script(fname.c_str());
  if(!script) {
    LOGN_ERROR("could not open " << fname);
    return;
  }

  String line;
  QString command("");
  while(std::getline(script,line)) {
    command.append(QString::fromStdString(line));
    command.append('\n');
    if(this->GetCodeBlockStatus(command)!=CODE_BLOCK_INCOMPLETE) {
      if(!RunCommand(command)) break;
      command=QString("");
    }
  }
  command.append('\n');
  command.append('\n');
  RunCommand(command);
}

bool PythonInterpreter::IsSimpleExpression(const QString& expr)
{
  try {
    parse_expr_cmd_(bp::str(expr.toStdString()));
    return true;
  } catch(bp::error_already_set&) {
    PyErr_Clear();
    return false;
  }
}

bool PythonInterpreter::RunCommand(const QString& command)
{
  bool flag=true;
  if(exec_wait_!=0) {
    exec_queue_.push_back(command);
    return flag;
  }

  // becore doing anything give Qt the chance to handle events for 100ms
  QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents,100);
  mutex_.lock();
  running_=true;
  mutex_.unlock();
#ifndef _MSC_VER
  SigIntHandler sh(sig_act_);
#endif
  try{
    if(command.indexOf(QString("\n"))==-1){
      if(command=="exit" || command=="quit"){
        emit Exit();
      }
      if (this->IsSimpleExpression(command)) {
        bp::object repr=main_module_.attr("__builtins__").attr("repr");
        bp::object result=bp::eval(bp::str(command.toStdString()),
                                   main_namespace_, main_namespace_);
        ost::String rstring=bp::extract<ost::String>(repr(result));
        if(rstring!="None"){
          output_redirector_.Write(rstring);
          output_redirector_.Write("\n");
        }
        emit Done(STATUS_OK,output_redirector_.GetOutput());
        flag=true;
      } else {
        bp::exec(bp::str(command.toStdString()),main_namespace_,main_namespace_);
        flag=true;
      }
    }else{
      bp::exec(bp::str(command.toStdString()),main_namespace_,main_namespace_);
    }
    emit Done(STATUS_OK,output_redirector_.GetOutput());
  }catch(bp::error_already_set){
    if(PyErr_ExceptionMatches(PyExc_SystemExit)){
      PyErr_Clear();
      emit Exit();
    } else{
      PyErr_Print();
      QString output=output_redirector_.GetOutput();
      std::cout << output.toStdString() << std::endl;
      emit Done(STATUS_ERROR,output);
      flag=false;
    }
  }
  mutex_.lock();
  running_=false;
  mutex_.unlock();
  return flag;
}

CodeBlockStatus PythonInterpreter::GetCodeBlockStatus(const QString& command)
{
  CodeBlockStatus status=CODE_BLOCK_COMPLETE;
  mutex_.lock();
  running_=true;
  mutex_.unlock();
#ifndef _MSC_VER
  SigIntHandler sh(sig_act_);
#endif
  QStringList lines=command.split("\n");  
  String cmd;  
  for (int i=0; i<lines.size(); ++i) {
    if (status==CODE_BLOCK_ERROR) {
      break;
    }
    bool complete=false;
    if (!(lines[i].size()>0 && (lines[i][0]=='\t' || lines[i][0]==' '))) {
      cmd.clear();
    }
    while (!complete && i<lines.size()) {
      cmd+=lines[i].toStdString();
      if (i<lines.size()-1) {
        cmd+="\n";
      }
      try {
        bp::object result=compile_command_(bp::str(cmd));
        try {
          String doc=bp::extract<ost::String>(result.attr("__doc__"));
          complete=true;
          status=CODE_BLOCK_COMPLETE;
        } catch(bp::error_already_set) {
          status=CODE_BLOCK_INCOMPLETE;
          ++i;          
          PyErr_Clear();
        }
      } catch(bp::error_already_set) {
        status=CODE_BLOCK_ERROR;
        complete=true;
        PyErr_Clear();
      }
    }
  }
  mutex_.lock();
  running_=false;
  mutex_.unlock();
  return status;
}

enum returncodes{
    RC_OK=0,
    RC_ERROR=2>>1,
    RC_LOOP=2,
    RC_EXIT=2<<1,
};

void PythonInterpreter::CallFunction(const bp::object& func,
                                     const bp::object& args)
{
  unsigned int retcode=RC_OK;
  bool result;
  try{
#ifndef _MSC_VER
  SigIntHandler sh(sig_act_);
#endif
    if (len(args)>0) {
      result=bp::extract<bool>(func(args));
    } else {
      result=bp::extract<bool>(func());
    }
    if (!result) {
      retcode|=RC_ERROR;
      PyErr_Print();
    }
  } catch(bp::error_already_set) {
    if (PyErr_ExceptionMatches(PyExc_KeyboardInterrupt)){
      retcode|=RC_EXIT;
    } else {
      PyErr_Print();
      retcode|=RC_ERROR;
    }
  }
}

bool PythonInterpreter::IsRunning()
{
  mutex_.lock();
  bool state=running_;
  mutex_.unlock();
  return state;
}

void PythonInterpreter::Abort()
{
  if(IsRunning()){
    raise(SIGINT);
  }
}

bp::object PythonInterpreter::RunInMainThread(const QString& widget)
{
  PyGILState_Release(gstate_);
  emit RunInMainThreadSignal(widget);
  gstate_ = PyGILState_Ensure();
  return main_module_.attr("Proxy").attr("_widget_");

}

void PythonInterpreter::AppendModulePath(const QString& entry)
{
  RunCommand(QString("sys.path.insert(0, os.path.expanduser(os.path.expandvars('")+ entry + QString("')))\n"));
}

void PythonInterpreter::AppendCommandlineArgument(const QString& arg)
{
  QString init_code("");
  static bool sys_argv_present=false;
  if(!sys_argv_present){
    sys_argv_present=true;
    init_code += "\nsys.argv=[]";
  }
  init_code += "\nsys.argv.append('" ;
  init_code += arg;
  init_code += "')\n";
  RunCommand(init_code);
}

void PythonInterpreter::SetCommandLineArguments(const QList<QString>& args)
{
  QString init_code("");
  init_code += "\nsys.argv=[]";  
  for (QList<QString>::const_iterator i=args.begin(), e=args.end(); i!=e; ++i) {
    init_code += "\nsys.argv.append('" ;
    init_code += *i;    
    init_code += "')\n";    
  }

  RunCommand(init_code);
}
bp::object PythonInterpreter::GetMainModule() const
{
  return main_module_;
}
bp::dict PythonInterpreter::GetMainNamespace() const
{
  bp::dict result=const_cast<bp::dict&>(main_namespace_).copy();
  bp::dict builtins_dict=bp::extract<bp::dict>(main_namespace_["__builtin__"].attr("__dict__"));
  result.update(builtins_dict);
  bp::list kwlist=bp::extract<bp::list>(main_namespace_["keyword"].attr("kwlist"));
  unsigned int kwlist_length=bp::len(kwlist);
  for(unsigned int i=0;i<kwlist_length;++i){
    result[kwlist[i]]=bp::str();
  }
  return result;
}


}} // ns

