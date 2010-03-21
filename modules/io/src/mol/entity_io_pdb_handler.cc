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
/*
  Author: Marco Biasini, Ansgar Philippsen
 */
#include <iostream>
#include <sstream>
#include <iomanip>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <ost/log.hh>
#include <ost/io/io_exception.hh>
#include "entity_io_pdb_handler.hh"
#include <ost/profile.hh>
#include <ost/io/mol/pdb_reader.hh>
#include <ost/io/mol/pdb_writer.hh>
namespace ost { namespace io {

using boost::format;


bool EntityIOPDBHandler::RequiresBuilder() const
{
  return true;
}


void EntityIOPDBHandler::Export(const mol::EntityView& ent,
                                const boost::filesystem::path& loc) const 
{
  PDBWriter writer(loc);
  if (boost::iequals(boost::filesystem::extension(loc), ".pqr")) {
    writer.SetFlags(PDB::PQR_FORMAT);
  }
  writer.Write(ent);
}

void EntityIOPDBHandler::Import(mol::EntityHandle& ent, 
                                std::istream& stream)
{
  PDBReader reader(stream);
  if (reader.HasNext()) {
    reader.Import(ent, "");
  }
}

void EntityIOPDBHandler::Export(const mol::EntityView& ent,
                                std::ostream& stream) const 
{
  PDBWriter writer(stream);
  writer.Write(ent);
}

namespace {

bool pdb_handler_is_responsible_for(const boost::filesystem::path& loc,
                                    const String& type) {
  if(type=="auto") {
    if(detail::FilenameEndsWith(loc.string(),".pdb") || detail::FilenameEndsWith(loc.string(),".ent") ||
       detail::FilenameEndsWith(loc.string(),".pdb.gz") || detail::FilenameEndsWith(loc.string(),".pqr") ||
       detail::FilenameEndsWith(loc.string(),".ent.gz")){
      return true;
    }

  } else if(type=="pdb") {
    return true;
  }

  return false;
}

}

void EntityIOPDBHandler::Import(mol::EntityHandle& ent,
                                const boost::filesystem::path& loc)
{
  PDBReader reader(loc);
  if (reader.HasNext()) {
    reader.Import(ent, "");
  }
}

bool EntityIOPDBHandler::ProvidesImport(const boost::filesystem::path& loc,
                                        const String& type)
{
  return pdb_handler_is_responsible_for(loc, type);
}

bool EntityIOPDBHandler::ProvidesExport(const boost::filesystem::path& loc,
                                        const String& type)
{
  return pdb_handler_is_responsible_for(loc, type);
}


}} // ns


