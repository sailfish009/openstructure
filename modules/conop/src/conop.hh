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
#ifndef OST_CONOP_HH
#define OST_CONOP_HH

#include <map>
#include <ost/conop/module_config.hh>
#include <ost/mol/entity_handle.hh>
#include "builder_fw.hh"

namespace ost {

namespace conop {

class DLLEXPORT_OST_CONOP Conopology {
  typedef std::map<String,BuilderP> BuilderMap;

public:
  // singleton
  static Conopology& Instance();

  // retrieve a builder by name
  BuilderP GetBuilder(const String& name="DEFAULT");

  // return default vdW radius
  // temporary location until more elaborate chemical information is impl
  Real GetDefaultAtomRadius(const String& element) const;

  // return default mass
  Real GetDefaultAtomMass(const String& element) const;

  /*
    convenience function, connect all atoms with given coordinates,
    such as after coordinate file import, based on a given builder

    does this need to live within Conopology ?
  */
  void ConnectAll(const BuilderP& b, mol::EntityHandle eh, int flag=0);

  void RegisterBuilder(const BuilderP& b, const String& name);
  void SetDefaultBuilder(const String& default_name);
private:
  Conopology();
  Conopology(const Conopology&) {}
  Conopology& operator=(const Conopology&) {return *this;}

  BuilderMap builder_map_;
  std::map<String,Real> ele_rad_map_;
  std::map<String,Real> ele_mass_map_;
};

}} //

#endif
