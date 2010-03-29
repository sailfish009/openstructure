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
#ifndef GEOM_ALIGNED_CUBOID_HH
#define GEOM_ALIGNED_CUBOID_HH

/*
  Author: Marco Biasini
 */

#include "vec3.hh"
#include "module_config.hh"

namespace geom {


/// \brief axis-aligned cuboid
/// 
/// For an arbitrarily oriented cuboid see \ref Cuboid
class DLLEXPORT_OST_GEOM AlignedCuboid {
public:
  AlignedCuboid(const Vec3& mmin, const Vec3& mmax);
  
  Vec3 GetSize() const;
  
  const Vec3& GetMin() const;
  
  const Vec3& GetMax() const;
  
  Vec3 GetCenter() const;
private:
  Vec3 min_;
  Vec3 max_;
};

AlignedCuboid DLLEXPORT_OST_GEOM Union(const AlignedCuboid& lhs, const AlignedCuboid& rhs);

}


#endif
