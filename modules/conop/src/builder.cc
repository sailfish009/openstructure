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
#include "builder.hh"

/*
   Author: Marco Biasini
 */

namespace ost { namespace conop {

Builder::~Builder() {}

void Builder::CompleteAtoms(mol::ResidueHandle rh) {}

void Builder::CheckResidueCompleteness(const mol::ResidueHandle& rh) {}

mol::ResidueKey Builder::IdentifyResidue(const mol::ResidueHandle& rh) {
  return mol::ResidueKey();
}
void Builder::FillResidueProps(mol::ResidueHandle residue){};
void Builder::ConnectAtomsOfResidue(mol::ResidueHandle rh) {}

void Builder::ConnectResidueToPrev(mol::ResidueHandle rh, 
                                   mol::ResidueHandle p) {
  this->ConnectResidueToNext(p, rh);
}

bool Builder::IsResidueComplete(const mol::ResidueHandle& rh)
{
  return true;
}

void Builder::ConnectResidueToNext(mol::ResidueHandle rh, 
                                   mol::ResidueHandle n) {}

void Builder::AssignTorsions(mol::ChainHandle ch) {}
void Builder::AssignTorsionsToResidue(mol::ResidueHandle rh) {}

void Builder::FillAtomProps(mol::AtomHandle atom) {}

bool Builder::DoesPeptideBondExist(const mol::AtomHandle& n, 
                                   const mol::AtomHandle& c)
{
  return this->IsBondFeasible(n, c);
}

bool Builder::IsBondFeasible(const mol::AtomHandle& atom_a,
                             const mol::AtomHandle& atom_b)
{
  Real radii=0.0;
  if (atom_a.GetRadius()>0.0) {
    radii=atom_a.GetRadius();
  } else {
    return false;
  }
  if (atom_b.GetRadius()>0.0) {
    radii+=atom_b.GetRadius();
  } else {
    return false;
  } 
  Real len=geom::Length2(atom_a.GetPos()-atom_b.GetPos());
  Real lower_bound=radii*radii*0.0625;
  Real upper_bound=lower_bound*6.0;
  return (len<=upper_bound && len>=lower_bound);
}

String Builder::GuessAtomElement(const String& aname, bool hetatm)
{
  static String l1[] = {
    "H","C","N","O","P","S","K"
  };
  static int l1c=7;
  static String l2[] = {
    "NA","MG","AL","SI","CL",
    "CA","CR","MN","FE","CO","NI","CU","ZN","AS","SE","BR","MO"
  };
  static int l2c=17;
  static String l3[] = {
    "B","F","V"
  };
  static int l3c=3;
  static String l4[] = {
    "HE","LI","BE","AR","SC","TI","GA","GE","KR"
  };
  static int l4c=9;

  String ele=aname.substr(0,2);
  // hydrogen hack
  if(ele[0]=='H') {
    return "H";
  }
  if (hetatm==false) {
    if (ele=="CA" || ele=="CB") {
      return "C";
    }
  }

  // two characters
  if(aname.size()==2) {
    for(int i=0;i<l2c;i++) {
      if(ele==l2[i]) return ele;
    }
    // check second character for match
    for(int i=0;i<l1c;i++) {
      if(ele[1]==l1[i][0]) {
        return l1[i];
      }
    }
    // still no match, repeat with less likely tables
    for(int i=0;i<l4c;i++) {
      if(ele==l4[i]) return ele;
    }
    // check second character for match
    for(int i=0;i<l3c;i++) {
      if(ele[1]==l3[i][0]) {
        return l3[i];
      }
    }
    // check second character for match
    for(int i=0;i<l1c;i++) {
      if(ele[0]==l1[i][0]) {
        return l1[i];
      }
    }
  } else {
    for(int i=0;i<l1c;i++) {
      if(ele==l1[i]) return ele;
    }
    for(int i=0;i<l3c;i++) {
      if(ele==l3[i]) return ele;
    }
  }
  size_t i=0;
  while (i<aname.size() && isdigit(aname[i])) {
    ++i;
  }
  return i<aname.size() ? String(1, aname[i]) : "";
}

bool Builder::AreResiduesConsecutive(const mol::ResidueHandle& r1, 
                                     const mol::ResidueHandle& r2)
{
  if (!r1 || !r2) return false; // protect against invalid handles
  if(r1.GetChain() != r2.GetChain()) return false;
  return r2.GetNumber().GetNum()==r1.GetNumber().GetNum()+1 ||
         r2.GetNumber().GetInsCode()==r1.GetNumber().NextInsertionCode().GetInsCode();
}

void Builder::GuessChemClass(mol::ResidueHandle res)
{
  // try peptide
  res.SetChemClass(mol::ChemClass());
  mol::AtomHandle ca=res.FindAtom("CA");
  if (!ca.IsValid() || ca.GetElement()!="C") return;
  mol::AtomHandle n=res.FindAtom("N");
  if (!n.IsValid() || n.GetElement()!="N") return;
  mol::AtomHandle c=res.FindAtom("C");
  if (!c.IsValid() || c.GetElement()!="C") return;
  mol::AtomHandle o=res.FindAtom("O");
  if (!o.IsValid() || o.GetElement()!="O") return;
  if (this->IsBondFeasible(n, ca) && this->IsBondFeasible(ca, c) &&
      this->IsBondFeasible(c, o)) {
    res.SetChemClass(mol::ChemClass(mol::ChemClass::PEPTIDE_LINKING));
  }
}


void Builder::AssignBackBoneTorsionsToResidue(mol::ResidueHandle res)
{

  mol::ResidueHandle prev=res.GetPrev();
  mol::ResidueHandle next=res.GetNext();
  mol::XCSEditor e=res.GetEntity().EditXCS(mol::BUFFERED_EDIT);
  //psi
  if (next.IsValid() && next.IsPeptideLinking()){
    mol::AtomHandle ca_this=res.FindAtom("CA");
    mol::AtomHandle n_this=res.FindAtom("N");
    mol::AtomHandle c_this=res.FindAtom("C");
    mol::AtomHandle n_next=next.FindAtom("N");
    if ((ca_this && n_this && c_this && n_next &&  BondExists(c_this, n_next))
        && !res.GetPsiTorsion()) {
      e.AddTorsion("PSI", n_this, ca_this, c_this, n_next);
    }
  };
  //phi
  if (prev.IsValid() && prev.IsPeptideLinking()) {
    mol::AtomHandle c_prev=prev.FindAtom("C");
    mol::AtomHandle n_this=res.FindAtom("N");
    mol::AtomHandle ca_this=res.FindAtom("CA");
    mol::AtomHandle c_this=res.FindAtom("C");
    if ((c_prev && n_this && ca_this && c_this && BondExists(c_prev, n_this))
        && !res.GetPhiTorsion()) {
      e.AddTorsion("PHI", c_prev, n_this, ca_this, c_this);
    }
  }
  //omega
  if (prev.IsValid() && prev.IsPeptideLinking()) {
    mol::AtomHandle ca_prev=prev.FindAtom("CA");
    mol::AtomHandle c_prev=prev.FindAtom("C");
    mol::AtomHandle n=res.FindAtom("N");
    mol::AtomHandle ca=res.FindAtom("CA");
    if ((ca_prev && c_prev && n && ca && BondExists(c_prev, n))
        && !res.GetOmegaTorsion()) {
      e.AddTorsion("OMEGA",ca_prev , c_prev, n, ca);
    }
  }
}



void Builder::DistanceBasedConnect(mol::AtomHandle atom)
{
  mol::EntityHandle ent=atom.GetEntity();
  mol::XCSEditor editor=ent.EditXCS(mol::BUFFERED_EDIT);
  mol::AtomHandleList alist = ent.FindWithin(atom.GetPos(),4.0);
  mol::ResidueHandle res_a=atom.GetResidue();
  for (mol::AtomHandleList::const_iterator it=alist.begin(),
       e=alist.end();it!=e;++it) {
    if (*it!=atom) {
      if (this->IsBondFeasible(atom, *it)) {
        if (Builder::AreResiduesConsecutive(res_a, it->GetResidue()) || 
            it->GetResidue()==res_a) {                      
            editor.Connect(*it, atom);
        }
      }
    }
  }
}

}} // ns
