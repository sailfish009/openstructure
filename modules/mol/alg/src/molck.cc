// #include <unistd.h>
// #include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
#include <ost/base.hh>
#include <ost/boost_filesystem_helper.hh>
// #include <ost/platform.hh>
#include <ost/conop/model_check.hh>
#include <ost/conop/conop.hh>
#include <ost/conop/amino_acids.hh>
#include <ost/io/mol/pdb_reader.hh>
// #include <ost/io/mol/pdb_writer.hh>
#include <ost/io/mol/mmcif_reader.hh>
// #include <ost/io/io_exception.hh>
#include <ost/conop/nonstandard.hh>
#include <ost/mol/alg/molck.hh>
// #if defined(__APPLE__)
// #include <mach-o/dyld.h>
// #endif
// using namespace ost;
using namespace ost::conop;
using namespace ost::mol;
using namespace ost::io;

// namespace po=boost::program_options;
namespace fs=boost::filesystem;

EntityHandle ost::mol::alg::molck::load_x(const String& file, const IOProfile& profile)
{
  try {
    EntityHandle ent = CreateEntity();
    if(file.compare(file.length() - 6, 6, ".mmcif") == 0 ||
       file.compare(file.length() - 4, 4, ".cif") == 0){
      MMCifReader reader(file,ent,profile);
      reader.Parse();
      return ent;
    }
    //if its not mmcif, we assume the file to be in pdb format without even
    //looking at the filename
    else{
      PDBReader reader(file, profile);
      if (reader.HasNext()) {
        reader.Import(ent);
        return ent;
      }
      std::cerr << "ERROR: '" << file << "' does not contain any ATOM records. "
              << "Are you sure this is a PDB file?" << std::endl;
      return EntityHandle();
    }
  } catch (std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    return EntityHandle();
  }
}

// load compound library, exiting if it could not be found...
CompoundLibPtr ost::mol::alg::molck::load_compound_lib(const String& custom_path)
{
  if (custom_path!="") {
    if (fs::exists(custom_path)) {  
      return CompoundLib::Load(custom_path);
    } else {
      std::cerr << "Could not find compounds.chemlib at the provided location, trying other options" << std::endl;
    }
  } 
  if (fs::exists("compounds.chemlib")) {
    return CompoundLib::Load("compounds.chemlib");
  }
  char result[ 1024 ]; 
  CompoundLibPtr lib;
  String exe_path; 
  #if defined(__APPLE__)
  uint32_t size=1023;
  if (!_NSGetExecutablePath(result, &size)) {
    exe_path=String(result); 
  }
  #else 
  ssize_t count = readlink( "/proc/self/exe", result, 1024 );
  exe_path = std::string( result, (count > 0) ? count : 0 );
  #endif
  if (exe_path.empty()) { 
    std::cerr << "Could not determine the path of the molck executable. Will only "
       "look for compounds.chemlib in the current working directory" << std::endl;
  } else {
    fs::path path_and_exe(exe_path);
    fs::path path_only=path_and_exe.branch_path();
    fs::path share_path = path_only.branch_path();
    share_path = share_path / "share" / "openstructure" / "compounds.chemlib";

    String share_path_string=BFPathToString(share_path);
      
    if (fs::exists(share_path_string)) {
      return CompoundLib::Load(share_path_string);
    }  
  }
  if (!lib) {
    std::cerr << "Could not load compounds.chemlib" << std::endl;
    exit(-1);
  }
  return CompoundLibPtr();
}

EntityHandle ost::mol::alg::molck::MapNonStandardResidues(EntityHandle& ent, CompoundLibPtr lib) {
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
        CopyResidue(*r,dest_res,new_edi,lib);
      }   
    }        
  }
  return new_ent;
}

void ost::mol::alg::molck::RemoveAtoms(EntityHandle& ent,
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

void ost::mol::alg::molck::CleanUpElementColumn(EntityHandle& ent, CompoundLibPtr lib){
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