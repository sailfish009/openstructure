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

#include <ost/io/io_exception.hh>
#include <ost/io/mol/mmcif_info.hh>

namespace ost { namespace io {

void MMCifInfo::AddAuthorsToCitation(StringRef id, std::vector<String> list)
{
  // find citation
  std::vector<MMCifInfoCitation>::iterator cit_it;
  for (cit_it = citations_.begin(); cit_it != citations_.end(); ++cit_it) {
    if (id == StringRef(cit_it->GetID().c_str(), cit_it->GetID().length())) {
      cit_it->SetAuthorList(list);
      return;
    }
  }

  throw IOException("No citation for identifier '" + id.str() + "' found.");
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
MMCifInfoStructRefSeq::AddDif(int seq_rnum, int db_rnum, const String& details)
{
	MMCifInfoStructRefSeqDifPtr d(new MMCifInfoStructRefSeqDif(seq_rnum, db_rnum,
				                                                     details));
	difs_.push_back(d);
	return d;
}


}} //ns
