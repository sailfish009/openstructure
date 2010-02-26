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
  template function implementation, to serve
  as the basis for RealFunction and ComplexFunction

  Author: Ansgar Philippsen
*/

#ifndef IPLT_FUNCTION_IMPL_H
#define IPLT_FUNCTION_IMPL_H

#include <ost/iplt/module_config.hh>
#include "function_base.hh"

namespace ost { namespace iplt { namespace detail {

template <typename T>
class TEMPLATE_EXPORT FunctionImpl: public Function {
 public:

  ////////////////////////////
  // Data interface

  virtual DataType GetType() const;

  /////////////////////////////
  // Function interface

  virtual Real GetIntpolReal(const Vec3 &v) const;
  virtual Complex GetIntpolComplex(const Vec3 &v) const;

  //////////////////////////////////
  // FunctionImpl interface

  /*!
    abstract method that returns actual function value. Must
    be implemented by derived classes
  */
  virtual T Func(const Vec3 &v) const = 0;

 protected:
  //! instantiation requires domain specification
  FunctionImpl(DataDomain d);

  FunctionImpl(const FunctionImpl& f);

  FunctionImpl& operator=(const FunctionImpl& f);
};

}}} // namespaces

#endif
