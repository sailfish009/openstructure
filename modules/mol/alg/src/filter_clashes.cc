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
#include <ost/mol/mol.hh>
#include <sstream>
#include "filter_clashes.hh"

namespace {
  
String bond_string(const ost::mol::AtomView& atom1, const ost::mol::AtomHandle& atom2) {
  String atom1_str = atom1.GetName();
  String atom2_str = atom2.GetName();
  String string1,string2;
  if (atom1_str < atom2_str) {
    string1 = atom1_str;
    string2 = atom2_str;
  } else {
    string1 = atom2_str;
    string2 = atom1_str;
  }
  std::stringstream stkey;
  stkey << string1 << "-" << string2;
  return stkey.str();  
}

String angle_string(const ost::mol::AtomHandle& atom1, const ost::mol::AtomView& atom, const ost::mol::AtomHandle& atom2 ) {
  String atom1_str = atom1.GetName();
  String atom2_str = atom2.GetName();
  String string1,string2;
  if (atom1_str < atom2_str) {
    string1 = atom1_str;
    string2 = atom2_str;
  } else {
    string1 = atom2_str;
    string2 = atom1_str;
  }
  std::stringstream stkey;
  stkey << string1 << "-" << atom << "-" << string2;
  return stkey.str();  
}


}  


namespace ost { namespace mol { namespace alg {

void ClashingDistances::SetClashingDistance(const String& ele1,const String& ele2, Real min_distance, Real tolerance)
{
  std::stringstream stkey;
  stkey << ele1 << "--" << ele2;
  String key=stkey.str();
  min_distance_[key]=std::make_pair<Real,Real>(min_distance,tolerance);
}

std::pair<Real,Real> ClashingDistances::GetClashingDistance(const String& ele1,const String& ele2) const
{
  std::stringstream stkey;
  stkey << ele1 << "--" << ele2;
  String key=stkey.str();
  return min_distance_.find(key)->second;
}

void ClashingDistances::PrintAllDistances() const
{
   for (std::map <String,std::pair<float,float> >::const_iterator index = min_distance_.begin();index != min_distance_.end();++index) {
     std::cout << index->first << "\t" << index->second.first << "\t" << index->second.second << std::endl;
   }    
}

Real ClashingDistances::GetMaxAdjustedDistance() const
{
  Real max_adjusted_distance=0;
  for (std::map <String,std::pair<float,float> >::const_iterator index = min_distance_.begin();index != min_distance_.end();++index) {
    Real distance = index->second.first; 
    Real tolerance = index->second.second; 
    if ((distance-tolerance) > max_adjusted_distance) {
      max_adjusted_distance=distance-tolerance;
    }
  }  
  return max_adjusted_distance;  
}

void StereoChemicalParams::SetParam(const String& param, const String& residue, Real value, Real st_dev)
{
  std::pair<String,String> key = std::make_pair<String,String>(param,residue);
  params_[key]=std::make_pair<Real,Real>(value,st_dev);
}

std::pair<Real,Real> StereoChemicalParams::GetParam(const String& param,const String& residue) const
{
  std::pair<String,String> key = std::make_pair<String,String>(param,residue);
  return params_.find(key)->second;
}

void StereoChemicalParams::PrintAllParameters() const 
{
   for (std::map <std::pair<String,String>,std::pair<float,float> >::const_iterator index = params_.begin();index != params_.end();++index) {
     std::cout << index->first.first << "\t" << index->first.second << "\t" << index->second.first << "\t" << index->second.second << std::endl;
   }    
};

EntityView CheckStereoChemistry(const EntityView& ent, const StereoChemicalParams& bond_table, const StereoChemicalParams& angle_table, Real bond_tolerance, Real angle_tolerance, bool always_remove_bb)
{
  
  EntityView filtered=ent.CreateEmptyView();
  ResidueViewList residues=ent.GetResidueList();
  for (ResidueViewList::iterator i=residues.begin(), e=residues.end(); i!=e; ++i) {
    bool remove_sc=false, remove_bb=false;
    ResidueView res=*i;
    String residue_str = res.GetName();
    const AtomViewList& atoms=res.GetAtomList();
    for (AtomViewList::const_iterator j=atoms.begin(), e2=atoms.end(); j!=e2; ++j) {
      AtomView atom=*j;
      String ele1=atom.GetElement();
      if (ele1=="H" || ele1=="D") {
        continue;
      }
      BondHandleList bonds = atom.GetBondList();
      for (BondHandeList::const_iterator bi = bonds.begin();bi!=bonds.end();++bi) {
	  BondHandle bond = *bi;
	  AtomHandle other_atom = bond.GetOther(atom.GetHandle());
	  String ele2 = other_atom.GetElement();
          if (ele2=="H" || ele2=="D") {
            continue;
          }
 	  if (other_atom.GetHashCode() > atom.GetHashCode()) {
	    Real blength = bond.GetLength();
	    String bond_str = bond_string(atom,other_atom);
	    std::pair<Real,Real> length_stddev = bond_table.GetParam(bond_str,residue_str);
	    Real ref_length = length_stddev.first;
	    Real ref_stddev = length_stddev.second;	   
	    Real min_length = ref_length - bond_tolerance*ref_stddev;
   	    Real max_length = ref_length + bond_tolerance*ref_stddev;
            if (blength < min_length || blength > max_length) {
              remove_sc=true;
              if (always_remove_bb==true) {
                remove_bb=true;
                continue;
              }
              String name=atom.GetName();
              if (name=="CA" || name=="N" || name=="O" || name=="C") {
                remove_bb=true;
              }
            }  
	  }  
      }
      
    
      for (BondHandeList::const_iterator bond_iter1=bonds.begin(); bond_iter1!=bonds.end(); ++bond_iter1) {
        BondHandle bond1=*bond_iter1;
        AtomHandle atom1= bond1.GetOther(atom.GetHandle());
        String ele_atom1=atom1.GetElement();
        if (ele_atom1=="H" || ele_atom1=="D") {
          continue;
        }
        if (atom1.GetResidue()!=res) {
          continue;	
        }	
        for (BondHandeList::const_iterator bond_iter2=bonds.begin(); bond_iter2!=bonds.end(); ++bond_iter2) {
          BondHandle bond2=*bond_iter2;
          AtomHandle atom2 = bond2.GetOther(atom.GetHandle());
          String ele_atom2=atom2.GetElement();
          if (ele_atom2=="H" || ele_atom2=="D") {
            continue;
          }
          if (atom2.GetResidue()!=res) {
            continue;	
          }	
          if (atom1.GetHashCode() > atom2.GetHashCode()) {
            Real awidth = ent.GetAngle(atom1,atom.GetHandle(),atom2);
            String angle_str = angle_string(atom1,atom,atom2);
            std::pair<Real,Real> width_stddev = bond_table.GetParam(angle_str,residue_str);
            Real ref_width = width_stddev.first;  
	    Real ref_stddev = width_stddev.second;	   
            Real min_width = ref_width - angle_tolerance*ref_stddev;
     	    Real max_width = ref_width + angle_tolerance*ref_stddev;
            if (awidth < min_width || awidth > max_width) {
              remove_sc=true;
              if (always_remove_bb==true) {
                remove_bb=true;
                continue;
              }
              String name=atom.GetName();
              if (name=="CA" || name=="N" || name=="O" || name=="C") {
                remove_bb=true;
              }
            }  
	  }  
        }
      } 
    }  
    if (remove_bb) {
      LOG_VERBOSE("removing whole residue " << res);
      continue;
    }
    if (remove_sc) {
      LOG_VERBOSE("removing sidechain of residue " << res);
      for (AtomViewList::const_iterator 
           j=atoms.begin(), e2=atoms.end(); j!=e2; ++j) {
       AtomView atom=*j;
       String name=atom.GetName();
       if (name=="CA" || name=="N" || name=="O" || name=="C") {
         filtered.AddAtom(atom);
       }
      }
      continue;
    }
    filtered.AddResidue(res, ViewAddFlag::INCLUDE_ATOMS);
  }
  return filtered;
}


EntityView CheckStereoChemistry(const EntityHandle& ent, const StereoChemicalParams& bond_table, const StereoChemicalParams& angle_table, Real bond_tolerance, Real angle_tolerance, bool always_remove_bb)
{
  return CheckStereoChemistry(ent.CreateFullView(), bond_table, angle_table, bond_tolerance, angle_tolerance, always_remove_bb);
}


EntityView FilterClashes(const EntityView& ent, const ClashingDistances& min_distances, bool always_remove_bb)
{
  std::cout << "Clashing Distances" << std::endl;
  min_distances.PrintAllDistances();
  exit(0);
  
  EntityView filtered=ent.CreateEmptyView();
  ResidueViewList residues=ent.GetResidueList();
  for (ResidueViewList::iterator 
       i=residues.begin(), e=residues.end(); i!=e; ++i) {
    bool remove_sc=false, remove_bb=false;
    ResidueView res=*i;
    const AtomViewList& atoms=res.GetAtomList();
    for (AtomViewList::const_iterator 
         j=atoms.begin(), e2=atoms.end(); j!=e2; ++j) {
      AtomView atom=*j;
      String ele1=atom.GetElement();
      if (ele1=="H" || ele1=="D") {
        continue;
      }
      AtomViewList within=ent.FindWithin(atom.GetPos(),min_distances.GetMaxAdjustedDistance());
      for (AtomViewList::iterator 
           k=within.begin(), e3=within.end(); k!=e3; ++k) {
        AtomView atom2=*k;
        if (atom2==atom) {
          continue;
        }
        String ele2=atom2.GetElement();
        if (ele2=="H" || ele2=="D") {
          continue;
        }
        // In theory, this should also trigger for disulfide bonds, but 
        // since we don't detect disulfides correctly, we can't count on that 
        // and we instead allow S-S distances down to 1.8.
        if (atom.GetHandle().FindBondToAtom(atom2.GetHandle()).IsValid()) {
          continue;
        }
        Real d=geom::Length2(atom.GetPos()-atom2.GetPos());
        std::pair <Real,Real> distance_tolerance=min_distances.GetClashingDistance(ele1, ele2);
	Real distance=distance_tolerance.first;
	Real tolerance=distance_tolerance.second;
	Real threshold=distance-tolerance;
        if (d<threshold*threshold) {
          remove_sc=true;
          if (always_remove_bb==true) {
            remove_bb=true;
            continue;
          }
          String name=atom.GetName();
          if (name=="CA" || name=="N" || name=="O" || name=="C") {
            remove_bb=true;
          }
        }
      }
    }
    
    if (remove_bb) {
      LOG_VERBOSE("removing whole residue " << res);
      continue;
    }
    if (remove_sc) {
      LOG_VERBOSE("removing sidechain of residue " << res);
      for (AtomViewList::const_iterator 
           j=atoms.begin(), e2=atoms.end(); j!=e2; ++j) {
       AtomView atom=*j;
       String name=atom.GetName();
       if (name=="CA" || name=="N" || name=="O" || name=="C") {
         filtered.AddAtom(atom);
       }
      }
      continue;
    }
    filtered.AddResidue(res, ViewAddFlag::INCLUDE_ATOMS);
  }
  return filtered;
}


EntityView FilterClashes(const EntityHandle& ent,  
                         const ClashingDistances& min_distances, bool always_remove_bb)
{
  return FilterClashes(ent.CreateFullView(), min_distances, always_remove_bb);
}


}}}

