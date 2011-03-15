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
#ifndef OST_SYMMETRY_HH
#define OST_SYMMETRY_HH

#include <map>
#include <boost/shared_ptr.hpp>

#include <ost/module_config.hh>
#include <ost/symop.hh>

namespace ost {

class Symmetry;
class StringRef;

typedef boost::shared_ptr<Symmetry> SymmetryPtr;


typedef enum {
  SYM_ORTHORHOMBIC,
  SYM_MONOCLINIC,
  SYM_CUBIC,
  SYM_TETRAGONAL,
  SYM_TRICLINIC,
  SYM_HEXAGONAL,
  SYM_TRIGONAL,
  SYM_UNKNOWN,
} SymmetryType;


class Symmetry {
public:  
  static String SYMOP_DATA;
  
  static SymmetryPtr FromHermannMauguinSymbol(const String& symbol);
  static SymmetryPtr FromCCP4Symbol(int spacegroup);
  
  const SymopList& GetSymops() const { return symops_; }
  
  SymmetryType GetType() const { return type_; }
  const String& GetHermannMauguinSymbol() const { return hm_symbol_; }
  
  int GetCCP4Symbol() const { return ccp4_symbol_; }
private:
  Symmetry();
  
  static SymmetryType TypeFromString(const StringRef& symmetry_type);
  static bool LoadSymmetryInfo();
  static std::map<String, SymmetryPtr>* hm_map_;
  static std::map<int, SymmetryPtr>* ccp4_map_;
  SymopList symops_;
  String        hm_symbol_;
  int           ccp4_symbol_;
  SymmetryType  type_;
};

}

#endif
