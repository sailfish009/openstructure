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
// along with this library; if not, write to the Free Software Foundation, Inc.,
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



void FillStereoChemicalParams(const String& header, StereoChemicalParams& table, std::vector<String>& stereo_chemical_props_file)
{
  bool found=false;
  std::vector<String>::const_iterator line_iter=stereo_chemical_props_file.begin();
  while (line_iter!=stereo_chemical_props_file.end()) {
    if ((*line_iter).length()!=0) {
      StringRef line_string_ref(line_iter->data(),(*line_iter).length());
      std::vector<StringRef> line_str_vec = line_string_ref.split();
      if (line_str_vec[0].str()==header) {
        found=true;
        line_iter++;
        while ((*line_iter)[0]!='-') {
	  if ((*line_iter)[0]!='#') {
	    StringRef second_line_string_ref(line_iter->data(),(*line_iter).length());
	    std::vector<StringRef> second_line_str_vec = second_line_string_ref.split();
	    if (second_line_str_vec.size()!=4) {
	      throw Error("The number of elements in one of the lines is wrong");            
	    } 
	    StringRef item = second_line_str_vec[0];
	    String res = second_line_str_vec[1].str();	  
	    std::pair<bool,float> parse_value = second_line_str_vec[2].to_float();
	    std::pair<bool,float> parse_stddev = second_line_str_vec[3].to_float();
	    Real value,stddev;
	    if (parse_value.first==true) {
	      value=static_cast<Real>(parse_value.second);
	    } else {
	      throw Error("One of the values in the third column is not a number");
	    };
	    if (parse_stddev.first==true) {
	      stddev=static_cast<Real>(parse_stddev.second);
	    } else {
	      throw Error("One of the values in the fourth column is not a number");
	    };
	    std::vector<StringRef> split_item = item.split('-');
	    String rearranged_item;
	    if (split_item.size() == 2) {
	      String atom1 = split_item[0].str();
	      String atom2 = split_item[1].str();
              if (atom2 < atom1) {
	         std::stringstream srearr;
	         srearr << atom2 << "-" << atom1;
	         rearranged_item=srearr.str();		     
	      } else {
		 rearranged_item = item.str();
              }	  
            } else if (split_item.size() == 3) {
              String atom1 = split_item[0].str();
              String atom = split_item[1].str();
              String atom2 = split_item[2].str();
              if (atom2 < atom1) {
                 std::stringstream srearr;
                 srearr << atom2 << "-" << atom << "-" << atom1;
                 rearranged_item=srearr.str();                
              } else {
                 rearranged_item = item.str();
              }                
            } else {
              throw Error("One of the strings describing the parameter has the wrong format");
            }            
            table.SetParam(rearranged_item,res,value,stddev);
            line_iter++;
            }  
        }
      }  
    }
    line_iter++;    
  }
  if (found==false) {
    throw Error("Could not find the relevant section in the stereo-chemical parameter file");
  };    
};  

void FillClashingDistances(ClashingDistances& table,  std::vector<String>& stereo_chemical_props_file)
{
  bool found=false;
  std::vector<String>::const_iterator line_iter=stereo_chemical_props_file.begin();
  while (line_iter!=stereo_chemical_props_file.end()) {
    if ((*line_iter).length()!=0) {
      StringRef line_string_ref(line_iter->data(),(*line_iter).length());
      std::vector<StringRef> line_str_vec = line_string_ref.split();
      if (line_str_vec[0].str()=="Non-bonded") {
        found=true;
        line_iter++;
        while ((*line_iter)[0]!='-') {
	  if ((*line_iter)[0]!='#') {
	    StringRef second_line_string_ref(line_iter->data(),(*line_iter).length());
	    std::vector<StringRef> second_line_str_vec = second_line_string_ref.split();
	    if (second_line_str_vec.size()!=3) {
	      throw Error("The number of elements in one of the lines is wrong");            
	    } 
	    String item = second_line_str_vec[0].str();

	    std::pair<bool,float> parse_value = second_line_str_vec[1].to_float();
	    std::pair<bool,float> parse_stddev = second_line_str_vec[2].to_float();
	    Real value,stddev;
	    if (parse_value.first==true) {
	      value=static_cast<Real>(parse_value.second);
	    } else {
	      throw Error("One of the distance values is not a number");
	    };
	    if (parse_stddev.first==true) {
	      stddev=static_cast<Real>(parse_stddev.second);
	    } else {
	      throw Error("One of the tolerance values is not a number");
	    }
	    StringRef itemsr(item.data(),item.length());
	    std::vector<StringRef> eles = itemsr.split('-');
            if (itemsr.size() != 3) {
	      throw Error("One of the strings describing the interacting atoms has the wrong format");
	    }  
	    String ele1=eles[0].str();
	    String ele2=eles[1].str();
            if (ele2 < ele1) {
              table.SetClashingDistance(ele2,ele1,value,stddev);
            } else {
              table.SetClashingDistance(ele1,ele2,value,stddev);
            }  
	    line_iter++;
	  }  
        }
      }  
    }
    line_iter++;    
  }
  if (found==false) {
    throw Error("Could not find the relevant section in the stereo-chemical parameter file");
  }    
}  

EntityHandle load(const String& file, const IOProfile& profile)
{
  try {
    PDBReader reader(file, profile);
    if (reader.HasNext()) {
      EntityHandle ent=CreateEntity();
      reader.Import(ent);
      conop::Conopology& conop_inst=conop::Conopology::Instance();

      conop::BuilderP builder=conop::Conopology::Instance().GetBuilder("DEFAULT");
      conop::RuleBasedBuilderPtr rbb=dyn_cast<conop::RuleBasedBuilder>(builder);
      if (!rbb) {
       std::cout << "Nope,not rule-based" << std::endl;
      } else {
       std::cout << "Yep,rule-based" << std::endl;
      }

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
  std::cerr << "   -f         filter clashes" << std::endl;
  std::cerr << "   -t         fault tolerant parsing" << std::endl;
  std::cerr << "   -p <file>  use specified parmeter file. Mandatory" << std::endl;
  std::cerr << "   -v <level> verbosity level (0=results only,1=problems reported, 2=full report)" << std::endl;
  std::cerr << "   -b <value> tolerance in stddevs for bonds" << std::endl;
  std::cerr << "   -a <value> tolerance in stddevs for angles" << std::endl;
  std::cerr << "   -m <value> clashing distance for unknwon atom types" << std::endl;
}

int main (int argc, char **argv)
{
  
  Real min_default_distance = 1.5;
  Real min_distance_tolerance = 0.0;
  Real bond_tolerance = 5.0;
  Real angle_tolerance = 5.0;
  
  IOProfile profile;
  profile.bond_feasibility_check=false;
  // parse options
  String sel;
  bool filter_clashes=false;
  po::options_description desc("Options");
  desc.add_options()
    ("calpha,c", "consider only calpha atoms")
    ("sel,s", po::value<String>(&sel)->default_value(""), "selection for reference")
    ("tolerant,t", "fault tolerant mode")
    ("filter-clashes,f", "filter clashes")
    ("parameter-file,p", po::value<String>(), "stereo chemical parameter file")
    ("verbosity,v", po::value<int>(), "verbosity level")    
    ("tolerance_bonds,b", po::value<Real>(), "tolerance in stddev for bonds")    
    ("tolerance_angles,a", po::value<Real>(), "tolerance in stddev for angles") 
    ("default_clash,m", po::value<Real>(), "clashing distance for unknown atom types")     
    ("files", po::value< std::vector<String> >(), "input file")    
  ;
  po::positional_options_description p;
  p.add("files", -1);
  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
                options(desc).positional(p).run(),
            vm);
  po::notify(vm);
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
  if (vm.count("filter-clashes")) {
    filter_clashes=true;
  }
  if (vm.count("tolerant")) {
    profile.fault_tolerant=true;
  }
  String parameter_filename;  
  if (vm.count("parameter-file")) {
    parameter_filename=vm["parameter-file"].as<String>();
  } else {
    std::cout << "Please specify a stereo-chemical parameter file" << std::endl;
    exit(-1);
  }
  int verbosity_level=0;
  if (vm.count("verbosity")) {
    verbosity_level=vm["verbosity"].as<int>();
    if (verbosity_level==0) {
      Logger::Instance().PushVerbosityLevel(0);
    } else if (verbosity_level==1) {
      Logger::Instance().PushVerbosityLevel(3);
    } else if (verbosity_level==2) {
      Logger::Instance().PushVerbosityLevel(4);
    } else {
      std::cout << "Verbosity level " << verbosity_level << " is not available" << std::endl;
      exit(-1);
    }
  }  
  if (vm.count("tolerance_bonds")) {
    bond_tolerance=vm["tolerance_bonds"].as<Real>();
  }
  if (vm.count("tolerance_angles")) {
    angle_tolerance=vm["tolerance_angles"].as<Real>();
  }
  if (vm.count("default_clash")) {
    min_default_distance=vm["default_clash"].as<Real>();
  }
  boost::filesystem::path loc(parameter_filename);
  boost::filesystem::ifstream infile(loc);
  if (!infile) {
    std::cout << "Couldn't find " << parameter_filename << std::endl;
    exit(-1);
  }
  std::vector<String> stereo_chemical_props;
  String line;
  while (std::getline(infile, line))
  {
   std::stringstream line_stream(line);
   stereo_chemical_props.push_back(line);
  }

  String ref_file=files.back();
  EntityHandle ref=load(ref_file, profile);
  if (!ref) {
    exit(-1);
  }
  files.pop_back();
  EntityView ref_view=ref.Select(sel);
  std::cout << "Parameter filename: " << parameter_filename << std::endl;
  std::cout << "Verbosiy level: " << verbosity_level << std::endl;
  if (filter_clashes) {
    std::cout << "Clash filtering: On " << std::endl;
  } else {
    std::cout << "Clash filtering: Off " << std::endl;
  }
  if (filter_clashes) {
    std::cout << "Tolerance in stddevs for bonds: " << bond_tolerance << std::endl;
    std::cout << "Tolerance in stddevs for angles: " << angle_tolerance << std::endl;
    std::cout << "Clashing distance for unknown atom types: " << min_default_distance << std::endl;
  }    
  for (size_t i=0; i<files.size(); ++i) {
    EntityHandle model=load(files[i], profile);
    if (!model) {
      if (!profile.fault_tolerant) {
        exit(-1);
      }
      continue;
    }    
    StereoChemicalParams bond_table, angle_table;
    ClashingDistances nonbonded_table(min_default_distance,min_distance_tolerance);    
    EntityView v=model.CreateFullView();
    if (filter_clashes) {
      try {
	FillStereoChemicalParams("Bond",bond_table,stereo_chemical_props);
      }	
      catch (Error& e) {
	std::cout << "Error reading 'Bond Lengths' section of the stereo-chemical parameter file." << std::endl;
        std::cout << e.what() << std::endl;
        exit(-1);
      } 
      try {
	FillStereoChemicalParams("Angle",angle_table,stereo_chemical_props); 
      }  
      catch (Error& e) {
	std::cout << "Error reading 'Angles' section of the stereo-chemical parameter file." << std::endl;
        std::cout << e.what() << std::endl;
        exit(-1);
      }    
      
      try {
	FillClashingDistances(nonbonded_table,stereo_chemical_props);  
      } 
      catch (Error& e) {
	std::cout << "Error reading 'Non-bonded Distances' section of the stereo-chemical parameter file." << std::endl;
        std::cout << e.what() << std::endl;
        exit(-1);
      }  
      
      v=alg::CheckStereoChemistry(v,bond_table,angle_table,bond_tolerance,angle_tolerance);
      v=alg::FilterClashes(v,nonbonded_table);
    }
    float cutoffs[]={0.5,1,2,4};
    String labels[]={"localldt0.5","localldt1","localldt2","ldtlocal4"};
    float ldt=0.0;    
    for (int n=0; n<4; ++n) { 
      ldt+=alg::LocalDistTest(v, ref_view, cutoffs[n], 8.0,labels[n]);
    }      
    ldt/=4.0;
    std::cout << "File: " << files[i] << std::endl; 
    std::cout << "Global LDT score: " << ldt << std::endl;
    std::cout << "Local LDT Score:" << std::endl;
    std::cout << "Chain\tResName\tResNum\tScore" << std::endl;
    
    for (ResidueViewIter rit=v.ResiduesBegin();rit!=v.ResiduesEnd();++rit){
      ResidueView ritv = *rit;
      Real ldt_local_sum = 0;
      for (int n=0; n<4; ++n) { 
        ldt_local_sum+=ritv.GetFloatProp(labels[n]);
      }      
      ldt_local_sum/=4.0;
      std::cout << ritv.GetChain() << "\t" << ritv.GetName() << "\t" << ritv.GetNumber() << '\t' << ldt_local_sum << std::endl;
    }  
    std::cout << std::endl;
  }
  return 0;
}      
