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
                 
  ele_rad_map_["NA"] = 2.27;
  ele_rad_map_["MG"] = 1.73;
  ele_rad_map_["SI"] = 2.10;
  ele_rad_map_["P"]  = 1.80;
  ele_rad_map_["S"]  = 1.80;
  ele_rad_map_["CL"] = 1.75;
                 
  ele_rad_map_["K"]  = 2.75;
  ele_rad_map_["CA"] = 2.00;
  ele_rad_map_["MN"] = 2.00;
  ele_rad_map_["FE"] = 2.00;
  ele_rad_map_["CO"] = 2.00;
  ele_rad_map_["NI"] = 1.63;
  ele_rad_map_["CU"] = 1.40;
  ele_rad_map_["ZN"] = 1.39;
  ele_rad_map_["BR"] = 1.85;

  ele_rad_map_["I"]  = 1.98;


  // Element masses according to IUPAC standards
  // see http://www.chem.qmul.ac.uk/iupac/AtWt/
  ele_mass_map_["H"]  = 1.00794;
  ele_mass_map_["HE"] = 4.002602;
  ele_mass_map_["LI"] = 6.941;
  ele_mass_map_["BE"] = 9.012182;
  ele_mass_map_["B"]  = 10.811;
  ele_mass_map_["C"]  = 12.0107;
  ele_mass_map_["N"]  = 14.0067;
  ele_mass_map_["O"]  = 15.9994;
  ele_mass_map_["F"]  = 18.9984032;
  ele_mass_map_["NE"] = 20.1797;
  ele_mass_map_["NA"] = 22.98976928;
  ele_mass_map_["MG"] = 24.3050;
  ele_mass_map_["AL"] = 26.9815386;
  ele_mass_map_["SI"] = 28.0855;
  ele_mass_map_["P"]  = 30.973762;
  ele_mass_map_["S"]  = 32.065;
  ele_mass_map_["CL"] = 35.453;
  ele_mass_map_["AR"] = 39.948;
  ele_mass_map_["K"]  = 39.0983;
  ele_mass_map_["CA"] = 40.078;
  ele_mass_map_["SC"] = 44.955912;
  ele_mass_map_["TI"] = 47.867;
  ele_mass_map_["V"]  = 50.9415;
  ele_mass_map_["CR"] = 51.9961;
  ele_mass_map_["MN"] = 54.938045;
  ele_mass_map_["FE"] = 55.845;
  ele_mass_map_["CO"] = 58.933195;
  ele_mass_map_["NI"] = 58.6934;
  ele_mass_map_["CU"] = 63.546;
  ele_mass_map_["ZN"] = 65.38;
  ele_mass_map_["GA"] = 69.723;
  ele_mass_map_["GE"] = 72.64;
  ele_mass_map_["AS"] = 74.92160;
  ele_mass_map_["SE"] = 78.96;
  ele_mass_map_["BR"] = 79.904;
  ele_mass_map_["KR"] = 83.798;
  ele_mass_map_["RB"] = 85.4678;
  ele_mass_map_["SR"] = 87.62;
  ele_mass_map_["Y"]  = 88.90585;
  ele_mass_map_["ZR"] = 91.224;
  ele_mass_map_["NB"] = 92.90638;
  ele_mass_map_["MO"] = 95.96;
  ele_mass_map_["TC"] = 98.0;
  ele_mass_map_["RU"] = 101.07;
  ele_mass_map_["RH"] = 102.90550;
  ele_mass_map_["PD"] = 106.42;
  ele_mass_map_["AG"] = 107.8682;
  ele_mass_map_["CD"] = 112.411;
  ele_mass_map_["IN"] = 114.818;
  ele_mass_map_["SN"] = 118.710;
  ele_mass_map_["SB"] = 121.760;
  ele_mass_map_["TE"] = 127.60;
  ele_mass_map_["I"]  = 126.90447;
  ele_mass_map_["XE"] = 131.293;
  ele_mass_map_["CS"] = 132.9054519;
  ele_mass_map_["BA"] = 137.327;
  ele_mass_map_["LA"] = 138.90547;
  ele_mass_map_["CE"] = 140.116;
  ele_mass_map_["PR"] = 140.90765;
  ele_mass_map_["ND"] = 144.242;
  ele_mass_map_["PM"] = 145.0;
  ele_mass_map_["SM"] = 150.36;
  ele_mass_map_["EU"] = 151.964;
  ele_mass_map_["GD"] = 157.25;
  ele_mass_map_["TB"] = 158.92535;
  ele_mass_map_["DY"] = 162.500;
  ele_mass_map_["HO"] = 164.93032;
  ele_mass_map_["ER"] = 167.259;
  ele_mass_map_["TM"] = 168.93421;
  ele_mass_map_["YB"] = 173.054;
  ele_mass_map_["LU"] = 174.9668;
  ele_mass_map_["HF"] = 178.49;
  ele_mass_map_["TA"] = 180.94788;
  ele_mass_map_["W"]  = 183.84;
  ele_mass_map_["RE"] = 186.207;
  ele_mass_map_["OS"] = 190.23;
  ele_mass_map_["IR"] = 192.217;
  ele_mass_map_["PT"] = 195.084;
  ele_mass_map_["AU"] = 196.966569;
  ele_mass_map_["HG"] = 200.59;
  ele_mass_map_["TL"] = 204.3833;
  ele_mass_map_["PB"] = 207.2;
  ele_mass_map_["BI"] = 208.98040;
  ele_mass_map_["PO"] = 209.0;
  ele_mass_map_["AT"] = 210.0;
  ele_mass_map_["RN"] = 222.0;
  ele_mass_map_["FR"] = 223.0;
  ele_mass_map_["RA"] = 226.0;
  ele_mass_map_["AC"] = 227.0;
  ele_mass_map_["TH"] = 232.03806;
  ele_mass_map_["PA"] = 231.03588;
  ele_mass_map_["U"]  = 238.02891;
  ele_mass_map_["NP"] = 237.0;
  ele_mass_map_["PU"] = 244.0;
  ele_mass_map_["AM"] = 243.0;
  ele_mass_map_["CM"] = 247.0;
  ele_mass_map_["BK"] = 247.0;
  ele_mass_map_["CF"] = 251.0;
  ele_mass_map_["ES"] = 252.0;
  ele_mass_map_["FM"] = 257.0;
  ele_mass_map_["MD"] = 258.0;
  ele_mass_map_["NO"] = 259.0;
  ele_mass_map_["LR"] = 262.0;
  ele_mass_map_["RF"] = 267.0;
  ele_mass_map_["DB"] = 268.0;
  ele_mass_map_["SG"] = 271.0;
  ele_mass_map_["BH"] = 272.0;
  ele_mass_map_["HS"] = 270.0;
  ele_mass_map_["MT"] = 276.0;
  ele_mass_map_["DS"] = 281.0;
  ele_mass_map_["RG"] = 280.0;
  ele_mass_map_["CN"] = 285.0;
  ele_mass_map_["UUT"]= 284.0;
  ele_mass_map_["UUQ"]= 289.0;
  ele_mass_map_["UUP"]= 288.0;
  ele_mass_map_["UUH"]= 293.0;
  ele_mass_map_["UUO"]= 294.0;  
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
  String upper_ele=element;
  std::transform(upper_ele.begin(),upper_ele.end(),upper_ele.begin(),toupper);  
  std::map<String,Real>::const_iterator it = ele_rad_map_.find(upper_ele);
  return it==ele_rad_map_.end() ? 1.5 : it->second;
}

Real Conopology::GetDefaultAtomMass(const String& element) const
{
  String upper_ele=element;
  std::transform(upper_ele.begin(),upper_ele.end(),upper_ele.begin(),toupper);  
  
  std::map<String,Real>::const_iterator it = ele_mass_map_.find(upper_ele);
  return it==ele_mass_map_.end() ? 1.0 : it->second;
}


}} // ns
