//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2019 by the OpenStructure authors
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

#include <ost/db/extract_data_helper.hh>

namespace ost { namespace db {

void ExtractValidPositions(const String& entry_name, const String& chain_name,
                           LinearIndexer& indexer,
                           LinearCharacterContainer& atomseq_container,
                           LinearPositionContainer& position_container,
                           ost::seq::SequenceHandle& seq,
                           geom::Vec3List& positions) {

  std::pair<uint64_t, uint64_t> data_range = indexer.GetDataRange(entry_name,
                                                                  chain_name);
  String atomseq;
  atomseq_container.GetCharacters(data_range, atomseq);
  geom::Vec3List extracted_positions;
  position_container.GetPositions(data_range, extracted_positions);

  positions.clear();
  std::vector<char> valid_seq;
  for(uint i = 0; i < atomseq.size(); ++i) {
    if(atomseq[i] != '-') {
      positions.push_back(extracted_positions[i]);
      valid_seq.push_back(atomseq[i]);
    }
  }

  seq = ost::seq::CreateSequence("extracted_seq", String(valid_seq.begin(), 
                                                         valid_seq.end()));
}

}} //ns
