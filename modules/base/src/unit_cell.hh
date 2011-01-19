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
#ifndef OST_UNIT_CELL_HH
#define OST_UNIT_CELL_HH

#include <ost/module_config.hh>
#include <ost/geom/geom.hh>
#include <ost/symmetry.hh>

namespace ost {
  
class DLLEXPORT UnitCell {
public:
  UnitCell() {}
  UnitCell(const geom::Vec3& vec_a, 
           const geom::Vec3& vec_b, const geom::Vec3& vec_c, 
           const SymmetryPtr& sym=SymmetryPtr()):
    dirs_(vec_a.x, vec_b.x, vec_c.x, 
          vec_a.y, vec_b.y, vec_c.y, 
          vec_a.z, vec_b.z, vec_c.z),
    frac_(geom::Invert(dirs_)),
    sym_(sym)
  {
  }
  
  UnitCell(Real a, Real b, Real c, Real alpha, Real beta, Real gamma, 
           const SymmetryPtr& sym=SymmetryPtr());


  Real GetGamma() const { return geom::Angle(this->GetA(), this->GetB()); }
  
  Real GetBeta() const { return geom::Angle(this->GetA(), this->GetC()); }
  
  Real GetAlpha() const { return geom::Angle(this->GetB(), this->GetC()); }
  
  /// \brief the transform to go from fractional to orthogonal coordinates
  const geom::Mat3& GetOrthoTF() const { return dirs_; }
  
  /// \brief the transform to go from orthogonal to fractional coordinates
  const geom::Mat3& GetFractionalTF() const { return frac_; }
  
  geom::Vec3 GetA() const { return dirs_.GetCol(0); }
  geom::Vec3 GetB() const { return dirs_.GetCol(1); }
  geom::Vec3 GetC() const { return dirs_.GetCol(2); }
  
  const SymmetryPtr& GetSymmetry() const { return sym_; }
private:
  geom::Mat3  dirs_;
  geom::Mat3  frac_;
  SymmetryPtr sym_;
};

}

#endif
