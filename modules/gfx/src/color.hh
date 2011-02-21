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
#ifndef OST_COLOR_HH
#define OST_COLOR_HH

/*
  Author: Ansgar Philippsen, Marco Biasini
*/

#include <iosfwd>

#include <boost/operators.hpp>

#include <ost/gfx/module_config.hh>
#include <ost/geom/geom.hh>

namespace ost { namespace gfx {

/// \brief color with red, green, blue and alpha component
class DLLEXPORT_OST_GFX Color :   
  private boost::additive<Color>,
  private boost::additive<Color, float>,
  private boost::multiplicative<Color, float>{
public:
  Color() {
    rgba[0]=1.0;
    rgba[1]=1.0;
    rgba[2]=1.0;
    rgba[3]=1.0;
  }
  
  Color(float r, float g, float b, float a=1.0) {
    rgba[0]=r;
    rgba[1]=g;
    rgba[2]=b;
    rgba[3]=a;
  }

  float& Red() {return rgba[0];}
  const float& Red() const {return rgba[0];}
  float& Green() {return rgba[1];}
  const float& Green() const {return rgba[1];}
  float& Blue() {return rgba[2];}
  const float& Blue() const {return rgba[2];}
  float& Alpha() {return rgba[3];}
  const float& Alpha() const {return rgba[3];}

  static Color FromRGB(unsigned char r, unsigned char g, 
                       unsigned char b, unsigned char a = 0xff) {
    static float f=1.0/255.0;
    return Color(f*static_cast<float>(r),f*static_cast<float>(g),
                 f*static_cast<float>(b),f*static_cast<float>(a));
  }

  geom::Vec3 ToHSV();

  // these also take care of operator[](uint i) !
  operator float* () {return rgba;}
  operator const float* () const {return rgba;}
  
  Color& operator*=(float rhs);
  Color& operator+=(float rhs);
    
  Color& operator+=(const Color& rhs);  
  Color& operator-=(const Color& rhs);    
  Color& operator-=(float rhs);
  Color& operator/=(float rhs);  
private:
  float rgba[4];
};

Color DLLEXPORT_OST_GFX HSV(double h, double s, double v);

DLLEXPORT_OST_GFX std::ostream& operator<<(std::ostream&, const Color& c);

}}

#endif

