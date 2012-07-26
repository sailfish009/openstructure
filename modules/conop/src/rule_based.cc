#include <ost/log.hh>
#include <ost/mol/xcs_editor.hh>
#include <ost/mol/bond_handle.hh>
#include <ost/mol/impl/residue_impl.hh>
#include <ost/mol/impl/atom_impl.hh>
#include <ost/mol/residue_handle.hh>
#include "rule_based.hh"
namespace ost { namespace conop {

struct OrdinalAtomComp {
  bool operator()(const mol::impl::AtomImplPtr& a,
                  const mol::impl::AtomImplPtr& b) const {
    return a->GetState()<b->GetState();
  }
};

void RuleBasedConop::Process(mol::EntityHandle ent)
{
  mol::ChainHandleList chains=ent.GetChainList();
  for (mol::ChainHandleList::iterator 
       i = chains.begin(), e = chains.end(); i!= e; ++i) {
    mol::ChainHandle& chain = *i;
    mol::ResidueHandleList residues = chain.GetResidueList();
    mol::ResidueHandle prev;
    for (mol::ResidueHandleList::iterator 
         j = residues.begin(), e2 = residues.end(); j != e2; ++j) {
      mol::ResidueHandle residue = *j;
      CompoundPtr compound = lib_->FindCompound(residue.GetName(), Compound::PDB);
      if (!compound) {
        // process unknown residue...
        continue;
      }
      this->ReorderAtoms(residue, compound);  
      bool unks = this->HasUnknownAtoms(residue, compound);
      if (unks) {
        LOG_WARNING("residue " << residue << " doesn't look like a standard " 
                    << residue.GetKey() << " (" << compound->GetFormula() << ")");
        residue.SetChemClass(mol::ChemClass(mol::ChemClass::UNKNOWN));
        residue.SetChemType(mol::ChemType(mol::ChemType::UNKNOWN));
        residue.SetOneLetterCode('?');
        continue;
      }
      this->FillResidueProps(residue, compound);
      this->ConnectAtomsOfResidue(residue, compound);
      this->ConnectResidues(prev, residue);
      prev = residue;
    }
  }
}

void RuleBasedConop::ConnectResidues(mol::ResidueHandle rh,
                                     mol::ResidueHandle next) 
{
  if (!next.IsValid() || !rh.IsValid()) {
    return;
  }

  mol::XCSEditor e=rh.GetEntity().EditXCS(mol::BUFFERED_EDIT);

  // check if both of the residues are able to form a peptide bond.
  if (rh.IsPeptideLinking() && next.IsPeptideLinking()) {
    // If we have an OXT then there is no peptide bond connecting the two
    // residues.
    if (rh.FindAtom("OXT"))
      return;
    mol::AtomHandle c=rh.FindAtom("C");
    mol::AtomHandle n=next.FindAtom("N");
    // Give subclasses a chance to give us their opinions on the feasibility of
    // the peptide bond.
    if (c.IsValid() && n.IsValid() && this->IsBondFeasible(c, n)) {
      e.Connect(c, n, 1);
      rh.SetIsProtein(true);
      next.SetIsProtein(true);
    }
  } else if (rh.IsNucleotideLinking() && next.IsNucleotideLinking()) {
    mol::AtomHandle c=rh.FindAtom("O3'");
    mol::AtomHandle n=next.FindAtom("P");
    if (c.IsValid() && n.IsValid() && this->IsBondFeasible(c, n)) {
      e.Connect(c, n, 1);
    }
  }
}

void RuleBasedConop::FillResidueProps(mol::ResidueHandle residue,
                                      CompoundPtr compound)
{
  residue.SetChemClass(compound->GetChemClass());
  residue.SetChemType(compound->GetChemType());
  residue.SetOneLetterCode(compound->GetOneLetterCode());
}

mol::AtomHandle RuleBasedConop::LocateAtom(const mol::AtomHandleList& ahl, 
                                           int ordinal) 
{
  if (ahl.empty())
    return mol::AtomHandle();
  const mol::AtomHandle* r_it=&ahl.back();
  if (static_cast<int>(ahl.size())>ordinal) {
    r_it=&ahl.front()+ordinal;
  }
  while ((r_it>=&ahl.front()) &&
         (static_cast<int>(r_it->Impl()->GetState())>ordinal)) {
    --r_it;
  }
  bool not_found=(r_it<&ahl.front() ||
                 static_cast<int>(r_it->Impl()->GetState())!=ordinal);
  return  not_found ? mol::AtomHandle() : *r_it;
}


void RuleBasedConop::ConnectAtomsOfResidue(mol::ResidueHandle rh, CompoundPtr compound) 
{

  //if (!compound) {
  //  dist_connect(this, rh.GetAtomList());
  //  return;
  //}
  //if (unknown_atoms_) {
  //  dist_connect(this, rh.GetAtomList());
  //  return;
  //}
  mol::XCSEditor e=rh.GetEntity().EditXCS(mol::BUFFERED_EDIT);
  BondSpecList::const_iterator j=compound->GetBondSpecs().begin();
  mol::AtomHandleList atoms=rh.GetAtomList();
  for(; j!=compound->GetBondSpecs().end(); ++j) {
      const BondSpec& bond=*j;
      mol::AtomHandle a1=this->LocateAtom(atoms, bond.atom_one);
      mol::AtomHandle a2=this->LocateAtom(atoms, bond.atom_two);
      if (a1.IsValid() && a2.IsValid()) { 
        if (!check_bond_feasibility_) {
          if (strict_hydrogens_ && (a1.GetElement()=="H" || 
                                                a2.GetElement()=="D")) {
            continue;
          }
          e.Connect(a1, a2, bond.order);
        } else { 
          if (this->IsBondFeasible(a1, a2)) {
            if (strict_hydrogens_ && (a1.GetElement()=="H" || 
                                                  a2.GetElement()=="D")) {
              continue;
            }
            e.Connect(a1, a2, bond.order);
          }
        }
      }
  }
  //for (mol::AtomHandleList::iterator i=atoms.begin(), e=atoms.end(); i!=e; ++i) {
  //  if (((*i).GetElement()=="H" || (*i).GetElement()=="D") && 
   //     (*i).GetBondCount()==0) {
  //    this->DistanceBasedConnect(*i);
 //   }
 // }
}

bool RuleBasedConop::IsBondFeasible(const mol::AtomHandle& atom_a,
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
void RuleBasedConop::ReorderAtoms(mol::ResidueHandle residue, 
                                  CompoundPtr compound)
{
  mol::impl::ResidueImplPtr impl=residue.Impl();
  mol::impl::AtomImplList::iterator i=impl->GetAtomList().begin();
  for (; i!=impl->GetAtomList().end(); ++i) {
    mol::impl::AtomImplPtr atom=*i;
    atom->SetState(std::numeric_limits<unsigned int>::max());
    int index=compound->GetAtomSpecIndex(atom->GetName());
    if (index==-1) {
      atom->SetState(std::numeric_limits<unsigned int>::max());
      continue;
    }
    atom->SetState((compound->GetAtomSpecs())[index].ordinal);
  }
  std::sort(impl->GetAtomList().begin(), impl->GetAtomList().end(),
            OrdinalAtomComp());
}

bool RuleBasedConop::HasUnknownAtoms(mol::ResidueHandle res, CompoundPtr compound)
{
  AtomSpecList::const_iterator j=compound->GetAtomSpecs().begin();
  mol::AtomHandleList atoms=res.GetAtomList();
  mol::AtomHandleList::iterator i=atoms.begin();
  for (mol::AtomHandleList::iterator 
       i=atoms.begin(), e=atoms.end(); i!=e; ++i) {
    if ((*i).Impl()->GetState()==std::numeric_limits<unsigned int>::max()) {
      if (((*i).GetElement()=="H" || (*i).GetElement()=="D") && 
          !strict_hydrogens_) {
        continue;
      }
      return true;
    }
  }
  return false;
}
}}
