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

#include <ost/message.hh>
#include <ost/gfx/color.hh>
using namespace ost;
using namespace ost::gfx;

namespace {
  float get_red(const Color& c) {
    return c[0];
  }

  void set_red(Color& c, float v) {
    c[0]=v;
  }
  
  float get_green(const Color& c) {
    return c[1];
  }
  
  void set_green(Color& c, float v) {
    c[1]=v;
  }
  
  float get_blue(const Color& c) {
    return c[2];
  }
  
  void set_blue(Color& c, float v) {
    c[2]=v;
  }
  
  float get_alpha(const Color& c) {
    return c[3];
  }

  void set_alpha(Color& c, float v) {
    c[3]=v;
  }

  float get(const Color& c, int i) {
    if(i<0 || i>3) {
      throw Error("Color: index out of bounds");
    }
    return c[i];
  }

  void set(Color& c, int i, float v) {
    if(i<0 || i>3) {
      throw Error("Color: index out of bounds");
    }
    c[i]=v;
  }

  std::string repr(const Color& c) {
    std::ostringstream m;
    m << "gfx.Color(" << c[0] << "," << c[1] << "," << c[2] << "," << c[3] << ")";
    return m.str();
  }

}

void export_color()
{
  class_<Color>("Color",init<>())
    .def(init<float, float, float, optional<float> >())
    .def(self_ns::str(self))
    .def("__repr__",repr)
    .def("Red",get_red)
    .def("Green",get_green)
    .def("Blue",get_blue)
    .def("Alpha",get_alpha)
    .def("ToHSV",&Color::ToHSV)
    .def("FromRGBA",&Color::FromRGB)
    .add_property("r",get_red,set_red)
    .add_property("g",get_green,set_green)
    .add_property("b",get_blue,set_blue)
    .add_property("a",get_alpha,set_alpha)
    .add_property("red",get_red,set_red)
    .add_property("green",get_green,set_green)
    .add_property("blue",get_blue,set_blue)
    .add_property("alpha",get_alpha,set_alpha)
    .def("__getitem__",get)
    .def("__setitem__",get)
    ;

  def("HSV",HSV);
  
}
