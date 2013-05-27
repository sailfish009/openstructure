#include "compound_lib_base.hh"

namespace ost { namespace conop {

bool CompoundLibBase::IsResidueComplete(const ost::mol::ResidueHandle& res, bool check_hydrogens, Compound::Dialect dialect) const{

  CompoundPtr compound = this->FindCompound(res.GetName(), dialect);

  AtomSpecList a_spec = compound->GetAtomSpecs();

  for(AtomSpecList::const_iterator it=a_spec.begin(); it!=a_spec.end();++it){
    if(it->element=="H" || it->element=="D"){
      if(check_hydrogens){
        if(!res.FindAtom(it->name).IsValid()){
          return false;
        }
      }
    }
    else{
      if(!res.FindAtom(it->name).IsValid()){
        return false;
      }
    }
  }
  return true;
}

}}//namespace
