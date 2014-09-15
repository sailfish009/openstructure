#include <ost/mol/mm/mm_observer.hh>

namespace ost { namespace mol{ namespace mm{

void TrajObserver::Init(boost::shared_ptr<OpenMM::Context> c, 
                        TopologyPtr top){

  if(registered_){
    throw ost::Error("Can register observer to only one simulation!");
  }

  registered_ = true;

  context_ = c;
  ost::mol::EntityHandle ent = top->GetEntity().Copy();
  MMModeller::AssignPDBNaming(ent);
  c_group_ = ost::mol::CreateCoordGroup(ent.GetAtomList());
}

void TrajObserver::Notify(){

  geom::Vec3List positions;
  StateExtractor::ExtractPositions(context_,positions,true);
  c_group_.AddFrame(positions);
}


void TrajWriter::Init(boost::shared_ptr<OpenMM::Context> c, 
                      TopologyPtr top){

  if(registered_){
    throw ost::Error("Can register observer to only one simulation!");
  }

  registered_ = true;

  context_ = c;
  ost::io::IOProfile profile("CHARMM",false,false,false,false,false);
  ost::io::PDBWriter writer(pdb_filename_, profile);
  writer.Write(top->GetEntity().GetAtomList());

  stream_.open(dcd_filename_.c_str(), std::ios::binary);

  x.resize(top->GetNumAtoms());
  y.resize(top->GetNumAtoms());
  z.resize(top->GetNumAtoms());


  // size of first header block in bytes
  int32_t magic_number=84;
  stream_.write(reinterpret_cast<char*>(&magic_number), 4);

  // magic string
  char crd[]={'C', 'O', 'R', 'D'};
  stream_.write(crd, 4);

  // icntrl[0], NSET, number of frames
  // we don't know the number of frames yet, will be written
  // when the finalize function is called.
  int32_t zero=0;
  stream_.write(reinterpret_cast<char*>(&zero), 4);

  // icntrl[1], ISTART, starting timestep
  stream_.write(reinterpret_cast<char*>(&zero), 4);

  // icntrl[2], NSAVC, timesteps between DCD saves
  int32_t one=1;
  stream_.write(reinterpret_cast<char*>(&one), 4);

  // icntrl[3] to icntrl[7], unused
  for (int i=3; i<=7; ++i) {
    stream_.write(reinterpret_cast<char*>(&zero), 4);
  }

  // icntrl[8], NAMNF, number of fixed atoms
  stream_.write(reinterpret_cast<char*>(&zero), 4);

  // icntrl[9], DELTA, timestep as float for CHARMM format
  float delta=1.0;
  stream_.write(reinterpret_cast<char*>(&delta), 4);

  // icntrl[10], CHARMM format: ucell per frame
  stream_.write(reinterpret_cast<char*>(&one), 4);

  // icntrl[11] to icntrl[18], unused
  for (int i=11; i<=18; ++i) {
    stream_.write(reinterpret_cast<char*>(&zero), 4);
  }

  // icntrl[19], charmm version
  int32_t charmm_version=24;
  stream_.write(reinterpret_cast<char*>(&charmm_version), 4);
  // bracket first header block
  stream_.write(reinterpret_cast<char*>(&magic_number), 4);

  //  no titles in title block
  int32_t four=4;
  stream_.write(reinterpret_cast<char*>(&four), 4);
  stream_.write(reinterpret_cast<char*>(&zero), 4);
  stream_.write(reinterpret_cast<char*>(&four), 4);

  // atom count block
  stream_.write(reinterpret_cast<char*>(&four), 4);
  int32_t atom_count=top->GetNumAtoms();  
  stream_.write(reinterpret_cast<char*>(&atom_count), 4);
  stream_.write(reinterpret_cast<char*>(&four), 4);
}

void TrajWriter::Notify(){

  if(!registered_){
    throw ost::Error("Trajectory writer is not registered to any simulation!");
  }

  if(!stream_){
    throw ost::Error("Trajectory writer does not contain open filestream. Did you already call the finalize funciton before?");
  }

  geom::Vec3List positions;
  StateExtractor::ExtractPositions(context_,positions,true,true);
  OpenMM::State openmm_state = context_->getState(0); //create minimal state
                                                      //to only extract periodic
                                                      //box information
  openmm_state.getPeriodicBoxVectors(ucell_a,ucell_b,ucell_c);

  int32_t out_n=positions.size()*4;

  // ucell
  int32_t bsize=48; // ucell block size, 6 doubles
  //context_->getSystem().getDefaultPeriodicBoxVectors(ucell_a,ucell_b,ucell_c);

  // a,alpha,b,beta,gamma,c (don't ask)
  // OpenMM only supports rectangular boxes, since we have to add
  // the cosines of the angles, we can directly set the values to 0.0

  double ucell[]={10 * sqrt(ucell_a.dot(ucell_a)),
                  0.0,
                  10 * sqrt(ucell_b.dot(ucell_b)),
                  0.0,
                  0.0,
                  10 * sqrt(ucell_c.dot(ucell_c))};
  stream_.write(reinterpret_cast<char*>(&bsize),4);
  stream_.write(reinterpret_cast<char*>(ucell),bsize);
  stream_.write(reinterpret_cast<char*>(&bsize),4);

  int k=0;
  for (geom::Vec3List::iterator i=positions.begin(); 
       i!=positions.end(); ++i, ++k) {
    x[k]=float((*i)[0]);
    y[k]=float((*i)[1]);
    z[k]=float((*i)[2]);
  }
  stream_.write(reinterpret_cast<char*>(&out_n), 4);
  stream_.write(reinterpret_cast<char*>(&x[0]), out_n);
  stream_.write(reinterpret_cast<char*>(&out_n), 4);
  stream_.write(reinterpret_cast<char*>(&out_n), 4);
  stream_.write(reinterpret_cast<char*>(&y[0]), out_n);
  stream_.write(reinterpret_cast<char*>(&out_n), 4);
  stream_.write(reinterpret_cast<char*>(&out_n), 4);
  stream_.write(reinterpret_cast<char*>(&z[0]), out_n);
  stream_.write(reinterpret_cast<char*>(&out_n), 4);

  ++frames_;

  stream_.seekp(8,std::ios_base::beg);
  stream_.write(reinterpret_cast<char*>(&frames_),4);
  stream_.seekp(0,std::ios_base::end);
}

void TrajWriter::Finalize(){
  if(!registered_){
    throw ost::Error("Trajectory writer is not registered to any simulation!");
  }
  if(!stream_){
    throw ost::Error("Trajectory writer does not contain open filestream. Did you already call the finalize function before?");
  }
  stream_.close();
}

}}}// ns
