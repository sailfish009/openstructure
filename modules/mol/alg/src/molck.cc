#include <ost/mol/xcs_editor.hh>
#include <ost/mol/alg/nonstandard.hh>
#include <ost/conop/model_check.hh>
#include <ost/conop/amino_acids.hh>
#include <ost/mol/alg/molck.hh>

using namespace ost::conop;
using namespace ost::mol;


void ost::mol::alg::MapNonStandardResidues(EntityHandle& ent, CompoundLibPtr lib) {
  // TODO: Maybe it is possible to make it in-place operation
  EntityHandle new_ent=CreateEntity();  
  ChainHandleList chains=ent.GetChainList();
  XCSEditor new_edi=new_ent.EditXCS();
  for (ChainHandleList::const_iterator c=chains.begin();c!=chains.end();++c) {
    ChainHandle new_chain = new_edi.InsertChain(c->GetName());
    ResidueHandleList residues = c->GetResidueList();
    for (ResidueHandleList::const_iterator r=residues.begin();r!=residues.end();++r) {
      AminoAcid aa = ResidueToAminoAcid(*r);
      if (aa!=XXX) {
        ResidueHandle dest_res = new_edi.AppendResidue(new_chain,r->GetName(),r->GetNumber());
        AtomHandleList atoms = r->GetAtomList();
        for (AtomHandleList::const_iterator a=atoms.begin();a!=atoms.end();++a) {
          new_edi.InsertAtom(dest_res,a->GetName(),a->GetPos(),a->GetElement(),a->GetOccupancy(),a->GetBFactor(),a->IsHetAtom());
        }
        continue;
      } else {
        CompoundPtr compound=lib->FindCompound(r->GetName(),Compound::PDB);
        if (!compound || !compound->IsPeptideLinking() || compound->GetChemClass()==ChemClass::D_PEPTIDE_LINKING || 
             OneLetterCodeToAminoAcid(compound->GetOneLetterCode())==XXX) {
           ResidueHandle dest_res = new_edi.AppendResidue(new_chain,r->GetName(),r->GetNumber());
           AtomHandleList atoms = r->GetAtomList();
           for (AtomHandleList::const_iterator a=atoms.begin();a!=atoms.end();++a) {
             new_edi.InsertAtom(dest_res,a->GetName(),a->GetPos(),a->GetElement(),a->GetOccupancy(),a->GetBFactor(),a->IsHetAtom());
           }
           continue;
        } 
        ResidueHandle dest_res = new_edi.AppendResidue(new_chain,OneLetterCodeToResidueName(compound->GetOneLetterCode()),r->GetNumber());
        ost::mol::alg::CopyResidue(*r,dest_res,new_edi,lib);
      }   
    }        
  }
  ent = new_ent;
}

void ost::mol::alg::RemoveAtoms(
                 EntityHandle& ent,
                 CompoundLibPtr lib,
                 bool rm_unk_atoms,
                 bool rm_non_std,
                 bool rm_hyd_atoms,
                 bool rm_oxt_atoms,
                 bool rm_zero_occ_atoms,
                 bool colored /*=true*/){
  XCSEditor edi=ent.EditXCS();
  Diagnostics diags;
  Checker checker(lib, ent, diags);
  if (rm_zero_occ_atoms) {
    std::cerr << "removing atoms with zero occupancy" << std::endl;
    int zremoved=0;
    AtomHandleList zero_atoms=checker.GetZeroOccupancy();
    for (AtomHandleList::const_iterator i=zero_atoms.begin(), e=zero_atoms.end(); i!=e; ++i) {
       edi.DeleteAtom(*i);
       zremoved++;   
    }
    std::cerr << " --> removed " << zremoved << " hydrogen atoms" << std::endl;
  }

  if (rm_hyd_atoms) {
    std::cerr << "removing hydrogen atoms" << std::endl;
    int hremoved=0;
    AtomHandleList hyd_atoms=checker.GetHydrogens();
    for (AtomHandleList::const_iterator i=hyd_atoms.begin(), e=hyd_atoms.end(); i!=e; ++i) {
       edi.DeleteAtom(*i);
       hremoved++;   
    }
    std::cerr << " --> removed " << hremoved << " hydrogen atoms" << std::endl;
  }
  
  if (rm_oxt_atoms) {
    std::cerr << "removing OXT atoms" << std::endl;
    int oremoved=0;
    AtomHandleList atoms=ent.GetAtomList();
    for (AtomHandleList::const_iterator i=atoms.begin(), e=atoms.end(); i!=e; ++i) {
       if (i->GetName()=="OXT") {
         edi.DeleteAtom(*i);
         oremoved++;   
       }
    }
    std::cerr << " --> removed " << oremoved << " OXT atoms" << std::endl;
  }

  checker.CheckForCompleteness();
  checker.CheckForUnknownAtoms();
  checker.CheckForNonStandard();
  for (Diagnostics::const_diag_iterator 
       j = diags.diags_begin(), e = diags.diags_end(); j != e; ++j) {
    const Diag* diag=*j;
    std::cerr << diag->Format(colored);
    switch (diag->GetType()) {
      case DIAG_UNK_ATOM:
        if (rm_unk_atoms) {
          edi.DeleteAtom(diag->GetAtom(0));
          std::cerr << " --> removed ";  
        }
        break;
      case DIAG_NONSTD_RESIDUE:
        if (rm_non_std) {
          edi.DeleteResidue(diag->GetResidue(0));
          std::cerr << " --> removed ";  
        }
        break;
      default:
        break;
    }
    std::cerr << std::endl;
  }
}

void ost::mol::alg::CleanUpElementColumn(EntityHandle& ent, CompoundLibPtr lib){
  ChainHandleList chains=ent.GetChainList();
  for (ChainHandleList::const_iterator c=chains.begin();c!=chains.end();++c) {
    ResidueHandleList residues = c->GetResidueList();
    for (ResidueHandleList::const_iterator r=residues.begin();r!=residues.end();++r) {
      CompoundPtr compound=lib->FindCompound(r->GetName(),Compound::PDB);            
      AtomHandleList atoms=r->GetAtomList();
      if (!compound) {
        for (AtomHandleList::iterator j=atoms.begin(), e2=atoms.end(); j!=e2; ++j) {
            j->SetElement("");
        }
        continue; 
      }
      for (AtomHandleList::iterator j=atoms.begin(), e2=atoms.end(); j!=e2; ++j) {
        int specindx=compound->GetAtomSpecIndex(j->GetName());
        if (specindx!=-1) {  
          j->SetElement(compound->GetAtomSpecs()[specindx].element);
        } else {
          j->SetElement("");
        }
      }
    }    
  }
}

void ost::mol::alg::Molck(
           ost::mol::EntityHandle& ent,
           ost::conop::CompoundLibPtr lib,
           const ost::mol::alg::MolckSettings& settings=ost::mol::alg::MolckSettings()){
  if (settings.map_nonstd_res)  {
    ost::mol::alg::MapNonStandardResidues(ent, lib);
  }
  ost::mol::alg::RemoveAtoms(ent, 
              lib, 
              settings.rm_unk_atoms,
              settings.rm_non_std,
              settings.rm_hyd_atoms,
              settings.rm_oxt_atoms,
              settings.rm_zero_occ_atoms,
              settings.colored);
  if (settings.assign_elem)  {
    ost::mol::alg::CleanUpElementColumn(ent, lib);
  }          
}