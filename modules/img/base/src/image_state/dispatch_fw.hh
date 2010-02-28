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

#ifndef IPLT_IMAGE_STATE_DISPATCH_FW_HH
#define IPLT_IMAGE_STATE_DISPATCH_FW_HH

namespace ost { namespace img { namespace image_state { namespace dispatch {

template <template <typename T, class D> class FNC>
struct unary_dispatch_ip;

template <template <typename T, class D> class FNC>
struct unary_dispatch_op;

template <template <typename T1, class D1, typename T2, class D2> class FNC>
struct binary_dispatch_ip;

template <template <typename T1, class D1, typename T2, class D2> class FNC>
struct binary_dispatch_op;

}}}} // ns




#endif
