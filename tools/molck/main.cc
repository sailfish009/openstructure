#include <unistd.h>
#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
#include <ost/base.hh>
#include <ost/boost_filesystem_helper.hh>
#include <ost/platform.hh>
#include <ost/conop/model_check.hh>
#include <ost/conop/conop.hh>
#include <ost/conop/amino_acids.hh>
#include <ost/io/mol/pdb_reader.hh>
#include <ost/io/mol/pdb_writer.hh>
#include <ost/io/io_exception.hh>
#include <ost/conop/nonstandard.hh>
#if defined(__APPLE__)
#include <mach-o/dyld.h>
#endif
using namespace ost;
using namespace ost::conop;
using namespace ost::mol;
using namespace ost::io;

namespace po=boost::program_options;
namespace fs=boost::filesystem;

EntityHandle load_x(const String& file, const IOProfile& profile)
{
  try {
    PDBReader reader(file, profile);
    if (reader.HasNext()) {
      EntityHandle ent=CreateEntity();
      reader.Import(ent);
      return ent;
    }
    std::cerr << "ERROR: '" << file << "' does not contain any ATOM records. "
              << "Are you sure this is a PDB file?" << std::endl;
    return EntityHandle();
  } catch (std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    return EntityHandle();
  }
}

// load compound library, exiting if it could not be found...
CompoundLibPtr load_compound_lib(const String& custom_path)
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
    std::cerr << "Could not determine the path of the molck executable. Will only look for compounds.chemlib in the current working directory" << std::endl;
  } else {
    fs::path path_and_exe(exe_path);
    fs::path path_only=path_and_exe.branch_path();
    fs::path share_path = path_only.branch_path();
    share_path/="share";
    share_path/="openstructure";
    share_path/="compounds.chemlib";

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

void usage()
{
  std::cerr << "usage: molck [options] file1.pdb [file2.pdb [...]]" << std::endl;
  std::cerr << "options" << std::endl;
  std::cerr << "  --complib=path   location of the compound library file" << std::endl;   
  std::cerr << "                   If not provided, the following locations are searched" << std::endl;   
  std::cerr << "                   in this order:" << std::endl;   
  std::cerr << "                   1. Working directory" << std::endl;   
  std::cerr << "                   2. OpenStructure standard library location" << std::endl;   
  std::cerr << "                      (if the executable is part of a standard" << std::endl;
  std::cerr << "                      OpenStructure installation)" << std::endl;
  std::cerr << "  --rm=<a>,<b>   remove atoms and residues matching some criteria" << std::endl;
  std::cerr << "          zeroocc  - Remove atoms with zero occupancy" << std::endl;
  std::cerr << "          hyd      - Remove hydrogen atoms" << std::endl;
  std::cerr << "          oxt      - Remove terminal oxygens" << std::endl;
  std::cerr << "          nonstd   - Remove all residues not " << std::endl 
            << "                     one of the 20 standard amino acids" << std::endl;
  std::cerr << "          unk      - Remove unknown atoms and atoms that " << std::endl 
            << "                     are not supposed to be part of a residue" << std::endl;
  std::cerr << "  --fix-ele      clean up element column" << std::endl;
  std::cerr << "  --stdout       write cleaned file(s) to stdout" << std::endl;
  std::cerr << "  --fileout=blueprint   write cleaned file(s) to disk" << std::endl;  
  std::cerr << "                        The blueprint string, which must contain a % character," << std::endl;
  std::cerr << "                        is used to generate the output filename and path" << std::endl; 
  std::cerr << "                        by replacing % with the input file name without" << std::endl;
  std::cerr << "                        the extension. Output files automatically add" << std::endl; 
  std::cerr << "                        '.pdb' extension" << std::endl;
  std::cerr << "  --color=auto|on|off " << std::endl 
            << "          whether output should be colored" << std::endl;
  std::cerr << "  --map-nonstd   maps modified residues back to the parent amino " << std::endl 
            << "          acid, e.g. MSE -> MET, SEP -> SER." << std::endl;
  exit(0);
}	

int main(int argc, char *argv[])
{
  if (argc<2) {
    usage();
  }
  IOProfile prof;
  prof.fault_tolerant=true;
  String rm;
  String color;
  bool colored = false;

  bool rm_unk_atoms=false;
  bool rm_hyd_atoms=false;
  bool rm_non_std=false;
  bool rm_oxt_atoms=false;
  bool rm_zero_occ_atoms=false;
  bool write_to_stdout = false;
  bool write_to_file = false;
  bool map_nonstd_res = false;
  bool assign_elem = false;
  String output_blueprint_string;
  String custom_path="";

  po::options_description desc("Options");
  desc.add_options()
    ("rm", po::value<String>(&rm)->default_value("hyd"), "atoms to be removed")
    ("color", po::value<String>(&color)->default_value("auto"), 
     "whether the output should be colored.")
    ("files", po::value< std::vector<String> >(), "input file(s)")
    ("stdout", "write cleaned file(s) to stdout")
    ("fileout", po::value<String>(&output_blueprint_string), "write cleaned file to output using blueprint to determine path")
    ("map-nonstd", "map non standard residues back to standard ones (e.g.: MSE->MET,SEP->SER,etc.)")
    ("fix-ele", "insert element") 
    ("complib", po::value<String>(&custom_path)->default_value(""),"location of the compound library file")       
  ;
  po::positional_options_description p;
  p.add("files", -1);
  std::vector<String> files;
  po::variables_map vm;
  
  try {
    po::store(po::command_line_parser(argc, argv).
                options(desc).positional(p).run(),
              vm);
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    usage();
    exit(-1);
  }
  po::notify(vm); 
  if (vm.count("files")) {
    files = vm["files"].as<std::vector<String> >();
  } else {
    usage();
    exit(-1);
  }
  if (vm.count("complib")) {
    custom_path = vm["complib"].as<String>();
  }    
  if (vm.count("stdout")) {
    write_to_stdout = true;
  }
  if (vm.count("fileout")) {
    write_to_file = true;
    output_blueprint_string = vm["fileout"].as<String>();
  }  
  if (vm.count("map-nonstd")) {
    map_nonstd_res = true;
  }
  if (vm.count("fix-ele")) {
    assign_elem = true;
  }
  
  std::vector<StringRef> rms=StringRef(rm.c_str(), rm.size()).split(',');
  for (size_t i=0; i<rms.size(); ++i) {
    if (rms[i] == StringRef("unk", 3)) {
      rm_unk_atoms = true;
    } else if (rms[i] == StringRef("nonstd", 6)) {
      rm_non_std = true;
    } else if (rms[i] == StringRef("hyd", 3)) {
      rm_hyd_atoms = true;
    } else if (rms[i] == StringRef("oxt", 3)) {
      rm_oxt_atoms = true;
    } else if (rms[i] == StringRef("zeroocc", 7)) {
      rm_zero_occ_atoms = true;
    } else {
      std::cerr << "unknown value to remove '" << rms[i] << "'" << std::endl;
      usage();
      exit(-1);
    }
  }
  if (color=="auto") {
    colored = isatty(STDERR_FILENO);
  } else if (color == "on" || color == "1" || color == "yes") {
    colored = true;
  } else if (color == "off" || color == "0" || color == "no") {
    colored = false;	
  } else {
    usage();
    exit(-1);
  }
  CompoundLibPtr lib=load_compound_lib(custom_path);  
  for (unsigned int i = 0; i < files.size(); ++i) {
    EntityHandle ent=load_x(files[i], prof);
    if (!ent.IsValid()) {
      continue;
    }
    if (map_nonstd_res)  {
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
    ent=new_ent;
    }

    XCSEditor edi=ent.EditXCS();
    DiagEngine diags;
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
      for (size_t j=0; j<diags.GetDiags().size(); ++j) {
      const Diag* diag=diags.GetDiags()[j];
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
    
    if (assign_elem)  {
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
 
    if (write_to_stdout) {
      PDBWriter writer(std::cout, prof);
      writer.Write(ent);
    }
    if (write_to_file) {
      fs::path input_file_path(files[i]);
      fs::path input_filename = BFPathStem(input_file_path);
 

      String input_filename_string=BFPathToString(input_filename);

      size_t replstart =output_blueprint_string.find('%');	

      if (replstart == String::npos) {
        std::cerr << "The output blueprint string does not contain a % character" << std::endl;
        exit(-1);
      } 
      String output_blueprint_string_copy = output_blueprint_string;
      output_blueprint_string_copy.replace(replstart,1,input_filename_string); 
      output_blueprint_string_copy+=".pdb";
 
      try {
        fs::path out_path(output_blueprint_string_copy);
        if (!exists(out_path)) {
          std::cerr << "Output path does not exist: " << output_blueprint_string_copy << std::endl;
          exit(-1);
        }
      } catch (std::exception& e) {

        size_t perden = String(e.what()).find("Permission denied");	

        if (perden != String::npos) {
          std::cerr << "Cannot write into output directory: " << output_blueprint_string_copy << std::endl;
          exit(-1);
        } else {
          std::cerr << e.what() << std::endl;
          exit(-1);
        }
      }
      std::cerr << "Writing out file: " << output_blueprint_string_copy << std::endl;
      PDBWriter writer(output_blueprint_string_copy, prof);
       writer.Write(ent);
    }
  }
 
  return 0;
}
