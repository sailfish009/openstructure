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

#ifndef IPLT_IMAGE_STATE_BINOP_HH
#define IPLT_IMAGE_STATE_BINOP_HH

#include "dispatch.hh"

namespace ost { namespace iplt { namespace image_state { namespace binop {

// add
template<typename T1, class D1, typename T2, class D2>
struct fnc_add_ip {
  void operator()(ImageStateImpl<T1,D1>* lhs, const ImageStateImpl<T2,D2>* rhs);
};

typedef dispatch::binary_dispatch_ip<fnc_add_ip> add_ip;

// sub
template<typename T1, class D1, typename T2, class D2>
struct fnc_sub_ip {
  void operator()(ImageStateImpl<T1,D1>* lhs, const ImageStateImpl<T2,D2>* rhs);
};

typedef dispatch::binary_dispatch_ip<fnc_sub_ip> sub_ip;

// mul
template<typename T1, class D1, typename T2, class D2>
struct fnc_mul_ip {
  void operator()(ImageStateImpl<T1,D1>* lhs, const ImageStateImpl<T2,D2>* rhs);
};

typedef dispatch::binary_dispatch_ip<fnc_mul_ip> mul_ip;

// div
template<typename T1, class D1, typename T2, class D2>
struct fnc_div_ip {
  void operator()(ImageStateImpl<T1,D1>* lhs, const ImageStateImpl<T2,D2>* rhs);
};

typedef dispatch::binary_dispatch_ip<fnc_div_ip> div_ip;

// paste
template<typename T1, class D1, typename T2, class D2>
struct fnc_paste_ip {
  void operator()(ImageStateImpl<T1,D1>* lhs, const ImageStateImpl<T2,D2>* rhs);
};

typedef dispatch::binary_dispatch_ip<fnc_paste_ip> paste_ip;

}}}} // ns

#endif
