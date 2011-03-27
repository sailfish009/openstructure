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
#ifndef OST_CHAIN_IMPL_HH
#define OST_CHAIN_IMPL_HH

#include <boost/enable_shared_from_this.hpp>

#include <ost/mol/module_config.hh>
#include <ost/geom/geom.hh>

#include <ost/mol/residue_prop.hh>
#include <ost/mol/impl/chain_impl_fw.hh>
#include <ost/mol/impl/residue_impl_fw.hh>
#include <ost/mol/impl/entity_impl_fw.hh>
#include <ost/mol/entity_visitor_fw.hh>
#include <ost/mol/impl/atom_impl_fw.hh>

#include <ost/generic_property.hh>

#include <ost/mol/sec_structure.hh>

namespace ost { namespace mol {namespace impl {

/// \internal
class ChainImpl: public GenericPropContainerImpl,
                 public boost::enable_shared_from_this<ChainImpl>
{
public:
  ChainImpl(const EntityImplPtr& e, const String& name);

  void SetName(const String& new_name);
  String GetName() const;
  
  /// \brief append new residue with exactly the same parameters as res, but 
  ///     no atoms and bonds                               
  ResidueImplPtr AppendResidue(const ResidueImplPtr& res);
  
  ResidueImplPtr InsertResidueBefore(int index, const ResNum& n, 
                                     const ResidueKey& k);
  ResidueImplPtr InsertResidueAfter(int index, const ResNum& n, 
                                    const ResidueKey& k);
  /// \brief  Append residue at end of chain.
  /// 
  /// Residue number will be automatically set to last+1. The first residue
  /// will have the number 1. insertion code from a previous residue are copied.
  ResidueImplPtr AppendResidue(const ResidueKey& k);

  /// \brief  Append residue at end of chain.
  ResidueImplPtr AppendResidue(const ResidueKey& k, const ResNum& n);
  
  // next residue, not necessarily in sequence
  ResidueImplPtr GetPrev(const ResidueImplPtr& r) const;
  
  /// previous residue, not necessarily in sequence
  ResidueImplPtr GetNext(const ResidueImplPtr& r) const;

  const ResidueImplList& GetResidueList() const;
  /// \brief whether the residue numbers are in ascending order
  bool InSequence() const;
  
  ResidueImplList& GetResidueList() {
    return residue_list_;
  }
  
  Real GetMass() const;
  geom::Vec3 GetCenterOfMass() const;
  geom::Vec3 GetCenterOfAtoms() const;
  
  /// \brief returns the axis-aligned bounding box of the entity
  geom::AlignedCuboid GetBounds() const;
  
  
  ///\brief  Get residue by number. Returns a invalid pointer if the chain
  ///        does not have any residue with this number.
  ResidueImplPtr FindResidue(const ResNum& number) const;
  
  AtomImplPtr FindAtom(const ResNum& number, 
                       const String& atom_name) const;  
                     
  //! Get number of residues of this chain
  int GetResidueCount() const;
  
  //! Get number of atoms of this chain
  int GetAtomCount() const;

  //! Get number of bonds of this chain
  int GetBondCount() const;
      
  void Apply(EntityVisitor& v);

  EntityImplPtr GetEntity() const;
  
  void DeleteResidue(const ResNum& number);
  
  void DeleteResidue(const ResidueImplPtr& residue);
  
  void DeleteAllResidues();

  void ReorderResidues();
  
  int GetIndex(const ResidueImplPtr& res) const;
  void AssignSecondaryStructure(SecStructure ss,
                                const ResNum& start,
                                const ResNum& end); 
  int GetIndexForResNum(const ResNum& number) const;
  
private:
  int GetIndexForResNumInSequence(const ResNum& number) const;
  void UpdateShifts();
  typedef struct {
    int start;
    int shift;
  } Shift;
  std::list<Shift> shifts_;
  EntityImplW      ent_;
  String           name_;
  ResidueImplList  residue_list_;
  /// \brief whether the residue numbers are in ascending order or not. Used
  ///        to optimize residue by number lookup.
  bool             in_sequence_;
};

}}} // ns

#endif

