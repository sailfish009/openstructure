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
#ifndef OST_GUI_PYTHON_SHELL_PROXY_HH
#define OST_GUI_PYTHON_SHELL_PROXY_HH

#include <ost/gui/python_shell/python_shell.hh>
#include <ost/gui/python_shell/python_shell_widget.hh>

#include "sip_handler.hh"

namespace ost { namespace gui {

  

class PythonShellProxy : public SipHandler<PythonShell> {
public:
  PythonShellProxy()
      : SipHandler<PythonShell>(NULL) {}

  PythonShellProxy(PythonShell* shell)
      : SipHandler<PythonShell>(shell) {}

  void Show()
  {
    Me()->show();
  }
  
  void Hide()
  {
    Me()->hide();
  }
  
  void SetTabWidth(int width) {
    Me()->PyShell()->SetTabWidth(width);
  }
  
  int GetTabWidth() {
   return Me()->PyShell()->GetTabWidth();
  }

};

}}

#endif
