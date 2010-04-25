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
#include "sanitizer.hh"
#include <ost/mol/impl/atom_impl.hh>
#include <ost/log.hh>
#include <limits>
#include <ost/mol/impl/residue_impl.hh>
namespace ost { namespace conop {


Sanitizer::Sanitizer(const CompoundLibPtr& lib)
  :compound_lib_(lib) {
}
bool Sanitizer::VisitAtom(const mol::AtomHandle& atom) {
  return true;
}

bool Sanitizer::VisitResidue(const mol::ResidueHandle& residue) {
  if (CompoundPtr comp=this->IdentifyResidue(residue)) {
    this->ReorderAtoms(residue, comp);
    this->VerifyCompleteness(residue, comp);
    return true;
  }
  LOGN_ERROR("Unknown residue with name " << residue.GetName());
  return false;
}

void Sanitizer::FillAtomProps(mol::AtomHandle atom, const AtomSpec& spec) {
  mol::AtomProp props=atom.GetAtomProps();
  if (props.element!=spec.element) {
    props.element=spec.element;
    LOGN_MESSAGE("Correcting element for " << atom.GetQualifiedName() <<
                 " (now " << spec.element << ", was " << props.element << ")");
    atom.SetAtomProps(props);
  }
}

void Sanitizer::VerifyCompleteness(const mol::ResidueHandle& residue,
                                   CompoundPtr compound) {
  AtomSpecList::const_iterator j=compound->GetAtomSpecs().begin();
  mol::AtomHandleList atoms=residue.GetAtomList();
  mol::AtomHandleList::iterator i=atoms.begin();
  for (; j!=compound->GetAtomSpecs().end() && i!=atoms.end(); ++j) {
    if ((*j).is_leaving || (*j).element=="H")
      continue;
    if ((*j).ordinal!=static_cast<int>((*i).Impl()->GetState())) {
      this->OnMissingAtom(residue, (*j).name);
    } else {
      this->FillAtomProps(*i, *j);
      ++i;
    }
  }
}

struct OrdinalComp {
  bool operator()(const mol::impl::AtomImplPtr& a,
                  const mol::impl::AtomImplPtr& b) const {
    return a->GetState()<b->GetState();
  }
};

void Sanitizer::ReorderAtoms(const mol::ResidueHandle& residue,
                             CompoundPtr compound) {
  mol::impl::ResidueImplPtr impl=residue.Impl();
  mol::impl::AtomImplList::iterator i=impl->GetAtomList().begin();
  for (; i!=impl->GetAtomList().end(); ++i) {
    mol::impl::AtomImplPtr atom=*i;
    int index=compound->GetAtomSpecIndex(atom->GetName());
    if (index==-1) {

      this->OnUnknownAtom(mol::AtomHandle(atom));
      atom->SetState(std::numeric_limits<int>::max());
      continue;
    }
    atom->SetState((compound->GetAtomSpecs())[index].ordinal);
  }
  std::sort(impl->GetAtomList().begin(), impl->GetAtomList().end(),
            OrdinalComp());
}

bool Sanitizer::VisitChain(const mol::ChainHandle& chain) {
  return true;
}

CompoundPtr Sanitizer::IdentifyResidue(const mol::ResidueHandle& residue) {
  CompoundPtr comp=compound_lib_->FindCompound(residue.GetName());
  if (!comp) {
    return this->OnUnknownResidue(residue);
  }
  return comp;
}
}}
