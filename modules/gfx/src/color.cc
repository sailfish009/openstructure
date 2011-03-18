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
/*
  Author: Ansgar Philippsen, Marco Biasini
*/

#include <ostream>

#include "color.hh"

namespace ost { namespace gfx {

namespace {
// maps hsv to rgb (0-1)
geom::Vec3 HSVtoRGB(const geom::Vec3& hsv)
{
  geom::Vec3 rgb;
  if (hsv[1]<1e-9){
    rgb[0]=hsv[2];
    rgb[1]=hsv[2];
    rgb[2]=hsv[2];
  } else {
    double var_h=hsv[0]*6.0<6.0?hsv[0]*6.0:0.0;
    int var_i =static_cast<int>(var_h);
    double var_1 = hsv[2]*(1-hsv[1]);
    double var_2 = hsv[2]*(1-hsv[1]*( var_h -var_i));
    double var_3 = hsv[2]*(1-hsv[1]*(1-(var_h-var_i)));
    switch(var_i){
    case 0:
      rgb[0]=hsv[2];
      rgb[1]=var_3;
      rgb[2]=var_1;
      break;
    case 1:
      rgb[0] = var_2;
      rgb[1] = hsv[2];
      rgb[2] = var_1;
      break;
    case 2:
      rgb[0] = var_1;
      rgb[1] = hsv[2];
      rgb[2] = var_3;
      break;
    case 3:
      rgb[0] = var_1 ; 
      rgb[1] = var_2 ; 
      rgb[2] = hsv[2];
      break;
    case 4:
      rgb[0] = var_3 ;
      rgb[1] = var_1 ;
      rgb[2] = hsv[2];
      break;
    case 5:
      rgb[0] = hsv[2];
      rgb[1] = var_1 ;
      rgb[2] = var_2;
      break;
    }
  }
  return rgb;
}

// maps rgb (0-1) to hsv
geom::Vec3 RGBtoHSV(const geom::Vec3& rgb)
{
  geom::Vec3 hsv;
  double var_R = ( rgb[0] / 255.0 );
  double var_G = ( rgb[1] / 255.0 );
  double var_B = ( rgb[2] / 255.0 );

  double var_Min = std::min(std::min( var_R, var_G), var_B );
  double var_Max = std::max(std::max( var_R, var_G), var_B );
  double del_Max = var_Max - var_Min;

  hsv[2] = var_Max;

  if ( del_Max < 1.0e-9 ){
    hsv[0] = 0.0;
    hsv[1] = 0.0;
  } else {
    hsv[1] = del_Max / var_Max;
    double del_R = ( ( ( var_Max - var_R ) / 6.0 ) + ( del_Max / 2.0 ) ) / del_Max;
    double del_G = ( ( ( var_Max - var_G ) / 6.0 ) + ( del_Max / 2.0 ) ) / del_Max;
    double del_B = ( ( ( var_Max - var_B ) / 6.0 ) + ( del_Max / 2.0 ) ) / del_Max;
    
    if ( var_R == var_Max ){
      hsv[0] = del_B - del_G;
    } else if ( var_G == var_Max ){
      hsv[0] = ( 1.0 / 3.0 ) + del_R - del_B;
    } else if ( var_B == var_Max ){
      hsv[0] = ( 2.0 / 3.0 ) + del_G - del_R;
    }
    if ( hsv[0] < 0 ){
      hsv[0] += 1;
    }
    if ( hsv[0] > 1 ){
      hsv[0] -= 1;
    }
  }
  return hsv;
}
} // anon ns

geom::Vec3 Color::ToHSV()
{
  return RGBtoHSV(geom::Vec3(rgba[0],rgba[1],rgba[2]));
}


Color HSV(double h, double s, double v)
{
  geom::Vec3 rgb=HSVtoRGB(geom::Vec3(h,s,v));
  return Color(rgb[0],rgb[1],rgb[2]);
}

std::ostream& operator<<(std::ostream& s, const Color& c)
{
  s << "{" << c.Red() << "," << c.Green() << "," << c.Blue() << "," << c.Alpha() << "}";
  return s;
}


Color& Color::operator*=(float rhs)
{
  rgba[0]*=rhs;
  rgba[1]*=rhs;
  rgba[2]*=rhs;
  rgba[3]*=rhs; 
  return *this;
}

Color& Color::operator+=(float rhs)
{
  rgba[0]+=rhs;
  rgba[1]+=rhs;
  rgba[2]+=rhs;
  rgba[3]+=rhs;
  return *this;
}
  
Color& Color::operator+=(const Color& rhs)
{
  rgba[0]+=rhs[0];
  rgba[1]+=rhs[1];
  rgba[2]+=rhs[2];
  rgba[3]+=rhs[3];  
  return *this;  
}

Color& Color::operator-=(const Color& rhs)
{
  rgba[0]-=rhs[0];
  rgba[1]-=rhs[1];
  rgba[2]-=rhs[2];
  rgba[3]-=rhs[3];  
  return *this;
}

Color& Color::operator-=(float rhs)
{
  rgba[0]-=rhs;
  rgba[1]-=rhs;
  rgba[2]-=rhs;
  rgba[3]-=rhs;
  return *this;
}

Color& Color::operator/=(float rhs)
{
  rgba[0]/=rhs;
  rgba[1]/=rhs;
  rgba[2]/=rhs;
  rgba[3]/=rhs;  
  return *this;
}

}} // ns
