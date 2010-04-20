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
#ifndef OST_ATOM_BASE_HH
#define OST_ATOM_BASE_HH

#include <ost/mol/module_config.hh>
#include <ost/geom/vec3.hh>
#include <ost/mol/impl/atom_impl_fw.hh>
#include <ost/generic_property.hh>

#include "property_id.hh"

namespace ost { namespace mol {

/// \brief base class for AtomHandle and AtomView
///
/// Groups common functionality for AtomView and AtomHandle.
///
/// Most of the atom properties such as temperature factor, element, radius
/// and charge can be accessed with #GetProp() that returns an object of
/// type AtomProp.
///
/// Like \ref ResidueBase residues, \ref ChainBase "chains" and 
/// \ref EntityBase "entities" atoms are generic property containers and may as 
/// such store \ref generic_properties "custom properties".
///
/// The position of the atoms can be accessed via #GetPos(). Note, however
/// that the position of an atom is undefined when there are pending changes
/// of an ICSEditor in buffered edit mode. Before calling #GetPos(),
/// ICSEditor::UpdateXCS() should be called explicitly.
class DLLEXPORT_OST_MOL AtomBase: public GenericPropertyContainer<AtomBase> {
public:
  AtomBase();
  AtomBase(const impl::AtomImplPtr& impl);
public:  
  friend class GenericPropertyContainer<AtomBase>;  
  ///\brief Get atom name. 
  ///
  /// In Python the atom name may also be accesssed over the property \c Name
  const String& GetName() const;
  
  /// \brief Set atom name
  ///
  /// You are free to set the atom name to whatever you want. You must be aware
  /// that certain algorithms and routines rely on proper naming of the atoms.
  /// It is thus advised to use IUPAC naming rules.
  ///
  /// \param atom_name is the new atom name.
  ///
  /// \sa      AtomBase::GetName
  void SetName(const String& atom_name);
    
  //! \brief Get global position in cartesian coordinates with entity transformations applied
  const geom::Vec3& GetPos() const;

  //! \brief Get original global position in cartesian coordinates (no entity transformation is applied)
  const geom::Vec3& GetOriginalPos() const;
  /// \brief get alternative atom position
  geom::Vec3 GetAltPos(const String& alt_group) const;
  
  std::vector<String> GetAltGroupNames() const;
    /// \name Atom properties
  //@{
  /// \brief Get atom properties such as element name, radius crystallographic
  ///     occupancy and temperature factors.
  ///
  /// \sa      #SetProp
  const AtomProp& GetProp() const;
    
  /// \brief  Set atom properties.
  void SetProp(const AtomProp& prop);
  //@} 
  
  /// \name Handle validity
  //@{
  /// \brief check validity of handle
  /// 
  /// check, whether the bond handle points to a valid atom.
  /// \note It is an error to use any method other than #IsValid, Impl and 
  ///       #operator bool() when the handle is invalid. An InvalidHandle
  ///       exception will be thrown.
  operator bool() const;
  /// \brief check validity of handle
  /// \sa #operator bool()
  bool IsValid() const;
  //@}
  
  /// \brief Get qualified name for atom.
  /// 
  /// The qualified name consists of the atom name as well as a unique residue
  /// identifier and chain name. For CA of a glycine with residue number 2 of
  /// chain A, the qualified name is "A.GLY2.CA".
  String GetQualifiedName() const;
  
  /// \brief Get String property by id  
  String GetStringProperty(Prop::ID prop_id) const;
                          
  /// \brief Get float property by id                          
  Real GetFloatProperty(Prop::ID prop_id) const;
  
  /// \brief Get int property by id
  int GetIntProperty(Prop::ID prop_id) const;
public:
  /// \brief get atom implementation.
  ///
  /// Intended for internal use.
  const impl::AtomImplPtr& Impl() const;
  
  /// \brief get atom implementation
  impl::AtomImplPtr& Impl();

  /// \brief Get unique identifier for atom
  /// 
  /// Get hash code that uniquely identifies every atom. The hash code is
  /// identical for all atom views pointing to a given atom.
  long GetHashCode() const;  
protected:
  
  GenericPropertyContainerImpl* GpImpl();
  
  const GenericPropertyContainerImpl* GpImpl() const;
  
  void CheckValidity() const;
  impl::AtomImplPtr   impl_;
};

DLLEXPORT_OST_MOL std::ostream& operator<<(std::ostream& os, 
                                            const AtomBase& atom);
}} // ns

#endif // OST_ATOM_BASE_HH

