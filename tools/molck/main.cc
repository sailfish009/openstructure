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
#include <ost/io/mol/mmcif_reader.hh>
#include <ost/io/io_exception.hh>
#include <ost/conop/nonstandard.hh>
#include <ost/mol/alg/molck.hh>
#if defined(__APPLE__)
#include <mach-o/dyld.h>
#endif
using namespace ost;
using namespace ost::conop;
using namespace ost::mol;
using namespace ost::io;
using namespace ost::mol::alg::molck;

namespace po=boost::program_options;
namespace fs=boost::filesystem;

const char* USAGE=
"this is molck - the molecule checker\n"
"usage: molck [options] file1.pdb [file2.pdb [...]]\n"
"options \n"
"  --complib=path    location of the compound library file. If not provided, the \n"
"    following locations are searched in this order: \n" 
"    1. Working directory, 2. OpenStructure standard library location (if the \n"
"    executable is part of a standard OpenStructure installation) \n"
"  --rm=<a>,<b>      remove atoms and residues matching some criteria \n"
"    zeroocc - Remove atoms with zero occupancy \n"
"    hyd - Remove hydrogen atoms \n"
"    oxt - Remove terminal oxygens \n"
"    nonstd - Remove all residues not one of the 20 standard amino acids \n"
"    unk - Remove unknown and atoms not following the nomenclature\n"
"  --fix-ele         clean up element column\n"
"  --stdout          write cleaned file(s) to stdout \n"
"  --out=filename    write cleaned file(s) to disk. % characters in the filename are \n"
"    replaced with the basename of the input file without extension. \n"
"    Default: %-molcked.pdb \n"
"  --color=auto|on|off \n"
"    whether output should be colored\n"
"  --map-nonstd   maps modified residues back to the parent amino acid, for example\n"
"    MSE -> MET, SEP -> SER.\n";

void usage()
{
  std::cerr << USAGE << std::endl;
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
    ("out,o", po::value<String>(&output_blueprint_string)->default_value("%-molcked.pdb"), 
     "write cleaned file to output using blueprint to determine path")
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
  } else {
    write_to_file = true;
    output_blueprint_string = vm["out"].as<String>();
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
      ent = MapNonStandardResidues(ent, lib);
    }

    RemoveAtoms(ent, 
                lib, 
                rm_unk_atoms,
                rm_non_std,
                rm_hyd_atoms,
                rm_oxt_atoms,
                rm_zero_occ_atoms,
                colored);
    
    if (assign_elem)  {
      CleanUpElementColumn(ent, lib);
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
      String output_blueprint_string_copy = output_blueprint_string;
      if (replstart != String::npos) {
        output_blueprint_string_copy.replace(replstart,1,input_filename_string); 
      } 
      try {
        fs::path out_path(output_blueprint_string_copy);
        if (out_path.has_parent_path() && !exists(out_path.parent_path())) {
          std::cerr << "Output path does not exist: " 
                    << output_blueprint_string_copy << std::endl;
          exit(-1);
        }
      } catch (std::exception& e) {

        size_t perden = String(e.what()).find("Permission denied");	

        if (perden != String::npos) {
          std::cerr << "Cannot write into output directory: " 
                    << output_blueprint_string_copy << std::endl;
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
