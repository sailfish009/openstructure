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
  Author: Ansgar Philippsen
*/

#include "paste_impl.hh"

#include "image_state/binop.hh"
#include "function.hh"
#include "image.hh"

namespace ost { namespace img { namespace detail {

PasteFnc::PasteFnc():
  target_()
{}

PasteFnc::PasteFnc(const ImageHandle& h):
  target_(h)
{}

template<typename V, class D>
void PasteFnc::VisitState(const image_state::ImageStateImpl<V,D>& isi)
{
  static image_state::binop::paste_ip f_paste_ip;
  f_paste_ip(target_.ImageStatePtr().get(),&isi);
}

void PasteFnc::VisitFunction(const Function& f)
{
  if(target_.GetType()==REAL) {
    for(ExtentIterator it(f.GetExtent());!it.AtEnd();++it) {
      target_.SetReal(it,f.GetReal(it));
    }
  } else {
    for(ExtentIterator it(f.GetExtent());!it.AtEnd();++it) {
      target_.SetComplex(it,f.GetComplex(it));
    }
  }
}

}

#ifdef NO_EXPL_INST
#else
template class image_state::ImageStateNonModAlgorithm<detail::PasteFnc>;
#endif

}} // ns
