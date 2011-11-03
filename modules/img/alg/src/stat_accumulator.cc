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


#include "stat_accumulator.hh"

namespace ost { namespace img { namespace alg {

StatAccumulator::StatAccumulator():
  mean_(0.0),
  sum_(0.0),
  sum2_(0.0),
  m2_(0.0),
  m3_(0.0),
  m4_(0.0),
  n_(1)

{
}

void StatAccumulator::operator()(Real val)
{
  Real delta = val - mean_;
  Real delta_n = delta / n_;
  Real delta_n2 = delta_n * delta_n;
  Real term = delta * delta_n * (n_-1);

  mean_ += delta_n;
  m4_ += term * delta_n2 * (n_*n_ - 3*n_ + 3) + 6 * delta_n2 * m2_ - 4 * delta_n * m3_;
  m3_ += term * delta_n * (n_ - 2) - 3 * delta_n * m2_;
  m2_ += term;
  sum_+=val;
  sum2_+=val*val;
  n_+=1;
}

StatAccumulator StatAccumulator::operator+(const StatAccumulator& acc2) const
{
  StatAccumulator acc(acc2);
  acc+=*this;
  return acc;
}

StatAccumulator& StatAccumulator::operator+=(const StatAccumulator& acc)
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
  Real na=n_-1;
  Real nb=acc.n_-1;
  Real nanb=na*nb;
  Real delta = acc.mean_ - mean_;
  Real delta_n = delta / (na+nb);
  Real delta_n2 = delta_n * delta_n;

  mean_ += nb*delta_n;
  m4_+=acc.m4_+delta*delta_n*delta_n2*nanb*(na*na-nanb+nb*nb)+6.0*delta_n2*(na*na*acc.m2_+nb*nb*m2_)+4.0*delta_n*(na*acc.m3_-nb*m3_);
  m3_+=acc.m3_+delta*delta_n2*nanb*(na-nb)+3.0*delta_n*(na*acc.m2_-nb*m2_);
  m2_ += acc.m2_+delta*delta_n*nanb;
  sum_+=acc.sum_;
  sum2_+=acc.sum2_;
  n_+=nb;
  return *this;
}

Real StatAccumulator::GetMean() const
{
  return mean_;
}

Real StatAccumulator::GetSum() const
{
  return sum_;
}

Real StatAccumulator::GetVariance() const
{
  if(n_==1.0){
    return 0.0;
  }
  return m2_/(n_-1);
}

Real StatAccumulator::GetStandardDeviation() const
{
  return sqrt(GetVariance());
}

Real StatAccumulator::GetRootMeanSquare() const
{
  if(n_==1.0){
    return 0.0;
  }
  return sqrt(sum2_/(n_-1));
}

Real StatAccumulator::GetSkewness() const
{
  if(m2_<1e-20){
    return 0.0;
  }else{
    return m3_/sqrt(m2_*m2_*m2_);
  }
}

Real StatAccumulator::GetKurtosis() const
{
  if(m2_<1e-20){
    return 0.0;
  }else{
    return ((n_-1)*m4_) / (m2_*m2_);
  }
}

}}} //ns
