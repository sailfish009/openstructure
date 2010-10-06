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
#ifndef OST_CONOP_COMPOUND_LIB_HH
#define OST_CONOP_COMPOUND_LIB_HH

#include <map>
#include <boost/shared_ptr.hpp>

// our own local copy of sqlite3
#include <ost/db/sqlite3.h>

#include <ost/conop/module_config.hh>
#include <ost/conop/compound.hh>

namespace ost { namespace conop {

class CompoundLib;

typedef boost::shared_ptr<CompoundLib> CompoundLibPtr;
typedef std::map<String, CompoundPtr> CompoundMap;

class DLLEXPORT_OST_CONOP CompoundLib {
public:
  static CompoundLibPtr Load(const String& database, bool readonly=true);
  static CompoundLibPtr Create(const String& database);
  ~CompoundLib();
  
  CompoundPtr FindCompound(const String& id, Compound::Dialect dialect);
  void AddCompound(const CompoundPtr& compound);
  CompoundLibPtr Copy(const String& filename) const;
  void ClearCache();
private:
    CompoundLib();
    void LoadAtomsFromDB(CompoundPtr comp, int pk);
    void LoadBondsFromDB(CompoundPtr comp, int pk);    
private:
  CompoundMap  compound_cache_;
  sqlite3*     conn_;
};

}}

#endif
