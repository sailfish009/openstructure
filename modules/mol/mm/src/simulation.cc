#include <ost/mol/mm/simulation.hh>

namespace ost{ namespace mol{ namespace mm{

Simulation::Simulation(const ost::mol::EntityHandle& handle, 
                       const MMSettingsPtr settings){

  //note, that ent_ will be "completed" inside this function!
  //(hydrogens and shit)

  TopologyPtr top = TopologyCreator::Create(handle,settings);
  this->Init(top);
}

Simulation::Simulation(const TopologyPtr top,
                       const MMSettingsPtr settings){

  this->Init(top);
}

void Simulation::Init(const TopologyPtr top){


  top_ = top;
  integrator_ = settings_->integrator;
  if(!integrator_){
    throw ost::Error("Settings must have a valid integrator attached to set up a simulation!");
  }
  system_ = SystemCreator::Create(top_,settings_,system_force_mapper_); 
  ost::mol::EntityHandle ent = top_->GetEntity();
  original_masses_ = top_->GetMasses();

  //setting up the context, which combines the system with an integrator
  //to proceed in time, but first we have to load the proper platform

  OpenMM::Platform::loadPluginsFromDirectory (settings_->openmm_plugin_directory);
  OpenMM::Platform* platform;

  switch(settings_->platform){
    case Reference:{
      platform = &OpenMM::Platform::getPlatformByName("Reference");
      break;
    }
    case OpenCL:{
      platform = &OpenMM::Platform::getPlatformByName("OpenCL");
      break;
    }
    case CUDA:{
      platform = &OpenMM::Platform::getPlatformByName("CUDA");
      break;
    }
    case CPU:{
      platform = &OpenMM::Platform::getPlatformByName("CPU");
      break;
    }
  }

  context_ = ContextPtr(new OpenMM::Context(*system_,*integrator_,*platform));

  ost::mol::AtomHandleList atom_list = ent.GetAtomList();
  std::vector<OpenMM::Vec3> positions;
  geom::Vec3 ost_vec;
  OpenMM::Vec3 open_mm_vec;
  for(ost::mol::AtomHandleList::iterator i = atom_list.begin();
      i!=atom_list.end();++i){
    ost_vec = i->GetPos();
    open_mm_vec[0] = ost_vec[0]/10;
    open_mm_vec[1] = ost_vec[1]/10;
    open_mm_vec[2] = ost_vec[2]/10;
    positions.push_back(open_mm_vec);
  }
  context_->setPositions(positions);

  //make sure the context satisfies the distance constraints
  context_->applyConstraints(0.00001);

  if(settings_->init_temperature > 0.0){
    context_->setVelocitiesToTemperature(settings_->init_temperature);
  }
}

geom::Vec3List Simulation::GetPositions(bool enforce_periodic_box, bool in_angstrom){
  geom::Vec3List return_vec;
  StateExtractor::ExtractPositions(context_,return_vec, enforce_periodic_box, in_angstrom);
  return return_vec;
}

geom::Vec3List Simulation::GetVelocities(){
  geom::Vec3List return_vec;
  StateExtractor::ExtractVelocities(context_, return_vec);
  return return_vec;
}

geom::Vec3List Simulation::GetForces(){
  geom::Vec3List return_vec;
  StateExtractor::ExtractForces(context_, return_vec);
  return return_vec;
}

void Simulation::SetPositions(geom::Vec3List& positions, bool in_angstrom){
  if(top_->GetNumAtoms() != positions.size()){
    throw ost::Error("Number of positions does not correspond to number of atoms in topology!");
  }
  if(in_angstrom) positions /= 10;
  std::vector<OpenMM::Vec3> openmm_positions;
  OpenMM::Vec3 open_mm_vec;
  for(geom::Vec3List::iterator i = positions.begin();
      i != positions.end(); ++i){
    open_mm_vec[0] = (*i)[0];
    open_mm_vec[1] = (*i)[1];
    open_mm_vec[2] = (*i)[2];
    openmm_positions.push_back(open_mm_vec);
  }
  context_->setPositions(openmm_positions);
}

void Simulation::SetVelocities(geom::Vec3List& velocities){
  if(top_->GetNumAtoms() != velocities.size()){
    throw ost::Error("Number of velocities does not correspond to number of atoms in topology!");
  }
  std::vector<OpenMM::Vec3> openmm_velocities;
  OpenMM::Vec3 open_mm_vec;
  for(geom::Vec3List::iterator i = velocities.begin();
      i != velocities.end(); ++i){
    open_mm_vec[0] = (*i)[0];
    open_mm_vec[1] = (*i)[1];
    open_mm_vec[2] = (*i)[2];
    openmm_velocities.push_back(open_mm_vec);
  }
  context_->setVelocities(openmm_velocities);
}

void Simulation::UpdateTopologyPositions(bool enforce_periodic_box){
  geom::Vec3List positions = this->GetPositions(enforce_periodic_box);
  top_->SetEntityPositions(positions);
}

void Simulation::MinimizeEnergy(const String& type, Real tolerance, int max_iterations){
  if(type == "lbfgs") OpenMM::LocalEnergyMinimizer::minimize(*context_, tolerance, max_iterations);
  else if(type == "steep") Steep::Minimize(*context_,tolerance, max_iterations);
  else{
    std::stringstream ss;
    ss << "Unknown minimization function type \""<< type <<"\" observed";
    throw ost::Error(ss.str());
  }
}

void Simulation::Steps(int steps){

  int time_to_next_notification;

  while(steps > 0){
    time_to_next_notification = this->TimeToNextNotification();
    //we can just do the remaining steps and subtract them from
    //time_to_notify_
    if(steps < time_to_next_notification){
      integrator_->step(steps);
      for(std::vector<int>::iterator i = time_to_notify_.begin();
          i != time_to_notify_.end(); ++i){
        (*i) -= steps;
      } 
      steps = 0;
    }else{
      integrator_->step(time_to_next_notification);
      for(std::vector<int>::iterator i = time_to_notify_.begin();
          i != time_to_notify_.end(); ++i){
        (*i) -= time_to_next_notification;
      } 
      for(uint i = 0; i < time_to_notify_.size(); ++i){
        if(time_to_notify_[i] == 0){
          observers_[i]->Notify();
          time_to_notify_[i] = observers_[i]->Rythm();
        }
      }
      steps -= time_to_next_notification;
    }
  }
}

Real Simulation::GetEnergy(){
  Real energy;
  StateExtractor::ExtractEnergy(context_,energy);
  return energy; 
}

Real Simulation::GetKineticEnergy(){
  Real energy;
  StateExtractor::ExtractKineticEnergy(context_,energy);
  return energy;
}

Real Simulation::GetPotentialEnergy(){
  Real energy;
  StateExtractor::ExtractPotentialEnergy(context_,energy);
  return energy;
}

void Simulation::Register(MMObserverPtr o){
  observers_.push_back(o);
  time_to_notify_.push_back(o->Rythm());
  o->Init(context_,top_);
}

int Simulation::TimeToNextNotification(){
  if(observers_.empty()){
    return std::numeric_limits<int>::max();
  }
  int minimal_time = std::numeric_limits<int>::max();
  for(std::vector<int>::iterator i = time_to_notify_.begin();
      i != time_to_notify_.end(); ++i){
    if(*i < minimal_time){
      minimal_time = *i;
    }
  }
  return minimal_time;
}

void Simulation::ResetHarmonicBond(uint index, Real bond_length, Real force_constant){
  if(system_force_mapper_.find(HarmonicBond) == system_force_mapper_.end()){
    throw ost::Error("There is no harmonic bond force in the simulation!");
  }
  uint force_index = system_force_mapper_[HarmonicBond];
  OpenMM::Force& force = system_->getForce(force_index);
  OpenMM::HarmonicBondForce* bond_force_ptr = reinterpret_cast<OpenMM::HarmonicBondForce*>(&force); 
  if(index >= uint(bond_force_ptr->getNumBonds())){
    throw ost::Error("Provided bond index exceeds number of bonds!");
  }
  double dummy_one, dummy_two;
  int particle1, particle2;
  //we know the parameters, but not the atom indices in the force object, we have to extract it from
  //the force itself
  bond_force_ptr->getBondParameters(index,particle1,particle2,dummy_one,dummy_two);
  bond_force_ptr->setBondParameters(index,particle1,particle2,bond_length,force_constant);
  bond_force_ptr->updateParametersInContext(*context_);
  top_->SetHarmonicBondParameters(index,bond_length,force_constant);
}

void Simulation::ResetHarmonicAngle(uint index, Real angle, Real force_constant){
  if(system_force_mapper_.find(HarmonicAngle) == system_force_mapper_.end()){
    throw ost::Error("There is no harmonic angle force in the simulation!");
  }
  uint force_index = system_force_mapper_[HarmonicAngle];
  OpenMM::Force& force = system_->getForce(force_index);
  OpenMM::HarmonicAngleForce* angle_force_ptr = reinterpret_cast<OpenMM::HarmonicAngleForce*>(&force); 
  if(index >= uint(angle_force_ptr->getNumAngles())){
    throw ost::Error("Provided angle index exceeds number of angles!");
  }
  double dummy_one, dummy_two;
  int particle1, particle2, particle3;
  //we know the parameters, but not the atom indices in the force object, we have to extract it from
  //the force itself
  angle_force_ptr->getAngleParameters(index,particle1,particle2,particle3,dummy_one,dummy_two);
  angle_force_ptr->setAngleParameters(index,particle1,particle2,particle3,angle,force_constant);
  angle_force_ptr->updateParametersInContext(*context_);
  top_->SetHarmonicAngleParameters(index, angle, force_constant);
}

void Simulation::ResetUreyBradleyAngle(uint index, Real angle, Real angle_force_constant, Real bond_length, Real bond_force_constant){
  if(system_force_mapper_.find(UreyBradleyAngle) == system_force_mapper_.end()){
    throw ost::Error("There is no urey bradley angle force in the simulation!");
  }
  uint force_index = system_force_mapper_[UreyBradleyAngle];
  OpenMM::Force& angle_force = system_->getForce(force_index);
  OpenMM::Force& bond_force = system_->getForce(force_index+1);

  OpenMM::HarmonicAngleForce* angle_force_ptr = reinterpret_cast<OpenMM::HarmonicAngleForce*>(&angle_force); 
  OpenMM::HarmonicBondForce* bond_force_ptr = reinterpret_cast<OpenMM::HarmonicBondForce*>(&bond_force); 
  if(index >= uint(angle_force_ptr->getNumAngles())){
    throw ost::Error("Provided angle index exceeds number of angles!");
  }
  double dummy_one, dummy_two;
  int particle1, particle2, particle3;
  //we know the parameters, but not the atom indices in the force object, we have to extract it from
  //the force itself
  angle_force_ptr->getAngleParameters(index,particle1,particle2,particle3,dummy_one,dummy_two);
  angle_force_ptr->setAngleParameters(index,particle1,particle2,particle3,angle,angle_force_constant);
  bond_force_ptr->setBondParameters(index,particle1,particle3,bond_length,bond_force_constant);
  angle_force_ptr->updateParametersInContext(*context_);
  bond_force_ptr->updateParametersInContext(*context_);
  top_->SetUreyBradleyAngleParameters(index,angle,angle_force_constant,bond_length,bond_force_constant);
}

void Simulation::ResetPeriodicDihedral(uint index, int multiplicity, Real phase, Real force_constant){
  if(system_force_mapper_.find(PeriodicDihedral) == system_force_mapper_.end()){
    throw ost::Error("There is no periodic dihedral force in the simulation!");
  }
  uint force_index = system_force_mapper_[PeriodicDihedral];
  OpenMM::Force& force = system_->getForce(force_index);
  OpenMM::PeriodicTorsionForce* dihedral_force_ptr = reinterpret_cast<OpenMM::PeriodicTorsionForce*>(&force); 
  if(index >= uint(dihedral_force_ptr->getNumTorsions())){
    throw ost::Error("Provided dihedral index exceeds number of dihedrals!");
  }
  int dummy_one;
  double dummy_two, dummy_three;
  int particle1, particle2, particle3, particle4;
  //we know the parameters, but not the atom indices in the force object, we have to extract it from
  //the force itself
  dihedral_force_ptr->getTorsionParameters(index,particle1,particle2,particle3,particle4,dummy_one,dummy_two,dummy_three);
  dihedral_force_ptr->setTorsionParameters(index,particle1,particle2,particle3,particle4,multiplicity,phase,force_constant);
  dihedral_force_ptr->updateParametersInContext(*context_);
  top_->SetPeriodicDihedralParameters(index, multiplicity, phase, force_constant);
}

void Simulation::ResetPeriodicImproper(uint index, int multiplicity, Real phase, Real force_constant){
  if(system_force_mapper_.find(PeriodicImproper) == system_force_mapper_.end()){
    throw ost::Error("There is no periodic improper force in the simulation!");
  }
  uint force_index = system_force_mapper_[PeriodicImproper];
  OpenMM::Force& force = system_->getForce(force_index);
  OpenMM::PeriodicTorsionForce* improper_ptr = reinterpret_cast<OpenMM::PeriodicTorsionForce*>(&force); 
  if(index >= uint(improper_ptr->getNumTorsions())){
    throw ost::Error("Provided improper index exceeds number of impropers!");
  }
  int dummy_one;
  double dummy_two, dummy_three;
  int particle1, particle2, particle3, particle4;
  //we know the parameters, but not the atom indices in the force object, we have to extract it from
  //the force itself
  improper_ptr->getTorsionParameters(index,particle1,particle2,particle3,particle4,dummy_one,dummy_two,dummy_three);
  improper_ptr->setTorsionParameters(index,particle1,particle2,particle3,particle4,multiplicity,phase,force_constant);
  improper_ptr->updateParametersInContext(*context_);
  top_->SetPeriodicImproperParameters(index, multiplicity, phase, force_constant);
}

void Simulation::ResetHarmonicImproper(uint index, Real phase, Real force_constant){
  if(system_force_mapper_.find(HarmonicImproper) == system_force_mapper_.end()){
    throw ost::Error("There is no harmonic improper force in the simulation!");
  }
  uint force_index = system_force_mapper_[HarmonicImproper];
  OpenMM::Force& force = system_->getForce(force_index);
  OpenMM::CustomTorsionForce* improper_ptr = reinterpret_cast<OpenMM::CustomTorsionForce*>(&force); 
  if(index >= uint(improper_ptr->getNumTorsions())){
    throw ost::Error("Provided improper index exceeds number of impropers!");
  }
  std::vector<double> parameters;
  parameters.push_back(0.0);
  parameters.push_back(0.0);
  int particle1, particle2, particle3, particle4;
  //we know the parameters, but not the atom indices in the force object, we have to extract it from
  //the force itself
  improper_ptr->getTorsionParameters(index,particle1,particle2,particle3,particle4,parameters);
  parameters[0] = phase;
  parameters[1] = force_constant;
  improper_ptr->setTorsionParameters(index,particle1,particle2,particle3,particle4,parameters);
  improper_ptr->updateParametersInContext(*context_);
  top_->SetHarmonicImproperParameters(index, phase, force_constant);
}

void Simulation::ResetLJPair(uint index, Real sigma, Real epsilon){
  if(system_force_mapper_.find(LJ) == system_force_mapper_.end()){
    throw ost::Error("There is no nonbonded force in the simulation!");
  }
  uint force_index = system_force_mapper_[LJ];
  OpenMM::Force& force = system_->getForce(force_index);
  OpenMM::NonbondedForce* nonbonded_ptr = reinterpret_cast<OpenMM::NonbondedForce*>(&force); 
  if(index >= uint(nonbonded_ptr->getNumExceptions())){
    throw ost::Error("Provided index exceeds number of defined pairs!");
  }
  double charge_prod, dummy_one, dummy_two;
  int particle1, particle2;
  //we know the parameters, but not the atom indices in the force object, we have to extract it from
  //the force itself
  nonbonded_ptr->getExceptionParameters(index,particle1,particle2,charge_prod,dummy_one,dummy_two);
  nonbonded_ptr->setExceptionParameters(index,particle1,particle2,charge_prod,sigma,epsilon);
  nonbonded_ptr->updateParametersInContext(*context_);
  top_->SetLJPairParameters(index, sigma, epsilon);
}

void Simulation::ResetDistanceConstraint(uint index, Real constraint_length){
  if(index >= uint(system_->getNumConstraints())){
    throw ost::Error("Provided index exceeds number of defined constraints!");
  }
  double dummy;
  int particle1, particle2;
  system_->getConstraintParameters(index,particle1,particle2,dummy);
  system_->setConstraintParameters(index,particle1,particle2,constraint_length);
  context_->reinitialize();
  top_->SetDistanceConstraintParameters(index, constraint_length);
}

void Simulation::AddPositionConstraint(uint index){
  if(index >= top_->GetNumAtoms()){
    throw ost::Error("Provided index exceeds number of atoms!");
  }
  system_->setParticleMass(index,0.0);
  context_->reinitialize();
  top_->AddPositionConstraint(index);
}

void Simulation::AddPositionConstraints(const std::vector<uint>& index){

  for(std::vector<uint>::const_iterator i = index.begin(); 
      i != index.end(); ++i){
    if(*i >= top_->GetNumAtoms()){
      throw ost::Error("Provided index exceeds number of atoms!");
    }
    system_->setParticleMass(*i,0.0);
    top_->AddPositionConstraint(*i);
  }
  context_->reinitialize();
}

void Simulation::ResetPositionConstraints(){
  for(uint i = 0; i < original_masses_.size(); ++i){
    system_->setParticleMass(i,original_masses_[i]);
  }
  top_->ResetPositionConstraints();
  context_->reinitialize();
}

void Simulation::ResetHarmonicPositionRestraint(uint index, const geom::Vec3& ref_position, Real k,
                                                Real x_scale, Real y_scale, Real z_scale){
  if(index >= uint(top_->GetNumHarmonicPositionRestraints())){
    throw ost::Error("Provided index exceeds number of defined harmonic position restraints!");
  }
  if(system_force_mapper_.find(HarmonicPositionRestraint) == system_force_mapper_.end()){
    throw ost::Error("There is no harmonic position restraint in the simulation!");
  }
  uint force_index = system_force_mapper_[HarmonicPositionRestraint];
  OpenMM::Force& force = system_->getForce(force_index);
  OpenMM::CustomExternalForce* restraint_ptr = reinterpret_cast<OpenMM::CustomExternalForce*>(&force); 
  if(index >= uint(restraint_ptr->getNumParticles())){
    throw ost::Error("Provided index exceeds number of defined harmonic position restraints!");
  }
  int particle;
  std::vector<double> parameters;
  restraint_ptr->getParticleParameters(index,particle,parameters);
  parameters[0] = ref_position[0];
  parameters[1] = ref_position[1];
  parameters[2] = ref_position[2];
  parameters[3] = k;
  parameters[4] = x_scale;
  parameters[5] = y_scale;
  parameters[6] = z_scale;
  restraint_ptr->setParticleParameters(index,particle,parameters);
  restraint_ptr->updateParametersInContext(*context_);
  top_->SetHarmonicPositionRestraintParameters(index,ref_position,k,x_scale,y_scale,z_scale);
}

void Simulation::ResetHarmonicDistanceRestraint(uint index, Real length, Real force_constant){
  if(index >= top_->GetNumHarmonicDistanceRestraints()){
    throw ost::Error("Provided index exceeds number of defined harmonic distance restraints!");
  }
  if(system_force_mapper_.find(HarmonicDistanceRestraint) == system_force_mapper_.end()){
    throw ost::Error("There is no harmonic distance restraint in the simulation!");
  }
  uint force_index = system_force_mapper_[HarmonicDistanceRestraint];
  OpenMM::Force& force = system_->getForce(force_index);
  OpenMM::HarmonicBondForce* restraint_ptr = reinterpret_cast<OpenMM::HarmonicBondForce*>(&force); 
  if(index >= uint(restraint_ptr->getNumBonds())){
    throw ost::Error("Provided index exceeds number of defined harmonic distance restraints!");
  }
  int particle_one,particle_two;
  double dummy_one, dummy_two;
  restraint_ptr->getBondParameters(int(index),particle_one,particle_two,dummy_one,dummy_two);
  restraint_ptr->setBondParameters(int(index),particle_one,particle_two,length,2*force_constant);
  restraint_ptr->updateParametersInContext(*context_);
  top_->SetHarmonicDistanceRestraintParameters(index,length,force_constant);
}

void Simulation::ResetLJ(uint index, Real sigma, Real epsilon){
  if(index >= top_->GetNumAtoms()){
    throw ost::Error("Provided index exceeds number of atoms!");
  }
  if(system_force_mapper_.find(LJ) == system_force_mapper_.end()){
    throw ost::Error("There is no nonbonded force in the simulation!");
  }
  uint force_index = system_force_mapper_[LJ];
  OpenMM::Force& force = system_->getForce(force_index);
  OpenMM::NonbondedForce* nonbonded_ptr = reinterpret_cast<OpenMM::NonbondedForce*>(&force); 

  double charge, dummy_one, dummy_two;

  nonbonded_ptr->getParticleParameters(index,charge,dummy_one,dummy_two);
  nonbonded_ptr->setParticleParameters(index,charge,sigma,epsilon);
  nonbonded_ptr->updateParametersInContext(*context_);
  top_->SetSigma(index, sigma);
  top_->SetEpsilon(index, epsilon);
}

void Simulation::ResetGBSA(uint index, Real radius, Real scaling){
  if(index >= top_->GetNumAtoms()){
    throw ost::Error("Provided index exceeds number of atoms!");
  }
  if(system_force_mapper_.find(GBSA) == system_force_mapper_.end()){
    throw ost::Error("There is no gbsa force in the simulation!");
  }
  uint force_index = system_force_mapper_[GBSA];
  OpenMM::Force& force = system_->getForce(force_index);
  OpenMM::GBSAOBCForce* gbsa_ptr = reinterpret_cast<OpenMM::GBSAOBCForce*>(&force); 

  double charge, dummy_one, dummy_two;

  gbsa_ptr->getParticleParameters(index,charge,dummy_one,dummy_two);
  gbsa_ptr->setParticleParameters(index,charge,radius,scaling);
  gbsa_ptr->updateParametersInContext(*context_);
  top_->SetGBSARadius(index, radius);
  top_->SetOBCScaling(index, scaling);
}

void Simulation::ResetCharge(uint index, Real charge){
  if(index >= top_->GetNumAtoms()){
    throw ost::Error("Provided index exceeds number of atoms!");
  }
  if(system_force_mapper_.find(LJ) == system_force_mapper_.end()){
    throw ost::Error("There is no nonbonded force in the simulation!");
  }
  uint force_index = system_force_mapper_[LJ];
  OpenMM::Force& force = system_->getForce(force_index);
  OpenMM::NonbondedForce* nonbonded_ptr = reinterpret_cast<OpenMM::NonbondedForce*>(&force); 

  double dummy, sigma, epsilon;

  //first update the parameters of the single particle
  nonbonded_ptr->getParticleParameters(index,dummy,sigma,epsilon);
  nonbonded_ptr->setParticleParameters(index,charge,sigma,epsilon);
  
  //the charge product has to be updated in the exceptions (1,4 interactions)
   std::vector<uint> lj_pair_indices = top_->GetLJPairIndices(index);
   Real old_charge = top_->GetCharge(index);
   double charge_prod;
   Real new_charge_prod;
   int particle_one, particle_two;

  for(std::vector<uint>::iterator i = lj_pair_indices.begin();
      i != lj_pair_indices.end(); ++i){
    nonbonded_ptr->getExceptionParameters(*i,particle_one,particle_two,charge_prod,sigma,epsilon);
    new_charge_prod = charge_prod/old_charge*charge; // fudge_qq * q1 * q2
    nonbonded_ptr->setExceptionParameters(*i,particle_one,particle_two,new_charge_prod,sigma,epsilon);
  } 

  nonbonded_ptr->updateParametersInContext(*context_);
  top_->SetCharge(index, charge);

  //the charge also affects gbsa force...
  if(system_force_mapper_.find(GBSA) != system_force_mapper_.end()){
    force_index = system_force_mapper_[GBSA];
    OpenMM::Force& force = system_->getForce(force_index);
    OpenMM::GBSAOBCForce* gbsa_ptr = reinterpret_cast<OpenMM::GBSAOBCForce*>(&force); 

    double radius,scaling;

    gbsa_ptr->getParticleParameters(index,dummy,radius,scaling);
    gbsa_ptr->setParticleParameters(index,charge,radius,scaling);
    gbsa_ptr->updateParametersInContext(*context_); 
  }
}

void Simulation::ResetMass(uint index, Real mass){
  if(index >= top_->GetNumAtoms()){
    throw ost::Error("Provided index exceeds number of atoms!");
  }
  system_->setParticleMass(index,mass);
  context_->reinitialize();
  top_->SetMass(index,mass);
}

geom::Vec3 Simulation::GetPeriodicBoxExtents(){
  geom::Vec3 vec;
  OpenMM::Vec3 ucell_a, ucell_b, ucell_c;
  OpenMM::State openmm_state = context_->getState(0); //create minimal state
                                                      //to only extract periodic
                                                      //box information
  openmm_state.getPeriodicBoxVectors(ucell_a,ucell_b,ucell_c);
  vec[0] = 10 * sqrt(ucell_a.dot(ucell_a));
  vec[1] = 10 * sqrt(ucell_a.dot(ucell_b));
  vec[2] = 10 * sqrt(ucell_a.dot(ucell_c));
  return vec;
}

void Simulation::SetPeriodicBoxExtents(geom::Vec3& vec){
  OpenMM::Vec3 ucell_a(vec[0]/10.0,0.0,0.0);
  OpenMM::Vec3 ucell_b(0.0,vec[1]/10.0,0.0);
  OpenMM::Vec3 ucell_c(0.0,0.0,vec[2]/10.0);
  context_->setPeriodicBoxVectors(ucell_a,ucell_b,ucell_c);
}



}}}

