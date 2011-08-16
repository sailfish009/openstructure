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

#include <ost/profile.hh>
#include <ost/log.hh>
#include <ost/dyn_cast.hh>
#include <ost/mol/xcs_editor.hh>
#include <ost/conop/conop.hh>

#include <ost/conop/rule_based_builder.hh>
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
  warned_rule_based_    = false;
  info_                 = MMCifInfo();
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
  info_                 = MMCifInfo();
  entity_desc_map_.clear();
  authors_map_.clear();
  bu_origin_map_.clear();
  bu_assemblies_.clear();
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
    this->TryStoreIdx(ENTITY_ID, "entity_id", header);
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
    this->TryStoreIdx(CITATION_ID, "id", header);
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
  } else if (header.GetCategory()=="citation_author") {
    category_ = CITATION_AUTHOR;
    // mandatory items
    this->TryStoreIdx(AUTHOR_CITATION_ID, "citation_id", header);
    this->TryStoreIdx(AUTHOR_NAME,        "name", header);
    this->TryStoreIdx(ORDINAL,            "ordinal", header);
    cat_available = true;
  } else if (header.GetCategory() == "exptl") {
    category_ = EXPTL;
    // mandatory items
    this->TryStoreIdx(CITATION_ID, "entry_id", header);
    this->TryStoreIdx(METHOD,      "method", header);
    cat_available = true;
  } else if (header.GetCategory() == "refine") {
    category_ = REFINE;
    // mandatory items
    this->TryStoreIdx(REFINE_ENTRY_ID, "entry_id", header);
    this->TryStoreIdx(LS_D_RES_HIGH,   "ls_d_res_high", header);
    this->TryStoreIdx(LS_D_RES_LOW,    "ls_d_res_low", header);
    cat_available = true;
  } else if (header.GetCategory() == "pdbx_struct_assembly") {
    category_ = PDBX_STRUCT_ASSEMBLY;
    // mandatory items
    this->TryStoreIdx(PSA_ID, "id", header);
    // optional
    indices_[PSA_DETAILS] = header.GetIndex("details");
    //indices_[METHOD_DETAILS] = header.GetIndex("method_details");
    cat_available = true;
  } else if (header.GetCategory() == "pdbx_struct_assembly_gen") {
    category_ = PDBX_STRUCT_ASSEMBLY_GEN;
    // mandatory items
    this->TryStoreIdx(ASSEMBLY_ID,     "assembly_id", header);
    this->TryStoreIdx(ASYM_ID_LIST,    "asym_id_list", header);
    this->TryStoreIdx(OPER_EXPRESSION, "oper_expression", header);
    cat_available = true;
  } else if (header.GetCategory() == "pdbx_struct_oper_list") {
    category_ = PDBX_STRUCT_OPER_LIST;
    // mandatory items
    this->TryStoreIdx(PSOL_ID,   "id",   header);
    this->TryStoreIdx(PSOL_TYPE, "type", header);
    // optional items
    indices_[VECTOR_1]   = header.GetIndex("vector[1]");
    indices_[VECTOR_2]   = header.GetIndex("vector[2]");
    indices_[VECTOR_3]   = header.GetIndex("vector[3]");
    indices_[MATRIX_1_1] = header.GetIndex("matrix[1][1]");
    indices_[MATRIX_1_2] = header.GetIndex("matrix[1][2]");
    indices_[MATRIX_1_3] = header.GetIndex("matrix[1][3]");
    indices_[MATRIX_2_1] = header.GetIndex("matrix[2][1]");
    indices_[MATRIX_2_2] = header.GetIndex("matrix[2][2]");
    indices_[MATRIX_2_3] = header.GetIndex("matrix[2][3]");
    indices_[MATRIX_3_1] = header.GetIndex("matrix[3][1]");
    indices_[MATRIX_3_2] = header.GetIndex("matrix[3][2]");
    indices_[MATRIX_3_3] = header.GetIndex("matrix[3][3]");
    cat_available = true;
  }
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
        edm_it->second.seqres = seqres.str_no_whitespace();        
      } else {
        throw IOException(this->FormatDiagnostic(STAR_DIAG_ERROR,
                   "'entity_poly.pdbx_seq_one_letter_code_can' not available.'",
                                                 this->GetCurrentLinenum()));
      }
    } else if (indices_[PDBX_SEQ_ONE_LETTER_CODE] != -1) {
      seqres=columns[indices_[PDBX_SEQ_ONE_LETTER_CODE]];
      conop::BuilderP builder=conop::Conopology::Instance().GetBuilder("DEFAULT");
      conop::RuleBasedBuilderPtr rbb=dyn_cast<conop::RuleBasedBuilder>(builder);
      if (!rbb) {
        if (!warned_rule_based_) {
          LOG_WARNING("SEQRES import requires the rule-based builder. Ignoring "
                      "SEQRES records");      
        }
        warned_rule_based_=true;
        return;
      }
      conop::CompoundLibPtr comp_lib=rbb->GetCompoundLib();
      edm_it->second.seqres = this->ConvertSEQRES(seqres.str_no_whitespace(),
                                                  comp_lib);
    } else {
      throw IOException(this->FormatDiagnostic(STAR_DIAG_ERROR,
                       "'entity_poly.pdbx_seq_one_letter_code' not available.'",
                                               this->GetCurrentLinenum()));
    }
  }
}

String MMCifParser::ConvertSEQRES(const String& seqres, 
                                  conop::CompoundLibPtr comp_lib)
{
  String can_seqres;
  for (String::const_iterator i=seqres.begin(), e=seqres.end(); i!=e; ++i) {
    if (*i=='(') {
      bool found_end_paren=false;
      String tlc;
      tlc.reserve(3);
      while ((++i)!=seqres.end()) {
        if (*i==')') {
          found_end_paren=true;
          break;
        }
        tlc.push_back(*i);
      }
      if (!found_end_paren) {
        throw IOException(this->FormatDiagnostic(STAR_DIAG_ERROR,
                          "'entity_poly.pdbx_seq_one_letter_code' contains "
                          "unmatched '('", this->GetCurrentLinenum()));
      }
      conop::CompoundPtr compound=comp_lib->FindCompound(tlc, 
                                                         conop::Compound::PDB);
      if (!compound) {
        if (tlc!="UNK") {

          LOG_WARNING("unknown residue '" << tlc << "' in SEQRES record. "
                      "Setting one-letter-code to '?'");
        }
        can_seqres.push_back('?');
        continue;
      }
      can_seqres.push_back(compound->GetOneLetterCode());
    } else {
      can_seqres.push_back(*i);
    }
  }
  return can_seqres;
}

void MMCifParser::ParseCitation(const std::vector<StringRef>& columns)
{
  // create citation object
  MMCifInfoCitation cit = MMCifInfoCitation();
  // just add info
  cit.SetID(columns[indices_[CITATION_ID]].str());
  if (indices_[ABSTRACT_ID_CAS] != -1) {
    cit.SetCAS(columns[indices_[ABSTRACT_ID_CAS]].str());
  }
  if (indices_[BOOK_ID_ISBN] != -1) {
    cit.SetISBN(columns[indices_[BOOK_ID_ISBN]].str());
  }
  if (indices_[BOOK_TITLE] != -1) {
    if (columns[indices_[BOOK_TITLE]] != StringRef(".", 1)) {
      cit.SetPublishedIn(columns[indices_[BOOK_TITLE]].str());
    }
  }
  if (indices_[JOURNAL_FULL] != -1) {
    if (columns[indices_[JOURNAL_FULL]] != StringRef(".", 1)) {
      if (cit.GetPublishedIn().length() > 0) {
        throw IOException(this->FormatDiagnostic(STAR_DIAG_WARNING,
                                                 "citation.book_title already occupies the 'published_in' field of this citation, cannot add " +
                                                 columns[indices_[JOURNAL_FULL]].str() +
                                                 ".",
                                                 this->GetCurrentLinenum()));
      } else {
        cit.SetPublishedIn(columns[indices_[JOURNAL_FULL]].str());
      }
    }
  }
  if (indices_[JOURNAL_VOLUME] != -1) {
    cit.SetVolume(columns[indices_[JOURNAL_VOLUME]].str());
  }
  if (indices_[PAGE_FIRST] != -1) {
    cit.SetPageFirst(columns[indices_[PAGE_FIRST]].str());
  }
  if (indices_[PAGE_LAST] != -1) {
    cit.SetPageLast(columns[indices_[PAGE_LAST]].str());
  }
  if (indices_[PDBX_DATABASE_ID_DOI] != -1) {
    cit.SetDOI(columns[indices_[PDBX_DATABASE_ID_DOI]].str());
  }
  if (indices_[PDBX_DATABASE_ID_PUBMED] != -1) {
    if (columns[indices_[PDBX_DATABASE_ID_PUBMED]][0]!='?') {
      cit.SetPubMed(this->TryGetInt(columns[indices_[PDBX_DATABASE_ID_PUBMED]],
                                    "citation.pdbx_database_id_PubMed"));
    }
  }
  if (indices_[YEAR] != -1) {
    if (columns[indices_[YEAR]][0]!='?') {
      cit.SetPubMed(this->TryGetInt(columns[indices_[YEAR]], "citation.year"));
    }
  }
  if (indices_[TITLE] != -1) {
    cit.SetTitle(columns[indices_[TITLE]].str());
  }

  // store citation (wo author, yet)
  info_.AddCitation(cit);
}

void MMCifParser::ParseCitationAuthor(const std::vector<StringRef>& columns)
{
  // get/ pack values
  MMCifCitationAuthorMap::iterator atm_it;
  std::vector<String> at_vec;
  std::vector<int> pos_vec;
  atm_it = authors_map_.find(columns[indices_[AUTHOR_CITATION_ID]].str());
  if (atm_it != authors_map_.end()) {
    at_vec = atm_it->second.second;
    pos_vec = atm_it->second.first;
  }
  at_vec.push_back(columns[indices_[AUTHOR_NAME]].str());
  pos_vec.push_back(this->TryGetInt(columns[indices_[ORDINAL]],
                            "citation_author.ordinal"));

  // sort new author into right position
  std::vector<int>::iterator pos_it;
  std::vector<String>::iterator atv_it;
  int ti;
  String ts; 
  pos_it = pos_vec.end();
  atv_it = at_vec.end();
  --pos_it;
  --atv_it;
  for (; pos_it != pos_vec.begin(); --pos_it, --atv_it) {
    if (*pos_it < *(pos_it-1)) {
      ti = *pos_it;
      *pos_it = *(pos_it-1);
      *(pos_it-1) = ti;
      ts = *atv_it;
      *atv_it = *(atv_it-1);
      *(atv_it-1) = ts;
    }
    else {
      break;
    }
  }

  // store new values in map
  if (atm_it != authors_map_.end()) {
    atm_it->second.second = at_vec;
    atm_it->second.first  = pos_vec;
  } else {
    authors_map_.insert(MMCifCitationAuthorMap::value_type(
                               columns[indices_[AUTHOR_CITATION_ID]].str(),
              std::pair<std::vector<int>, std::vector<String> >(pos_vec, at_vec)
                               ));
  }
}

void MMCifParser::ParseExptl(const std::vector<StringRef>& columns)
{
  info_.SetMethod(columns[indices_[METHOD]].str());
}

void MMCifParser::ParseRefine(const std::vector<StringRef>& columns)
{
  info_.SetResolution(this->TryGetReal(columns[indices_[LS_D_RES_HIGH]],
                                       "refine.ls_d_res_high"));
}

void MMCifParser::ParsePdbxStructAssembly(const std::vector<StringRef>& columns)
{
  if (indices_[PSA_DETAILS] != -1) {
    bu_origin_map_.insert(std::pair<String,
                                    String>(columns[indices_[PSA_ID]].str(),
                                         columns[indices_[PSA_DETAILS]].str()));
  } else {
    bu_origin_map_.insert(std::pair<String,
                                    String>(columns[indices_[PSA_ID]].str(),
                                            "?"));
  }
}

void MMCifParser::StoreExpression(const char* l, const char* s,
                                  bool& is_range, int lborder,
                                  std::vector<String>& single_block)
{
  std::stringstream ss;
  int rborder;

  if (l != s) {
    if (is_range) {
      is_range = false;
      rborder = this->TryGetInt(StringRef(l, s-l),
                                "pdbx_struct_assembly_gen.oper_expression");
      for (lborder += 1; lborder < rborder; lborder++) {
        ss << lborder;
        single_block.push_back(ss.str());
        ss.str("");
      }
    }
    single_block.push_back(String(l, s-l));
  }
}

void MMCifParser::StoreRange(const char*& l, const char* s, bool& is_range,
                             int& lborder, std::vector<String>& single_block)
{
  if (is_range) {
    throw IOException(this->FormatDiagnostic(STAR_DIAG_WARNING,
                                             "pdbx_struct_assembly_gen.oper_expression is missing a right border for a range expression.",
                                             this->GetCurrentLinenum()));
  }
  is_range = true;
  if (l != s) {
    lborder = this->TryGetInt(StringRef(l, s-l),
                              "pdbx_struct_assembly_gen.oper_expression");
    single_block.push_back(String(l, s-l));
  }
  l = s+1;
}

std::vector<std::vector<String> > MMCifParser::UnPackOperExperession(StringRef expression)
{
  std::vector<std::vector<String> > unpacked;
  std::vector<String> single_block;
  int lborder;
  bool is_range = false;
  std::stringstream ss;
  const char* s = expression.begin();
  const char* e = expression.end();
  const char* l = expression.begin();

  if (*s == '(') {
    ++s;
    ++l;
    // multiple blocks
    while (s != e) {
      if (*s == ',') {
        StoreExpression(l, s, is_range, lborder, single_block);
        l = s+1;
      } else if (*s == '-') {
        StoreRange(l, s, is_range, lborder, single_block);
      } else if (*s == '(') {
        ++l;
      } else if (*s == ')') {
        StoreExpression(l, s, is_range, lborder, single_block);
        l = s+1;
        if (single_block.size() > 0) {
          unpacked.push_back(single_block);
        }
        single_block.clear();
      }
      ++s;
    }
  } else {
    // single block
    while (s != e) {
      if (*s == ',') {
        StoreExpression(l, s, is_range, lborder, single_block);
        l = s+1;
      } else if (*s == '-') {
        StoreRange(l, s, is_range, lborder, single_block);
      }
      ++s;
    }
    StoreExpression(l, s, is_range, lborder, single_block);

    if (is_range) {
      throw IOException(this->FormatDiagnostic(STAR_DIAG_WARNING,
                                               "pdbx_struct_assembly_gen.oper_expression is missing a right border for a range expression.",
                                               this->GetCurrentLinenum()));
    }
    unpacked.push_back(single_block);
  }

  return unpacked;
}

void MMCifParser::ParsePdbxStructAssemblyGen(const std::vector<StringRef>& columns)
{
  MMCifBioUAssembly assembly;
  assembly.biounit = MMCifInfoBioUnit();

  assembly.biounit.SetDetails(columns[indices_[ASSEMBLY_ID]].str());

  std::vector<StringRef> tmp_chains=columns[indices_[ASYM_ID_LIST]].split(',');
  std::vector<StringRef>::const_iterator tc_it;
  for (tc_it = tmp_chains.begin(); tc_it != tmp_chains.end(); ++tc_it) {
    assembly.biounit.AddChain(tc_it->str());
  }

  assembly.operations =
    this->UnPackOperExperession(columns[indices_[OPER_EXPRESSION]]);

  bu_assemblies_.push_back(assembly);
}

void MMCifParser::ParsePdbxStructOperList(const std::vector<StringRef>& columns)
{
  MMCifInfoTransOpPtr op(new MMCifInfoTransOp);

  op->SetID(columns[indices_[PSOL_ID]].str());
  op->SetType(columns[indices_[PSOL_TYPE]].str());

  if ((indices_[VECTOR_1] != -1)&&
      (indices_[VECTOR_2] != -1)&&
      (indices_[VECTOR_3] != -1)) {
    op->SetVector(this->TryGetReal(columns[indices_[VECTOR_1]],
                                   "pdbx_struct_oper_list.vector[1]"),
                  this->TryGetReal(columns[indices_[VECTOR_2]],
                                   "pdbx_struct_oper_list.vector[2]"),
                  this->TryGetReal(columns[indices_[VECTOR_3]],
                                   "pdbx_struct_oper_list.vector[3]"));
  }

  if ((indices_[MATRIX_1_1] != -1)&&
      (indices_[MATRIX_1_2] != -1)&&
      (indices_[MATRIX_1_3] != -1)&&
      (indices_[MATRIX_2_1] != -1)&&
      (indices_[MATRIX_2_2] != -1)&&
      (indices_[MATRIX_2_3] != -1)&&
      (indices_[MATRIX_3_1] != -1)&&
      (indices_[MATRIX_3_2] != -1)&&
      (indices_[MATRIX_3_3] != -1)) {
    op->SetMatrix(this->TryGetReal(columns[indices_[MATRIX_1_1]],
                                   "pdbx_struct_oper_list.matrix[1][1]"),
                  this->TryGetReal(columns[indices_[MATRIX_1_2]],
                                   "pdbx_struct_oper_list.matrix[1][2]"),
                  this->TryGetReal(columns[indices_[MATRIX_1_3]],
                                   "pdbx_struct_oper_list.matrix[1][3]"),
                  this->TryGetReal(columns[indices_[MATRIX_2_1]],
                                   "pdbx_struct_oper_list.matrix[2][1]"),
                  this->TryGetReal(columns[indices_[MATRIX_2_2]],
                                   "pdbx_struct_oper_list.matrix[2][2]"),
                  this->TryGetReal(columns[indices_[MATRIX_2_3]],
                                   "pdbx_struct_oper_list.matrix[2][3]"),
                  this->TryGetReal(columns[indices_[MATRIX_3_1]],
                                   "pdbx_struct_oper_list.matrix[3][1]"),
                  this->TryGetReal(columns[indices_[MATRIX_3_2]],
                                   "pdbx_struct_oper_list.matrix[3][2]"),
                  this->TryGetReal(columns[indices_[MATRIX_3_3]],
                                   "pdbx_struct_oper_list.matrix[3][3]"));
  }

  info_.AddOperation(op);
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
    LOG_TRACE("processing citation entry");
    this->ParseCitation(columns);
    break;
  case CITATION_AUTHOR:
    LOG_TRACE("processing citation_author entry")
    this->ParseCitationAuthor(columns);
    break;
  case EXPTL:
    LOG_TRACE("processing exptl entry")
    this->ParseExptl(columns);
    break;
  case REFINE:
    LOG_TRACE("processing refine entry")
    this->ParseRefine(columns);
    break;
  case PDBX_STRUCT_ASSEMBLY:
    LOG_TRACE("processing pdbx_struct_assembly entry")
    this->ParsePdbxStructAssembly(columns);
    break;
  case PDBX_STRUCT_ASSEMBLY_GEN:
    LOG_TRACE("processing pdbx_struct_assembly_gen entry")
    this->ParsePdbxStructAssemblyGen(columns);
    break;
  case PDBX_STRUCT_OPER_LIST:
    LOG_TRACE("processing pdbx_struct_oper_list entry")
    this->ParsePdbxStructOperList(columns);
    break;
  default:
    throw IOException(this->FormatDiagnostic(STAR_DIAG_ERROR,
                       "Uncatched category '"+ header.GetCategory() +"' found.",
                                             this->GetCurrentLinenum()));
    return;
  }
}

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

  // process citations (couple with authors
  // iterate citations
  MMCifCitationAuthorMap::const_iterator atm_it;
  for (atm_it = authors_map_.begin(); atm_it != authors_map_.end(); ++atm_it) {
    info_.AddAuthorsToCitation(StringRef(atm_it->first.c_str(),
                                         atm_it->first.length()),
                               atm_it->second.second);
  }

  bool found;
  MMCifBioUAssemblyVector::iterator bua_it;
  std::vector<std::vector<String> >::const_iterator aol_it;
  std::vector<String>::const_iterator aob_it;
  std::vector<MMCifInfoTransOpPtr> operation_list;
  std::map<String, String>::const_iterator buom_it;
  std::vector<MMCifInfoTransOpPtr> operations = info_.GetOperations();
  std::vector<MMCifInfoTransOpPtr>::const_iterator buop_it;
  for (bua_it = bu_assemblies_.begin();
       bua_it != bu_assemblies_.end();
       ++bua_it) {
    // pair with pdbx_struct_assembly entry
    buom_it = bu_origin_map_.find(bua_it->biounit.GetDetails());
    if (buom_it == bu_origin_map_.end()) {
      throw IOException(this->FormatDiagnostic(STAR_DIAG_ERROR,
                                               "No pdbx_struct_assembly.id '"+
                                               bua_it->biounit.GetDetails() +
                         "' found as requested by pdbx_struct_assembly_gen.")); 
    }
    bua_it->biounit.SetDetails(buom_it->second);

    // pair with pdbx_struct_oper_list
    for (aol_it = bua_it->operations.begin();
         aol_it != bua_it->operations.end();
         ++aol_it) {
      operation_list.clear();
      for (aob_it = aol_it->begin(); aob_it != aol_it->end(); aob_it++) {
        found = false;
        for (buop_it = operations.begin();
             buop_it != operations.end();
             ++buop_it) {
          if ((*buop_it)->GetID() == *aob_it) {
            operation_list.push_back(*buop_it);
            found = true;
            break;
          }
        }
        if (!found) {
          throw IOException(this->FormatDiagnostic(STAR_DIAG_ERROR,
                                                "No pdbx_struct_oper_list.id '"+
                                                   *aob_it +
                          "' found as requested by pdbx_struct_assembly_gen."));
        }
      }
      bua_it->biounit.AddOperations(operation_list);
    }
    info_.AddBioUnit(bua_it->biounit);
  }
  bu_assemblies_.clear();

  LOG_INFO("imported "
           << chain_count_ << " chains, "
           << residue_count_ << " residues, "
           << atom_count_ << " atoms;");
}

}}
