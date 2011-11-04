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

#ifndef OST_STAT_ACCUMULATOR_HH
#define OST_STAT_ACCUMULATOR_HH

#include <ost/base.hh>
#include <ost/img/alg/module_config.hh>

namespace ost { namespace img { namespace alg {

template<unsigned int MAX_MOMENT=4>
class DLLEXPORT_IMG_ALG StatAccumulator
{
public:
  StatAccumulator():
    mean_(0.0),
    sum_(0.0),
    sum2_(0.0),
    m2_(0.0),
    m3_(0.0),
    m4_(0.0),
    n_(1)
  {}

  void operator()(Real val)
  {
    Real delta,delta_n,delta_n2,term;
    if(MAX_MOMENT>0){
      delta = val - mean_;
      delta_n = delta / n_;
    }
    if(MAX_MOMENT>3){
      delta_n2 = delta_n * delta_n;
    }
    if(MAX_MOMENT>1){
      term = delta * delta_n * (n_-1);
    }
    if(MAX_MOMENT>3){
      m4_ += term * delta_n2 * (n_*n_ - 3*n_ + 3) + 6 * delta_n2 * m2_ - 4 * delta_n * m3_;
    }
    if(MAX_MOMENT>2){
      m3_ += term * delta_n * (n_ - 2) - 3 * delta_n * m2_;
    }
    if(MAX_MOMENT>1){
      m2_ += term;
    }
    if(MAX_MOMENT>0){
      mean_ += delta_n;
    }
    n_+=1;
    sum_+=val;
    sum2_+=val*val;
  }

  StatAccumulator operator+(const StatAccumulator& acc2) const
  {
    StatAccumulator acc(acc2);
    acc+=*this;
    return acc;
  }

  StatAccumulator& operator+=(const StatAccumulator& acc)
  {
    if(acc.n_==1){
      return *this;
    }
    if(n_==1){
      mean_=acc.mean_;
      sum_=acc.sum_;
      sum2_=acc.sum2_;
      m2_=acc.m2_;
      m3_=acc.m3_;
      m4_=acc.m4_;
      n_=acc.n_;
      return *this;
    }
    Real delta,delta_n,delta_n2,na,nanb;
    Real nb=acc.n_-1;
    if(MAX_MOMENT>0){
      na=n_-1;
      delta = acc.mean_ - mean_;
      delta_n = delta / (na+nb);
    }
    if(MAX_MOMENT>1){
      nanb=na*nb;
    }
    if(MAX_MOMENT>2){
      delta_n2 = delta_n * delta_n;
    }
    if(MAX_MOMENT>3){
      m4_+=acc.m4_+delta*delta_n*delta_n2*nanb*(na*na-nanb+nb*nb)+6.0*delta_n2*(na*na*acc.m2_+nb*nb*m2_)+4.0*delta_n*(na*acc.m3_-nb*m3_);
    }
    if(MAX_MOMENT>2){
      m3_+=acc.m3_+delta*delta_n2*nanb*(na-nb)+3.0*delta_n*(na*acc.m2_-nb*m2_);
    }
    if(MAX_MOMENT>1){
      m2_ += acc.m2_+delta*delta_n*nanb;
    }
    if(MAX_MOMENT>0){
      mean_ += nb*delta_n;
    }
    sum_+=acc.sum_;
    sum2_+=acc.sum2_;
    n_+=nb;
    return *this;
  }

  Real GetNumSamples() const
  {
    return n_-1.0;
  }

  Real GetMean() const
  {
    if(MAX_MOMENT<1){
      throw Error("Mean was not calculated.");
    }
    return mean_;
  }

  Real GetSum() const
  {
    return sum_;
  }

  Real GetVariance() const
  {
    if(MAX_MOMENT<2){
      throw Error("Variance was not calculated.");
    }
    if(n_==1.0){
      return 0.0;
    }
    return m2_/(n_-1);
  }

  Real GetStandardDeviation() const
  {
    return sqrt(GetVariance());
  }

  Real GetRootMeanSquare() const
  {
    if(n_==1.0){
      return 0.0;
    }
    return sqrt(sum2_/(n_-1));
  }

  Real GetSkewness() const
  {
    if(MAX_MOMENT<3){
      throw Error("Skewness was not calculated.");
    }
    if(m2_<1e-20){
      return 0.0;
    }else{
      return m3_/sqrt(m2_*m2_*m2_);
    }
  }

  Real GetKurtosis() const
  {
    if(MAX_MOMENT<4){
      throw Error("Kurtosis was not calculated.");
    }
    if(m2_<1e-20){
      return 0.0;
    }else{
      return ((n_-1)*m4_) / (m2_*m2_);
    }
  }

private:
  Real mean_;
  Real sum_;
  Real sum2_;
  Real m2_;
  Real m3_;
  Real m4_;
  Real n_;
};

}}} //ns
#endif // OST_STAT_ACCUMULATOR_HH
