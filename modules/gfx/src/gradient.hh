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
#ifndef OST_GFX_GRADIENT_HH
#define OST_GFX_GRADIENT_HH

/*
  Author: Marco Biasini
 */

#include <vector>

#include "color.hh"

#include <ost/info/info.hh>
#include <ost/info/info_fw.hh>


namespace ost { namespace gfx {

/// \brief color gradient
/// 
/// Gradients map a scalar value in the range of 0 to 1 to a color. The 
/// gradient consists of zero or more color stops. These stop points define the 
/// output of GetColorAt(). New stops can be added with SetColorAt().
/// 
/// \sa \ref gradient.py "Gradient Example"
class DLLEXPORT_OST_GFX Gradient {
public:
  struct Stop {
    Stop(): t(0.0) {}
    
    Stop(float tt, const Color& c): t(tt), color(c) {}
    float t;
    Color color;
    
    Color GetColor(){
      return color;
    }
    
    float GetRel(){
      return t;
    }
    
    bool operator==(const Stop& ref) const
    {
      return t==ref.t && color==ref.color;
    }
    
  };
  
  typedef std::vector<Stop> StopList;
  
public:
  /*!
    In python, the gradient can also be initialize with a dictionary, mapping
    stops to either float triplets or gfx.Color objects
  */
  Gradient();

  /// \brief initialize with a pre-define gradient name
  Gradient(const String& name);

  /// \brief get color
  Color GetColorAt(float t) const;
  /// \brief set color
  /// 
  /// \param t is the position where the new \ref Stop "stop" should be inserted
  /// \param color is the color of the stop
  void SetColorAt(float t, const Color& color);  
  
  /// \brief convert gradient to info
  void GradientToInfo(info::InfoGroup& group) const;

  /// \brief get list of color stops
  StopList GetStops() const;
  
  /// \brief create gradient from info
  static gfx::Gradient GradientFromInfo(const info::InfoGroup& group);
private:
  std::vector<Stop>  stops_;

};


///  \example gradient.py
///
/// Draw a set of lines and color them using a gradient
}}

#endif
