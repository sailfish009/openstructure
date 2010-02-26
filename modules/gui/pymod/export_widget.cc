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
#include <string>

#include <boost/python.hpp>
#include <boost/python/register_ptr_to_python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
using namespace boost::python;

#include <ost/gui/widget.hh>
#include <ost/gui/widget_registry.hh>
#include <ost/gui/gosty_app.hh>
#include <ost/gui/perspective.hh>
#include <ost/gui/panel_bar/panels.hh>

#include "sip_handler.hh"

using namespace ost;
using namespace ost::gui;


struct WrappedWidget : public Widget
{
  WrappedWidget(PyObject *p, QObject* object):
       Widget(dynamic_cast<QWidget*>(object)){ }

  virtual bool Restore(const QString& prefix){return true;}

  virtual bool Save(const QString& prefix){return true;}

  virtual ~WrappedWidget(){ }

};

void export_Widget()
{
  class_<Widget, WrappedWidget, boost::noncopyable>("WrappedWidget",init<QObject*>())
    .def("Save", &WrappedWidget::Save)
    .def("Restore", &WrappedWidget::Restore)
    .def("SetDestroyOnClose", &WrappedWidget::SetDestroyOnClose)
    .def("DestroyOnClose", &WrappedWidget::DestroyOnClose)
  ;
}

