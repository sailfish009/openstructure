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
#ifndef OST_MOL_ALG_LOCAL_DIST_TEST_HH
#define OST_MOL_ALG_LOCAL_DIST_TEST_HH

#include <ost/mol/entity_view.hh>
#include <ost/mol/alg/module_config.hh>
#include <ost/seq/alignment_handle.hh>

namespace ost { namespace mol { namespace alg {
  
  
class UniqueAtomIdentifier
{
  
public:
  UniqueAtomIdentifier(const String& chain,const ResNum& residue,const String& residue_name, const String& atom): chain_(chain),residue_(residue),residue_name_(residue_name),atom_(atom) {}  

  // to make the compiler happy (boost python map suite)
  UniqueAtomIdentifier(): chain_(""),residue_(ResNum(1)),residue_name_(""),atom_("") {}  
    
  String GetChainName() const { return chain_; } 
  ResNum GetResNum() const { return residue_; }  
  String GetResidueName() const { return residue_name_; }
  String GetAtomName() const { return atom_; }
  bool operator==(const UniqueAtomIdentifier& rhs) const;
  bool operator<(const UniqueAtomIdentifier& rhs) const;	
    
private:

  String chain_;
  ResNum residue_;
  String residue_name_;
  String atom_;    
};

typedef std::pair<UniqueAtomIdentifier,UniqueAtomIdentifier> UAtomIdentifiers;
typedef std::map<std::pair<UniqueAtomIdentifier,UniqueAtomIdentifier>,std::pair<float,float> > ResidueRDMap;
typedef std::map<ost::mol::ResNum,ResidueRDMap> GlobalRDMap;
typedef std::map<UniqueAtomIdentifier,int> ExistenceMap;

Real DLLEXPORT_OST_MOL_ALG LocalDistTest(const EntityView& mdl,
                                         const GlobalRDMap& dist_list,
                                         std::vector<Real> cutoff_list, 
                                         const String& local_ldt_property_string="");

Real DLLEXPORT_OST_MOL_ALG LocalDistTest(const EntityView& mdl,
                                         const EntityView& target,
                                         Real cutoff_list, 
                                         Real max_dist,
                                         const String& local_ldt_property_string="");

Real DLLEXPORT_OST_MOL_ALG LocalDistTest(const ost::seq::AlignmentHandle& aln,
                                         Real cutoff, Real max_dist,
                                         int ref_index=0, int mdl_index=1);


Real DLLEXPORT_OST_MOL_ALG LDTHA(EntityView& v, const GlobalRDMap& global_dist_list);
Real DLLEXPORT_OST_MOL_ALG OldStyleLDTHA(EntityView& v, const GlobalRDMap& global_dist_list);


GlobalRDMap CreateDistanceList(const EntityView& ref,Real max_dist);
GlobalRDMap CreateDistanceListFromMultipleReferences(const std::vector<EntityView>& ref_list,Real cutoff, Real max_dist);
void PrintGlobalRDMap(const GlobalRDMap& glob_dist_list);
void PrintResidueRDMap(const ResidueRDMap& res_dist_list);
bool IsStandardResidue(String rn);

}}}

#endif


