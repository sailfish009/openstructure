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

/*
  Author: Marco Biasini
 */

#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>

#include <ost/mol/mol.hh>
#include <ost/io/mol/star_parser.hh>
#include <ost/conop/compound_lib.hh>

using namespace ost;


typedef enum {
  ATOM_SPEC,
  BOND_SPEC,
  DONT_KNOW
} LoopType;

class ChemDictParser : public io::StarParser {
public:
  ChemDictParser(std::istream& stream): 
    io::StarParser(stream),
    compound_(new conop::Compound("UNK")), last_(0), loop_type_(DONT_KNOW)
  {
    this->InitTypeMap();
  }
  
  virtual bool OnBeginData(const StringRef& data_name) 
  {    
    compound_.reset(new conop::Compound(data_name.str()));
    if (last_!=data_name[0]) {
      last_=data_name[0];
      std::cout << last_ << std::flush;
    }
    atom_map_.clear();
    insert_=true;
    return true; 
  }
  
  virtual bool OnBeginLoop(const io::StarLoopDesc& header) 
  { 
    if (header.GetCategory()=="chem_comp_atom") {
      loop_type_=ATOM_SPEC;
      indices_[ATOM_NAME]=header.GetIndex("atom_id");
      indices_[ALT_ATOM_NAME]=header.GetIndex("alt_atom_id");
      indices_[ELE]=header.GetIndex("type_symbol");      
      indices_[IS_LEAVING]=header.GetIndex("pdbx_leaving_atom_flag");
      indices_[IS_AROMATIC]=header.GetIndex("pdbx_aromatic_flag");
      indices_[ORDINAL]=header.GetIndex("pdbx_ordinal");      
      return true;
    } else if (header.GetCategory()=="chem_comp_bond") {
      loop_type_=BOND_SPEC;
      indices_[ATOM_ID1]=header.GetIndex("atom_id_1");
      indices_[ATOM_ID2]=header.GetIndex("atom_id_2");
      indices_[BOND_ORDER]=header.GetIndex("value_order");
      return true;
    }
    loop_type_=DONT_KNOW;
    return false;
  }
  
  virtual void OnDataRow(const io::StarLoopDesc& header, 
                         const std::vector<StringRef>& columns) 
  {                        
    if (loop_type_==ATOM_SPEC) {
      // compound_->AddBond()
      conop::AtomSpec atom;
      atom.is_leaving=columns[indices_[IS_LEAVING]][0]=='Y';
      atom.name=columns[indices_[ATOM_NAME]].str();
      atom.alt_name=columns[indices_[ALT_ATOM_NAME]].str();
      atom.ordinal=columns[indices_[ORDINAL]].to_int().second-1;
      atom.element=columns[indices_[ELE]].str();
      atom.is_aromatic=columns[indices_[IS_AROMATIC]][0]=='Y';
      compound_->AddAtom(atom);
      atom_map_[atom.name]=atom.ordinal;
    } else if (loop_type_==BOND_SPEC) {
      conop::BondSpec bond;
      String atom_one=columns[indices_[ATOM_ID1]].str();
      String atom_two=columns[indices_[ATOM_ID2]].str();      
      bond.atom_one=atom_map_[atom_one];
      bond.atom_two=atom_map_[atom_two];
      char b=columns[indices_[BOND_ORDER]][0];
      bond.order=b=='D' ? 2 : (b=='S' ? 1 : 3);
      if (bond.atom_one>bond.atom_two) {
        std::swap(bond.atom_one, bond.atom_two);
      }
      compound_->AddBond(bond);
    }
  }
  
  virtual void OnDataItem(const io::StarDataItem& item)
  {
    if (item.GetCategory()==StringRef("chem_comp", 9)) {
      if (item.GetName()==StringRef("type", 4)) {
        // convert type to uppercase
        String type=item.GetValue().str();
        for (String::iterator i=type.begin(), e=type.end(); i!=e; ++i) {
          *i=toupper(*i);
        }
        std::map<String, mol::ChemClass>::iterator i=tm_.find(type);
        if (i!=tm_.end()) {
          compound_->SetChemClass(i->second);          
        } else {
          std::cout << "unknown type '" << type << "' for compound " 
                    << compound_->GetID() << std::endl;
        }
      } else if (item.GetName()==StringRef("formula", 7)) {
        compound_->SetFormula(item.GetValue().str());
      } else if (item.GetName()==StringRef("one_letter_code", 15)) {
        if (item.GetValue().length()==1) {
          compound_->SetOneLetterCode(item.GetValue()[0]);   
        }
      } else if (item.GetName()==StringRef("pdbx_initial_date", 17)) {        
        compound_->SetCreationDate(conop::Date::FromString(item.GetValue()));
      } else if (item.GetName()==StringRef("pdbx_modified_date", 18)) {
        compound_->SetModificationDate(conop::Date::FromString(item.GetValue()));
      }
    } else if (item.GetName()==StringRef("atom_id", 7)) {
      atom_.name=item.GetValue().str();
    } else if (item.GetName()==StringRef("alt_atom_id", 11)) {
      atom_.alt_name=item.GetValue().str();
    } else if (item.GetName()==StringRef("type_symbol", 11)) {
      atom_.element=item.GetValue().str();
    } else if (item.GetName()==StringRef("pdbx_ordinal", 12)) {
      atom_.ordinal=item.GetValue().to_int().second-1;
    }
  }
  
  virtual void OnEndData()
  {
    if (insert_) {
      if (compound_->GetAtomSpecs().empty()) {
        compound_->AddAtom(atom_);
      }
      lib_->AddCompound(compound_);      
    }
  }
  
  void SetCompoundLib(const conop::CompoundLibPtr& lib) 
  {
    lib_=lib;
  }
private:
  void InitTypeMap();  
  conop::CompoundLibPtr                   lib_;
  conop::CompoundPtr                      compound_;
  typedef enum {
    ATOM_NAME=0,
    ALT_ATOM_NAME=1,
    IS_AROMATIC=2,
    ORDINAL=3,
    IS_LEAVING=4,
    ELE=5,
    STEREO_CONF=6,
    ATOM_ID1=0,
    ATOM_ID2=1,
    BOND_ORDER=2
  } PropIndex;
  char                                    last_;
  int                                     indices_[10];
  bool                                    insert_;
  static std::map<String, mol::ChemClass> tm_;  
  std::map<String, int>                   atom_map_;
  LoopType                                loop_type_;  
  conop::AtomSpec                         atom_;
};

std::map<String, mol::ChemClass> ChemDictParser::tm_=std::map<String, mol::ChemClass>();

void ChemDictParser::InitTypeMap()
{
  if (!tm_.empty())
    return;
  tm_["L-PEPTIDE COOH CARBOXY TERMINUS"]=mol::ChemClass(mol::ChemClass::LPeptideLinking);
  tm_["L-PEPTIDE NH3 AMINO TERMINUS"]=mol::ChemClass(mol::ChemClass::LPeptideLinking);   
  tm_["D-PEPTIDE NH3 AMINO TERMINUS"]=mol::ChemClass(mol::ChemClass::DPeptideLinking);
  tm_["L-SACCHARIDE 1,4 AND 1,4 LINKING"]=mol::ChemClass(mol::ChemClass::LSaccharide);
  tm_["D-SACCHARIDE 1,4 AND 1,4 LINKING"]=mol::ChemClass(mol::ChemClass::DSaccharide);
  tm_["L-SACCHARIDE"]=mol::ChemClass(mol::ChemClass::LSaccharide);
  tm_["D-SACCHARIDE"]=mol::ChemClass(mol::ChemClass::DSaccharide);
  tm_["SACCHARIDE"]=mol::ChemClass(mol::ChemClass::Saccharide);
  tm_["D-PEPTIDE LINKING"]=mol::ChemClass(mol::ChemClass::DPeptideLinking);
  tm_["L-PEPTIDE LINKING"]=mol::ChemClass(mol::ChemClass::LPeptideLinking);
  tm_["L-PEPTIDE-LINKING"]=mol::ChemClass(mol::ChemClass::LPeptideLinking);
  tm_["DNA LINKING"]=mol::ChemClass(mol::ChemClass::DNALinking);
  tm_["RNA LINKING"]=mol::ChemClass(mol::ChemClass::RNALinking);
  tm_["L-DNA LINKING"]=mol::ChemClass(mol::ChemClass::DNALinking);
  tm_["L-RNA LINKING"]=mol::ChemClass(mol::ChemClass::RNALinking);  
  tm_["R-DNA LINKING"]=mol::ChemClass(mol::ChemClass::DNALinking);
  tm_["R-RNA LINKING"]=mol::ChemClass(mol::ChemClass::RNALinking);  
  tm_["DNA OH 3 PRIME TERMINUS"]=mol::ChemClass(mol::ChemClass::DNALinking);
  tm_["PEPTIDE-LIKE"]=mol::ChemClass(mol::ChemClass::PeptideLinking);
  tm_["PEPTIDE LINKING"]=mol::ChemClass(mol::ChemClass::PeptideLinking);
  tm_["PEPTIDE-LINKING"]=mol::ChemClass(mol::ChemClass::PeptideLinking);  
  tm_["NON-POLYMER"]=mol::ChemClass(mol::ChemClass::NonPolymer);
  tm_["RNA OH 3 PRIME TERMINUS"]=mol::ChemClass(mol::ChemClass::RNALinking);
  tm_["?"]=mol::ChemClass(mol::ChemClass::Unknown);  
  
}

void PrintUsage()
{
  std::cout << "usage: chemdict_tool action compound-dict db" << std::endl;
  std::cout << "supported actions are:" << std::endl;
  std::cout << "  create  - creates a new db " << std::endl;
  std::cout << "  update  - update existing db" << std::endl;
}

int main(int argc, char const *argv[])
{
  if (argc!=4) {
    PrintUsage();
    return 0;
  }
  boost::iostreams::filtering_stream<boost::iostreams::input>  filtered_istream;  
  std::ifstream istream(argv[2]);
  if (boost::iequals(".gz", boost::filesystem::extension(argv[2]))) {
    filtered_istream.push(boost::iostreams::gzip_decompressor());
  }
  filtered_istream.push(istream);  
  ChemDictParser cdp(filtered_istream);
  conop::CompoundLibPtr compound_lib;
  if (!strcmp(argv[1], "create")) {
    compound_lib=conop::CompoundLib::Create(argv[3]);
  } else if (!strcmp(argv[1], "update")) {
    compound_lib=conop::CompoundLib::Load(argv[3]);
  } else {
    PrintUsage();
    return 0;
  }

  assert(compound_lib);
  conop::CompoundLibPtr in_mem_lib=compound_lib->Copy(":memory:");  
  compound_lib.reset();  
  cdp.SetCompoundLib(in_mem_lib);
  cdp.Parse();
  in_mem_lib->Copy(argv[3]);  
  return 0;
}
