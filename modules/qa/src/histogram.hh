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
#ifndef OST_QA_HISTOGRAM_HH
#define OST_QA_HISTOGRAM_HH
/*
  Author: Marco Biasini
 */
#include <ost/stdint.hh>

#include <ost/qa/module_config.hh> 
#include <ost/qa/multi_classifier.hh>

namespace ost { namespace qa {

/// \brief histogram specialization for multi classifier  
template <typename T1,
          typename T2=impl::NullType,
          typename T3=impl::NullType,
          typename T4=impl::NullType,
          typename T5=impl::NullType,
          typename T6=impl::NullType,
          typename T7=impl::NullType>
class Histogram : public MultiClassifier<uint32_t, T1, T2, T3, T4, T5, T6, T7> 
{
public:
  typedef Classifier<T1>                       C1;
  typedef Classifier<T2>                       C2;
  typedef Classifier<T3>                       C3;
  typedef Classifier<T4>                       C4;
  typedef Classifier<T5>                       C5;
  typedef Classifier<T6>                       C6;
  typedef Classifier<T7>                       C7;  
#ifdef _MSC_VER
  Histogram(typename C1::ConstRefType c1,
            typename C2::ConstRefType c2=C2::Type(),
            typename C3::ConstRefType c3=C3::Type(),
            typename C4::ConstRefType c4=C4::Type(),
            typename C5::ConstRefType c5=C5::Type(),
            typename C6::ConstRefType c6=C6::Type(),
            typename C7::ConstRefType c7=C7::Type())
#else
  Histogram(typename C1::ConstRefType c1,
            typename C2::ConstRefType c2=typename C2::Type(),
            typename C3::ConstRefType c3=typename C3::Type(),
            typename C4::ConstRefType c4=typename C4::Type(),
            typename C5::ConstRefType c5=typename C5::Type(),
            typename C6::ConstRefType c6=typename C6::Type(),
            typename C7::ConstRefType c7=typename C7::Type())
#endif
   : MultiClassifier<uint32_t, T1, T2, T3, T4, 
                     T5, T6, T7>(0, c1, c2, c3, c4, c5, c6, c7) {        
  }
  
  Histogram(): 
    MultiClassifier<uint32_t, T1, T2, T3, T4, T5, T6, T7>() 
  {
  }
};

}} // ost::qa

#endif
