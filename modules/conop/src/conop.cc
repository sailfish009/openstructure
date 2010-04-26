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
#include <ost/mol/mol.hh>
#include <ost/log.hh>

#include "conop.hh"
#include "heuristic_builder.hh"
#include <ost/profile.hh>

namespace ost { namespace conop {

// Meyers singleton implementation
Conopology& Conopology::Instance()
{
  static Conopology impl;
  return impl;
}

Conopology::Conopology():
  builder_map_(),
  ele_rad_map_(),
  ele_mass_map_()
{
  builder_map_["HEURISTIC"]=BuilderP(new HeuristicBuilder());
  builder_map_["DEFAULT"]=builder_map_["HEURISTIC"];

  ele_rad_map_["H"]  = 1.09;

  ele_rad_map_["C"]  = 1.75;
  ele_rad_map_["N"]  = 1.55;
  ele_rad_map_["O"]  = 1.52;
  ele_rad_map_["F"]  = 1.47;

  ele_rad_map_["Na"] = 2.27;
  ele_rad_map_["Mg"] = 1.73;
  ele_rad_map_["Si"] = 2.10;
  ele_rad_map_["P"]  = 1.80;
  ele_rad_map_["S"]  = 1.80;
  ele_rad_map_["Cl"] = 1.75;

  ele_rad_map_["K"]  = 2.75;
  ele_rad_map_["Ca"] = 2.00;
  ele_rad_map_["Mn"] = 2.00;
  ele_rad_map_["Fe"] = 2.00;
  ele_rad_map_["Co"] = 2.00;
  ele_rad_map_["Ni"] = 1.63;
  ele_rad_map_["Cu"] = 1.40;
  ele_rad_map_["Zn"] = 1.39;
  ele_rad_map_["Br"] = 1.85;

  ele_rad_map_["I"]  = 1.98;


  // Element masses according to IUPAC standards
  // see http://www.chem.qmul.ac.uk/iupac/AtWt/
  ele_mass_map_["H"]  = 1.00794;
  ele_mass_map_["He"] = 4.002602;
  ele_mass_map_["Li"] = 6.941;
  ele_mass_map_["Be"] = 9.012182;
  ele_mass_map_["B"]  = 10.811;
  ele_mass_map_["C"]  = 12.0107;
  ele_mass_map_["N"]  = 14.0067;
  ele_mass_map_["O"]  = 15.9994;
  ele_mass_map_["F"]  = 18.9984032;
  ele_mass_map_["Ne"] = 20.1797;
  ele_mass_map_["Na"] = 22.98976928;
  ele_mass_map_["Mg"] = 24.3050;
  ele_mass_map_["Al"] = 26.9815386;
  ele_mass_map_["Si"] = 28.0855;
  ele_mass_map_["P"]  = 30.973762;
  ele_mass_map_["S"]  = 32.065;
  ele_mass_map_["Cl"] = 35.453;
  ele_mass_map_["Ar"] = 39.948;
  ele_mass_map_["K"]  = 39.0983;
  ele_mass_map_["Ca"] = 40.078;
  ele_mass_map_["Sc"] = 44.955912;
  ele_mass_map_["Ti"] = 47.867;
  ele_mass_map_["V"]  = 50.9415;
  ele_mass_map_["Cr"] = 51.9961;
  ele_mass_map_["Mn"] = 54.938045;
  ele_mass_map_["Fe"] = 55.845;
  ele_mass_map_["Co"] = 58.933195;
  ele_mass_map_["Ni"] = 58.6934;
  ele_mass_map_["Cu"] = 63.546;
  ele_mass_map_["Zn"] = 65.38;
  ele_mass_map_["Ga"] = 69.723;
  ele_mass_map_["Ge"] = 72.64;
  ele_mass_map_["As"] = 74.92160;
  ele_mass_map_["Se"] = 78.96;
  ele_mass_map_["Br"] = 79.904;
  ele_mass_map_["Kr"] = 83.798;
  ele_mass_map_["Rb"] = 85.4678;
  ele_mass_map_["Sr"] = 87.62;
  ele_mass_map_["Y"]  = 88.90585;
  ele_mass_map_["Zr"] = 91.224;
  ele_mass_map_["Nb"] = 92.90638;
  ele_mass_map_["Mo"] = 95.96;
  ele_mass_map_["Tc"] = 98.0;
  ele_mass_map_["Ru"] = 101.07;
  ele_mass_map_["Rh"] = 102.90550;
  ele_mass_map_["Pd"] = 106.42;
  ele_mass_map_["Ag"] = 107.8682;
  ele_mass_map_["Cd"] = 112.411;
  ele_mass_map_["In"] = 114.818;
  ele_mass_map_["Sn"] = 118.710;
  ele_mass_map_["Sb"] = 121.760;
  ele_mass_map_["Te"] = 127.60;
  ele_mass_map_["I"]  = 126.90447;
  ele_mass_map_["Xe"] = 131.293;
  ele_mass_map_["Cs"] = 132.9054519;
  ele_mass_map_["Ba"] = 137.327;
  ele_mass_map_["La"] = 138.90547;
  ele_mass_map_["Ce"] = 140.116;
  ele_mass_map_["Pr"] = 140.90765;
  ele_mass_map_["Nd"] = 144.242;
  ele_mass_map_["Pm"] = 145.0;
  ele_mass_map_["Sm"] = 150.36;
  ele_mass_map_["Eu"] = 151.964;
  ele_mass_map_["Gd"] = 157.25;
  ele_mass_map_["Tb"] = 158.92535;
  ele_mass_map_["Dy"] = 162.500;
  ele_mass_map_["Ho"] = 164.93032;
  ele_mass_map_["Er"] = 167.259;
  ele_mass_map_["Tm"] = 168.93421;
  ele_mass_map_["Yb"] = 173.054;
  ele_mass_map_["Lu"] = 174.9668;
  ele_mass_map_["Hf"] = 178.49;
  ele_mass_map_["Ta"] = 180.94788;
  ele_mass_map_["W"]  = 183.84;
  ele_mass_map_["Re"] = 186.207;
  ele_mass_map_["Os"] = 190.23;
  ele_mass_map_["Ir"] = 192.217;
  ele_mass_map_["Pt"] = 195.084;
  ele_mass_map_["Au"] = 196.966569;
  ele_mass_map_["Hg"] = 200.59;
  ele_mass_map_["Tl"] = 204.3833;
  ele_mass_map_["Pb"] = 207.2;
  ele_mass_map_["Bi"] = 208.98040;
  ele_mass_map_["Po"] = 209.0;
  ele_mass_map_["At"] = 210.0;
  ele_mass_map_["Rn"] = 222.0;
  ele_mass_map_["Fr"] = 223.0;
  ele_mass_map_["Ra"] = 226.0;
  ele_mass_map_["Ac"] = 227.0;
  ele_mass_map_["Th"] = 232.03806;
  ele_mass_map_["Pa"] = 231.03588;
  ele_mass_map_["U"]  = 238.02891;
  ele_mass_map_["Np"] = 237.0;
  ele_mass_map_["Pu"] = 244.0;
  ele_mass_map_["Am"] = 243.0;
  ele_mass_map_["Cm"] = 247.0;
  ele_mass_map_["Bk"] = 247.0;
  ele_mass_map_["Cf"] = 251.0;
  ele_mass_map_["Es"] = 252.0;
  ele_mass_map_["Fm"] = 257.0;
  ele_mass_map_["Md"] = 258.0;
  ele_mass_map_["No"] = 259.0;
  ele_mass_map_["Lr"] = 262.0;
  ele_mass_map_["Rf"] = 267.0;
  ele_mass_map_["Db"] = 268.0;
  ele_mass_map_["Sg"] = 271.0;
  ele_mass_map_["Bh"] = 272.0;
  ele_mass_map_["Hs"] = 270.0;
  ele_mass_map_["Mt"] = 276.0;
  ele_mass_map_["Ds"] = 281.0;
  ele_mass_map_["Rg"] = 280.0;
  ele_mass_map_["Cn"] = 285.0;
  ele_mass_map_["Uut"]= 284.0;
  ele_mass_map_["Uuq"]= 289.0;
  ele_mass_map_["Uup"]= 288.0;
  ele_mass_map_["Uuh"]= 293.0;
  ele_mass_map_["Uuo"]= 294.0;  
}

void Conopology::RegisterBuilder(const BuilderP& b, const String& name) {
  if (!GetBuilder(name))
  builder_map_[name]=b;
}
void Conopology::SetDefaultBuilder(const String& default_name) {
  BuilderP builder=GetBuilder(default_name);
  if (builder)
    builder_map_["DEFAULT"]=builder;
  else
    assert(0 && "what is this?");
}

BuilderP Conopology::GetBuilder(const String& name)
{
  BuilderMap::iterator it=builder_map_.find(name);
  if (it!=builder_map_.end()){
    return (*it).second;
  }
  return BuilderP();
}

namespace {

class PropAssigner: public mol::EntityVisitor {
public:
  PropAssigner(const BuilderP& builder): builder_(builder) {}
  virtual bool VisitResidue(const mol::ResidueHandle& res)
  {
    String key=builder_->IdentifyResidue(res);
    builder_->CheckResidueCompleteness(res);
    builder_->FillResidueProps(res);
    return true;
  }
  
  virtual bool VisitAtom(const mol::AtomHandle& atom)
  {
    builder_->FillAtomProps(atom);
    return false;
  }
private:
  BuilderP builder_;
};


class Connector: public mol::EntityVisitor
{
public:
  Connector(const BuilderP& b):
    builder_(b),
    prev_()
  {}

  virtual bool VisitChain(const mol::ChainHandle& chain) {
    prev_=mol::ResidueHandle(); // reset prev
    return true;
  }

  virtual bool VisitResidue(const mol::ResidueHandle& res) {
    builder_->ConnectAtomsOfResidue(res);
    if(prev_) {
        builder_->ConnectResidueToPrev(res,prev_);
    }
    prev_=res;
    return false;
  }

private:
  BuilderP builder_;
  mol::ResidueHandle prev_;
};

class TorsionMaker: public mol::EntityVisitor
{
public:
  TorsionMaker(const BuilderP& b):
    builder_(b)
  {}

  virtual bool VisitChain(const mol::ChainHandle& chain) {
    builder_->AssignTorsions(chain);
    return false;
  }

private:
  BuilderP builder_;
};
} // ns

void Conopology::ConnectAll(const BuilderP& b, mol::EntityHandle eh, int flag)
{
  Profile profile_connect("ConnectAll");
  LOGN_DEBUG("Conopology: ConnectAll: building internal coordinate system");
  mol::XCSEditor xcs_e=eh.RequestXCSEditor(mol::BUFFERED_EDIT);
  PropAssigner a(b);
  eh.Apply(a);
  LOGN_DUMP("Conopology: ConnectAll: connecting all bonds");
  Connector connector(b);
  eh.Apply(connector);

  LOGN_DUMP("Conopology: ConnectAll: assigning all torsions");
  TorsionMaker tmaker(b);
  eh.Apply(tmaker);
}

Real Conopology::GetDefaultAtomRadius(const String& element) const
{
  std::map<String,Real>::const_iterator it = ele_rad_map_.find(element);
  return it==ele_rad_map_.end() ? 1.5 : it->second;
}

Real Conopology::GetDefaultAtomMass(const String& element) const
{
  std::map<String,Real>::const_iterator it = ele_mass_map_.find(element);
  return it==ele_mass_map_.end() ? 1.0 : it->second;
}


}} // ns
