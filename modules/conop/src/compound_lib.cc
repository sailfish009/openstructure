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

/*
  Author: Marco Biasini
 */

#include <iostream>
#include <sstream>
#include <boost/format.hpp>
#include <ost/log.hh>
#include "compound_lib.hh"


using boost::format;

namespace ost { namespace conop {

namespace {

const char* CREATE_CMD[]={                                                         
"CREATE TABLE IF NOT EXISTS chem_compounds (                                    "
"  id              INTEGER PRIMARY KEY AUTOINCREMENT,                           "
"  tlc             VARCHAR(3) NOT NULL,                                         "
"  olc             VARCHAR(1) NOT NULL,                                         "
"  dialect         VARCHAR(1) NOT NULL,                                         "
"  chem_class      VARCHAR(1),                                                  "
"  formula         VARCHAR(64) NOT NULL,                                        "
"  pdb_initial     TIMESTAMP,                                                   "
"  pdb_modified    TIMESTAMP                                                    "
");",
" CREATE UNIQUE INDEX IF NOT EXISTS commpound_tlc_index ON chem_compounds       "
"                                  (tlc, dialect)",
"CREATE TABLE IF NOT EXISTS atoms (                                             "
" id              INTEGER PRIMARY KEY AUTOINCREMENT,                            "
" compound_id     INTEGER REFERENCES chem_compounds (id) ON DELETE CASCADE,     "
" name            VARCHAR(4) NOT NULL,                                          "
" alt_name        VARCHAR(4) NOT NULL,                                          "
" element         VARCHAR(2) NOT NULL,                                          "
" is_aromatic     VARCHAR(1) NOT NULL,                                          "
" stereo_conf     VARCHAR(1) NOT NULL,                                          "
" is_leaving      VARCHAR(1) NOT NULL,                                          "
" ordinal         INT                                                           "
");",
" CREATE INDEX IF NOT EXISTS atom_name_index ON atoms                           "
"                                  (compound_id, name, alt_name)",
" CREATE TABLE IF NOT EXISTS bonds  (                                           "
"   id              INTEGER PRIMARY KEY AUTOINCREMENT,                          "
"   compound_id     INTEGER REFERENCES chem_compounds (id) ON DELETE CASCADE,   "
"   atom_one        INTEGER REFERENCES atoms (id) ON DELETE CASCADE,            "
"   atom_two        INTEGER REFERENCES atoms (id) ON DELETE CASCADE,            "
"   bond_order      INT,                                                        "
"   stereo_conf     VARCHAR(1) NOT NULL                                         "
" );",
" CREATE INDEX IF NOT EXISTS bond_index ON bonds (compound_id)",
" CREATE TRIGGER delete_related_objects                                         "
" BEFORE DELETE ON chem_compounds                                               "
" FOR EACH ROW BEGIN                                                            "
"     DELETE FROM bonds WHERE compound_id=OLD.id;                               "
"     DELETE FROM atoms WHERE compound_id=OLD.id;                               "
" END",
" DROP TRIGGER IF EXISTS delete_related_objects", NULL};    


const char* INSERT_COMPOUND_STATEMENT="INSERT INTO chem_compounds               "
"        (tlc, olc, dialect, chem_class, formula, pdb_initial, pdb_modified)    "
" VALUES (?, ?, ?, ?, ?, DATE(?), DATE(?))";

const char* INSERT_ATOM_STATEMENT="INSERT INTO atoms                            "
"        (compound_id, name, alt_name, element, is_aromatic, stereo_conf,       "
"         is_leaving, ordinal)                                                  "
" VALUES (?, ?, ?, ?, ?, ?, ?, ?)";

const char* INSERT_BOND_STATEMENT="insert into bonds                            "
"        (compound_id, atom_one, atom_two, bond_order, stereo_conf)             "
" VALUES (?, ?, ?, ?, ?)";



}


void CompoundLib::AddCompound(const CompoundPtr& compound)
{
  sqlite3_stmt* stmt=NULL;  
  int retval=sqlite3_prepare_v2(conn_, INSERT_COMPOUND_STATEMENT, 
                                strlen(INSERT_COMPOUND_STATEMENT), &stmt, NULL);
  if (SQLITE_OK==retval) {
    sqlite3_bind_text(stmt, 1, compound->GetID().c_str(), 
                      compound->GetID().length(), NULL);
    char olc=compound->GetOneLetterCode();
    sqlite3_bind_text(stmt, 2, &olc, 1, NULL);
    char chem_type=compound->GetChemClass();
    char dialect=compound->GetDialect();
    sqlite3_bind_text(stmt, 3, &dialect, 1, NULL);
    sqlite3_bind_text(stmt, 4, &chem_type, 1, NULL);
    sqlite3_bind_text(stmt, 5, compound->GetFormula().c_str(), 
                      compound->GetFormula().length(), NULL);
    std::stringstream ss;
    ss << compound->GetCreationDate().year << "-" 
       << compound->GetCreationDate().month << "-" 
       << compound->GetCreationDate().day;
    String date=ss.str();
    ss.str("");
    ss << compound->GetModificationDate().year << "-" 
       << compound->GetModificationDate().month << "-" 
       << compound->GetModificationDate().day;
    sqlite3_bind_text(stmt, 6, date.c_str(), date.length(), NULL);
    date=ss.str();
    sqlite3_bind_text(stmt, 7, date.c_str(), date.length(), NULL);        
  } else {
    LOG_ERROR(sqlite3_errmsg(conn_));
    sqlite3_finalize(stmt);    
    return;
  }
  retval=sqlite3_step(stmt);
  if (SQLITE_DONE!=retval) {
    if (sqlite3_errcode(conn_)==SQLITE_CONSTRAINT) {
      LOG_ERROR("Compound '" << compound->GetID() << "' already exists for the "
                << compound->GetDialectAsString() << " dialect.");
    } else {
      LOG_ERROR(sqlite3_errmsg(conn_));
    }
  }
  sqlite3_finalize(stmt);  
  sqlite3_int64 compound_id=sqlite3_last_insert_rowid(conn_);
  // insert atoms
  const AtomSpecList& al=compound->GetAtomSpecs();
  std::vector<sqlite3_int64> atom_ids(al.size(), 0);
  for (AtomSpecList::const_iterator i=al.begin(), e=al.end(); i!=e; ++i) {
    const AtomSpec& a=*i;
    retval=sqlite3_prepare_v2(conn_, INSERT_ATOM_STATEMENT, 
                              strlen(INSERT_ATOM_STATEMENT), &stmt, NULL);
    if (SQLITE_OK==retval) {
      sqlite3_bind_int64(stmt, 1, compound_id);
      sqlite3_bind_text(stmt, 2, a.name.c_str(), a.name.length(), NULL);
      sqlite3_bind_text(stmt, 3, a.alt_name.c_str(), 
                        a.alt_name.length(), NULL);
      sqlite3_bind_text(stmt, 4, a.element.c_str(), a.element.length(), NULL);                        
      sqlite3_bind_int(stmt, 5, a.is_aromatic);
      sqlite3_bind_int(stmt, 6, 0);                  
      sqlite3_bind_int(stmt, 7, a.is_leaving);
      sqlite3_bind_int(stmt, 8, i-al.begin());      
      retval=sqlite3_step(stmt);
      assert(retval==SQLITE_DONE);
      atom_ids[i-al.begin()]=sqlite3_last_insert_rowid(conn_);
    } else {
      LOG_ERROR(sqlite3_errmsg(conn_));
    }
    sqlite3_finalize(stmt);
  }
  const BondSpecList& bl=compound->GetBondSpecs();
  for (BondSpecList::const_iterator i=bl.begin(), e=bl.end(); i!=e; ++i) {
    const BondSpec& b=*i;
    retval=sqlite3_prepare_v2(conn_, INSERT_BOND_STATEMENT, 
                              strlen(INSERT_BOND_STATEMENT), &stmt, NULL);    
    if (SQLITE_OK==retval) {
      sqlite3_bind_int64(stmt, 1, compound_id);
      sqlite3_bind_int64(stmt, 2, atom_ids[b.atom_one]);
      sqlite3_bind_int64(stmt, 3, atom_ids[b.atom_two]);      
      sqlite3_bind_int(stmt, 4, b.order);
      sqlite3_bind_int(stmt, 5, 0);
      retval=sqlite3_step(stmt);
      assert(retval==SQLITE_DONE);
    } else {
      LOG_ERROR(sqlite3_errmsg(conn_));
    }
    sqlite3_finalize(stmt);    
  }
}

CompoundLibPtr CompoundLib::Copy(const String& filename) const
{
  CompoundLibPtr clone=CompoundLibPtr(new CompoundLib);
  int retval=sqlite3_open(filename.c_str(), &clone->conn_);
  if (SQLITE_OK==retval) {
    sqlite3_backup* backup;

    backup=sqlite3_backup_init(clone->conn_, "main", conn_, "main");
    if (backup){
      sqlite3_backup_step(backup, -1);
      sqlite3_backup_finish(backup);
    }
    int rc=sqlite3_errcode(clone->conn_);
    if (rc!=SQLITE_OK) {
      LOG_ERROR(sqlite3_errmsg(clone->conn_));
      return CompoundLibPtr();
    }
    return clone;      
  }
  LOG_ERROR(sqlite3_errmsg(clone->conn_));
  return CompoundLibPtr();
}



CompoundLibPtr CompoundLib::Create(const String& database)
{
  CompoundLibPtr lib(new CompoundLib);
  int retval=sqlite3_open(database.c_str(), &lib->conn_);
  if (SQLITE_OK==retval) {
    const char** cmd=CREATE_CMD;
    while (*cmd) {
      sqlite3_stmt* stmt;
      retval=sqlite3_prepare_v2(lib->conn_, *cmd, strlen(*cmd), &stmt, NULL);
      if (SQLITE_OK==retval) {
        retval=sqlite3_step(stmt);
        sqlite3_finalize(stmt);        
        assert(SQLITE_DONE==retval);
      } else {
        LOG_ERROR(sqlite3_errmsg(lib->conn_));
        sqlite3_finalize(stmt);
        return CompoundLibPtr();
      }      
      ++cmd;
    }
    return lib;
  }
  LOG_ERROR(sqlite3_errmsg(lib->conn_));
  return CompoundLibPtr();  
}


CompoundLibPtr CompoundLib::Load(const String& database, bool readonly) 
{
  int flags=readonly ? SQLITE_OPEN_READONLY : SQLITE_OPEN_READWRITE;
  CompoundLibPtr lib(new CompoundLib);
  int retval=sqlite3_open_v2(database.c_str(), &lib->conn_, flags, NULL);
  if (SQLITE_OK==retval) {
    return lib;
  }
  LOG_ERROR(sqlite3_errmsg(lib->conn_));
  return CompoundLibPtr();
}

void CompoundLib::LoadAtomsFromDB(CompoundPtr comp, int pk) {
  String aq=str(format("SELECT name, alt_name, element, ordinal, is_leaving "
                       "FROM atoms WHERE compound_id=%d "
                       "ORDER BY ordinal ASC") % pk);  
  sqlite3_stmt* stmt;
  int retval=sqlite3_prepare_v2(conn_, aq.c_str(), 
                                static_cast<int>(aq.length()),
                                &stmt, NULL);
  if (SQLITE_OK==retval) {
      int ret=0;
      while (SQLITE_ROW==(ret=sqlite3_step(stmt))) {
        AtomSpec atom_sp;
        atom_sp.name=String(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        atom_sp.alt_name=String(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));        
        atom_sp.element=String(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))); 
        atom_sp.ordinal=sqlite3_column_int(stmt, 3);  
        atom_sp.is_leaving=bool(sqlite3_column_int(stmt, 4)!=0);
        comp->AddAtom(atom_sp);
      }
  } else {
    LOG_ERROR(sqlite3_errmsg(conn_));
  }
  sqlite3_finalize(stmt);
}
void CompoundLib::ClearCache()
{
  compound_cache_.clear();
}

void CompoundLib::LoadBondsFromDB(CompoundPtr comp, int pk) {
  sqlite3_stmt* stmt;
  String aq=str(format("SELECT a1.ordinal, a2.ordinal, b.bond_order FROM bonds AS b "
                       "LEFT JOIN atoms AS a1 ON b.atom_one=a1.id "
                       "LEFT JOIN atoms as a2 ON b.atom_two=a2.id "
                       "WHERE b.compound_id=%d") % pk);
  int retval=sqlite3_prepare_v2(conn_, aq.c_str(), 
                                static_cast<int>(aq.length()),
                                &stmt, NULL);
  if (SQLITE_OK==retval) {
      int ret=0;
      while (SQLITE_ROW==(ret=sqlite3_step(stmt))) {
        BondSpec bond_sp;
        bond_sp.atom_one=sqlite3_column_int(stmt, 0);
        bond_sp.atom_two=sqlite3_column_int(stmt, 1);
        bond_sp.order=sqlite3_column_int(stmt, 2);
        comp->AddBond(bond_sp);
      }
  } else {
    LOG_ERROR(sqlite3_errmsg(conn_));
  } 
  sqlite3_finalize(stmt);  
}

CompoundPtr CompoundLib::FindCompound(const String& id, 
                                      Compound::Dialect dialect) {
  CompoundMap::iterator i=compound_cache_.find(id);
  if (i!=compound_cache_.end()) {
    return i->second;
  }
  String query="SELECT id, tlc, olc, chem_class, dialect FROM chem_compounds"
               " WHERE tlc='"+id+"' AND dialect='"+String(1, char(dialect))+"'";
  sqlite3_stmt* stmt;
  int retval=sqlite3_prepare_v2(conn_, query.c_str(), 
                                static_cast<int>(query.length()),
                                &stmt, NULL);
  if (SQLITE_OK==retval) {
    int ret=sqlite3_step(stmt);
    if (SQLITE_DONE==ret) {
      sqlite3_finalize(stmt);        
      return CompoundPtr();
    }
    if (SQLITE_ROW==ret) {
      int pk=sqlite3_column_int(stmt, 0);
      const char* id=reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
      CompoundPtr compound(new Compound(id));
      compound->SetOneLetterCode((sqlite3_column_text(stmt, 2))[0]);
      compound->SetChemClass(mol::ChemClass(sqlite3_column_text(stmt, 3)[0]));
      compound->SetDialect(Compound::Dialect(sqlite3_column_text(stmt, 4)[0]));
      // Load atoms and bonds      
      this->LoadAtomsFromDB(compound, pk);
      this->LoadBondsFromDB(compound, pk);
      compound_cache_.insert(std::make_pair(compound->GetID(), compound));
      sqlite3_finalize(stmt);
      return compound;   
    }
    assert(SQLITE_DONE==sqlite3_step(stmt));
  } else {
    LOG_ERROR("ERROR: " << sqlite3_errmsg(conn_));
    sqlite3_finalize(stmt);      
    return CompoundPtr();
  }
  sqlite3_finalize(stmt);    
  return CompoundPtr();
}

CompoundLib::CompoundLib() 
  : conn_(NULL) {
}

CompoundLib::~CompoundLib() {
  if (conn_) {
    sqlite3_close(conn_);
  }
}
}}
