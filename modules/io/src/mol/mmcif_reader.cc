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

//#include <boost/iostreams/filter/gzip.hpp>
//#include <boost/filesystem/convenience.hpp>
//#include <boost/algorithm/string.hpp>
//#include <boost/algorithm/string/trim.hpp>

//#include <boost/format.hpp>
//#include <boost/lexical_cast.hpp>

#include <ost/profile.hh>
#include <ost/log.hh>
#include <ost/mol/xcs_editor.hh>
#include <ost/io/mol/mmcif_reader.hh>

namespace ost { namespace io {

MMCifParser::MMCifParser(std::istream& stream, mol::EntityHandle& ent_handle,
                         const IOProfile& profile):
  StarParser(stream, true), profile_(profile), ent_handle_(ent_handle)
{
  this->Init();
}

MMCifParser::MMCifParser(const String& filename, mol::EntityHandle& ent_handle,
                         const IOProfile& profile):
  StarParser(filename, true), profile_(profile), ent_handle_(ent_handle)
{
  this->Init();
}

void MMCifParser::Init()
{
  warned_name_mismatch_ = false;
  category_             = DONT_KNOW;
  chain_count_          = 0;
  atom_count_           = 0;
  residue_count_        = 0;
  auth_chain_id_        = false;
  go_on_                = true;
  //memset(indices_, -1, MAX_ITEMS_IN_ROW * sizeof(int));
  restrict_chains_      = "";
  //curr_chain_           = mol::ChainHandle();
  //curr_residue_         = mol::ResidueHandle();
}

void MMCifParser::ClearState()
{
  curr_chain_           = mol::ChainHandle();
  curr_residue_         = mol::ResidueHandle();
  chain_count_          = 0;
  residue_count_        = 0;
  atom_count_           = 0;
  category_             = DONT_KNOW;
  warned_name_mismatch_ = false;
}

void MMCifParser::SetRestrictChains(const String& restrict_chains)
{
  restrict_chains_ = restrict_chains;
}

bool MMCifParser::IsValidPDBIdent(const StringRef& pdbid)
{
  if (pdbid.length() == PDBID_LEN && isdigit(pdbid[0])) {
    return true;
  }
  return false;
}

bool MMCifParser::OnBeginData(const StringRef& data_name) 
{
  LOG_DEBUG("MCIFFReader: " << profile_);
  Profile profile_import("MMCifReader::OnBeginData");

  // check for PDB id
  if (!this->IsValidPDBIdent(data_name)) {
    throw IOException(this->FormatDiagnostic(STAR_DIAG_ERROR,
                         "No valid PDB id found for data block, read instead \'"
                                             + data_name.str() + "\'",
                                             this->GetCurrentLinenum()));
  }

  this->ClearState();

  go_on_ = true;

  return true;
}

bool MMCifParser::OnBeginLoop(const StarLoopDesc& header)
{
  // set whole array to -1
  memset(indices_, -1, MAX_ITEMS_IN_ROW * sizeof(int));
  // walk through possible categories
  if (header.GetCategory() == "atom_site") {
    category_ = ATOM_SITE;
    // mandatory items
    this->TryStoreIdx(AUTH_ASYM_ID,    "auth_asym_id",    header);
    this->TryStoreIdx(ID,              "id",              header);
    this->TryStoreIdx(LABEL_ALT_ID,    "label_alt_id",    header);
    this->TryStoreIdx(LABEL_ASYM_ID,   "label_asym_id",   header);
    this->TryStoreIdx(LABEL_ATOM_ID,   "label_atom_id",   header);
    this->TryStoreIdx(LABEL_COMP_ID,   "label_comp_id",   header);
    this->TryStoreIdx(LABEL_ENTITY_ID, "label_entity_id", header);
    this->TryStoreIdx(LABEL_SEQ_ID,    "label_seq_id",    header);
    this->TryStoreIdx(TYPE_SYMBOL,     "type_symbol",     header);
    // assumed mandatory
    this->TryStoreIdx(CARTN_X, "Cartn_x", header);
    this->TryStoreIdx(CARTN_Y, "Cartn_y", header);
    this->TryStoreIdx(CARTN_Z, "Cartn_z", header);
    // optional
    indices_[OCCUPANCY] = header.GetIndex("occupancy");
    indices_[B_ISO_OR_EQUIV] = header.GetIndex("B_iso_or_equiv");
    indices_[GROUP_PDB] = header.GetIndex("group_PDB");
    return true;
  } /*else if (header.GetCategory()=="entity_poly") {
  } else if (header.GetCategory()=="pdbx_poly_seq_scheme") {
  } else if (header.GetCategory()=="pdbx_struct_assembly") {
  } else if (header.GetCategory()=="struct_conf") {
  }*/
  return false;
}

bool MMCifParser::EnsureEnoughColumns(const std::vector<StringRef>& columns,
                                      size_t size)
{
  if (columns.size() < size) {
    if (profile_.fault_tolerant) {
      return false;
    }
    throw IOException(this->FormatDiagnostic(STAR_DIAG_ERROR,
                                             "Not enough data fields",
                                             this->GetCurrentLinenum()));
  }  
  return true;
}

mol::ResNum to_res_num(int num, char ins_code)
{
  return mol::ResNum(num, ins_code==' ' ? '\0' : ins_code);
}

bool MMCifParser::ParseAtomIdent(const std::vector<StringRef>& columns,
                                 String& chain_name,
                                 StringRef& res_name,
                                 mol::ResNum& resnum,
                                 StringRef& atom_name,
                                 char& alt_loc)
{
  // ATOM name
  atom_name = columns[indices_[LABEL_ATOM_ID]];
  if (profile_.calpha_only) { // unit test profile
    if (atom_name != StringRef("CA", 2)) {
      return false;
    }
  }
  // CHAIN ID
  if (auth_chain_id_) { // unit test different IDs
    chain_name = columns[indices_[AUTH_ASYM_ID]].str();
  } else {
    chain_name = columns[indices_[LABEL_ASYM_ID]].str();
  }
  if (restrict_chains_.size() > 0 && // unit test
      restrict_chains_.find(chain_name) == String::npos) { // unit test
    return false;
  } 

  std::pair<bool, int> a_num = this->TryGetInt(columns[indices_[ID]],
                                               "_atom_site.id",
                                          profile_.fault_tolerant); // unit test

  alt_loc = columns[indices_[LABEL_ALT_ID]][0];
  res_name = columns[indices_[LABEL_COMP_ID]];
  std::pair<bool, int> res_num =this->TryGetInt(columns[indices_[LABEL_SEQ_ID]],
                                               "_atom_site.label_seq_id",
                                          profile_.fault_tolerant); // unit test
  if (!res_num.first) { // unit test
    if (profile_.fault_tolerant) {
      return false;
    }
  }

  resnum=to_res_num(res_num.second, ' ');

  return true;
}

void MMCifParser::ParseAndAddAtom(const std::vector<StringRef>& columns)
{
  if (!this->EnsureEnoughColumns(columns, 12)) {
    return;
  }
  mol::XCSEditor editor=ent_handle_.EditXCS(mol::BUFFERED_EDIT); // potbl
  char alt_loc=0;
  String chain_name;
  StringRef res_name, atom_name;
  mol::ResNum res_num(0);
  if (!this->ParseAtomIdent(columns,
                            chain_name,
                            res_name,
                            res_num,
                            atom_name,
                            alt_loc)) {
    return;                            
  }
  Real occ = 1.00f, temp = 0;
  geom::Vec3 apos;
  
  for (int i = CARTN_X; i <= CARTN_Z; ++i) {
    std::pair<bool, float> result = this->TryGetFloat(columns[indices_[i]],
                                                      "atom_site.cartn_[xyz]",
                                                      profile_.fault_tolerant);
    if (!result.first) { // unit test
      if (profile_.fault_tolerant) { // unit test
        return;
      }
    }
    apos[i - CARTN_X] = result.second;
  }

  if (indices_[OCCUPANCY] != -1) { // unit test
    occ = this->TryGetReal(columns[indices_[OCCUPANCY]], "atom_site.occupancy");
  }
  if (indices_[B_ISO_OR_EQUIV] != -1) { // unit test
    temp = this->TryGetReal(columns[indices_[B_ISO_OR_EQUIV]],
                            "atom_site.B_iso_or_equiv");
  }

  // determine element
  String s_ele(columns[indices_[TYPE_SYMBOL]].str());

  String aname(atom_name.str());  
  // some postprocessing
  LOG_TRACE( "s_chain: [" << chain_name << "]" );

  // determine chain and residue update
  bool update_chain=false;
  bool update_residue=false;
  if(!curr_chain_) { // unit test
      update_chain=true;
      update_residue=true;
  } else if(curr_chain_.GetName() != chain_name) { // unit test
    update_chain=true;
    update_residue=true;
  }

  if(!curr_residue_) { // unit test
    update_residue=true;
  } else if(curr_residue_.GetNumber() != res_num) { // unit test
    update_residue=true;
  }

  if(update_chain) { // unit test
    curr_chain_ = ent_handle_.FindChain(chain_name);
    if(!curr_chain_.IsValid()) { // unit test
      LOG_DEBUG("new chain " << chain_name);
      curr_chain_=editor.InsertChain(chain_name);
      ++chain_count_;
    }
    assert(curr_chain_.IsValid());
  }

  if(update_residue) { // unit test
    curr_residue_=mol::ResidueHandle();
    if (profile_.join_spread_atom_records) { // unit test
      curr_residue_=curr_chain_.FindResidue(res_num);
    }
    if (!curr_residue_.IsValid()) { // unit test
      LOG_DEBUG("new residue " << res_name << " " << res_num);
      curr_residue_ =editor.AppendResidue(curr_chain_, res_name.str(), res_num);
      warned_name_mismatch_=false;
      ++residue_count_; 
    }
    assert(curr_residue_.IsValid());
  }

  // finally add atom
  LOG_DEBUG("adding atom " << aname << " (" << s_ele << ") @" << apos);
  mol::AtomHandle ah;
  if (curr_residue_.GetName()!=res_name.str()) { // unit test
    if (!profile_.fault_tolerant && alt_loc=='.') { // unit test
      std::stringstream ss;
      ss << "Residue with number " << res_num << " has more than one name.";
      throw IOException(this->FormatDiagnostic(STAR_DIAG_ERROR,
                                               ss.str(),
                                               this->GetCurrentLinenum()));
    }
    if (!warned_name_mismatch_) { // unit test
      if (alt_loc=='.') { // unit test
        LOG_WARNING("Residue with number " << res_num << " has more than one "
                    "name. Ignoring atoms for everything but the first");
      } else {
        LOG_WARNING("Residue with number " << res_num 
                    << " contains a microheterogeneity. Everything but atoms "
                    "for the residue '" << curr_residue_.GetName() 
                    << "' will be ignored");
      }
    }
    warned_name_mismatch_=true;
    return;
  }
  if (alt_loc!='.') { // unit test
    // Check if there is already a atom with the same name.
    mol::AtomHandle me=curr_residue_.FindAtom(aname);
    if (me.IsValid()) { // unit test
      try {
        editor.AddAltAtomPos(String(1, alt_loc), me, apos);
      } catch (Error) {
        LOG_INFO("Ignoring atom alt location since there is already an atom "
                 "with name " << aname << ", but without an alt loc");
        return;
      }
      return;
    } else {
      ah = editor.InsertAltAtom(curr_residue_, aname,
                                String(1, alt_loc), apos, s_ele);
      ++atom_count_;
      }
  } else {
    mol::AtomHandle atom=curr_residue_.FindAtom(aname);
    if (atom.IsValid() && !profile_.quack_mode) { // unit test
      if (profile_.fault_tolerant) { // unit test
        LOG_WARNING("duplicate atom '" << aname << "' in residue " 
                    << curr_residue_);
        return;
      }
      throw IOException(this->FormatDiagnostic(STAR_DIAG_ERROR,
                                               "Duplicate atom '"+aname+
                                               "' in residue "+
                                               curr_residue_.GetQualifiedName(),
                                               this->GetCurrentLinenum()));
    }
    ah = editor.InsertAtom(curr_residue_, aname, apos, s_ele);
    ++atom_count_;
  }
  ah.SetBFactor(temp);

  ah.SetOccupancy(occ);

  // record type
  ah.SetHetAtom(indices_[GROUP_PDB] == -1 ? false :  
                columns[indices_[GROUP_PDB]][0]=='H');

}

void MMCifParser::OnDataRow(const StarLoopDesc& header, 
                            const std::vector<StringRef>& columns)
{
  switch(category_) {
  case ATOM_SITE:
    LOG_TRACE("processing atom_site entry");
    this->ParseAndAddAtom(columns);
    break;
  default:
    return;
  }
}

    /*
void PDBReader::Import(mol::EntityHandle& ent,
                       const String& restrict_chains)
{
  do {
    switch(curr_line[0]) {
      case 'A':
      case 'a':
        if (IEquals(curr_line.substr(0, 6), StringRef("ANISOU", 6))) {
          if (!charmm_style_) {
            LOG_TRACE("processing ANISOU entry");
            this->ParseAnisou(curr_line, line_num_, ent);            
          }
        }
        break;
      case 'C':
      case 'c':
        if (curr_line.size()<20) {
          LOG_TRACE("skipping entry");
          continue;
        }
        if (IEquals(curr_line.substr(0, 6), StringRef("COMPND", 6))) {
          LOG_TRACE("processing COMPND entry");
          this->ParseCompndEntry(curr_line, line_num_);
        }
        break;
      case 'E':
      case 'e':
        if (curr_line.size()<3) {
          continue;
        }
        if (IEquals(curr_line.rtrim(), StringRef("END", 3))) {
          hard_end_=true;
          go_on=false;
          break;
        }
        if (curr_line.size()<6) {
          continue;
        }
        if (IEquals(curr_line.substr(0, 6), StringRef("ENDMDL", 6))) {
          go_on=false;
          num_model_records_=0;
          break;
        }
      case 'H':
      case 'h':
        if (curr_line.size()<6) {
          continue;
        }
        if (IEquals(curr_line.substr(0, 6), StringRef("HETATM", 6))) {
          if (profile_.no_hetatms)
            continue;
          LOG_TRACE("processing HETATM entry");
          this->ParseAndAddAtom(curr_line, line_num_, ent, 
                                StringRef("HETATM", 6));
        } else if (IEquals(curr_line.substr(0, 6), StringRef("HELIX ", 6))) {
          if (!charmm_style_) {
            this->ParseHelixEntry(curr_line);            
          }
        } else if (IEquals(curr_line.substr(0, 6), StringRef("HET   ", 6))) {
          // remember het entry to mark the residues as ligand during ATOM import
          char chain=curr_line[12];
          std::pair<bool, int> num=curr_line.substr(13, 4).ltrim().to_int();
          if (!num.first) {
            if (profile_.fault_tolerant) {
              LOG_WARNING("Invalid HET entry on line " << line_num_);
              continue;
            } else {
              String msg=str(format("Invalid HET entry on line %d")%line_num_);
              throw IOException(msg);
            }
          }
          hets_.push_back(HetEntry(chain, to_res_num(num.second,
                                   curr_line[17])));
        }
        break;
      case 'M':
      case 'm':
        if (curr_line.size()<6) {
          continue;
        }
        if (IEquals(curr_line.substr(0, 6), StringRef("MODEL ", 6))) {
          ++num_model_records_;          
          if (num_model_records_<2) {
            continue;
          }
          if (profile_.fault_tolerant) {
            go_on=false;
            num_model_records_=1;
            break;
          }
          String msg=str(format("MODEL record without matching ENDMDL on line %d")%line_num_);
          throw IOException(msg);
        }
        break;
      case 'S':
      case 's':
        if (curr_line.size()<6) {
          continue;
        }
        if (IEquals(curr_line.substr(0, 6), StringRef("SHEET ", 6))) {
          if (!charmm_style_) {
            this->ParseStrandEntry(curr_line);
          }
        }
        if (IEquals(curr_line.substr(0, 6), StringRef("SEQRES", 6))) {
          if (read_seqres_) {
            this->ParseSeqRes(curr_line, line_num_);
          }
        }
        break;
      default:
        break;
    }
  }  while (std::getline(in_, curr_line_) && ++line_num_ && go_on);
  LOG_INFO("imported "
               << chain_count_ << " chains, "
               << residue_count_ << " residues, "
               << atom_count_ << " atoms; with "
               << helix_list_.size() << " helices and "
               << strand_list_.size() << " strands");
  this->AssignSecStructure(ent);
  this->AssignMolIds(ent);
  for (HetList::const_iterator i=hets_.begin(), e=hets_.end(); i!=e; ++i) {
    mol::ResidueHandle res=ent.FindResidue(String(1, i->chain), i->num);
    if (res.IsValid()) {
      res.SetIsLigand(true);
    }      
  }
}
     */

  
  /*
  virtual void OnEndLoop() { }

  virtual void OnDataItem(const StarDataItem& item) { }

  };*/

void MMCifParser::OnEndData()
{
  LOG_INFO("imported "
           << chain_count_ << " chains, "
           << residue_count_ << " residues, "
           << atom_count_ << " atoms;");
}

}}
