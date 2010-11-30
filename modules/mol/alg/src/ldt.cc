#include <ost/mol/alg/local_dist_test.hh>
#include <ost/io/mol/pdb_reader.hh>
#include <ost/io/io_exception.hh>
using namespace ost;
using namespace ost::io;
using namespace ost::mol;

EntityHandle load(const String& file)
{
  try {
    PDBReader reader(file);
    if (reader.HasNext()) {
      EntityHandle ent=CreateEntity();
      reader.Import(ent);
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
  // parse options
  String sel;
  int flags=0;
  bool filter_clashes=false;
  char ch=0;
  while((ch=getopt(argc, argv, "ftcs:"))!=-1) {
    switch (ch) {
      case 'c':
        flags|=PDB::CALPHA_ONLY;
        break;
      case 't':
        flags|=PDB::SKIP_FAULTY_RECORDS;
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
  PDB::PushFlags(flags);

  String ref_file=argv[argc-1];
  EntityHandle ref=load(ref_file);
  if (!ref) {
    return -1;
  }
  EntityView ref_view=ref.Select(sel);
  for (int i=0; i<argc-1; ++i) {
    EntityHandle model=load(argv[i]);
    if (!model) {
      if (!(flags&PDB::SKIP_FAULTY_RECORDS)) {
        return -1;
      }
      continue;
    }
    EntityView v=model.CreateFullView();
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