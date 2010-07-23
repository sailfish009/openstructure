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
#include <iostream>

#include <boost/python.hpp>
#include <QString>
#include <ost/config.hh>

using namespace boost::python;

void export_AlignmentView();
void export_Tool();
void export_Plot();
void export_GLWin();
void export_Gosty();
void export_PyShell();
void export_SceneWin();
void export_SequenceViewer();
void export_Panels();
void export_Perspective();
void export_SipHandler();
void export_SceneSelection();
void export_MainArea();
void export_MenuBar();
void export_RemoteSiteLoader();
void export_FileLoader();
void export_FileViewer();
void export_Widget();

#if OST_SPNAV_ENABLED
void export_Input();
#endif

#ifdef OST_IMG_ENABLED
void export_data_viewer();
void export_overlay();
void export_overlay_manager();
#endif

#include <boost/python/to_python_converter.hpp>

namespace {

  struct qstring_to_python_string
  {
    static PyObject* convert(QString const& s)
    {
      return boost::python::incref(boost::python::object(s.unicode()).ptr());
    }
  };

  struct qstring_from_python_string
  {
    qstring_from_python_string()
    {
      boost::python::converter::registry::push_back(
        &convertible,
        &construct,
        boost::python::type_id<QString>());
    }

    static void* convertible(PyObject* obj_ptr)
    {
      if (!PyString_Check(obj_ptr)) return 0;
      return obj_ptr;
    }

    static void construct(
      PyObject* obj_ptr,
      boost::python::converter::rvalue_from_python_stage1_data* data)
    {
      const char* value = PyString_AsString(obj_ptr);
      if (value == 0) boost::python::throw_error_already_set();
      void* storage = (
        (boost::python::converter::rvalue_from_python_storage<QString>*)
          data)->storage.bytes;
      new (storage) QString(value);
      data->convertible = storage;
    }
  };


}

BOOST_PYTHON_MODULE(_gui)
{

  boost::python::to_python_converter<QString, qstring_to_python_string>();
  qstring_from_python_string();
  export_SipHandler();  
  export_Gosty();
  export_AlignmentView();
  export_Tool();
  export_Plot();
  export_GLWin();
  export_MainArea();
  export_PyShell();
  export_Panels();
  export_MenuBar();
  export_Perspective();  
  export_SceneWin();
  export_SceneSelection();
  export_SequenceViewer();
  export_RemoteSiteLoader();
  export_FileLoader();
  export_FileViewer();
  export_Widget();

  #if OST_SPNAV_ENABLED
  export_Input();
  #endif

  #if OST_IMG_ENABLED
  export_data_viewer();
  export_overlay();
  export_overlay_manager();
  #endif
}
