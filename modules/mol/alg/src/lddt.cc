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
// along with this library; if not, write to the  Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//------------------------------------------------------------------------------
#include <iomanip>
#if defined (_MSC_VER)
#define BOOST_ALL_DYN_LINK 1
#endif
#include <boost/program_options.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/convenience.hpp>
#include <ost/mol/alg/local_dist_diff_test.hh>
#include <ost/mol/alg/filter_clashes.hh>
#include <ost/io/mol/pdb_reader.hh>
#include <ost/io/io_exception.hh>
#include <ost/conop/conop.hh>
#include <ost/string_ref.hh>
#include <ost/conop/amino_acids.hh>
#include <ost/mol/iterator.hh>
#include <ost/platform.hh>
#include <ost/log.hh>

#include <ost/conop/rule_based_builder.hh>
#include <ost/dyn_cast.hh>

using namespace ost;
using namespace ost::io;
using namespace ost::mol;
using namespace ost::mol::alg;
namespace po=boost::program_options;

// loads a file
EntityHandle load(const String& file, const IOProfile& profile)
{
  try {
    PDBReader reader(file, profile);
    if (reader.HasNext()) {
      EntityHandle ent=CreateEntity();
      reader.Import(ent);
      conop::Conopology& conop_inst=conop::Conopology::Instance();
      conop_inst.ConnectAll(conop_inst.GetBuilder(), ent);
      return ent;
    }
    std::cerr << "ERROR: '" << file << "' does not contain any ATOM records. "
              << "Are you sure this is a PDB file?" << std::endl;
    return EntityHandle();
  } catch (io::IOException& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    return EntityHandle();
  }
}

// prints usage output
void usage()
{
  std::cerr << "usage: lddt [options] <mod1> [mod1 [mod2]] <re1>[,ref2,ref3]" << std::endl;
  std::cerr << "   -s         selection performed on ref" << std::endl;
  std::cerr << "   -c         use Calphas only" << std::endl;
  std::cerr << "   -f         perform structural checks and filter input data" << std::endl;
  std::cerr << "   -t         fault tolerant parsing" << std::endl;
  std::cerr << "   -p <file>  use specified parmeter file. Mandatory" << std::endl;
  std::cerr << "   -v <level> verbosity level (0=results only,1=problems reported, 2=full report)" << std::endl;
  std::cerr << "   -b <value> tolerance in stddevs for bonds" << std::endl;
  std::cerr << "   -a <value> tolerance in stddevs for angles" << std::endl;
  std::cerr << "   -m <value> clashing distance for unknwon atom types" << std::endl;
  std::cerr << "   -r <value> distance inclusion radius" << std::endl;
  std::cerr << "   -e         print version" << std::endl;
}

// computes coverage
std::pair<int,int> compute_coverage (const EntityView& v,const GlobalRDMap& glob_dist_list)
{
  int second=0;
  int first=0;
  if (v.GetResidueList().size()==0) {
    return std::make_pair<int,int>(0,1);
  }
  ChainView vchain=v.GetChainList()[0];
  for (GlobalRDMap::const_iterator i=glob_dist_list.begin();i!=glob_dist_list.end();++i)
  {
    ResNum rnum = (*i).first;
    second++;
    if (vchain.FindResidue(rnum)) {
      first++;
    }
  }
  return std::make_pair<int,int>(first,second);
}

int main (int argc, char **argv)
{
  // sets some default values for parameters
  String version = "1.2";
  Real bond_tolerance = 8.0;
  Real angle_tolerance = 8.0;
  Real radius=15.0; 

  // creates the required loading profile
  IOProfile profile;
  profile.bond_feasibility_check=false;

  // parses options
  String sel;
  bool structural_checks=false;
  po::options_description desc("Options");
  desc.add_options()
    ("calpha,c", "consider only calpha atoms")
    ("sel,s", po::value<String>(&sel)->default_value(""), "selection performed on reference structure")
    ("tolerant,t", "fault tolerant mode")
    ("structural-checks,f", "perform stereo-chemical and clash checks")
    ("version,e", "version")
    ("parameter-file,p", po::value<String>(), "stereo-chemical parameter file")
    ("verbosity,v", po::value<int>(), "verbosity level")
    ("bond_tolerance,b", po::value<Real>(), "tolerance in stddev for bonds")
    ("angle_tolerance,a", po::value<Real>(), "tolerance in stddev for angles")
    ("inclusion_radius,r", po::value<Real>(), "distance inclusion radius")
    ("files", po::value< std::vector<String> >(), "input file(s)")
    ("reference",po::value<String>(),"reference(s)")
  ;
  po::positional_options_description p;
  p.add("files", -1);
  po::variables_map vm;
  try {
    po::store(po::command_line_parser(argc, argv).
                options(desc).positional(p).run(),
              vm);
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
    usage();
  }
  po::notify(vm);
  if (vm.count("version")) {
    std::cout << "Version: " << version << std::endl;
    exit(0);
  }
  std::vector<String> files;
  if (vm.count("files")) {
    files=vm["files"].as<std::vector<String> >();
  } else {
    usage();
    exit(-1);
  }
  if (vm.count("calpha")) {
    profile.calpha_only=true;
  }
  if (vm.count("structural-checks")) {
    structural_checks=true;
  }
  if (vm.count("tolerant")) {
    profile.fault_tolerant=true;
  }
  String parameter_filename;
  if (vm.count("parameter-file")) {
      parameter_filename=vm["parameter-file"].as<String>();
    } else if (structural_checks==true) {
    std::cout << "Please specify a stereo-chemical parameter file" << std::endl;
    exit(-1);
  }
  int verbosity_level=0;
  int ost_verbosity_level=2;
  if (vm.count("verbosity")) {
    verbosity_level=vm["verbosity"].as<int>();
    if (verbosity_level==0) {
      ost_verbosity_level=2;    
    } else if (verbosity_level==1) {
      ost_verbosity_level=3;
    } else if (verbosity_level==2) {
      ost_verbosity_level=4;
    } else {
      std::cout << "Verbosity level " << verbosity_level << " is not available" << std::endl;
      exit(-1);
    }
  }

  Logger::Instance().PushVerbosityLevel(ost_verbosity_level);

  if (vm.count("bond_tolerance")) {
    bond_tolerance=vm["bond_tolerance"].as<Real>();
  }
  if (vm.count("angle_tolerance")) {
    angle_tolerance=vm["angle_tolerance"].as<Real>();
  }
  if (vm.count("inclusion_radius")) {
    radius=vm["inclusion_radius"].as<Real>();
  }

  std::vector<Real> cutoffs;
  cutoffs.push_back(0.5);
  cutoffs.push_back(1.0);
  cutoffs.push_back(2.0);
  cutoffs.push_back(4.0);
    
  // loads the reference file and creates the list of distances to check in lddt    
  // if the reference file is a comma-separated list of files, switches to multi-
  // reference mode
  GlobalRDMap glob_dist_list; 
  String ref_file=files.back();    
  ost::StringRef ref_file_sr(ref_file.c_str(),ref_file.length());
  std::vector<StringRef> ref_file_split_sr=ref_file_sr.split(',');
  if (ref_file_split_sr.size()==1) { 
    std::cout << "Multi-reference mode: Off" << std::endl;  
    String ref_filename = ref_file_split_sr[0].str();  
    EntityHandle ref=load(ref_filename, profile);
    if (!ref) {
      exit(-1);
    }  
    glob_dist_list = CreateDistanceList(ref.CreateFullView(),radius);  
  } else {
    std::cout << "Multi-reference mode: On" << std::endl;  
    std::vector<EntityView> ref_list;  
    for (std::vector<StringRef>::const_iterator ref_file_split_sr_it = ref_file_split_sr.begin();
         ref_file_split_sr_it != ref_file_split_sr.end();++ref_file_split_sr_it) {
      String ref_filename = ref_file_split_sr_it->str();  
      EntityHandle ref=load(ref_filename, profile);
      if (!ref) {
        exit(-1);
      }
      ref_list.push_back(ref.CreateFullView());
    } 
    glob_dist_list = CreateDistanceListFromMultipleReferences (ref_list,cutoffs,radius);  
  }         
  files.pop_back();

  // prints out parameters used in the lddt calculation
  std::cout << "Verbosity level: " << verbosity_level << std::endl;
  if (structural_checks) {
    std::cout << "Stereo-chemical and steric clash checks: On " << std::endl;
  } else {
    std::cout << "Stereo-chemical and steric clash checks: Off " << std::endl;
  }
  std::cout << "Inclusion Radius: " << radius << std::endl;
  if (structural_checks) {
    std::cout << "Parameter filename: " << parameter_filename << std::endl;
    std::cout << "Tolerance in stddevs for bonds: " << bond_tolerance << std::endl;
    std::cout << "Tolerance in stddevs for angles: " << angle_tolerance << std::endl;
    LOG_INFO("Log entries format:");
    LOG_INFO("BOND INFO FORMAT:  Chain  Residue  ResNum  Bond  Min  Max  Observed  Z-score  Status");
    LOG_INFO("ANGLE INFO FORMAT:  Chain  Residue  ResNum  Angle  Min  Max  Observed  Z-score  Status");
    LOG_INFO("CLASH INFO FORMAT:  Chain1  Residue1  ResNum1  Atom1  Chain2  Residue2  ResNum2  Atom2  Observed  Difference  Status");
  }
  LOG_INFO("LDDT INFO FORMAT:  Chain1  Residue1  ResNum1  Atom1  Chain2  Residue2  ResNum2  Atom2  ModelDist  TargetDist  Difference  Tolerance Status");

  // cycles through the models to evaluate 
  for (size_t i=0; i<files.size(); ++i) {
    EntityHandle model=load(files[i], profile);
    if (!model) {
      if (!profile.fault_tolerant) {
        exit(-1);
      }
      continue;
    }
    EntityView v=model.CreateFullView();

    // The code in this following block is only used to make CASP9 models load correctly and normally commented out
    EntityView model2=model.Select("aname!=CEN,NV,OT1,OT,CAY,CY,OXT,1OCT,NT,OT2,2OCT,OVL1,OC1,O1,OC2,O2,OVU1");
    EntityView v1=model2.Select("not (rname==GLY and aname==CB)");
    boost::filesystem::path pathstring(files[i]);
    #if BOOST_FILESYSTEM_VERSION==3 || BOOST_VERSION<103400
    String filestring=pathstring.string();
    #else
    String filestring=pathstring.file_string();
    #endif      
    if (filestring.substr(5,5)=="TS257" || filestring.substr(5,5)=="TS458" ) {
      for (AtomHandleIter ait=v1.GetHandle().AtomsBegin();ait!=v1.GetHandle().AtomsEnd();++ait){
        AtomHandle aitv = *ait;
        String atomname=aitv.GetName();            
        String firstletter=atomname.substr(0,1);
        aitv.SetElement(firstletter);
      }
    }  
    v=v1;  
    std::cout << "File: " << files[i] << std::endl; 
    std::pair<int,int> cov = compute_coverage(v,glob_dist_list);
    std::cout << "Coverage: " << (float(cov.first)/float(cov.second)) << " (" << cov.first << " out of " << cov.second << " residues)" << std::endl;

    if (structural_checks) {
      // reads in parameter files   
      boost::filesystem::path loc(parameter_filename);
      boost::filesystem::ifstream infile(loc);
      if (!infile) {
        std::cout << "Could not find " << parameter_filename << std::endl;
        exit(-1);
      }
      std::vector<String> stereo_chemical_props;
      String line;
      while (std::getline(infile, line))
      {
        std::stringstream line_stream(line);
        stereo_chemical_props.push_back(line);
      }
      StereoChemicalParams bond_table = FillStereoChemicalParams("Bond",stereo_chemical_props);
      if (bond_table.IsEmpty()) {
        std::cout << "Error reading the Bond section of the stereo-chemical parameter file." << std::endl;
        exit(-1);
      }
      StereoChemicalParams angle_table = FillStereoChemicalParams("Angle",stereo_chemical_props);
      if (angle_table.IsEmpty()) {
        std::cout << "Error reading the Angles section of the stereo-chemical parameter file." << std::endl;
        exit(-1);
      }

      ClashingDistances nonbonded_table = FillClashingDistances(stereo_chemical_props);

      if (nonbonded_table.IsEmpty()) {
        std::cout << "Error reading the Clashing section of the stereo-chemical parameter file." << std::endl;
        exit(-1);
      }
      // performs structural checks and filters the structure   
      v=alg::CheckStereoChemistry(v,bond_table,angle_table,bond_tolerance,angle_tolerance);
      cov = compute_coverage(v,glob_dist_list);
      std::cout << "Coverage after stereo-chemical checks: " << (float(cov.first)/float(cov.second)) << " (" << cov.first << " out of " << cov.second << " residues)" << std::endl;
      v=alg::FilterClashes(v,nonbonded_table);
      cov = compute_coverage(v,glob_dist_list);
      std::cout << "Coverage after clashing checks: " << (float(cov.first)/float(cov.second)) << " (" << cov.first << " out of " << cov.second << " residues)" << std::endl;
    }
    if (cov.first==0) {
      std::cout << "Global LDDT score: 0.0" << std::endl;
      return 0;
    }

    // computes the lddt score   
    String label="localldt";
    std::pair<int,int> total_ov=alg::LocalDistDiffTest(v, glob_dist_list, cutoffs, label);
    Real lddt = static_cast<Real>(total_ov.first)/(static_cast<Real>(total_ov.second) ? static_cast<Real>(total_ov.second) : 1);
    std::cout << "Global LDDT score: " << lddt << std::endl;
    std::cout << "(" << std::fixed << total_ov.first << " conserved distances in the model out of " << total_ov.second  << " checked)" << std::endl;

    // prints the residue-by-residue statistics  
    std::cout << "Local LDDT Score:" << std::endl;
    std::cout << "Chain\tResName\tResNum\tScore\t(Conserved/Total)" << std::endl;
    for (ResidueViewIter rit=v.ResiduesBegin();rit!=v.ResiduesEnd();++rit){
      ResidueView ritv = *rit;
      Real lddt_local = 0;
      int conserved_dist = 0;
      int total_dist = 0;      
      if (ritv.HasProp(label)) {
          lddt_local=ritv.GetFloatProp(label);
          conserved_dist=ritv.GetIntProp(label+"_conserved");
      total_dist=ritv.GetIntProp(label+"_total");
      }
      if (lddt_local!=0) {
        std::cout << ritv.GetChain() << "\t" << ritv.GetName() << "\t" << ritv.GetNumber() << '\t' << lddt_local << "\t" << "("<< conserved_dist << "/" << total_dist << ")" <<std::endl;
      }
    }
    std::cout << std::endl;
  }
  return 0;
}
