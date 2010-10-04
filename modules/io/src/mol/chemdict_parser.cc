#include <ost/io/mol/chemdict_parser.hh>

namespace ost { namespace io {

using namespace ost::conop;
  
bool ChemdictParser::OnBeginData(const StringRef& data_name) 
{    
  compound_.reset(new Compound(data_name.str()));
  compound_->SetDialect(dialect_);
  if (last_!=data_name[0]) {
    last_=data_name[0];
    std::cout << last_ << std::flush;
  }
  atom_map_.clear();
  insert_=true;
  return true; 
}

bool ChemdictParser::OnBeginLoop(const StarLoopDesc& header)
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

void ChemdictParser::OnDataRow(const StarLoopDesc& header, 
                               const std::vector<StringRef>& columns) 
{                        
  if (loop_type_==ATOM_SPEC) {
    AtomSpec atom;
    atom.is_leaving=columns[indices_[IS_LEAVING]][0]=='Y';
    atom.name=columns[indices_[ATOM_NAME]].str();
    atom.alt_name=columns[indices_[ALT_ATOM_NAME]].str();
    atom.ordinal=columns[indices_[ORDINAL]].to_int().second-1;
    atom.element=columns[indices_[ELE]].str();
    atom.is_aromatic=columns[indices_[IS_AROMATIC]][0]=='Y';
    compound_->AddAtom(atom);
    atom_map_[atom.name]=atom.ordinal;
  } else if (loop_type_==BOND_SPEC) {
    BondSpec bond;
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

void ChemdictParser::OnDataItem(const StarDataItem& item)
{
  if (item.GetCategory()==StringRef("chem_comp", 9)) {
    if (item.GetName()==StringRef("type", 4)) {
      // convert type to uppercase
      String type=item.GetValue().str();
      for (String::iterator i=type.begin(), e=type.end(); i!=e; ++i) {
        *i=toupper(*i);
      }
      // The type of water is set to "?". let's change it to water...
      if (compound_->GetID()=="HOH") {
        compound_->SetChemClass(mol::ChemClass(mol::ChemClass::Water));
        compound_->SetOneLetterCode('.');
      } else {
        std::map<String, mol::ChemClass>::iterator i=tm_.find(type);
        if (i!=tm_.end()) {
          compound_->SetChemClass(i->second);
        } else {
          std::cout << "unknown type '" << type << "' for compound "
                    << compound_->GetID() << std::endl;
        }
      }

    } else if (item.GetName()==StringRef("formula", 7)) {
      compound_->SetFormula(item.GetValue().str());
    } else if (item.GetName()==StringRef("one_letter_code", 15)) {
      if (item.GetValue().length()==1) {
        compound_->SetOneLetterCode(item.GetValue()[0]);   
      }
    } else if (item.GetName()==StringRef("pdbx_initial_date", 17)) {        
      compound_->SetCreationDate(Date::FromString(item.GetValue()));
    } else if (item.GetName()==StringRef("pdbx_modified_date", 18)) {
      compound_->SetModificationDate(Date::FromString(item.GetValue()));
    }
  } else if (item.GetName()==StringRef("atom_id", 7)) {
    atom_.name=item.GetValue().str();
  } else if (item.GetName()==StringRef("alt_atom_id", 11)) {
    if (compound_->GetID()=="ILE" && item.GetValue()==StringRef("CD1", 3)) {
      atom_.alt_name="CD";
    } else {
      atom_.alt_name=item.GetValue().str();       
    }

  } else if (item.GetName()==StringRef("type_symbol", 11)) {
    atom_.element=item.GetValue().str();
  } else if (item.GetName()==StringRef("pdbx_ordinal", 12)) {
    atom_.ordinal=item.GetValue().to_int().second-1;
  }
}

void ChemdictParser::OnEndData()
{
  if (insert_) {
    if (compound_->GetAtomSpecs().empty()) {
      compound_->AddAtom(atom_);
    }
    lib_->AddCompound(compound_);      
  }
}

std::map<String, mol::ChemClass> ChemdictParser::tm_=std::map<String, mol::ChemClass>();

void ChemdictParser::InitTypeMap()
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
  tm_["WATER"]=mol::ChemClass(mol::ChemClass::Water);
}

}}