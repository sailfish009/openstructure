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
#ifndef OST_RESIDUE_BASE_HH
#define OST_RESIDUE_BASE_HH

#include <ost/mol/module_config.hh>
#include <ost/mol/residue_prop.hh>
#include <ost/mol/impl/residue_impl_fw.hh>
#include <ost/mol/sec_structure.hh>
#include <ost/mol/handle_type_fw.hh>
#include <ost/mol/chem_class.hh>

#include <ost/generic_property.hh>
#include "property_id.hh"

namespace ost { namespace mol {

/// \brief base class for ResidueHandle and ResidueView    
///
/// This class implements the common functionality for both ResidueHandle and 
/// ResidueView.
///
/// Like \ref AtomHandle "atoms" and \ref ChainHandle "chains", residues are generic
/// property containers and as such can store user-defined properties.
///
/// Each residue has a \ref ResNum "residue number" that uniquely 
/// identifies the residue in a chain. This number consists of a numeric part and
/// an optional alphanumeric insertion code.
///
/// Residues have names that are usually 3 characters long. The most common residue
/// types such as the 20 standard amino acids and nucleotides also have a defined
/// \ref #GetOneLetterCode "one-letter code". For other residues, the one-letter
/// code is set to \c ?. 
///
/// Residues have a \ref ChemClass "chemical class". For standard amino acids this
/// class is ChemClass::LPeptideLinking, indicating that the residue is capable to
/// participate in a peptide bond. For nucleotides, the chemical class is either
/// ChemClass::RNALinking or ChemClass::DNALinking. For unknown compounds the 
/// chemical class is ChemClass::Unknown.
/// 
/// When loading an entity from file, the one-letter and chemical class of a
/// residue are assigned by the \ref conop::Builder "default builder".
class DLLEXPORT_OST_MOL ResidueBase: 
   public GenericPropertyContainer<ResidueBase> {
public:
  ResidueBase();
  ResidueBase(const impl::ResidueImplPtr& impl);
  ResidueBase(const ResidueBase& rhs);
public:
  friend class ConstGenericPropertyContainer<ResidueBase>;
  /// \brief return residue number
  const ResNum& GetNumber() const;

  /// \brief return the residue key.
  const ResidueKey& GetKey() const;

  /// \brief return the residue name, at the moment an alias for GetKey().
  const String& GetName() const;

  /// \brief  get fully qualified name of residue, including the chain name,
  ///         residue name and residue number
  String GetQualifiedName() const;

  /// \name Handle validity
  //@{
  /// \brief check validity of handle
  ///
  /// check, whether the residue handle points to a valid residue.
  /// \note It is an error to use any method other than #IsValid, #Impl and
  ///       #operator bool() when the handle is invalid. An InvalidHandle
  ///       exception will be thrown.
  operator bool() const;
  /// \brief check validity of handle
  /// \sa #operator bool()
  bool IsValid() const;
  //@}
  
  /// \brief whether the residue can form peptide bonds
  bool IsPeptideLinking() const;

  /// \brief whether the residue can be part of a polynucleotide
  bool IsNucleotideLinking() const;
  
  ChemClass GetChemClass() const;
  
  void SetChemClass(ChemClass cc);
  
  /// \brief    Get secondary structure type. By default, the residue is in COIL
  ///           conformation.
  SecStructure GetSecStructure() const;

  /// \brief      Set secondary structure type
  ///
  /// \sa        ResidueHandle::GetSecStructure
  void SetSecStructure(SecStructure ss);

  /// \brief  Get phi torsion
  /// 
  ///  If no phi torsion has been assigned to this residue an invalid
  /// TorsionHandle will be returned.
  TorsionHandle GetPhiTorsion() const;

  /// \brief Get psi torsion
  ///
  /// If no psi torsion has been assigned to this residue an invalid TorsionHandle
  /// will be returned.
  TorsionHandle GetPsiTorsion() const;

  /// \brief Get omega torsion
  ///
  /// If no omega torsion has been assigned to this residue an invalid TorsionHandle
  /// will be returned.
  TorsionHandle GetOmegaTorsion() const;
  char GetOneLetterCode() const;

  void SetOneLetterCode(char olc);
  
  /// \brief Get String property by id  
  String GetStringProperty(Prop::ID prop_id) const;
                          
  /// \brief Get float property by id                          
  Real GetFloatProperty(Prop::ID prop_id) const;
  
  /// \brief Get int property by id
  int GetIntProperty(Prop::ID prop_id) const;
    
public:
  impl::ResidueImplPtr& Impl();

  const impl::ResidueImplPtr& Impl() const;
protected:
  GenericPropertyContainerImpl* GpImpl();
  
  const GenericPropertyContainerImpl* GpImpl() const;  
  void CheckValidity() const;
private:
  impl::ResidueImplPtr impl_;

};

DLLEXPORT_OST_MOL std::ostream& operator<<(std::ostream& os, 
                                            const ResidueBase& residue);
}} // ns

#endif // OST_RESIDUE_BASE_HH

