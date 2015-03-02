//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2015 by the OpenStructure authors
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
  real and complex function abstract base class,
  implemented in terms of the templated
  function implementation

  Author: Ansgar Philippsen
*/

#ifndef IMG_FUNCTION_H
#define IMG_FUNCTION_H

#include "function_base.hh"
#include "function_impl.hh"

namespace ost { namespace img {

/// \sa \ref modulate_image.py "Modulate Image Example"
typedef detail::FunctionImpl<Real> RealFunction;
typedef detail::FunctionImpl<Complex> ComplexFunction;


}} // namespace img

#endif

/// \example modulate_image.py
///
/// This script shows how to define a python function that modulates an image by a sine. Note the explicit call of RealFunction.__init__ in the modulator class.
