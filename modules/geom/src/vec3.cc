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


namespace geom {

typedef Eigen::Matrix3f EMat3;

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

  Eigen::SVD<EMat3> svd(inertia_mat);
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

Line3 Vec3List::GetODRLine()
{
  Vec3 center=this->GetCenter();
  Vec3 direction=this->GetPrincipalAxes().GetRow(2);
  return Line3(center,center+direction);
}

Plane Vec3List::GetODRPlane()
{
  Vec3 origin=this->GetCenter();
  Vec3 normal=this->GetPrincipalAxes().GetRow(0);
  return Plane(origin,normal);
}
  
}
