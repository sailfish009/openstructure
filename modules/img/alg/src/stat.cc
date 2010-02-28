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
  Authors: Andreas Schenk,  Ansgar Philippsen
*/

#include <utility>
#include <cmath>
#include <iostream>

#include <ost/message.hh>
#include <ost/img/value_util.hh>

#include "stat.hh"

namespace ost { namespace img { namespace alg {

namespace {
  typedef std::pair<Real,Point> ValIndex;
}

template <typename T, class D>
void StatBase::VisitState(const ImageStateImpl<T,D>& isi)
{
  sum_=0.0;
  Real sum2=0.0,sum3=0.0,sum4=0.0;
  Real n = static_cast<Real>(isi.GetSize().GetVol());
  Vec3 sumcenter(0.0,0.0,0.0);

  if(n==0.0)  return;
  ValIndex minindex(std::numeric_limits<Real>::max(),Point(0,0,0));
  ValIndex maxindex(-std::numeric_limits<Real>::max(),Point(0,0,0));
  min_ = std::numeric_limits<Real>::max();
  max_ = -std::numeric_limits<Real>::max();

  int wi=isi.GetSize()[0];
  int he=isi.GetSize()[1];
  int de=isi.GetSize()[2];
  for(int i=0;i<wi;++i) {
    for(int j=0;j<he;++j) {
      for(int k=0;k<de;++k) {
        Real val=Val2Val<T,Real>(isi.Value(Index(i,j,k)));
        Real oval=val;
        ValIndex vi(val,Point(i,j,k));
        minindex=std::min<ValIndex>(vi,minindex);
        maxindex=std::max<ValIndex>(vi,maxindex);
        sumcenter+=Vec3(i,j,k)*val;
        sum_+=val;
        val*=oval;
        sum2+=val;
        val*=oval;
        sum3+=val;
        val*=oval;
        sum4+=val;
      }
    }
  }
  min_=minindex.first;
  max_=maxindex.first;
  minpos_=minindex.second+isi.GetExtent().GetStart();
  maxpos_=maxindex.second+isi.GetExtent().GetStart();  
  mean_ = sum_/n;
  var_=sum2/n-mean_*mean_;
  std_dev_=sqrt(var_);
  rms_=sqrt(sum2/n);
  if(std_dev_>0.0){
    skewness_=sqrt(n)*(sum3-3.0*mean_*sum2+3.0*mean_*mean_*sum_ -n*mean_*mean_*mean_)/(std_dev_*std_dev_*std_dev_);
    kurtosis_= n*(sum4-4.0*sum3*mean_+6.0*sum2*mean_*mean_-4.0*sum_*mean_*mean_*mean_+n*mean_*mean_*mean_*mean_)/(var_*var_)-3.0; 
  }else{
    skewness_=0.0;
    kurtosis_=0.0;
  }
  if(sum_!=0.0){
    center_of_mass_=sumcenter/sum_+isi.GetExtent().GetStart().ToVec3();
  }else{
    center_of_mass_=Vec3(0.0,0.0,0.0);
  }
}

void StatBase::VisitFunction(const Function& fnc)
{
  sum_=0.0;
  Real sum2=0.0,sum3=0.0,sum4=0.0;
  ValIndex minindex(std::numeric_limits<Real>::max(),Point(0,0,0));
  ValIndex maxindex(-std::numeric_limits<Real>::max(),Point(0,0,0));
  Real n = (Real)(fnc.GetSize().GetVol());
  Vec3 sumcenter(0.0,0.0,0.0);

  if(n==0.0)  return;

  min_ = std::numeric_limits<Real>::max();
  max_ = -std::numeric_limits<Real>::max();

  for(ExtentIterator it(fnc.GetExtent());!it.AtEnd(); ++it) {
    Real val=fnc.GetReal(it);
    Real oval=val;
    ValIndex vi(val,it);
    minindex=std::min<ValIndex>(vi,minindex);
    maxindex=std::max<ValIndex>(vi,maxindex);
    sumcenter+=Point(it).ToVec3()*val;
    sum_+=val;
    val*=oval;
    sum2+=val;
    val*=oval;
    sum3+=val;
    val*=oval;
    sum4+=val;
  }
  min_=minindex.first;
  max_=maxindex.first;
  minpos_=minindex.second;
  maxpos_=maxindex.second;  
  mean_ = sum_/n;
  var_=sum2/n-mean_*mean_;
  std_dev_=sqrt(var_);
  rms_=sqrt(sum2/n);
  if(std_dev_>0.0){
    skewness_=sqrt(n)*(sum3-3*mean_*sum2+3*mean_*mean_*sum_-n*mean_*mean_*mean_)/(std_dev_*std_dev_*std_dev_);
    kurtosis_= n*(sum4-4*sum3*mean_+6*sum2*mean_*mean_-4*sum_*mean_*mean_*mean_+n*mean_*mean_*mean_*mean_)/(var_*var_)-3; 
  }else{
    skewness_=0.0;
    kurtosis_=0.0;
  }
  if(sum_!=0.0){
    center_of_mass_=sumcenter/sum_;
  }else{
    center_of_mass_=Vec3(0.0,0.0,0.0);
  }
}

std::ostream& operator<<(std::ostream& o, const Stat& s)
{
  o << "mean=" << s.GetMean() << " ";
  o << "var=" << s.GetVariance() << " ";
  o << "std-dev=" << s.GetStandardDeviation() << " ";
  o << "min=" << s.GetMinimum() << " " << s.GetMinimumPosition() << " ";
  o << "max=" << s.GetMaximum() << " "<< s.GetMaximumPosition() << " ";
  o << "rms=" << s.GetRootMeanSquare() << " ";
  o << "skewness=" << s.GetSkewness() << " ";
  o << "kurtosis=" << s.GetKurtosis() << " ";
  o << "center of mass=" << s.GetCenterOfMass() << " ";
  return o;
}


}
template class TEMPLATE_DEF_EXPORT ImageStateNonModAlgorithm<alg::StatBase>;
}} // ns
