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

#ifndef OST_DB_EXTRACT_DATA_HELPER_HH
#define OST_DB_EXTRACT_DATA_HELPER_HH

#include <ost/geom/vec3.hh>
#include <ost/seq/sequence_handle.hh>
#include <ost/db/linear_indexer.hh>
#include <ost/db/binary_container.hh>
#include <ost/seq/alignment_handle.hh>

namespace ost { namespace db {

void ExtractValidPositions(const String& entry_name, const String& chain_name,
                           LinearIndexer& indexer,
                           LinearCharacterContainer& atom_seqcontainer,
                           LinearPositionContainer& position_container,
                           ost::seq::SequenceHandle& seq,
                           geom::Vec3List& positions);

void ExtractTemplateDataDisCo(const String& entry_name, const String& chain_name,
                              const ost::seq::AlignmentHandle& aln,
                              LinearIndexerPtr indexer, 
                              LinearCharacterContainerPtr seqres_container,
                              LinearCharacterContainerPtr atomseq_container,
                              LinearPositionContainerPtr& position_container,
                              std::vector<int>& residue_numbers,
                              geom::Vec3List& positions);

void ExtractTemplateDataGMQE(const String& entry_name, const String& chain_name,
                             const ost::seq::AlignmentHandle& aln,
                             LinearIndexerPtr indexer, 
                             LinearCharacterContainerPtr seqres_container,
                             LinearCharacterContainerPtr atomseq_container,
                             LinearCharacterContainerPtr dssp_container,
                             LinearPositionContainerPtr& n_position_container,
                             LinearPositionContainerPtr& ca_position_container,
                             LinearPositionContainerPtr& c_position_container,
                             LinearPositionContainerPtr& cb_position_container,
                             std::vector<int>& residue_numbers,
                             String& dssp,
                             geom::Vec3List& n_positions,
                             geom::Vec3List& ca_positions,
                             geom::Vec3List& c_positions,
                             geom::Vec3List& cb_positions);
}} //ns

#endif
