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
#include <algorithm>

#include <Eigen/SVD>

#include "vec3.hh"
// TODO: these are for the (misplaced) Vec3List algorithm functions
#include "vecmat3_op.hh"
#include "composite3.hh"
#include "composite3_op.hh"

namespace geom {


#if OST_DOUBLE_PRECISION
typedef Eigen::Matrix3d EMat3;
#else
typedef Eigen::Matrix3f EMat3;
#endif


Mat3 Vec3List::GetInertia() const
{
  Mat3 cov(0,0,0,0,0,0,0,0,0);
  Vec3 center=this->GetCenter();
  for (Vec3List::const_iterator i=this->begin(),e=this->end(); i!=e; ++i) {
    Vec3 p=*i-center;
    cov(0,0)+=p.y*p.y+p.z*p.z;
    cov(1,1)+=p.x*p.x+p.z*p.z;
    cov(2,2)+=p.x*p.x+p.y*p.y;
    cov(0,1)-=p.x*p.y;
    cov(1,2)-=p.y*p.z;
    cov(0,2)-=p.x*p.z;
  }  
  cov(1,0)=cov(0,1);    
  cov(2,1)=cov(1,2);    
  cov(2,0)=cov(0,2);  
  return cov;
}

Mat3 Vec3List::GetPrincipalAxes() const
{
  Mat3 inertia=this->GetInertia();  
  EMat3 inertia_mat(inertia.Data());

  Eigen::JacobiSVD<EMat3> svd(inertia_mat,Eigen::ComputeThinU);
  EMat3 rot=svd.matrixU();
  Mat3 axes(rot.data());
  return axes;
}

Vec3 Vec3List::GetCenter() const
{
  Vec3 center;
  if (this->empty()) {
    return center;
  }
  for (Vec3List::const_iterator i=this->begin(),e=this->end(); i!=e; ++i) {
    center+=*i;
  }
  return center/=this->size();
}

Line3 Vec3List::GetODRLine() const
{
  Vec3 center=this->GetCenter();
  Vec3 direction=this->GetPrincipalAxes().GetRow(2);
  return Line3(center,center+direction);
}

Plane Vec3List::GetODRPlane() const
{
  Vec3 origin=this->GetCenter();
  Vec3 normal=this->GetPrincipalAxes().GetRow(0);
  return Plane(origin,normal);
}

std::pair<Line3, Real> Vec3List::FitCylinder(const Vec3& initial_direction) const
  { 
    Vec3 center=this->GetCenter();
    Line3 axis=Line3(center,center+initial_direction), axis_old;
    Real radius,res_sum_old,res_sum,delta_0=0.01,prec=0.0000001,err,norm,delta;
    unsigned long n_step=1000, n_res=this->size();
    Vec3 v,gradient_dir,gradient_center;
    
    radius=0.0;
    delta=delta_0;
    for (Vec3List::const_iterator i=this->begin(),e=this->end(); i!=e; ++i) {
      radius+=geom::Distance(axis,(*i));
    }
    radius/=Real(n_res);
    res_sum=0.0;
    for (Vec3List::const_iterator i=this->begin(),e=this->end(); i!=e; ++i) {
      Real r=Distance(axis,(*i))-radius;
      res_sum+=r*r;
    }
    unsigned long k=0;
    err=2.0*prec;
    while (err>prec && k<n_step) {
      res_sum_old=res_sum;
      axis_old=axis;
      //radius=0.0;
      if (k>50) {
        delta=delta_0*50.0*50.0/(k*k);
      }
      //for (Vec3List::const_iterator i=this->begin(),e=this->end(); i!=e; ++i) {
      //  radius+=Distance(axis,(*i));
      //}
      radius/=Real(n_res);
      for (int j=0; j!=3; ++j){
        res_sum=0.0;
        v=Vec3(0.0,0.0,0.0);
        v[j]=delta;
        axis=Line3(axis_old.GetOrigin(),axis_old.GetOrigin()+axis_old.GetDirection()+v);
        radius=0.0;
        for (Vec3List::const_iterator i=this->begin(),e=this->end(); i!=e; ++i) {
          radius+=Distance(axis,(*i));
        }
        radius/=Real(n_res);
        for (Vec3List::const_iterator i=this->begin(),e=this->end(); i!=e; ++i) {
          Real r=Distance(axis,(*i))-radius;
          res_sum+=r*r;
        }
        gradient_dir[j]=(res_sum-res_sum_old)/delta;
      }
      norm=Dot(gradient_dir,gradient_dir);
      if (norm>1.) {
        gradient_dir=Normalize(gradient_dir);
      }
      for (int j=0; j!=3; ++j){
        res_sum=0.0;
        v=Vec3(0.0,0.0,0.0);
        v[j]=delta;
        axis=Line3(axis_old.GetOrigin()+v,axis_old.GetOrigin()+axis_old.GetDirection()+v);
        radius=0.0;
        for (Vec3List::const_iterator i=this->begin(),e=this->end(); i!=e; ++i) {
          radius+=Distance(axis,(*i));
        }
        radius/=Real(n_res);
        for (Vec3List::const_iterator i=this->begin(),e=this->end(); i!=e; ++i) {
          Real r=Distance(axis,(*i))-radius;
          res_sum+=r*r;
        }
        gradient_center[j]=(res_sum-res_sum_old)/delta;
      }
      norm=Dot(gradient_center,gradient_center);
      if (norm>1.) {
        gradient_center=Normalize(gradient_center);
      }      
      axis=Line3(axis_old.GetOrigin()-50*delta*gradient_center,axis_old.GetOrigin()-50*delta*gradient_center+axis_old.GetDirection()-delta*gradient_dir);
      radius=0.0;
      for (Vec3List::const_iterator i=this->begin(),e=this->end(); i!=e; ++i) {
        radius+=Distance(axis,(*i));
      }
      radius/=Real(n_res);
      res_sum=0.0;
      for (Vec3List::const_iterator i=this->begin(),e=this->end(); i!=e; ++i) {
        Real r=Distance(axis,(*i))-radius;
        res_sum+=r*r;
      }
      err=fabs((res_sum-res_sum_old)/float(n_res));
      k++;
    }
    if (err>prec) {
      std::cout<<"axis fitting did not converge"<<std::endl;
    }
    return std::make_pair(axis,radius);
  }
}
