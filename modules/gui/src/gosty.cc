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
#include <utility>
#include <vector>
#include <signal.h>
#include <iostream>
#include <sstream>
#include <ctime>

#include <ost/gui/module_config.hh>
#include <ost/log.hh>
#include <ost/platform.hh>
#include <ost/gui/python_shell/text_logger.hh>
#include <ost/gui/python_shell/python_shell.hh>
#include <ost/gui/python_shell/python_interpreter.hh>
#include "gl_win.hh"
#include "widget_state_saver.hh"
#include "gosty.hh"

// must come last
#include <QApplication>
#include <QResource>
#include <QFileInfo>
#include <QDir>
#include <QTimer>

using namespace ost;
using namespace ost::gui;

namespace ost { namespace gui{ namespace detail {

DelayedScriptExecutor::DelayedScriptExecutor()
{
  // the trick here is, that timers won't get executed until we call app.exec()
  QTimer::singleShot(0, this, SLOT(Exec()));
}
void DelayedScriptExecutor::Exec()
{
  PythonInterpreter& interp=PythonInterpreter::Instance();  
  interp.Start();
}

}}}

namespace {

static void sigint_handler(int) 
{ 
  std::cerr << "shutting down..." << std::endl; 
  QApplication::exit(-1);
} 

static void reclaim_signals() 
{ 
#ifndef _MSC_VER
  struct sigaction sa; 
  sa.sa_sigaction=0; // first assign this in case sigaction is union 
  sa.sa_handler = sigint_handler; 
  sa.sa_mask = sigset_t(); 
  sa.sa_flags = 0; 
  sigaction(SIGINT, &sa, 0);
#endif
}

String get_ost_root()
{
  QDir dir(QApplication::applicationDirPath());

#if OST_DEBIAN_STYLE_LIBEXEC  
  #ifdef _MSC_VER
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
  #else
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
  #endif
#else
  #ifdef _MSC_VER
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
  #else
    dir.cdUp();
    dir.cdUp();
  #endif
#endif
  return dir.path().toStdString();
}

void setup_python_search_path(const String& root, PythonInterpreter& pi)
{
#ifdef _MSC_VER
  pi.AppendModulePath(QString::fromStdString(root+"\\lib\\openstructure"));
#else  
#  if (defined(__ppc64__) || defined(__x86_64__)) && !defined(__APPLE__)
  pi.AppendModulePath(QString::fromStdString(root+"/lib64/openstructure"));
#  else
  pi.AppendModulePath(QString::fromStdString(root+"/lib/openstructure"));
#  endif
#endif
  pi.AppendModulePath(".");  
}
  
int setup_resources(QApplication& app) 
{
  QResource qr(":/images");
   if(!qr.isValid()) {
     LOG_ERROR("no valid /image Qt resource found");
     return -1;
   }
   int sizes[]={512,256,128,32,16, 0};
   int i=0;
   QIcon icon;
   while (sizes[i]>0) {
    icon.addFile(QString(":images/icon")+QString::number(sizes[i]), 
                 QSize(sizes[i], sizes[i]));
    ++i; 
   }
   app.setWindowIcon(icon);
   return 0;
}


void read_logger_settings(const QString& group_name, TextLogger* logger)
{
  QSettings settings;
  settings.beginGroup("logging");
  settings.beginGroup(group_name);   
  logger->SetCodeLogging(settings.value("log_code",QVariant(false)).toBool());
  logger->SetOutputLogging(settings.value("log_output",QVariant(true)).toBool());
  logger->SetErrorLogging(settings.value("log_error",QVariant(true)).toBool());
  settings.endGroup();
  settings.endGroup();
}

int init_python_interpreter()
{
  // the order of these two calls is important!
  PythonInterpreter::Instance();
  reclaim_signals();
  //
  PythonInterpreter& py=PythonInterpreter::Instance();
  String root =get_ost_root();
  ost::SetPrefixPath(root);
  if(root == "") {
    return -1;
  }
  // setup python shell logging
  TextLogger* console_logger=new TextLogger(stdout);
  read_logger_settings("console", console_logger);
  if (console_logger->GetErrorLogging()) {
    QObject::connect(&PythonInterpreter::Instance(),  
                     SIGNAL(ErrorOutput(unsigned int, const QString &)),
                     console_logger,
                     SLOT(AppendOutput(unsigned int, const QString &)));
  }
  if (console_logger->GetOutputLogging()) {
    QObject::connect(&PythonInterpreter::Instance(), 
                     SIGNAL(Output(unsigned int, const QString &)),
                     console_logger,
                     SLOT(AppendOutput(unsigned int, const QString &)));
  }  
  setup_python_search_path(root, py);
  py.RunCommand("from ost import *");
  py.RunCommand("gui_mode=True");
  return 0;
}

void prepare_scripts(int argc, char** argv, PythonInterpreter& py)
{
  for (int param_iter=2; param_iter<argc; ++param_iter) {
    py.AppendCommandlineArgument(QString(argv[param_iter]));
  }
  py.RunScript(argv[1]);
}


}

// initialise gosty - the graphical open structure interpreter
int main(int argc, char** argv)
{
  int dummy_argc=1;
  
  QApplication app(dummy_argc,argv);
  QCoreApplication::setOrganizationName("OpenStructure");
  QCoreApplication::setOrganizationDomain("openstructure.org");
  QCoreApplication::setApplicationName(QString(argv[2]));
	app.setLibraryPaths(QStringList());
  if (int rv=setup_resources(app)<0) {
    return rv;
  }
  if (int r=init_python_interpreter()<0) {
    return r;
  }
  PythonInterpreter& py_int=PythonInterpreter::Instance();
  // todo remove RunInitRC and replace with general call to run script (with dngrc as argument)
  //py_int.RunInitRC();
  prepare_scripts(argc,argv,py_int);
  //  delay all execution of python scripts after app.exec() has been called.
  ost::gui::detail::DelayedScriptExecutor delayed_executor;
  return app.exec();
}
