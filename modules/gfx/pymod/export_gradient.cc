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
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
using namespace boost::python;

#include <ost/message.hh>
#include <ost/gfx/gradient.hh>
using namespace ost;
using namespace ost::gfx;

namespace {
  Gradient* make_gradient(const dict& d)
  {
    std::auto_ptr<Gradient> grad(new Gradient);
    list keys = d.keys();
    for(int i=0;i<len(keys);++i) {
      extract<float> fex(keys[i]);
      if(!fex.check()) {
        throw std::runtime_error("expected floats as keys");
      }
      float mark = fex();
      Color col;
      object val = d[keys[i]];
      extract<Color> cex(val);
      if(cex.check()) {
        // use gfx.Color
        col=cex();
      } else {
        // try simple sequence
        if(len(val)!=3) {
          throw std::runtime_error("expected values of gfx.Color or float triplets");
        }
        try {
          col=gfx::Color(extract<float>(val[0]),extract<float>(val[1]),extract<float>(val[2]));
        } catch (...) {
          throw std::runtime_error("expected values of gfx.Color or float triplets");
        }
      }
      grad->SetColorAt(mark,col);
    }
    return grad.release();
  }
}

void export_gradient()
{
  class_<Gradient>("Gradient", init<>())
    .def(init<const String&>())
    .def("__init__", make_constructor(make_gradient))
    .def("SetColorAt", &Gradient::SetColorAt)
    .def("GetColorAt", &Gradient::GetColorAt)
    .def("GetStops", &Gradient::GetStops)
    .def("GradientToInfo", &Gradient::GradientToInfo)
    .def("GradientFromInfo", &Gradient::GradientFromInfo).staticmethod("GradientFromInfo")
  ;
  implicitly_convertible<String, Gradient>();

  class_<Gradient::StopList>("GradientStopList", init<>())
    .def(vector_indexing_suite<Gradient::StopList>())
  ;

  class_<Gradient::Stop>("GradientStop", init<>())
    .def("GetColor", &Gradient::Stop::GetColor)
    .add_property("color", &Gradient::Stop::GetColor)
    .def("GetRel", &Gradient::Stop::GetRel)
    .add_property("rel", &Gradient::Stop::GetRel)
  ;

}
