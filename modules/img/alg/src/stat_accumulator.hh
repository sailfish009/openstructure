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

#include <boost/math/special_functions/binomial.hpp>
#include <ost/base.hh>
#include <ost/message.hh>
#include <ost/img/alg/module_config.hh>

namespace ost { namespace img { namespace alg {


template<unsigned int MAX_MOMENT=4>
class  StatAccumulator
{
public:
  StatAccumulator():
    sum_(0.0),
    sum2_(0.0),
    m_(),
    w_(0.0),
    n_(0)
  {
    for(unsigned int i=0;i<MAX_MOMENT;++i){
      m_[i]=0.0;
    }

  }

  StatAccumulator(Real val, Real w=1.0):
    sum_(val),
    sum2_(val*val),
    m_(),
    w_(w),
    n_(1)
  {
    if(MAX_MOMENT>0){
      m_[0]=val;
      for(unsigned int i=1;i<MAX_MOMENT;++i){
        m_[i]=0.0;
      }
    }
  }

  void operator()(Real val, Real w=1.0)
  {
    *this+=StatAccumulator(val,w);
  }

  StatAccumulator<MAX_MOMENT> operator+(const StatAccumulator<MAX_MOMENT>& acc2) const
  {
    StatAccumulator<MAX_MOMENT> acc(acc2);
    acc+=*this;
    return acc;
  }

  StatAccumulator<MAX_MOMENT>& operator+=(const StatAccumulator<MAX_MOMENT>& acc)
  {
    if(0.0>=w_){
      *this=acc;
      return *this;
    }
    if(0.0>=acc.w_){
      return *this;
    }
    n_+=acc.n_;
    Real wn=w_+acc.w_;

    sum_+=acc.sum_;
    sum2_+=acc.sum2_;

    if(MAX_MOMENT>0){
      Real delta=acc.m_[0]-m_[0];
      Real delta_w=delta/wn;
      if(MAX_MOMENT>2){
        Real w1w2_delta_w=w_*acc.w_*delta_w;
        Real w1w2_delta_wp=w1w2_delta_w*w1w2_delta_w;
        Real iw2=1.0/acc.w_;
        Real iw2pm1=iw2;
        Real miw1=-1.0/w_;
        Real miw1pm1=miw1;
        Real mn[MAX_MOMENT]; // only MAX_MOMENT-2 values needed but overdimensioned to allow compilation for MAX_MOMENT==0 (even though it gets kicked out in the end by the dead code elimination)
        for(unsigned int p=3;p<=MAX_MOMENT;++p){
          w1w2_delta_wp*=w1w2_delta_w;
          iw2pm1*=iw2;
          miw1pm1*=miw1;
          Real delta_wk=1.0;
          Real s=0.0;
          Real mw2k=1.0;
          Real w1k=1.0;
          for(unsigned int k=1;k<=p-2;++k){
            w1k*=w_;
            mw2k*=-acc.w_;
            delta_wk*=delta_w;
            s+=boost::math::binomial_coefficient<Real>(p, k)*(mw2k*m_[p-k-1]+w1k*acc.m_[p-k-1])*delta_wk;
          }
          mn[p-3]=acc.m_[p-1]+s+w1w2_delta_wp*(iw2pm1-miw1pm1);
        }
        for(unsigned int p=3;p<=MAX_MOMENT;++p){
          m_[p-1]+=mn[p-3];
        }
      }
      if(MAX_MOMENT>1){
        m_[1]+=acc.m_[1]+delta_w*delta*acc.w_*w_;
      }
      m_[0]+=delta_w*acc.w_;
      w_=wn;
    }
    return *this;
  }

  Real GetNumSamples() const
  {
    return n_;
  }

  Real GetWeight() const
  {
    return w_;
  }

  Real GetMoment(unsigned int i) const
  {
    if(1>i){
      throw Error("Invalid moment.");
    }
    if(MAX_MOMENT<i){
      throw Error("Moment was not calculated.");
    }
    return m_[i-1];
  }

  Real GetMean() const
  {
    if(MAX_MOMENT<1){
      throw Error("Mean was not calculated.");
    }
    return m_[0];
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
    if(n_==0.0){
      return 0.0;
    }
    return m_[1]/(w_);
  }

  Real GetStandardDeviation() const
  {
    return sqrt(GetVariance());
  }

  Real GetRootMeanSquare() const
  {
    if(n_==0.0){
      return 0.0;
    }
    return sqrt(sum2_/(w_));
  }

  Real GetSkewness() const
  {
    if(MAX_MOMENT<3){
      throw Error("Skewness was not calculated.");
    }
    if(m_[1]<1e-20){
      return 0.0;
    }else{
      return m_[2]/sqrt(m_[1]*m_[1]*m_[1]);
    }
  }

  Real GetKurtosis() const
  {
    if(MAX_MOMENT<4){
      throw Error("Kurtosis was not calculated.");
    }
    if(m_[1]<1e-20){
      return 0.0;
    }else{
      return w_*m_[3] / (m_[1]*m_[1]);
    }
  }

private:
  Real sum_;
  Real sum2_;
  Real m_[MAX_MOMENT];
  Real w_;
  unsigned int n_;
};

}}} //ns
#endif // OST_STAT_ACCUMULATOR_HH
