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
#if defined (_MSC_VER)
#define BOOST_ALL_DYN_LINK 1
#endif
#include <boost/program_options.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/convenience.hpp>
#include <ost/mol/alg/local_dist_test.hh>
#include <ost/mol/alg/filter_clashes.hh>
#include <ost/io/mol/pdb_reader.hh>
#include <ost/io/io_exception.hh>
#include <ost/conop/conop.hh>
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

void usage()
{
  std::cerr << "usage: ldt [options] <mod1> [mod1 [mod2]] <ref>" << std::endl;
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
  String version = "Beta - 2012-01-17";
  Real min_default_distance = 1.5;
  Real min_distance_tolerance = 0.0;
  Real bond_tolerance = 8.0;
  Real angle_tolerance = 8.0;
  Real radius=15.0;
  
  IOProfile profile;
  profile.bond_feasibility_check=false;
  // parse options
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
    ("default_clash,m", po::value<Real>(), "clashing distance for unknown atom types")
    ("inclusion_radius,r", po::value<Real>(), "distance inclusion radius")
    ("files", po::value< std::vector<String> >(), "input file(s)")
	("reference",po::value<String>(),"reference(s)")
  ;
  po::positional_options_description p;
  p.add("files", -1);
  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
                options(desc).positional(p).run(),
            vm);
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
  if (vm.count("default_clash")) {
    min_default_distance=vm["default_clash"].as<Real>();
  }
  if (vm.count("inclusion_radius")) {
    radius=vm["inclusion_radius"].as<Real>();
  }
  String ref_file=files.back();
  EntityHandle ref=load(ref_file, profile);
  if (!ref) {
    exit(-1);
  }
  files.pop_back();
  EntityView ref_view=ref.Select(sel);
  GlobalRDMap glob_dist_list = CreateDistanceList(ref_view,radius);
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
    std::cout << "Clashing distance for unknown atom types: " << min_default_distance << std::endl;
    LOG_INFO("Log entries format:");
    LOG_INFO("BOND INFO FORMAT:  Chain  Residue  ResNum  Bond  Min  Max  Observed  Z-score  Status");
    LOG_INFO("ANGLE INFO FORMAT:  Chain  Residue  ResNum  Angle  Min  Max  Observed  Z-score  Status");
    LOG_INFO("CLASH INFO FORMAT:  Chain1  Residue1  ResNum1  Atom1  Chain2  Residue2  ResNum2  Atom2  Observed  Difference  Status");
  }
  LOG_INFO("LDT INFO FORMAT:  Chain1  Residue1  ResNum1  Atom1  Chain2  Residue2  ResNum2  Atom2  ModelDist  TargetDist  Difference  Tolerance Status");
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

      ClashingDistances nonbonded_table = FillClashingDistances(stereo_chemical_props,min_default_distance,min_distance_tolerance);

      if (nonbonded_table.IsEmpty()) {
        std::cout << "Error reading the Clashing section of the stereo-chemical parameter file." << std::endl;
        exit(-1);
      }  
      v=alg::CheckStereoChemistry(v,bond_table,angle_table,bond_tolerance,angle_tolerance);
      cov = compute_coverage(v,glob_dist_list);
      std::cout << "Coverage after stereo-chemical checks: " << (float(cov.first)/float(cov.second)) << " (" << cov.first << " out of " << cov.second << " residues)" << std::endl;
      v=alg::FilterClashes(v,nonbonded_table);
      cov = compute_coverage(v,glob_dist_list);
      std::cout << "Coverage after clashing checks: " << (float(cov.first)/float(cov.second)) << " (" << cov.first << " out of " << cov.second << " residues)" << std::endl;
    }
    if (cov.first==0) {
      std::cout << "Global LDT score: 0.0" << std::endl;
      return 0;
    }
    Real ldt=LDTHA(v, glob_dist_list);	

    std::cout << "Global LDT score: " << ldt << std::endl;
    std::cout << "Local LDT Score:" << std::endl;
    std::cout << "Chain\tResName\tResNum\tScore" << std::endl;
    for (ResidueViewIter rit=v.ResiduesBegin();rit!=v.ResiduesEnd();++rit){
      ResidueView ritv = *rit;
      Real ldt_local = 0;
      if (ritv.HasProp("localldt")) {
          ldt_local=ritv.GetFloatProp("localldt");
      }
      std::cout << ritv.GetChain() << "\t" << ritv.GetName() << "\t" << ritv.GetNumber() << '\t' << ldt_local << std::endl;
    }
    std::cout << std::endl;
  }
  return 0;
}
