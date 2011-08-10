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
  memset(category_counts_, 0, DONT_KNOW * sizeof(int));
  chain_count_          = 0;
  atom_count_           = 0;
  residue_count_        = 0;
  auth_chain_id_        = false;
  seqres_can_           = false;
  has_model_            = false;
  restrict_chains_      = "";
  subst_res_id_         = "";
  curr_chain_           = mol::ChainHandle();
  curr_residue_         = mol::ResidueHandle();
  seqres_               = seq::CreateSequenceList();
  read_seqres_          = false;
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
  seqres_               = seq::CreateSequenceList();
  entity_desc_map_.clear();
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

  return true;
}

bool MMCifParser::OnBeginLoop(const StarLoopDesc& header)
{
  bool cat_available = false;
  category_ = DONT_KNOW;
  // set whole array to -1
  memset(indices_, -1, MAX_ITEMS_IN_ROW * sizeof(int));
  // walk through possible categories
  if (header.GetCategory() == "atom_site") {
    category_ = ATOM_SITE;
    // mandatory items
    this->TryStoreIdx(AUTH_ASYM_ID,    "auth_asym_id",    header);
    this->TryStoreIdx(AS_ID,           "id",              header);
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
    indices_[OCCUPANCY]          = header.GetIndex("occupancy");
    indices_[B_ISO_OR_EQUIV]     = header.GetIndex("B_iso_or_equiv");
    indices_[GROUP_PDB]          = header.GetIndex("group_PDB");
    indices_[AUTH_SEQ_ID]        = header.GetIndex("auth_seq_id");
    indices_[PDBX_PDB_INS_CODE]  = header.GetIndex("pdbx_PDB_ins_code");
    indices_[PDBX_PDB_MODEL_NUM] = header.GetIndex("pdbx_PDB_model_num");

    // post processing
    if (category_counts_[category_] > 0) {
      if ((has_model_ && (indices_[PDBX_PDB_MODEL_NUM] == -1))||
          (!has_model_ && (indices_[PDBX_PDB_MODEL_NUM] != -1))) { // unit test
        throw IOException(this->FormatDiagnostic(STAR_DIAG_ERROR,
                              "Not all atom_site entries carry a model number.",
                                                 this->GetCurrentLinenum()));
      }
    }
    cat_available = true;
  } else if (header.GetCategory() == "entity") {
    category_ = ENTITY;
    // mandatory items
    this->TryStoreIdx(E_ID, "id", header);
    // optional
    indices_[E_TYPE]  = header.GetIndex("type");
    indices_[PDBX_DESCRIPTION] = header.GetIndex("pdbx_description");
    cat_available = true;
  } else if (header.GetCategory() == "entity_poly") {
    category_ = ENTITY_POLY;
    // mandatory
    this->TryStoreIdx(ENTITY_ID, "entity_id",    header);
    // optional
    indices_[EP_TYPE]  = header.GetIndex("type");
    indices_[PDBX_SEQ_ONE_LETTER_CODE] =
      header.GetIndex("pdbx_seq_one_letter_code");
    indices_[PDBX_SEQ_ONE_LETTER_CODE_CAN] =
      header.GetIndex("pdbx_seq_one_letter_code_can");
    cat_available = true;
  } else if (header.GetCategory() == "citation") {
    category_ = CITATION;
    // mandatory items
    this->TryStoreIdx(CITATION_ID, "id",    header);
    // optional
    indices_[ABSTRACT_ID_CAS]         = header.GetIndex("abstract_id_CAS");
    indices_[BOOK_ID_ISBN]            = header.GetIndex("book_id_ISBN");
    indices_[BOOK_TITLE]              = header.GetIndex("book_title");
    indices_[JOURNAL_FULL]            = header.GetIndex("journal_full");
    indices_[YEAR]                    = header.GetIndex("year");
    indices_[TITLE]                   = header.GetIndex("title");
    indices_[JOURNAL_VOLUME]          = header.GetIndex("journal_volume");
    indices_[PAGE_FIRST]              = header.GetIndex("page_first");
    indices_[PAGE_LAST]               = header.GetIndex("page_last");
    indices_[PDBX_DATABASE_ID_DOI]    = header.GetIndex("pdbx_database_id_DOI");
    indices_[PDBX_DATABASE_ID_PUBMED] =
      header.GetIndex("pdbx_database_id_PubMed");
    cat_available = true;
  }
  /*else if (header.GetCategory()=="pdbx_poly_seq_scheme") {
  } else if (header.GetCategory()=="pdbx_struct_assembly") {
  } else if (header.GetCategory()=="struct_conf") {
  }*/
  category_counts_[category_]++;
  return cat_available;
}

mol::ResNum to_res_num(int num, char ins_code)
{
  return mol::ResNum(num, ins_code==' ' ? '\0' : ins_code);
}

bool MMCifParser::ParseAtomIdent(const std::vector<StringRef>& columns,
                                 String& chain_name,
                                 StringRef& res_name,
                                 mol::ResNum& resnum,
                                 bool& valid_res_num,
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

  std::pair<bool, int> a_num = this->TryGetInt(columns[indices_[AS_ID]],
                                               "atom_site.id",
                                          profile_.fault_tolerant); // unit test

  alt_loc = columns[indices_[LABEL_ALT_ID]][0];
  res_name = columns[indices_[LABEL_COMP_ID]];
  std::pair<bool, int> res_num;
  if (columns[indices_[LABEL_SEQ_ID]][0] != '.') {
    res_num =this->TryGetInt(columns[indices_[LABEL_SEQ_ID]],
                             "atom_site.label_seq_id",
                             profile_.fault_tolerant); // unit test
    if (!res_num.first) { // unit test
      if (profile_.fault_tolerant) {
        return false;
      }
    }
    valid_res_num = true;
  } else {
    valid_res_num = false;
    return true;
  }

  resnum = to_res_num(res_num.second, ' ');

  return true;
}

void MMCifParser::ParseAndAddAtom(const std::vector<StringRef>& columns)
{
  mol::XCSEditor editor=ent_handle_.EditXCS(mol::BUFFERED_EDIT); // potbl
  char alt_loc=0;
  String chain_name;
  StringRef res_name, atom_name;
  mol::ResNum res_num(0);
  bool valid_res_num = false;
  if (indices_[PDBX_PDB_MODEL_NUM] != -1) {
    if (has_model_) {
      if (curr_model_ != TryGetInt(columns[indices_[PDBX_PDB_MODEL_NUM]],
                                   "atom_site.pdbx_PDB_model_num")) {
        return;
      }
    } else {
      has_model_ = true;
      curr_model_ = TryGetInt(columns[indices_[PDBX_PDB_MODEL_NUM]],
      "atom_site.pdbx_PDB_model_num");
    }
  }

  if (!this->ParseAtomIdent(columns,
                            chain_name,
                            res_name,
                            res_num,
                            valid_res_num,
                            atom_name,
                            alt_loc)) {// unit test
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
  bool update_chain = false;
  bool update_residue = false;
  if(!curr_chain_) { // unit test
      update_chain=true;
      update_residue=true;
  } else if(curr_chain_.GetName() != chain_name) { // unit test
    update_chain=true;
    update_residue=true;
  }

  if(!curr_residue_) { // unit test
    update_residue=true;
  } else if (!valid_res_num) {
    if (indices_[AUTH_SEQ_ID] != -1 &&
        indices_[PDBX_PDB_INS_CODE] != -1) {
      if (subst_res_id_ !=
          chain_name +
          columns[indices_[AUTH_SEQ_ID]].str() +
          columns[indices_[PDBX_PDB_INS_CODE]].str()) {
        update_residue=true;

        subst_res_id_ = chain_name +
                        columns[indices_[AUTH_SEQ_ID]].str() +
                        columns[indices_[PDBX_PDB_INS_CODE]].str();
      }
    } else {
      throw IOException(this->FormatDiagnostic(STAR_DIAG_ERROR,
                                           "Missing residue number information",
                                               this->GetCurrentLinenum()));
    }
  } else if(curr_residue_.GetNumber() != res_num) { // unit test
    update_residue=true;
  }

  if(update_chain) { // unit test
    curr_chain_ = ent_handle_.FindChain(chain_name);
    if(!curr_chain_.IsValid()) { // unit test
      LOG_DEBUG("new chain " << chain_name);
      curr_chain_=editor.InsertChain(chain_name);
      ++chain_count_;
      // store entity id
      chain_id_pairs_.push_back(std::pair<mol::ChainHandle,String>(curr_chain_,
                                     columns[indices_[LABEL_ENTITY_ID]].str()));
    }
    assert(curr_chain_.IsValid());
  } else if (chain_id_pairs_.back().second != // unit test
             columns[indices_[LABEL_ENTITY_ID]].str()) {
    // check that label_entity_id stays the same
    throw IOException(this->FormatDiagnostic(STAR_DIAG_ERROR,
        "Change of 'atom_site.label_entity_id' item for chain " +
        curr_chain_.GetName() + "! Expected: " + chain_id_pairs_.back().second +
        ", found: " + columns[indices_[LABEL_ENTITY_ID]].str() + ".",
                                             this->GetCurrentLinenum()));
  }

  if(update_residue) { // unit test
    curr_residue_=mol::ResidueHandle();
    if (valid_res_num && profile_.join_spread_atom_records) { // unit test
      curr_residue_=curr_chain_.FindResidue(res_num);
    }
    if (!curr_residue_.IsValid()) { // unit test
      LOG_DEBUG("new residue " << res_name << " " << res_num);
      if (valid_res_num) {
        curr_residue_ = editor.AppendResidue(curr_chain_,
                                             res_name.str(),
                                             res_num);
      } else {
        curr_residue_ = editor.AppendResidue(curr_chain_, res_name.str());
      }
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

void MMCifParser::ParseEntity(const std::vector<StringRef>& columns)
{
  bool store = false; // is it worth storing this record?
  MMCifEntityDesc desc;

  // type
  if (indices_[E_TYPE] != -1) {
    desc.type = mol::ChainTypeFromString(columns[indices_[E_TYPE]]);
    store = true;
  }

  // description
  if (indices_[PDBX_DESCRIPTION] != -1) {
    desc.details = columns[indices_[PDBX_DESCRIPTION]].str();
  } else {
    desc.details = "";
  }

  if (store) {
    desc.seqres = "";
    entity_desc_map_.insert(
                   MMCifEntityDescMap::value_type(columns[indices_[E_ID]].str(),
                                                  desc)
                            );
  }
}

void MMCifParser::ParseEntityPoly(const std::vector<StringRef>& columns)
{
  // we assume that the entity cat. ALWAYS comes before the entity_poly cat.
  // search entity
  MMCifEntityDescMap::iterator edm_it =
    entity_desc_map_.find(columns[indices_[ENTITY_ID]].str());

  if (edm_it == entity_desc_map_.end()) {
    throw IOException(this->FormatDiagnostic(STAR_DIAG_ERROR,
                     "'entity_poly' category defined before 'entity' for id '" +
                                            columns[indices_[ENTITY_ID]].str() +
                                             "' or missing.",
                                             this->GetCurrentLinenum()));
  }

  // store type
  if (indices_[EP_TYPE] != -1) {
    edm_it->second.type = mol::ChainTypeFromString(columns[indices_[EP_TYPE]]);
  }

  // store seqres
  if (edm_it->second.seqres.length() > 0) {
    throw IOException(this->FormatDiagnostic(STAR_DIAG_ERROR,
     "entity_poly.pdbx_seq_one_letter_code[_can] clash: sequence for entry '" +
                                            columns[indices_[ENTITY_ID]].str() +
                                             "' is already set to '" +
                                             edm_it->second.seqres + "'.",
                                             this->GetCurrentLinenum()));
  }
  if (read_seqres_) {
    StringRef seqres;
    if (seqres_can_) {
      if (indices_[PDBX_SEQ_ONE_LETTER_CODE_CAN] != -1) {
        seqres=columns[indices_[PDBX_SEQ_ONE_LETTER_CODE_CAN]];
      } else {
        throw IOException(this->FormatDiagnostic(STAR_DIAG_ERROR,
                   "'entity_poly.pdbx_seq_one_letter_code_can' not available.'",
                                                 this->GetCurrentLinenum()));
      }
    } else if (indices_[PDBX_SEQ_ONE_LETTER_CODE] != -1) {
      seqres=columns[indices_[PDBX_SEQ_ONE_LETTER_CODE]];
    } else {
      throw IOException(this->FormatDiagnostic(STAR_DIAG_ERROR,
                       "'entity_poly.pdbx_seq_one_letter_code' not available.'",
                                               this->GetCurrentLinenum()));
    }
    edm_it->second.seqres = seqres.str_no_whitespace();
  }
}

void MMCifParser::ParseCitation(const std::vector<StringRef>& columns)
{
  // fetch dependencies from dscription, like article requires year
}

void MMCifParser::OnDataRow(const StarLoopDesc& header, 
                            const std::vector<StringRef>& columns)
{
  switch(category_) {
  case ATOM_SITE:
    LOG_TRACE("processing atom_site entry");
    this->ParseAndAddAtom(columns);
    break;
  case ENTITY:
    LOG_TRACE("processing entity entry");
    this->ParseEntity(columns);
    break;
  case ENTITY_POLY:
    LOG_TRACE("processing entity_poly entry");
    this->ParseEntityPoly(columns);
    break;
  case CITATION:
    LOG_TRACE("processing citation entry")
    this->ParseCitation(columns);
    break;
  default:
    throw IOException(this->FormatDiagnostic(STAR_DIAG_ERROR,
                       "Uncatched category '"+ header.GetCategory() +"' found.",
                                             this->GetCurrentLinenum()));
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
  mol::XCSEditor editor=ent_handle_.EditXCS(mol::BUFFERED_EDIT);

  // process chain types
  std::vector<std::pair<mol::ChainHandle, String> >::const_iterator css;
  MMCifEntityDescMap::const_iterator edm_it;
  for (css = chain_id_pairs_.begin(); css != chain_id_pairs_.end(); ++css) {
    edm_it = entity_desc_map_.find(css->second);

    if (edm_it != entity_desc_map_.end()) {
      editor.SetChainType(css->first, edm_it->second.type);
      editor.SetChainDescription(css->first, edm_it->second.details);
      if (edm_it->second.seqres.length() > 0) {
        seqres_.AddSequence(seq::CreateSequence(css->first.GetName(),
                                                edm_it->second.seqres));
      }
    } else {
      LOG_WARNING("No entity description found for atom_site.label_entity_id '"
                  << css->second << "'");
    }
  }

  LOG_INFO("imported "
           << chain_count_ << " chains, "
           << residue_count_ << " residues, "
           << atom_count_ << " atoms;");
}

}}
