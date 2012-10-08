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

#include <ost/log.hh>
#include <ost/mol/chain_view.hh>
#include <ost/mol/residue_view.hh>
#include <ost/mol/alg/consistency_checks.hh>

namespace ost { namespace mol { namespace alg {
  
/// \brief Checks that residue types with the same ResNum in the two structures match
///
/// Requires a reference structure and a probe structure. The function checks that all the 
/// residues in the reference structure that appear in the probe structure (i.e., that have the 
/// same ResNum) are of the same residue type. 

bool CheckResidueTypes (const mol::EntityView& probe, const mol::EntityView& reference)
{
  bool return_value = true;
  ChainViewList ref_chains = reference.GetChainList();
  for (ChainViewList::const_iterator rci = ref_chains.begin(), rce=ref_chains.end(); rci!=rce; ++rci) {
    ChainView probe_chain = probe.FindChain(rci->GetName());
    if (probe_chain.IsValid()) { 
      ResidueViewList ref_residues = rci->GetResidueList();       
      for (ResidueViewList::iterator rri=ref_residues.begin(), rre=ref_residues.end(); rri!=rre; ++rri) {
        ResidueView probe_residue = probe_chain.FindResidue(rri->GetNumber());
        if (probe_residue.IsValid()) {
          if (probe_residue.GetName()!=rri->GetName()) {
            return_value =false;
            LOG_VERBOSE("Type mismatch for residue " << probe_residue.GetNumber());
            LOG_VERBOSE("In reference: " << rri->GetName() << ", in compared structure: " << probe_residue.GetName());
          } 
        }            
      }
    }
  }          
  return return_value;
}
 
}}}



