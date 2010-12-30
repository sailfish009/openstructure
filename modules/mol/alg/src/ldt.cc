//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2010 by the OpenStructure authors
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
#include <ost/mol/alg/local_dist_test.hh>
#include <ost/mol/alg/filter_clashes.hh>
#include <ost/io/mol/pdb_reader.hh>
#include <ost/io/io_exception.hh>
#include <ost/conop/conop.hh>
using namespace ost;
using namespace ost::io;
using namespace ost::mol;

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
  std::cerr << "   -s        selection performed on ref" << std::endl;
  std::cerr << "   -c        use Calphas only" << std::endl;
  std::cerr << "   -f        filter clashes (not implemented yet)" << std::endl;
  std::cerr << "   -t        fault tolerant parsing" << std::endl;  
}

int main (int argc, char* const *argv)
{
  IOProfile profile;
  // parse options
  String sel;
  bool filter_clashes=false;
  char ch=0;
  while((ch=getopt(argc, argv, "ftcs:"))!=-1) {
    switch (ch) {
      case 'c':
        profile.calpha_only=true;
        break;
      case 't':
        profile.fault_tolerant=true;
        break;
      case 'f':
        filter_clashes=true;
        break;
      case 's':
        sel=optarg;
        break;
      case '?':
      default:
        usage();
        return -1;
    }
  }
  argc-=optind;
  argv+=optind;
  if (argc<2) {
    usage();
    return -1;
  }  

  String ref_file=argv[argc-1];
  EntityHandle ref=load(ref_file, profile);
  if (!ref) {
    return -1;
  }
  EntityView ref_view=ref.Select(sel);
  for (int i=0; i<argc-1; ++i) {
    EntityHandle model=load(argv[i], profile);
    if (!model) {
      if (!profile.fault_tolerant) {
        return -1;
      }
      continue;
    }
    EntityView v=model.CreateFullView();
    if (filter_clashes) {
      v=alg::FilterClashes(v);
    }
    float cutoffs[]={0.5,1,2,4};
    float ldt=0.0;
    for (int n=0; n<4; ++n) {
      ldt+=alg::LocalDistTest(v, ref_view, cutoffs[n], 8.0);
    }
    ldt/=4.0;
    std::cout << argv[i] << " " << ldt << std::endl;
  }
  return 0;
}