#include <ost/mol/mm/state_extractor.hh>


namespace ost { namespace mol{ namespace mm{

void StateExtractor::ExtractPotentialEnergy(boost::shared_ptr<OpenMM::Context> context,
  	                        Real& energy){

  OpenMM::State s = context->getState(OpenMM::State::Energy);
  energy = s.getPotentialEnergy();
}

void StateExtractor::ExtractKineticEnergy(boost::shared_ptr<OpenMM::Context> context,
	                      Real& energy){

  OpenMM::State s = context->getState(OpenMM::State::Energy);
  energy = s.getKineticEnergy();
}

void StateExtractor::ExtractEnergy(boost::shared_ptr<OpenMM::Context> context,
	               Real& energy){

  OpenMM::State s = context->getState(OpenMM::State::Energy);
  energy = s.getKineticEnergy() + s.getPotentialEnergy();
}

void StateExtractor::ExtractPositions(boost::shared_ptr<OpenMM::Context> context,
	                    geom::Vec3List& positions, bool enforce_periodic_box, bool in_angstrom){

  positions.clear();
  OpenMM::State s = context->getState(OpenMM::State::Positions, enforce_periodic_box);
  std::vector<OpenMM::Vec3> p = s.getPositions();
  for(std::vector<OpenMM::Vec3>::iterator i = p.begin(); i != p.end(); ++i){
    positions.push_back(geom::Vec3((*i)[0],(*i)[1],(*i)[2]));
  }
  if(in_angstrom) positions*=10.0;
}

void StateExtractor::ExtractVelocities(boost::shared_ptr<OpenMM::Context> context,
	                                     geom::Vec3List& velocities){

  velocities.clear();
  OpenMM::State s = context->getState(OpenMM::State::Velocities);
  std::vector<OpenMM::Vec3> v = s.getVelocities();
  for(std::vector<OpenMM::Vec3>::iterator i = v.begin(); i != v.end(); ++i){
    velocities.push_back(geom::Vec3((*i)[0],(*i)[1],(*i)[2]));
  }
}

void StateExtractor::ExtractForces(boost::shared_ptr<OpenMM::Context> context,
	               geom::Vec3List& forces){

  forces.clear();
  OpenMM::State s = context->getState(OpenMM::State::Forces);
  std::vector<OpenMM::Vec3> f = s.getForces();
  for(std::vector<OpenMM::Vec3>::iterator i = f.begin(); i != f.end(); ++i){
    forces.push_back(geom::Vec3((*i)[0],(*i)[1],(*i)[2]));
  }
}


}}} //ns
