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
#ifndef OST_ATOM_PROP_HH
#define OST_ATOM_PROP_HH

#include <ost/mol/module_config.hh>
#include <ost/geom/mat3.hh>

namespace ost { namespace mol {



class DLLEXPORT_OST_MOL AtomProp {
public:
  AtomProp()
    : element(""), radius(0.0), occupancy(1.0),
      b_factor(0.0), charge(0.0), mass(0.0),
      is_hetatm(false), has_anisou(false),
      ordinal(0), anisou(geom::Mat3()) {

  }
  String        element;
  Real          radius;
  Real          occupancy;
  Real          b_factor;
  Real          charge;
  Real          mass;
  bool          is_hetatm;
  bool          has_anisou;
  
  int           ordinal;
  geom::Mat3    anisou;
private:
  unsigned char flags;
};

}} // ns

#endif

