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

#ifndef IMG_IMAGE_STATE_INST_H
#error "binop.cc must be included from image_state_inst.hh"
#endif

#include "binop.hh"

#include <ost/img/extent_iterator.hh>

namespace ost { namespace img { namespace image_state { namespace binop {

template<typename T1, class D1, typename T2, class D2>
void fnc_add_ip<T1,D1,T2,D2>::operator()(ImageStateImpl<T1,D1>* lhs, const ImageStateImpl<T2,D2>* rhs)
{
  Extent ov=Overlap(lhs->GetExtent(),rhs->GetExtent());
  for(ExtentIterator it(ov);!it.AtEnd();++it) {
    lhs->Value(it)+=Val2Val<T2,T1>(rhs->Value(it));
  }
}



template<typename T1, class D1, typename T2, class D2>
void fnc_sub_ip<T1,D1,T2,D2>::operator()(ImageStateImpl<T1,D1>* lhs, const ImageStateImpl<T2,D2>* rhs)
{
  Extent ov=Overlap(lhs->GetExtent(),rhs->GetExtent());
  for(ExtentIterator it(ov);!it.AtEnd();++it) {
    lhs->Value(it)-=Val2Val<T2,T1>(rhs->Value(it));
  }
}


template<typename T1, class D1, typename T2, class D2>
void fnc_mul_ip<T1,D1,T2,D2>::operator()(ImageStateImpl<T1,D1>* lhs, const ImageStateImpl<T2,D2>* rhs)
{
  Extent ov=Overlap(lhs->GetExtent(),rhs->GetExtent());
  for(ExtentIterator it(ov);!it.AtEnd();++it) {
    lhs->Value(it)*=Val2Val<T2,T1>(rhs->Value(it));
  }
}


template<typename T1, class D1, typename T2, class D2>
void fnc_div_ip<T1,D1,T2,D2>::operator()(ImageStateImpl<T1,D1>* lhs, const ImageStateImpl<T2,D2>* rhs)
{
  Extent ov=Overlap(lhs->GetExtent(),rhs->GetExtent());
  for(ExtentIterator it(ov);!it.AtEnd();++it) {
    lhs->Value(it)/=Val2Val<T2,T1>(rhs->Value(it));
  }
}


// partial specialization for half_frequency rhs
template<typename T1, class D1>
struct fnc_paste_ip<T1,D1,Complex,HalfFrequencyDomain> {
  void operator()(ImageStateImpl<T1,D1>* lhs,
		  const ImageStateImpl<Complex,HalfFrequencyDomain>* rhs)
  {
    Extent ov=Overlap(lhs->GetExtent(),rhs->GetLogicalExtent());
    for(ExtentIterator it(ov);!it.AtEnd();++it) {
      lhs->Value(it)=Val2Val<Complex,T1>(rhs->GetComplex(it));
    }
  }
};

// default for rest
template<typename T1, class D1, typename T2, class D2>
void fnc_paste_ip<T1,D1,T2,D2>::operator()(ImageStateImpl<T1,D1>* lhs, const ImageStateImpl<T2,D2>* rhs)
{
  Extent ov=Overlap(lhs->GetExtent(),rhs->GetExtent());
  for(ExtentIterator it(ov);!it.AtEnd();++it) {
    lhs->Value(it)=Val2Val<T2,T1>(rhs->Value(it));
  }
}

// required explicit template instantiation for optimization to work
template struct fnc_paste_ip<Real,SpatialDomain,Complex,HalfFrequencyDomain>;
template struct fnc_paste_ip<Complex,SpatialDomain,Complex,HalfFrequencyDomain>;
template struct fnc_paste_ip<Word,SpatialDomain,Complex,HalfFrequencyDomain>;
template struct fnc_paste_ip<Real,FrequencyDomain,Complex,HalfFrequencyDomain>;
template struct fnc_paste_ip<Complex,FrequencyDomain,Complex,HalfFrequencyDomain>;
template struct fnc_paste_ip<Complex,HalfFrequencyDomain,Complex,HalfFrequencyDomain>;

}

template struct dispatch::binary_dispatch_ip<binop::fnc_add_ip>;
template struct dispatch::binary_dispatch_ip<binop::fnc_sub_ip>;
template struct dispatch::binary_dispatch_ip<binop::fnc_mul_ip>;
template struct dispatch::binary_dispatch_ip<binop::fnc_div_ip>;
template struct dispatch::binary_dispatch_ip<binop::fnc_paste_ip>;

}}} // ns

