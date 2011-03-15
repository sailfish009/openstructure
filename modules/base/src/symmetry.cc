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

#include <ctype.h>

#include <iostream>
#include <fstream>

#include "string_ref.hh"
#include "log.hh"
#include "platform.hh"
#include "symmetry.hh"



namespace ost {


std::map<String, SymmetryPtr>* Symmetry::hm_map_=NULL;
std::map<int, SymmetryPtr>* Symmetry::ccp4_map_=NULL;

Symmetry::Symmetry()
{
}

SymmetryPtr Symmetry::FromHermannMauguinSymbol(const String& symbol)
{
  if (!Symmetry::LoadSymmetryInfo()) {
    return SymmetryPtr();
  }
  std::map<String, SymmetryPtr>::iterator i=hm_map_->find(symbol);
  if (i!=hm_map_->end()) {
    return i->second;
  }
  return SymmetryPtr();
}

SymmetryPtr Symmetry::FromCCP4Symbol(int symbol)
{
  if (!Symmetry::LoadSymmetryInfo()) {
    return SymmetryPtr();
  }
  std::map<int, SymmetryPtr>::iterator i=ccp4_map_->find(symbol);
  if (i!=ccp4_map_->end()) {
    return i->second;
  }
  return SymmetryPtr();
}

bool Symmetry::LoadSymmetryInfo()
{
  if (Symmetry::hm_map_ && Symmetry::ccp4_map_) {
    return true;
  }
  /*String shared_data_path=GetSharedDataPath();
  assert(shared_data_path.size()!=0);
  String symop_filename=shared_data_path+"/symop.lib";*/
  std::stringstream instream(SYMOP_DATA);
  
  /*if (!instream) {
    LOG_ERROR("Can't load symmetry operation information: '" 
              << symop_filename << "' does not exist.");
    return false;
  }*/
  uint line_num=1;
  String curr_line;
  bool not_done=std::getline(instream, curr_line);
  
  hm_map_=new std::map<String, SymmetryPtr>;
  ccp4_map_=new std::map<int, SymmetryPtr>;
  std::vector<StringRef> tokens;
  while (not_done) {
    StringRef sref=StringRef(curr_line.c_str(), curr_line.size()).rtrim();
    if (sref.empty()) {
      continue;
    }
    assert(!isspace(sref[0]));
    SymmetryPtr curr_sym(new Symmetry);
    if (!sref.tokenize(tokens, true)) {
      return false;
    }
    
    curr_sym->type_=Symmetry::TypeFromString(tokens[5]);
    curr_sym->hm_symbol_=tokens[6].str();
    std::pair<bool, int> ibp= tokens[0].trim().to_int();
    curr_sym->ccp4_symbol_=ibp.second;
    while ((not_done=std::getline(instream, curr_line)) && ++line_num) {
      sref=StringRef(curr_line.c_str(), curr_line.size()).rtrim();
      if (!isspace(sref[0])) {
        break;
      }
      try {
        curr_sym->symops_.push_back(Symop::FromString(sref.ltrim().str()));
      } catch (SymopParseError& error) {
        LOG_ERROR("Error on line " << line_num << ": " << error.what());
      }
    }
    hm_map_->insert(std::make_pair(curr_sym->hm_symbol_, curr_sym));
    if (curr_sym->ccp4_symbol_==0) {
      continue;
    }
    ccp4_map_->insert(std::make_pair(curr_sym->ccp4_symbol_, curr_sym));
  }
  return true;
}


SymmetryType Symmetry::TypeFromString(const StringRef& symmetry_type)
{
  if (symmetry_type==StringRef("TETRAGONAL", 10)) return SYM_TETRAGONAL;
  if (symmetry_type==StringRef("CUBIC", 5)) return SYM_CUBIC;
  if (symmetry_type==StringRef("MONOCLINIC", 10)) return SYM_MONOCLINIC;
  if (symmetry_type==StringRef("ORTHORHOMBIC", 12)) return SYM_ORTHORHOMBIC;
  if (symmetry_type==StringRef("TRICLINIC", 9)) return SYM_TRICLINIC;
  if (symmetry_type==StringRef("HEXAGONAL", 9)) return SYM_HEXAGONAL;
  if (symmetry_type==StringRef("TRIGONAL", 8)) return SYM_TRIGONAL;
  LOG_WARNING("unknown symmetry type " << symmetry_type << ".");
  return SYM_UNKNOWN;
}

}

