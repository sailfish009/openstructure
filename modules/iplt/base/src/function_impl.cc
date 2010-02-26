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
  function implementation

  Author: Ansgar Philippsen
*/

#include "function_impl.hh"
#include "value_util.hh"

namespace ost { namespace iplt { namespace detail {

template <typename T>
DataType FunctionImpl<T>::GetType() const
{
  return Val2Type<T>();
}


template <typename T>
Real FunctionImpl<T>::GetIntpolReal(const Vec3 &v) const
{
  return Val2Val<T,Real>(Func(v));
}

template <typename T>
Complex FunctionImpl<T>::GetIntpolComplex(const Vec3 &v) const
{
  return Val2Val<T,Complex>(Func(v));
}

template <typename T>
FunctionImpl<T>::FunctionImpl(DataDomain d):
  Function(d)
{}


template <typename T>
FunctionImpl<T>::FunctionImpl(const FunctionImpl<T>& f):
  Function(f)
{}

template <typename T>
FunctionImpl<T>& FunctionImpl<T>::operator=(const FunctionImpl<T>& f) 
{
  Function::operator=(f);
  return *this;
}


} // namespace

// explicit instantiations
template class TEMPLATE_DEF_EXPORT detail::FunctionImpl<Real>;
template class TEMPLATE_DEF_EXPORT detail::FunctionImpl<Complex>;

}} // namespace
