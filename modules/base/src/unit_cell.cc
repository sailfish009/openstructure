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
#include "unit_cell.hh"

namespace ost {

UnitCell::UnitCell(Real a, Real b, Real c, Real alpha, Real beta, Real gamma, 
                   const SymmetryPtr& sym)
{
  Real cos_alpha=cos(alpha);
  Real cos_beta=cos(beta);  
  Real sin_beta=sin(beta);

  Real cos_gamma=cos(gamma);
  Real sin_gamma=sin(gamma);
  Real cos_alpha_s=(cos_gamma*cos_beta-cos_alpha)/(sin_beta*sin_gamma);
  Real sin_alpha_s=sqrt(1.0-cos_alpha_s*cos_alpha_s);
  geom::Vec3 a_dir(1, 0, 0);
  geom::Vec3 b_dir(cos_gamma, sin_gamma, 0.0);
  geom::Vec3 c_dir(cos_beta, -sin_beta*cos_alpha_s, sin_beta*sin_alpha_s);
  geom::Vec3 a_vec=a_dir*a;
  geom::Vec3 b_vec=b_dir*b;
  geom::Vec3 c_vec=c_dir*c;
  dirs_=geom::Mat3(a_vec.x, b_vec.x, c_vec.x,
                   a_vec.y, b_vec.y, c_vec.y,
                   a_vec.z, b_vec.z, c_vec.z);
  frac_=geom::Invert(dirs_);
  sym_=sym;
}

}
