//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2020 by the OpenStructure authors
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

#include <ost/io/io_exception.hh>
#include <ost/io/mol/mmcif_info.hh>

namespace ost { namespace io {

void MMCifInfo::AddMMCifPDBChainTr(String cif, String pdb)
{
  std::map<String, String>::iterator tr_it = cif_2_pdb_chain_id_.find(cif);
  if (tr_it != cif_2_pdb_chain_id_.end()) {
    throw IOException("mmCIF chain id '"+ cif +"' is already mapped to '"+
                      tr_it->second+"'.");
  }
  cif_2_pdb_chain_id_.insert(std::pair<String, String>(cif, pdb));
}

String MMCifInfo::GetMMCifPDBChainTr(String cif) const
{
  std::map<String, String>::const_iterator tr_it =
    cif_2_pdb_chain_id_.find(cif);
  if (tr_it == cif_2_pdb_chain_id_.end()) { return ""; }
  return tr_it->second;
}

void MMCifInfo::AddPDBMMCifChainTr(String pdb, String cif)
{
  std::map<String, String>::iterator tr_it = pdb_2_cif_chain_id_.find(pdb);
  if (tr_it != pdb_2_cif_chain_id_.end()) {
    throw IOException("PDB chain id '"+ pdb +"' is already mapped to '"+
                      tr_it->second+"'.");
  }
  pdb_2_cif_chain_id_.insert(std::pair<String, String>(pdb, cif));
}

String MMCifInfo::GetPDBMMCifChainTr(String pdb) const
{
  std::map<String, String>::const_iterator tr_it =
    pdb_2_cif_chain_id_.find(pdb);
  if (tr_it == pdb_2_cif_chain_id_.end()) { return ""; }
  return tr_it->second;
}

void MMCifInfo::AddMMCifEntityIdTr(String cif, String ent_id)
{
  std::map<String, String>::iterator tr_it = cif_2_entity_id_.find(cif);
  if (tr_it != cif_2_entity_id_.end()) {
    throw IOException("mmCIF chain id '" + cif + "' is already mapped to "
                      "entity id '" + tr_it->second + "'.");
  }
  cif_2_entity_id_.insert(std::pair<String, String>(cif, ent_id));
}

String MMCifInfo::GetMMCifEntityIdTr(String cif) const
{
  std::map<String, String>::const_iterator tr_it =
    cif_2_entity_id_.find(cif);
  if (tr_it == cif_2_entity_id_.end()) { return ""; }
  return tr_it->second;
}

void MMCifInfo::AddAuthorsToCitation(StringRef id, std::vector<String> list)
{
  // find citation
  std::vector<MMCifInfoCitation>::iterator cit_it;
  for (cit_it = citations_.begin(); cit_it != citations_.end(); ++cit_it) {
    String cit_id = cit_it->GetID(); // to ensure lifetime of StringRef-pointers
    if (id == StringRef(cit_id.c_str(), cit_id.length())) {
      cit_it->SetAuthorList(list);
      return;
    }
  }

  throw IOException("No citation for identifier '" + id.str() + "' found.");
}

void MMCifInfo::AddBioUnit(MMCifInfoBioUnit bu)
{
  std::vector<MMCifInfoBioUnit>::iterator bu_it;
  for (bu_it = biounits_.begin(); bu_it != biounits_.end(); ++bu_it) {
    if (bu_it->GetID() == bu.GetID()) {
      break;
    }
  }
  if (bu_it != biounits_.end()) {
    bu_it->Merge(bu);
    return;
  }
  biounits_.push_back(bu);
}

void MMCifInfoBioUnit::AddChain(String chain)
{
  chains_.push_back(chain);

  if (tr_chains_.size()) {
    tr_chains_.back().second = chains_.size();
  }
  else {
    std::pair<int, int> tr_interval = std::pair<int, int>(0, 1);
    tr_chains_.push_back(tr_interval);
  }
}

void MMCifInfoBioUnit::SetChainList(std::vector<String> chains)
{
  chains_ = chains;

  if (tr_chains_.size()) {
    tr_chains_.clear();
  }
  std::pair<int, int> tr_interval = std::pair<int, int>(0, chains_.size());
  tr_chains_.push_back(tr_interval);
}

void MMCifInfoBioUnit::AddOperations(std::vector<MMCifInfoTransOpPtr> operations)
{
  operations_.push_back(operations);

  if (tr_operations_.size()) {
    tr_operations_.back().second = operations_.size();
  }
  else {
    std::pair<int, int> tr_interval = std::pair<int, int>(0, 1);
    tr_operations_.push_back(tr_interval);
  }
}

void MMCifInfoBioUnit::Merge(MMCifInfoBioUnit& from)
{
  // merge chains
  int old_size = chains_.size();
  chains_.insert(chains_.end(), from.chains_.begin(), from.chains_.end());
  std::pair<int, int> tr_interval = std::pair<int, int>(old_size,
                                                        chains_.size());
  tr_chains_.push_back(tr_interval);
  // merge operations
  old_size = operations_.size();
  operations_.insert(operations_.end(),
                     from.operations_.begin(),
                     from.operations_.end());
  tr_interval.first = old_size;
  tr_interval.second = operations_.size();
  tr_operations_.push_back(tr_interval);
}

MMCifInfoStructRefSeqPtr 
MMCifInfoStructRef::AddAlignedSeq(const String& aid, const String& chain_name, 
		                              int seq_begin, int seq_end, int db_begin, 
		                              int db_end)
{
	std::map<String, MMCifInfoStructRefSeqPtr>::const_iterator i=seqs_.find(aid);
	if (i!=seqs_.end()) {
		throw IOException("duplicate align_id for struct_ref '"+id_+"'");
	}
	MMCifInfoStructRefSeqPtr p(new MMCifInfoStructRefSeq(aid, chain_name,
				                                               seq_begin, seq_end, 
				                                               db_begin, db_end));
	seqs_[aid]=p;
	return p;
}


MMCifInfoStructRefSeqPtr 
MMCifInfoStructRef::GetAlignedSeq(const String& aid) const
{

  std::map<String, MMCifInfoStructRefSeqPtr>::const_iterator i=seqs_.find(aid);
  return i==seqs_.end() ? MMCifInfoStructRefSeqPtr() : i->second;
}

MMCifInfoStructRefSeqDifPtr 
MMCifInfoStructRefSeq::AddDif(int seq_rnum, const String& db_rnum, const String& details)
{
	MMCifInfoStructRefSeqDifPtr d(new MMCifInfoStructRefSeqDif(seq_rnum, db_rnum,
				                                                     details));
	difs_.push_back(d);
	return d;
}

}} //ns
