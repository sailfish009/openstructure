//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2010 by the OpenStructure authors
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
#include <ost/info/info.hh>

#include "geom_info_conversion.hh"

namespace ost { namespace info {

void Mat2ToInfo(const geom::Mat2& mat, InfoGroup& group)
{
  std::ostringstream ss;
  ss << mat(0, 0) << "\t" << mat(1,0) << "\t" 
     << mat(0, 1) << "\t" << mat(1, 1);
  group.SetTextData(ss.str());
}

geom::Mat2 Mat2FromInfo(const InfoGroup& group)
{
  std::istringstream ss(group.GetTextData());
  geom::Mat2 mat;
  ss >> mat(0, 0) >> mat(1,0) >>  mat(0, 1) >> mat(1, 1);
  return mat;
}

void Mat3ToInfo(const geom::Mat3& mat, InfoGroup& group)
{
  std::ostringstream ss;
  ss << mat(0, 0) << "\t" << mat(1, 0) << "\t" << mat(2, 0) << "\t"
     << mat(0, 1) << "\t" << mat(1, 1) << "\t" << mat(2, 1) << "\t"
     << mat(0, 2) << "\t" << mat(1, 2) << "\t" << mat(2, 2);
  group.SetTextData(ss.str());
}


geom::Mat3 Mat3FromInfo(const InfoGroup& group)
{
  std::istringstream ss(group.GetTextData());
  geom::Mat3 mat;
  ss >> mat(0, 0) >> mat(1,0) >> mat(2, 0) 
     >> mat(0, 1) >> mat(1,1) >> mat(2, 1) 
     >> mat(0, 2) >> mat(1,2) >> mat(2, 2);
  return mat;
}

void Mat4ToInfo(const geom::Mat4& mat, InfoGroup& group)
{
  std::ostringstream ss;  
  ss << mat(0, 0) << "\t" << mat(1, 0) << "\t" << mat(2, 0) << "\t" << mat(3, 0) << "\t"
     << mat(0, 1) << "\t" << mat(1, 1) << "\t" << mat(2, 1) << "\t" << mat(3, 1) << "\t"
     << mat(0, 2) << "\t" << mat(1, 2) << "\t" << mat(2, 2) << "\t" << mat(3, 2) << "\t"
     << mat(0, 3) << "\t" << mat(1, 3) << "\t" << mat(2, 3) << "\t" << mat(3, 3);
  group.SetTextData(ss.str());     
}


geom::Mat4 Mat4FromInfo(const InfoGroup& group)
{
  std::istringstream ss(group.GetTextData());
  geom::Mat4 mat;
  ss >> mat(0, 0) >> mat(1,0) >> mat(2, 0) >> mat(3, 0) 
     >> mat(0, 1) >> mat(1,1) >> mat(2, 1) >> mat(3, 1) 
     >> mat(0, 2) >> mat(1,2) >> mat(2, 2) >> mat(3, 2)
     >> mat(0, 3) >> mat(1,3) >> mat(2, 3) >> mat(3, 3);     
  return mat;
}
  
}}
