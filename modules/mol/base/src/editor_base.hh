//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2010 by the OpenStructure authors
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
#ifndef OST_EDITOR_BASE_HS
#define OST_EDITOR_BASE_HS
/*
  Author: Marco Biasini
 */
 
#include <ost/mol/module_config.hh>

#include <ost/geom/geom.hh>

#include "entity_handle.hh"
#include "residue_prop.hh"
#include "editor_type_fw.hh"

namespace ost { namespace mol {

/// \brief base class for entity editors
///
/// Entities, chains, residues and atoms are assembled and modified via 
/// editors.
/// Editors are usually used as one of the concrete subclasses of EditorBase:
/// XCSEditor and ICSEditor. The former operates on the external carthesian
/// coordinate system while the latter operates on the internal coordinate
/// system. To create a new editor, use EntityHandle::EditXCS() and
/// EntityHandle::EditICS(), respectively.
/// 
/// For an introduction to the concept of editors go \ref editors "here"
class DLLEXPORT_OST_MOL EditorBase {
public:
  /// \brief create new chain
  ///
  /// create new chain and attach it to entity.
  ///
  /// \param chain_name
  ///          The chain name. Can be an arbitrary String. However, if you
  ///          intend to use the PDB export funtionality, the \c chain_name
  ///          should be a single letter, preferably in the range A-Z.
  ///
  /// \return  The newly created chain handle
  ChainHandle InsertChain(const String& chain_name);

  /// \name Inserting, removing and modifying order of residues
  ///@{
  /// \brief Append residue to the end of the chain
  /// 
  /// Append residue with residue key \var k to the end of the chain. The 
  /// residue number will be set to the residue number of the last added residue
  /// plus one but with the alternative position identifier left unchanged.
  /// The following code will append a new residue with key GLY:
  /// \code
  /// ResidueHandle gly=chain.AppendResidue("GLY");
  /// \endcode
  /// \note     The created residue is empty and does not contain any atoms.
  /// \return   The newly created residue
  /// \sa       #InsertResidueBefore, EntityHandle::InsertChain, CreateEntity,
  ///           ResidueHandle::InsertAtom.
  ResidueHandle AppendResidue(ChainHandle chain, const ResidueKey& k);
  
  ResidueHandle AppendResidue(ChainHandle chain, const ResidueKey& k, 
                              const ResNum& num); 
  /// \brief  Insert residue into chain
  /// 
  /// Insert residue with residue number \var num and key \var k into the
  /// chain.
  /// \code
  /// EntityHandle e=CreateEntity();
  /// ChainHandle chain=e.InsertChain("A");
  /// ResidueHandle first=chain.AppendResidue("MET");
  /// ResNum number=first.GetNumber();  
  /// //the next two lines have the same effect  
  /// chain.AppendResidue("GLY");
  /// chain.InsertResidueBefore("GLY, "++number)
  /// \endcode
  /// \sa     #AppendResidue, EntityHandle::InsertChain, CreateEntity,
  ///         ResidueHandle::InsertAtom.
  ResidueHandle InsertResidueBefore(ChainHandle chain, int index, 
                                    const ResNum& num,
                                    const ResidueKey& k);
  /// \brief insert residue into chain
  /// 
  /// This method is identical to InsertResidueBefore() but inserts the
  /// residue after the specified position
  /// \sa #InsertResidueBefore
  ResidueHandle InsertResidueAfter(ChainHandle chain, int index,
                                   const ResNum& num,
                                   const ResidueKey& k);
  
  /// \name Adding/removing atoms
  //@{
  /// \brief  Insert new atom
  ///
  /// Insert new atom and add it to residue. For atoms with alternative atom
  /// locations use ResidueHandle::InsertAltAtom().
  /// \param residue is the parent residue
  /// \param name is the atom name. While free to choose a name, it is adviced 
  ///     to properly name the residues according to IUPAC rules as several
  ///     algorithms as well as most builders in the conop module rely on proper 
  ///     naming.
  /// \param pos is the position of the atom in global coordinates
  AtomHandle InsertAtom(ResidueHandle residue, const String& name, 
                        const geom::Vec3& pos, const String& ele="");

  /// \brief Insert new atom with alternative position indicator
  /// \sa EditorBase::AddAltAtomPos(), ResidueHandle
  AtomHandle InsertAltAtom(ResidueHandle residue, const String& name, 
                           const String& alt_group, const geom::Vec3& pos,
                           const String& ele="");
  /// \brief  Add alternative atom position
  /// \param group is the name of the alternative atom position group. If no 
  ///     group of that name exists, it will be created.
  /// \param atom is the atom handle the additional location belongs to. It is
  ///     required that the atom has been inserted via InsertAltAtom, If the
  ///     atom is a conventional atom without alternative location, a
  ///     Error will be thrown.
  /// \param position
  ///         is the alternative position
  /// \sa EditorBase::InsertAltAtom(), ResidueHandle
  void AddAltAtomPos(const String& group, const AtomHandle& atom, 
                     const geom::Vec3& position);
  //\}
  
  /// \brief connect two atoms with bond
  ///
  /// If there is already a bond between the two atoms, no new bond is created.
  /// Instead, the already existing bond is returned.
  BondHandle Connect(const AtomHandle& first, const AtomHandle& second);
  BondHandle Connect(const AtomHandle& first, const AtomHandle& second,
                     unsigned char bond_order);
  BondHandle Connect(const AtomHandle& first, const AtomHandle& second,
                     Real len, Real theta, Real phi);
  BondHandle Connect(const AtomHandle& first, const AtomHandle& second,
                     Real len, Real theta, Real phi,
                     unsigned char bond_order);

  void RenameChain(ChainHandle chain, const String& new_name);
  /// \brief   Delete all atoms of residue
  ///
  /// All associated torsions and bonds will also be removed
  void DeleteAllAtoms(ResidueHandle res);
  
  /// \brief  Delete specific atom from this residue
  ///
  ///  All associated torsions and bonds will also be removed as well.
  ///
  /// \param atom
  ///         is the atom to remove. If no such atom exists, this method will
  ///         have no effect
  void DeleteAtom(const AtomHandle& atom);
  
  /// \brief Add named torsion to entity
  TorsionHandle AddTorsion(const String& name, const AtomHandle& a1,
                           const AtomHandle& a2, const AtomHandle& a3,
                           const AtomHandle& a4);  
  /// \brief delete chain
  ///
  /// all associated atoms, residues, torsions and bonds will also be deleted.
  void DeleteChain(const ChainHandle& chain);

  /// \brief Delete residue
  ///
  /// Atoms, bonds and torsions involving this residue are removed as well
  ///
  /// \sa ChainHandle::DeleteResidue, DeleteAtom
  void DeleteResidue(const ResidueHandle& residue_handle);

  /// \brief reorder residues of given chain based on their residue number
  void ReorderResidues(const ChainHandle& chain);

  /// \brief reorder residues of all chains based on their residue number
  void ReorderAllResidues();
    
  /// \brief Get edit mode of editor
  EditMode GetMode() const;
  
  /// \brief change the name of the atom to the new name  
  void RenameAtom(AtomHandle atom, const String& new_name);
protected:
  EditorBase(const EntityHandle& ent, EditMode mode);
  void UpdateTrace();
  EntityHandle     ent_;
  EditMode         mode_;
};
 
}} //ns

#endif
