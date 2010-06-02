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
#include <vector>

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include <ost/gui/sequence/sequence_viewer.hh>

#include "sip_handler.hh"

using namespace boost::python;
using namespace ost;
using namespace ost::gui;

namespace {

void change_display_mode_a(SequenceViewerV2* seq_viewer, const QString& mode)
{
  seq_viewer->ChangeDisplayMode(mode);
}

void change_display_mode_b(SequenceViewerV2* seq_viewer, const gfx::EntityP& entity, const QString& mode)
{
  seq_viewer->ChangeDisplayMode(entity, mode);
}

void change_display_mode_c(SequenceViewerV2* seq_viewer, const seq::AlignmentHandle& alignment, const QString& mode)
{
  seq_viewer->ChangeDisplayMode(alignment, mode);
}

void get_current_display_mode_a(SequenceViewerV2* seq_viewer)
{
  seq_viewer->GetCurrentDisplayMode();
}

void get_current_display_mode_b(SequenceViewerV2* seq_viewer, const gfx::EntityP& entity)
{
  seq_viewer->GetCurrentDisplayMode(entity);
}

void get_current_display_mode_c(SequenceViewerV2* seq_viewer, const seq::AlignmentHandle& alignment)
{
  seq_viewer->GetCurrentDisplayMode(alignment);
}

std::vector<QString> get_display_modes_a(SequenceViewerV2* seq_viewer)
{
  std::vector<QString> modes;
  const QStringList& list = seq_viewer->GetDisplayModes();
  for (int i=0; i<list.size(); i++){
    modes.push_back(list.at(i));
  }
  return modes;
}

std::vector<QString> get_display_modes_b(SequenceViewerV2* seq_viewer, const gfx::EntityP& entity)
{
  std::vector<QString> modes;
  const QStringList& list = seq_viewer->GetDisplayModes(entity);
  for (int i=0; i<list.size(); i++){
    modes.push_back(list.at(i));
  }
  return modes;
}

std::vector<QString> get_display_modes_c(SequenceViewerV2* seq_viewer, const seq::AlignmentHandle& alignment)
{
  std::vector<QString> modes;
  const QStringList& list = seq_viewer->GetDisplayModes(alignment);
  for (int i=0; i<list.size(); i++){
    modes.push_back(list.at(i));
  }
  return modes;
}

}


void export_SequenceViewerV2()
{
  class_<SequenceViewerV2, boost::noncopyable >("SequenceViewerV2",init<>())
    .def("Show", &SequenceViewerV2::show)
    .def("Hide", &SequenceViewerV2::hide)
    .def("AddAlignment", &SequenceViewerV2::AddAlignment)
    .def("RemoveAlignment", &SequenceViewerV2::RemoveAlignment)
    .def("GetDisplayModes", &get_display_modes_a)
    .def("GetDisplayModes", &get_display_modes_b)
    .def("GetDisplayModes", &get_display_modes_c)
    .def("GetCurrentDisplayMode", &get_current_display_mode_a)
    .def("GetCurrentDisplayMode", &get_current_display_mode_b)
    .def("GetCurrentDisplayMode", &get_current_display_mode_c)
    .def("ChangeDisplayMode",&change_display_mode_a)
    .def("ChangeDisplayMode",&change_display_mode_b)
    .def("ChangeDisplayMode",&change_display_mode_c)
    .def("GetQObject",&get_py_qobject<SequenceViewerV2>)
    .add_property("qobject", &get_py_qobject<SequenceViewerV2>)
  ;

  class_<std::vector<QString> >("QStringList", init<>())
    .def(vector_indexing_suite<std::vector<QString>, true >())
  ;
}

