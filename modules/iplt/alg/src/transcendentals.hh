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
  "overloaded" transcendentals for images; instead of providing the
  typical func(image) interface, which would limit the use to out-of-place
  application, a full fletched iplt::alg interface is given for each:

  cos, exp, log, log10, pow, sin, sqrt, tan

  Author: Ansgar Philippsen
*/

#ifndef IPLT_ALG_TRANS_HH
#define IPLT_ALG_TRANS_HH

#include <ost/iplt/image.hh>
#include <ost/iplt/image_state.hh>
#include <ost/iplt/alg/module_config.hh>


#define IPLT_ALG_TRANSCENDENTALS_BLOCK(FF,NN,SS)                               \
struct FF {					                                                           \
  FF() {}                                                                      \
  ~FF() {}                                                                     \
  template <typename T, class D>                                               \
  void VisitState(ImageStateImpl<T,D>& isi) const {	                           \
    T* end = isi.Data().GetEnd();                                              \
    for(T* it = isi.Data().GetData(); it!=end; ++it) {                         \
      (*it) = SS (*it);                                                        \
    }                                                                          \
  }                                                                            \
  template <class D>                                                           \
  void VisitState(ImageStateImpl<Word,D>& isi) const {                         \
    Word* end = isi.Data().GetEnd();                                           \
    for(Word* it = isi.Data().GetData(); it!=end; ++it) {                      \
      (*it) = static_cast<Word>(SS(static_cast<Real>(*it)));                 \
    }                                                                          \
  }                                                                            \
  static String GetAlgorithmName() {return "";}                                \
};                                                                             \
typedef ImageStateConstModIPAlgorithm<FF> NN;       

namespace ost { namespace iplt { namespace alg {

IPLT_ALG_TRANSCENDENTALS_BLOCK(CosFnc,Cos,std::cos)
IPLT_ALG_TRANSCENDENTALS_BLOCK(ExpFnc,Exp,std::exp)
IPLT_ALG_TRANSCENDENTALS_BLOCK(LogFnc,Log,std::log)
IPLT_ALG_TRANSCENDENTALS_BLOCK(Log10Fnc,Log10,std::log)
IPLT_ALG_TRANSCENDENTALS_BLOCK(SinFnc,Sin,std::sin)
IPLT_ALG_TRANSCENDENTALS_BLOCK(SqrtFnc,Sqrt,std::sqrt)
IPLT_ALG_TRANSCENDENTALS_BLOCK(TanFnc,Tan,std::tan)


struct PowFnc {
  PowFnc():exp_(1.0) {}
  PowFnc(Real exp): exp_(exp) {}

  template <typename T, class D>
  void VisitState(ImageStateImpl<T,D>& isi) const {
    T* end = isi.Data().GetEnd();
    for(T* it = isi.Data().GetData(); it!=end; ++it) {
      (*it) = std::pow(*it,exp_);
    }
  }
  static String GetAlgorithmName() {return "";}
 private:
  Real exp_;
};

typedef ImageStateConstModIPAlgorithm<PowFnc> Pow;           


}}} // ns

#undef IPLT_ALG_TRANSCENDENTALS_BLOCK


#endif
