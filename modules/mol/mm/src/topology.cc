#include <ost/mol/mm/topology.hh>
#include <ost/mol/mm/mm_modeller.hh>


namespace ost{ namespace mol{ namespace mm{



Topology::Topology(const ost::mol::EntityHandle& ent, const std::vector<Real>& masses){

  num_atoms_ = ent.GetAtomCount();
  if(num_atoms_ != masses.size()){
    throw ost::Error("Number of atoms in entity and number of masses do not match when creating topology!");
  }
  ent_ = ent.Copy();
  num_residues_ = ent_.GetResidueCount();
  atom_list_ = ent_.GetAtomList();
  res_list_ = ent_.GetResidueList();
  atom_masses_ = masses;
  fudge_lj_ = 1.0;
  fudge_qq_ = 1.0;

  for(uint i = 0; i < atom_list_.size(); ++i){
    atom_index_mapper_[atom_list_[i].GetHashCode()] = i;
  }  

  for(uint i = 0; i < res_list_.size(); ++i){
    residue_index_mapper_[res_list_[i].GetHashCode()] = i;
  }

  std::map<String,uint> per_residue_mapper;
  ost::mol::AtomHandleList residue_atom_list;
  for(uint i = 0; i < num_residues_; ++i){
    per_residue_mapper.clear();
    residue_atom_list = res_list_[i].GetAtomList();
    for(ost::mol::AtomHandleList::iterator j = residue_atom_list.begin();
        j != residue_atom_list.end(); ++j){
      per_residue_mapper[j->GetName()] = atom_index_mapper_[j->GetHashCode()];
    }
    atom_name_mapper_.push_back(per_residue_mapper);
  }
}

uint Topology::AddHarmonicBond(uint index_one,
                               uint index_two, 
                               Real bond_length,
                               Real force_constant){

  if(index_one >= num_atoms_ || index_two >= num_atoms_){
    throw ost::Error("Index of bond atom exceeds number of atoms present in topology!");
  }
  Index<2> index(index_one,index_two);
  std::vector<Real> parameters;
  parameters.push_back(bond_length);
  parameters.push_back(force_constant);
  harmonic_bonds_.push_back(std::make_pair(index,parameters));
  return harmonic_bonds_.size()-1;
}

uint Topology::AddHarmonicAngle(uint index_one,
                                uint index_two,
                                uint index_three,
                                Real angle,
                                Real force_constant){

  if(uint(index_one) >= num_atoms_ || uint(index_two) >= num_atoms_ || 
     uint(index_three) >= num_atoms_){
    throw ost::Error("Index of angle atom exceeds number of atoms present in topology!");
  }
  Index<3> index(index_one,index_two,index_three); 
  std::vector<Real> parameters;
  parameters.push_back(angle);
  parameters.push_back(force_constant);
  harmonic_angles_.push_back(std::make_pair(index,parameters));
  return harmonic_angles_.size()-1;
}

uint Topology::AddUreyBradleyAngle(uint index_one,
                                   uint index_two,
                                   uint index_three,
                                   Real angle,
                                   Real angle_force_constant,
                                   Real bond_length,
                                   Real bond_force_constant){

  if(index_one >= num_atoms_ || index_two >= num_atoms_ || 
     index_three >= num_atoms_){
    throw ost::Error("Index of angle atom exceeds number of atoms present in topology!");
  }
  Index<3> index(index_one,index_two,index_three); 
  std::vector<Real> parameters;
  parameters.push_back(angle);
  parameters.push_back(angle_force_constant);
  parameters.push_back(bond_length);
  parameters.push_back(bond_force_constant);
  urey_bradley_angles_.push_back(std::make_pair(index,parameters));
  return urey_bradley_angles_.size()-1;
}

uint Topology::AddPeriodicDihedral(uint index_one,
                                   uint index_two,
                                   uint index_three,
                                   uint index_four,
                                   int multiplicity,
                                   Real phase,
                                   Real force_constant){

  if(index_one >= num_atoms_ || index_two >= num_atoms_||
     index_three >= num_atoms_ || index_four >= num_atoms_){
    throw ost::Error("Index of dihedral atom exceeds number of atoms present in topology!");
  }
  Index<4> index(index_one,index_two,index_three,index_four);
  std::vector<Real> parameters;
  parameters.push_back(multiplicity);
  parameters.push_back(phase);
  parameters.push_back(force_constant);
  periodic_dihedrals_.push_back(std::make_pair(index,parameters));
  return periodic_dihedrals_.size()-1;
}

uint Topology::AddPeriodicImproper(uint index_one,
                                   uint index_two,
                                   uint index_three,
                                   uint index_four,
                                   int multiplicity,
                                   Real phase,
                                   Real force_constant){

  if(index_one >= num_atoms_ || index_two >= num_atoms_||
     index_three >= num_atoms_ || index_four >= num_atoms_){
    throw ost::Error("Index of improper atom exceeds number of atoms present in topology!");
  }
  Index<4> index(index_one,index_two,index_three,index_four);
  std::vector<Real> parameters;
  parameters.push_back(multiplicity);
  parameters.push_back(phase);
  parameters.push_back(force_constant);
  periodic_impropers_.push_back(std::make_pair(index,parameters));
  return periodic_impropers_.size()-1;
}

uint Topology::AddHarmonicImproper(uint index_one,
                                   uint index_two,
                                   uint index_three,
                                   uint index_four,
                                   Real angle,
                                   Real force_constant){

  if(index_one >= num_atoms_ || index_two >= num_atoms_||
     index_three >= num_atoms_ || index_four >= num_atoms_){
    throw ost::Error("Index of improper atom exceeds number of atoms present in topology!");
  }
  Index<4> index(index_one,index_two,index_three,index_four);
  std::vector<Real> parameters;
  parameters.push_back(angle);
  parameters.push_back(force_constant);
  harmonic_impropers_.push_back(std::make_pair(index,parameters));
  return harmonic_impropers_.size()-1;
}

uint Topology::AddCMap(uint index_one,
                       uint index_two,
                       uint index_three,
                       uint index_four,
                       uint index_five,
                       int dimension,
                       std::vector<Real> values){

  if(index_one >= num_atoms_ || index_two >= num_atoms_ ||
     index_three >= num_atoms_ || index_four >= num_atoms_ ||
     index_five >= num_atoms_){
    throw ost::Error("Index of cmap atom exceeds number of atoms present in topology!");
  }
  if(uint(dimension * dimension) != values.size()){
    throw ost::Error("Dimension of CMap is inconsistent with its values!");
  }
  Index<5> index(index_one,index_two,index_three,index_four,index_five);
  cmaps_.push_back(std::make_pair(index,values));
  return cmaps_.size()-1;
}

uint Topology::AddLJPair(uint index_one, 
                         uint index_two,
                         Real sigma,
                         Real epsilon){

  if(index_one >= num_atoms_ || index_two >= num_atoms_){
    throw ost::Error("Index of lj pair atom exceeds number of atoms present in topology!");
  }
  Index<2> index(index_one,index_two);
  Index<2> reverse_index(index_two,index_one);
  if(added_lj_pairs_.find(index) != added_lj_pairs_.end() || added_lj_pairs_.find(reverse_index) != added_lj_pairs_.end()){
    throw ost::Error("This particular lj 1,4-interaction is already parametrized!");
  }
  added_lj_pairs_.insert(index);
  std::vector<Real> parameters;
  parameters.push_back(sigma);
  parameters.push_back(epsilon);
  lj_pairs_.push_back(std::make_pair(index,parameters));
  return lj_pairs_.size()-1;
}

uint Topology::AddExclusion(uint index_one,
                            uint index_two){
  Index<2> index(index_one,index_two);
  Index<2> reverse_index(index_two,index_one);
  if(index_one >= num_atoms_ || index_two >= num_atoms_){
    throw ost::Error("Index of exclusion atom exceeds number of atoms present in topology!");
  }
  if(added_exclusions_.find(index) != added_exclusions_.end() || added_exclusions_.find(reverse_index) != added_exclusions_.end()){
    throw ost::Error("This particular exclusion has already been set!");
  }
  added_exclusions_.insert(index);
  exclusions_.push_back(index);
  return exclusions_.size()-1;
}

uint Topology::AddDistanceConstraint(uint index_one, 
                                     uint index_two,
                                     Real distance){

  if(index_one >= num_atoms_ || index_two >= num_atoms_){
    throw ost::Error("Index of distance constraint atom exceeds number of atoms present in topology!");
  }
  Index<2> index(index_one,index_two);
  Index<2> reverse_index(index_two,index_one);
  if(added_distance_constraints_.find(index) != added_distance_constraints_.end() || added_distance_constraints_.find(reverse_index) != added_distance_constraints_.end()){
    throw ost::Error("This particular distance constraint is already parametrized!");
  }
  added_distance_constraints_.insert(index);
  std::vector<Real> parameters;
  parameters.push_back(distance);
  distance_constraints_.push_back(std::make_pair(index,parameters));
  return distance_constraints_.size()-1;
}

void Topology::AddPositionConstraint(uint index){
  if(index >= num_atoms_){
    throw ost::Error("Index of position constraint exceeds number of atoms present in topology!");
  }
  std::vector<uint>::iterator it = std::find(position_constraints_.begin(),position_constraints_.end(),index);
  if(it != position_constraints_.end()) return; //already present
  position_constraints_.push_back(index);
}

uint Topology::AddHarmonicPositionRestraint(uint ind, const geom::Vec3& ref_position, Real k, 
                                            Real x_scale, Real y_scale, Real z_scale){
  if(ind >= num_atoms_){
    throw ost::Error("Index of harmonic distance constraint exceeds number of atoms present in topology!");
  }
  Index<1> index(ind);
  std::vector<Real> parameters;
  parameters.push_back(ref_position[0]);
  parameters.push_back(ref_position[1]);
  parameters.push_back(ref_position[2]);
  parameters.push_back(k);
  parameters.push_back(x_scale);
  parameters.push_back(y_scale);
  parameters.push_back(z_scale);

  harmonic_position_restraints_.push_back(std::make_pair(index,parameters));
  return harmonic_position_restraints_.size()-1;
}

uint Topology::AddHarmonicDistanceRestraint(uint index_one, uint index_two,
                                            Real length, Real force_constant){
  if(index_one >= num_atoms_ || index_two >= num_atoms_){
    throw ost::Error("Index of distance restraint atom exceeds number of atoms present in topology!");
  }
  Index<2> index(index_one,index_two);
  std::vector<Real> parameters;
  parameters.push_back(length);
  parameters.push_back(force_constant);
  harmonic_distance_restraints_.push_back(std::make_pair(index,parameters));
  return harmonic_distance_restraints_.size()-1;
}

void Topology::SetSigmas(const std::vector<Real>& sigmas){
  if(sigmas.size() != num_atoms_){
    throw ost::Error("Expect the same number of sigma parameters than atoms!");
  }
  sigmas_ = sigmas;
}

void Topology::SetSigma(uint index, Real sigma){
  if(sigmas_.empty()){
    throw ost::Error("Modification of sigma parameter requires initial assignment of sigma parameters in globo!");
  }
  if(index >= num_atoms_){
    throw ost::Error("Given atom index exceeds number off available atoms!");
  }
  sigmas_[index] = sigma;
}

void Topology::SetEpsilons(const std::vector<Real>& epsilons){
  if(epsilons.size() != num_atoms_){
    throw ost::Error("Expect the same number of epsilon parameters than atoms!");
  }
  epsilons_ = epsilons;
}

void Topology::SetEpsilon(uint index, Real epsilon){
  if(epsilons_.empty()){
    throw ost::Error("Modification of epsilon parameter requires initial assignment of epsilon parameters in globo!");
  }
  if(index >= num_atoms_){
    throw ost::Error("Given atom index exceeds number off available atoms!");
  }
  epsilons_[index] = epsilon;
}

void Topology::SetGBSARadii(const std::vector<Real>& gbsa_radii){
  if(gbsa_radii.size() != num_atoms_){
    throw ost::Error("Expect the same number of gbsa parameters than atoms!");
  }
  gbsa_radii_ = gbsa_radii;
}

void Topology::SetGBSARadius(uint index, Real radius){
  if(gbsa_radii_.empty()){
    throw ost::Error("Modification of gbsa radius requires initial assignment of gbsa radii in globo!");
  }
  if(index >= num_atoms_){
    throw ost::Error("Given atom index exceeds number off available atoms!");
  }
  gbsa_radii_[index] = radius;
}

void Topology::SetOBCScalings(const std::vector<Real>& obc_scaling){
  if(obc_scaling.size() != num_atoms_){
    throw ost::Error("Expect the same number of gbsa parameters than atoms!");
  }
  obc_scaling_ = obc_scaling;
}

void Topology::SetOBCScaling(uint index, Real scaling){
  if(obc_scaling_.empty()){
    throw ost::Error("Modification of obc scaling parameter requires initial assignment of obc scaling parameters in globo!");
  }
  if(index >= num_atoms_){
    throw ost::Error("Given atom index exceeds number off available atoms!");
  }
  obc_scaling_[index] = scaling;
}

void Topology::SetCharges(const std::vector<Real>& charges){
  if(charges.size() != num_atoms_){
    throw ost::Error("Expect the same number of charges than atoms!");
  }
  charges_ = charges;
}

void Topology::SetCharge(uint index, Real charge){
  if(charges_.empty()){
    throw ost::Error("Modification of charge parameter requires initial assignment of charges in globo!");
  }
  if(index >= num_atoms_){
    throw ost::Error("Given atom index exceeds number off available atoms!");
  }
  charges_[index] = charge;  
}

void Topology::SetMasses(const std::vector<Real>& masses){
  if(masses.size() != num_atoms_){
    throw ost::Error("Expect masses vector to have the same number of elements than atoms in the topologies entity!");
  }
  atom_masses_ = masses;
}

void Topology::SetMass(uint index, Real mass){
  if(atom_masses_.empty()){
    throw ost::Error("Modification of mass parameter requires initial assignment of masses in globo!");
  }
  if(index >= num_atoms_){
    throw ost::Error("Given atom index exceeds number off available atoms!");
  }
  atom_masses_[index] = mass;

}


void Topology::GetHarmonicBondParameters(uint index, uint& index_one, uint& index_two,
                                         Real& bond_length, Real& force_constant) const{
  if(index >= harmonic_bonds_.size()){
    throw ost::Error("Provided index exceeds number of harmonic bonds present in topology!");
  }
  bond_length = harmonic_bonds_[index].second[0];
  force_constant = harmonic_bonds_[index].second[1];
  index_one = harmonic_bonds_[index].first[0];
  index_two = harmonic_bonds_[index].first[1];
}

void Topology::GetHarmonicAngleParameters(uint index, uint& index_one, uint& index_two, uint& index_three,
                                          Real& angle, Real& force_constant) const{
  if(index >= harmonic_angles_.size()){
    throw ost::Error("Provided index exceeds number of harmonic angles present in topology!");
  }
  angle = harmonic_angles_[index].second[0];
  force_constant = harmonic_angles_[index].second[1];
  index_one = harmonic_angles_[index].first[0];
  index_two = harmonic_angles_[index].first[1];
  index_three = harmonic_angles_[index].first[2];
}

void Topology::GetUreyBradleyAngleParameters(uint index, uint& index_one, uint& index_two, uint& index_three,
                                             Real& angle, Real& angle_force_constant, Real& bond_length, Real& bond_force_constant) const{
  if(index >= urey_bradley_angles_.size()){
    throw ost::Error("Provided index exceeds number of urey_bradley angles present in topology!");
  }
  angle = urey_bradley_angles_[index].second[0];
  angle_force_constant = urey_bradley_angles_[index].second[1];
  bond_length = urey_bradley_angles_[index].second[2];
  bond_force_constant = urey_bradley_angles_[index].second[3];
  index_one = urey_bradley_angles_[index].first[0];
  index_two = urey_bradley_angles_[index].first[1];
  index_three = urey_bradley_angles_[index].first[2];
}

void Topology::GetPeriodicDihedralParameters(uint index, uint& index_one, uint& index_two, uint& index_three, uint& index_four,
                                             int& multiplicity, Real& phase, Real& force_constant) const{
  if(index >= periodic_dihedrals_.size()){
    throw ost::Error("Provided index exceeds number of periodic dihedrals present in topology!");
  }
  multiplicity = int(periodic_dihedrals_[index].second[0]);
  phase = periodic_dihedrals_[index].second[1];
  force_constant = periodic_dihedrals_[index].second[2];
  index_one = periodic_dihedrals_[index].first[0];
  index_two = periodic_dihedrals_[index].first[1];
  index_three = periodic_dihedrals_[index].first[2];
  index_four = periodic_dihedrals_[index].first[3];
}

void Topology::GetPeriodicImproperParameters(uint index, uint& index_one, uint& index_two, uint& index_three, uint& index_four,
                                             int& multiplicity, Real& phase, Real& force_constant) const{
  if(index >= periodic_impropers_.size()){
    throw ost::Error("Provided index exceeds number of periodic impropers present in topology!");
  }
  multiplicity = int(periodic_impropers_[index].second[0]);
  phase = periodic_impropers_[index].second[1];
  force_constant = periodic_impropers_[index].second[2];
  index_one = periodic_impropers_[index].first[0];
  index_two = periodic_impropers_[index].first[1];
  index_three = periodic_impropers_[index].first[2];
  index_four = periodic_impropers_[index].first[3];
}

void Topology::GetHarmonicImproperParameters(uint index, uint& index_one, uint& index_two, uint& index_three, uint& index_four,
                                             Real& angle, Real& force_constant) const{
  if(index >= harmonic_impropers_.size()){
    throw ost::Error("Provided index exceeds number of harmonic impropers present in topology!");
  }
  angle = harmonic_impropers_[index].second[0];
  force_constant = harmonic_impropers_[index].second[1];
  index_one = harmonic_impropers_[index].first[0];
  index_two = harmonic_impropers_[index].first[1];
  index_three = harmonic_impropers_[index].first[2];
  index_four = harmonic_impropers_[index].first[3];
}

void Topology::GetCMapParameters(uint index, uint& index_one, uint& index_two, uint& index_three, uint& index_four, uint& index_five,
                                 int& dimension, std::vector<Real>& map) const{
  if(index >= cmaps_.size()){
    throw ost::Error("Provided index exceeds number of cmaps present in topology!");
  }
  dimension = int(sqrt(cmaps_[index].second.size()));
  map = cmaps_[index].second;
  index_one = cmaps_[index].first[0];
  index_two = cmaps_[index].first[1];
  index_three = cmaps_[index].first[2];
  index_four = cmaps_[index].first[3];
  index_five = cmaps_[index].first[4];
  
}

void Topology::GetLJPairParameters(uint index, uint& index_one, uint& index_two,
                                   Real& sigma, Real& epsilon) const{
  if(index >= lj_pairs_.size()){
    throw ost::Error("Provided index exceeds number of lj pairs present in topology!");
  }
  sigma = lj_pairs_[index].second[0];
  epsilon = lj_pairs_[index].second[1];
  index_one = lj_pairs_[index].first[0];
  index_two = lj_pairs_[index].first[1];
}

void Topology::GetDistanceConstraintParameters(uint index, uint& index_one, uint& index_two,
                                               Real& distance) const{
  if(index >= distance_constraints_.size()){
    throw ost::Error("Provided index exceeds number of distance_constraints present in topology!");
  }
  distance = distance_constraints_[index].second[0];
  index_one = distance_constraints_[index].first[0];
  index_two = distance_constraints_[index].first[1];
}

void Topology::GetHarmonicPositionRestraintParameters(uint index, uint& atom_index, geom::Vec3& ref_position,
                                               Real& k, Real& x_scale, Real& y_scale, Real& z_scale) const{
  if(index >= harmonic_position_restraints_.size()){
    throw ost::Error("Provided index exceeds numnber of harmonic position constraints present in topology!");
  }
  atom_index = harmonic_position_restraints_[index].first[0];
  ref_position[0] = harmonic_position_restraints_[index].second[0];
  ref_position[1] = harmonic_position_restraints_[index].second[1];
  ref_position[2] = harmonic_position_restraints_[index].second[2];
  k = harmonic_position_restraints_[index].second[3];
  x_scale = harmonic_position_restraints_[index].second[4];
  y_scale = harmonic_position_restraints_[index].second[5];
  z_scale = harmonic_position_restraints_[index].second[6];
}

void Topology::GetHarmonicDistanceRestraintParameters(uint index, uint& atom_one, uint& atom_two, Real& length,
                                            Real& force_constant) const{
  if(index >= harmonic_distance_restraints_.size()){
    throw ost::Error("Provided index exceeds number of harmonic distance restraints present in topology!");
  }
  atom_one = harmonic_distance_restraints_[index].first[0];
  atom_two = harmonic_distance_restraints_[index].first[1];
  length = harmonic_distance_restraints_[index].second[0];
  force_constant = harmonic_distance_restraints_[index].second[1];
}


void Topology::SetHarmonicBondParameters(uint index, const Real& bond_length, const Real& force_constant){
  if(index >= harmonic_bonds_.size()){
    throw ost::Error("Provided index exceeds number of harmonic bonds present in topology!");
  }
  harmonic_bonds_[index].second[0] = bond_length;
  harmonic_bonds_[index].second[1] = force_constant;
}

void Topology::SetHarmonicAngleParameters(uint index, const Real& angle, const Real& force_constant){
  if(index >= harmonic_angles_.size()){
    throw ost::Error("Provided index exceeds number of harmonic angles present in topology!");
  }
  harmonic_angles_[index].second[0] = angle;
  harmonic_angles_[index].second[1] = force_constant;
}

void Topology::SetUreyBradleyAngleParameters(uint index, const Real& angle, const Real& angle_force_constant, const Real& bond_length, const Real& bond_force_constant){
  if(index >= urey_bradley_angles_.size()){
    throw ost::Error("Provided index exceeds number of urey_bradley angles present in topology!");
  }
  urey_bradley_angles_[index].second[0] = angle;
  urey_bradley_angles_[index].second[1] = angle_force_constant;
  urey_bradley_angles_[index].second[2] = bond_length;
  urey_bradley_angles_[index].second[3] = bond_force_constant;
}

void Topology::SetPeriodicDihedralParameters(uint index, const int& multiplicity, const Real& phase, const Real& force_constant){
  if(index >= periodic_dihedrals_.size()){
    throw ost::Error("Provided index exceeds number of periodic dihedrals present in topology!");
  }
  periodic_dihedrals_[index].second[0] = multiplicity;
  periodic_dihedrals_[index].second[1] = phase;
  periodic_dihedrals_[index].second[2] = force_constant;
}

void Topology::SetPeriodicImproperParameters(uint index, const int& multiplicity, const Real& phase, const Real& force_constant){
  if(index >= periodic_impropers_.size()){
    throw ost::Error("Provided index exceeds number of periodic impropers present in topology!");
  }
  periodic_impropers_[index].second[0] = multiplicity;
  periodic_impropers_[index].second[1] = phase;
  periodic_impropers_[index].second[2] = force_constant;
}

void Topology::SetHarmonicImproperParameters(uint index, const Real& angle, const Real& force_constant){
  if(index >= harmonic_impropers_.size()){
    throw ost::Error("Provided index exceeds number of harmonic impropers present in topology!");
  }
  harmonic_impropers_[index].second[0] = angle;
  harmonic_impropers_[index].second[1] = force_constant;
}

void Topology::SetCMapParameters(uint index, const int& dimension, const std::vector<Real>& map){
  if(index >= cmaps_.size()){
    throw ost::Error("Provided index exceeds number of cmaps present in topology!");
  }
  if(uint(dimension * dimension) != map.size()){
    throw ost::Error("Provided dimension and map are inconsistent!");
  }
  cmaps_[index].second = map;
}

void Topology::SetLJPairParameters(uint index, const Real& sigma, const Real& epsilon){
  if(index >= lj_pairs_.size()){
    throw ost::Error("Provided index exceeds number of lj pairs present in topology!");
  }
  lj_pairs_[index].second[0] = sigma;
  lj_pairs_[index].second[1] = epsilon;
}

void Topology::SetDistanceConstraintParameters(uint index, const Real& distance){
  if(index >= distance_constraints_.size()){
    throw ost::Error("Provided index exceeds number of distance_constraints present in topology!");
  }
  distance_constraints_[index].second[0] = distance;
}

void Topology::SetHarmonicPositionRestraintParameters(uint index, const geom::Vec3& ref_position, Real k, 
                                                      Real x_scale, Real y_scale, Real z_scale){
  if(index >= harmonic_position_restraints_.size()){
    throw ost::Error("Provided index exceeds number of harmonic position restraints present in topology!");
  }
  harmonic_position_restraints_[index].second[0] = ref_position[0];
  harmonic_position_restraints_[index].second[1] = ref_position[1];
  harmonic_position_restraints_[index].second[2] = ref_position[2];
  harmonic_position_restraints_[index].second[3] = k;
  harmonic_position_restraints_[index].second[4] = x_scale;
  harmonic_position_restraints_[index].second[5] = y_scale;
  harmonic_position_restraints_[index].second[6] = z_scale;
}

void Topology::SetHarmonicDistanceRestraintParameters(uint index, Real length, 
                                                      Real force_constant){
  if(index >= harmonic_distance_restraints_.size()){
    throw ost::Error("Provided index exceeds number of harmonic distance restraints present in topology!");
  }
  harmonic_distance_restraints_[index].second[0] = length;
  harmonic_distance_restraints_[index].second[1] = force_constant; 

}

uint Topology::GetAtomIndex(const ost::mol::AtomHandle& at) const{
  std::map<long,uint>::const_iterator i = atom_index_mapper_.find(at.GetHashCode());
  if(i == atom_index_mapper_.end()){
    throw ost::Error("Could not find atom in attached entity!");
  }
  return i->second;
}


uint Topology::GetResidueIndex(const ost::mol::ResidueHandle& res) const{
  std::map<long,uint>::const_iterator i = residue_index_mapper_.find(res.GetHashCode());
  if(i == residue_index_mapper_.end()){
    throw ost::Error("Could not find residue in attached entity!");
  }
  return i->second;
}

uint Topology::GetAtomIndex(uint residue_index, const String& atom_name) const{
  if(residue_index < 0 || residue_index >= num_residues_){
    throw ost::Error("Residue index out of bounds");
  }
  if(atom_name[0] == '+'){
    ost::mol::ResidueHandle actual_res = res_list_[residue_index];
    ost::mol::ResidueHandle next_residue = actual_res.GetNext();
    if(!next_residue){
      throw ost::Error("Could not get next residue!");
    }
    String next_atom_name = atom_name.substr(1);
    std::map<String,uint>::const_iterator it = atom_name_mapper_[this->GetResidueIndex(next_residue)].find(next_atom_name);
    if(it == atom_name_mapper_[this->GetResidueIndex(next_residue)].end()){
      std::stringstream ss;
      ss << "Could not find required atom " << next_atom_name;
      ss << " from residue " << next_residue.GetQualifiedName();
    }
    return it->second;
  }
  if(atom_name[0] == '-'){
    ost::mol::ResidueHandle actual_res = res_list_[residue_index];
    ost::mol::ResidueHandle prev_residue = actual_res.GetPrev();
    if(!prev_residue){
      throw ost::Error("Could not get prev residue!");
    }
    String prev_atom_name = atom_name.substr(1);
    std::map<String,uint>::const_iterator it = atom_name_mapper_[this->GetResidueIndex(prev_residue)].find(prev_atom_name);
    if(it == atom_name_mapper_[this->GetResidueIndex(prev_residue)].end()){
      std::stringstream ss;
      ss << "Could not find required atom " << prev_atom_name;
      ss << " from residue " << prev_residue.GetQualifiedName();
    }
    return it->second;
  }
  std::map<String,uint>::const_iterator it = atom_name_mapper_[residue_index].find(atom_name);
  if(it == atom_name_mapper_[residue_index].end()){
    std::stringstream ss;
    ss << "Could not find required atom " << atom_name;
    ss << " from residue " << res_list_[residue_index].GetQualifiedName();
  }
  return it->second;
}

Real Topology::GetMass(uint index) const{
  if(index >= num_atoms_) throw ost::Error("Provided index exceeds number of atoms present in topology!");
  return atom_masses_[index];
}

Real Topology::GetSigma(uint index) const{
  if(index >= num_atoms_) throw ost::Error("Provided index exceeds number of atoms present in topology!");
  if(sigmas_.empty()) throw ost::Error("No sigmas set!");
  return sigmas_[index];
}

Real Topology::GetEpsilon(uint index) const{
  if(index >= num_atoms_) throw ost::Error("Provided index exceeds number of atoms present in topology!");
  if(epsilons_.empty()) throw ost::Error("No epsilons set!");
  return epsilons_[index];
}

Real Topology::GetGBSARadius(uint index) const{
  if(index >= num_atoms_) throw ost::Error("Provided index exceeds number of atoms present in topology!");
  if(gbsa_radii_.empty()) throw ost::Error("No gbsa radii set!");
  return gbsa_radii_[index];
}

Real Topology::GetOBCScaling(uint index) const{
  if(index >= num_atoms_) throw ost::Error("Provided index exceeds number of atoms present in topology!");
  if(obc_scaling_.empty()) throw ost::Error("No obc scalings set!");
  return obc_scaling_[index];
}


Real Topology::GetCharge(uint index) const{
  if(index >= num_atoms_) throw ost::Error("Provided index exceeds number of atoms present in topology!");
  if(charges_.empty()) throw ost::Error("No charges set!");
  return charges_[index];
}


void Topology::SetEntityPositions(const geom::Vec3List& positions){
  if(positions.size() != num_atoms_){
    throw ost::Error("Expect positions vector to have the same number of elements than atoms in the topologies entity!");
  }
  ost::mol::XCSEditor ed = ent_.EditXCS(ost::mol::BUFFERED_EDIT);
  for(uint i = 0; i < num_atoms_; ++i){
    ed.SetAtomPos(atom_list_[i],positions[i]);
  }
  ed.UpdateICS();
}

std::vector<uint> Topology::GetHarmonicBondIndices(uint index_one,
                                                    uint index_two) const{
  Index<2> index(index_one,index_two);
  Index<2> reverse_index(index_two,index_one);
  std::vector<uint> return_indices;
  for(uint i = 0; i < harmonic_bonds_.size(); ++i){
    if(index == harmonic_bonds_[i].first || reverse_index == harmonic_bonds_[i].first){
      return_indices.push_back(i);
    }
  }
  return return_indices;
}

std::vector<uint> Topology::GetHarmonicAngleIndices(uint index_one,
                                                     uint index_two,
                                                     uint index_three) const{
  Index<3> index(index_one,index_two,index_three);
  Index<3> reverse_index(index_three,index_two,index_one);
  std::vector<uint> return_indices;
  for(uint i = 0; i < harmonic_angles_.size(); ++i){
    if(index == harmonic_angles_[i].first || reverse_index == harmonic_angles_[i].first){
      return_indices.push_back(i);
    }
  }
  return return_indices;
}

std::vector<uint> Topology::GetUreyBradleyAngleIndices(uint index_one,
                                                        uint index_two,
                                                        uint index_three) const{
  Index<3> index(index_one,index_two,index_three);
  Index<3> reverse_index(index_three,index_two,index_one);
  std::vector<uint> return_indices;
  for(uint i = 0; i < urey_bradley_angles_.size(); ++i){
    if(index == urey_bradley_angles_[i].first || reverse_index == urey_bradley_angles_[i].first){
      return_indices.push_back(i);
    }
  }
  return return_indices;
}

std::vector<uint> Topology::GetPeriodicDihedralIndices(uint index_one,
                                                        uint index_two,
                                                        uint index_three,
                                                        uint index_four) const{
  Index<4> index(index_one,index_two,index_three,index_four);
  Index<4> reverse_index(index_four,index_three,index_two,index_one);
  std::vector<uint> return_indices;
  for(uint i = 0; i < periodic_dihedrals_.size(); ++i){
    if(index == periodic_dihedrals_[i].first || reverse_index == periodic_dihedrals_[i].first){
      return_indices.push_back(i);
    }
  }
  return return_indices;
}

std::vector<uint> Topology::GetPeriodicImproperIndices(uint index_one,
                                                        uint index_two,
                                                        uint index_three,
                                                        uint index_four) const{
  Index<4> index(index_one,index_two,index_three,index_four);
  Index<4> reverse_index(index_four,index_three,index_two,index_one);
  std::vector<uint> return_indices;
  for(uint i = 0; i < periodic_impropers_.size(); ++i){
    if(index == periodic_impropers_[i].first || reverse_index == periodic_impropers_[i].first){
      return_indices.push_back(i);
    }
  }
  return return_indices;
}

std::vector<uint> Topology::GetHarmonicImproperIndices(uint index_one,
                                                        uint index_two,
                                                        uint index_three,
                                                        uint index_four) const{
  Index<4> index(index_one,index_two,index_three,index_four);
  Index<4> reverse_index(index_four,index_three,index_two,index_one);
  std::vector<uint> return_indices;
  for(uint i = 0; i < harmonic_impropers_.size(); ++i){
    if(index == harmonic_impropers_[i].first || reverse_index == harmonic_impropers_[i].first){
      return_indices.push_back(i);
    }
  }
  return return_indices;
}

std::vector<uint> Topology::GetCMapIndices(uint index_one,
                                            uint index_two,
                                            uint index_three,
                                            uint index_four,
                                            uint index_five) const{
  //note, that in case of cmaps the order of the atoms matters
  Index<5> index(index_one,index_two,index_three,index_four,index_five);
  std::vector<uint> return_indices;
  for(uint i = 0; i < cmaps_.size(); ++i){
    if(index == cmaps_[i].first){
      return_indices.push_back(i);
    }
  }
  return return_indices;
}

int Topology::GetLJPairIndex(uint index_one,
                                             uint index_two) const{
  Index<2> index(index_one,index_two);
  Index<2> reverse_index(index_two,index_one);
  for(uint i = 0; i < lj_pairs_.size(); ++i){
    if(index == lj_pairs_[i].first || reverse_index == lj_pairs_[i].first){
      return i;
    }
  }
  return -1;
}

int Topology::GetDistanceConstraintIndex(uint index_one,
                                         uint index_two) const{
  Index<2> index(index_one,index_two);
  Index<2> reverse_index(index_two,index_one);
  for(uint i = 0; i < distance_constraints_.size(); ++i){
    if(index == distance_constraints_[i].first || reverse_index == distance_constraints_[i].first){
      return i;
    }
  }
  return -1;
}

std::vector<uint> Topology::GetHarmonicPositionRestraintIndices(uint atom_index) const{
  Index<1> index(atom_index);
  std::vector<uint> return_indices;
  for(uint i = 0; i < harmonic_position_restraints_.size(); ++i){
    if(index == harmonic_position_restraints_[i].first){
      return_indices.push_back(i);
    }
  }
  return return_indices;
}

std::vector<uint> Topology::GetHarmonicDistanceRestraintIndices(uint index_one, uint index_two) const{

  Index<2> index(index_one,index_two);
  Index<2> reverse_index(index_two,index_one);
  std::vector<uint> return_indices;
  for(uint i = 0; i < harmonic_distance_restraints_.size(); ++i){
    if(index == harmonic_distance_restraints_[i].first || reverse_index == harmonic_distance_restraints_[i].first){
      return_indices.push_back(i);
    }
  }
  return return_indices;
}

std::vector<uint> Topology::GetHarmonicBondIndices(uint atom_index) const{
  std::vector<uint> return_indices;
  for(uint i = 0; i < harmonic_bonds_.size(); ++i){
    if(harmonic_bonds_[i].first[0] == atom_index || harmonic_bonds_[i].first[1] == atom_index){
      return_indices.push_back(i);
    }
  }
  return return_indices;
}

std::vector<uint> Topology::GetHarmonicAngleIndices(uint atom_index) const{
  std::vector<uint> return_indices;
  for(uint i = 0; i < harmonic_angles_.size(); ++i){
    if(harmonic_angles_[i].first[0] == atom_index || harmonic_angles_[i].first[1] == atom_index ||
       harmonic_angles_[i].first[2] == atom_index){
      return_indices.push_back(i);
    }
  }
  return return_indices;
}

std::vector<uint> Topology::GetUreyBradleyAngleIndices(uint atom_index) const{
  std::vector<uint> return_indices;
  for(uint i = 0; i < urey_bradley_angles_.size(); ++i){
    if(urey_bradley_angles_[i].first[0] == atom_index || urey_bradley_angles_[i].first[1] == atom_index ||
       urey_bradley_angles_[i].first[2] == atom_index){
      return_indices.push_back(i);
    }
  }
  return return_indices;
}

std::vector<uint> Topology::GetPeriodicDihedralIndices(uint atom_index) const{
  std::vector<uint> return_indices;
  for(uint i = 0; i < periodic_dihedrals_.size(); ++i){
    if(periodic_dihedrals_[i].first[0] == atom_index || periodic_dihedrals_[i].first[1] == atom_index ||
       periodic_dihedrals_[i].first[2] == atom_index || periodic_dihedrals_[i].first[3] == atom_index){
      return_indices.push_back(i);
    }
  }
  return return_indices;
}

std::vector<uint> Topology::GetPeriodicImproperIndices(uint atom_index) const{
  std::vector<uint> return_indices;
  for(uint i = 0; i < periodic_impropers_.size(); ++i){
    if(periodic_impropers_[i].first[0] == atom_index || periodic_impropers_[i].first[1] == atom_index ||
       periodic_impropers_[i].first[2] == atom_index || periodic_impropers_[i].first[3] == atom_index){
      return_indices.push_back(i);
    }
  }
  return return_indices;
}

std::vector<uint> Topology::GetHarmonicImproperIndices(uint atom_index) const{
  std::vector<uint> return_indices;
  for(uint i = 0; i < harmonic_impropers_.size(); ++i){
    if(harmonic_impropers_[i].first[0] == atom_index || harmonic_impropers_[i].first[1] == atom_index ||
       harmonic_impropers_[i].first[2] == atom_index || harmonic_impropers_[i].first[3] == atom_index){
      return_indices.push_back(i);
    }
  }
  return return_indices;
}

std::vector<uint> Topology::GetCMapIndices(uint atom_index) const{
  std::vector<uint> return_indices;
  for(uint i = 0; i < cmaps_.size(); ++i){
    if(cmaps_[i].first[0] == atom_index || cmaps_[i].first[1] == atom_index ||
       cmaps_[i].first[2] == atom_index || cmaps_[i].first[3] == atom_index ||
       cmaps_[i].first[4] == atom_index){
      return_indices.push_back(i);
    }
  }
  return return_indices;
}

std::vector<uint> Topology::GetLJPairIndices(uint atom_index) const{
  std::vector<uint> return_indices;
  for(uint i = 0; i < lj_pairs_.size(); ++i){
    if(lj_pairs_[i].first[0] == atom_index || lj_pairs_[i].first[1] == atom_index){
      return_indices.push_back(i);
    }
  }
  return return_indices;
}

std::vector<uint> Topology::GetDistanceConstraintIndices(uint atom_index) const{
  std::vector<uint> return_indices;
  for(uint i = 0; i < distance_constraints_.size(); ++i){
    if(distance_constraints_[i].first[0] == atom_index || distance_constraints_[i].first[1] == atom_index){
      return_indices.push_back(i);
    }
  }
  return return_indices;
}

std::vector<uint> Topology::GetHarmonicDistanceRestraintIndices(uint atom_index) const{
  std::vector<uint> return_indices;
  for(uint i = 0; i < harmonic_distance_restraints_.size(); ++i){
    if(harmonic_distance_restraints_[i].first[0] == atom_index || harmonic_distance_restraints_[i].first[1] == atom_index){
      return_indices.push_back(i);
    }
  }
  return return_indices;
}

void Topology::Merge(TopologyPtr p){  
  //check whether there is chain from the new entity is already present in the actual entity 
  ost::mol::EntityHandle source_ent = p->GetEntity();
  ost::mol::ChainHandleList source_chains = source_ent.GetChainList();
  for(ost::mol::ChainHandleList::iterator i = source_chains.begin();
      i != source_chains.end(); ++i){
    if(ent_.FindChain(i->GetName()).IsValid()){
      std::stringstream ss;
      ss << "Chain with name \"" << i->GetName() << "\" from source topology";
      ss << "is already present in destination topology!";
      throw ost::Error(ss.str());
    }
  }
  //check whether the fudge parameters are consistent
  if(p->GetFudgeLJ() != fudge_lj_ || p->GetFudgeQQ() != fudge_qq_){
    throw ost::Error("Expect the fudge parameters of source topology to consistent with the fudge parameters from the destination topology!");
  }

  //mapper of hashcode from source atom to index in added_atom list
  std::map<long,int> index_mapper;
  ost::mol::AtomHandleList added_atoms;

  //let's create an editor to copy over all chains, residues and atoms
  //from the source topologies entity
  ost::mol::XCSEditor ed = ent_.EditXCS(ost::mol::BUFFERED_EDIT);
  for(ost::mol::ChainHandleList::iterator i = source_chains.begin();
      i != source_chains.end(); ++i){
    ost::mol::ResidueHandleList res_list = i->GetResidueList();
    ost::mol::ChainHandle added_chain = ed.InsertChain(i->GetName());

    for(ost::mol::ResidueHandleList::iterator j = res_list.begin();
        j != res_list.end(); ++j){
      ost::mol::AtomHandleList atom_list = j->GetAtomList();
      ost::mol::ResidueHandle added_residue = ed.AppendResidue(added_chain,*j);

      for(ost::mol::AtomHandleList::iterator k = atom_list.begin();
          k != atom_list.end(); ++k){
        index_mapper[k->GetHashCode()] = added_atoms.size();
        added_atoms.push_back(ed.InsertAtom(added_residue,*k));
      }
    }
  }

  //let's rebuild the connectivity
  ost::mol::BondHandleList bond_list = source_ent.GetBondList();
  for(ost::mol::BondHandleList::iterator i = bond_list.begin();
      i != bond_list.end(); ++i){
    ed.Connect(added_atoms[index_mapper[i->GetFirst().GetHashCode()]],
               added_atoms[index_mapper[i->GetSecond().GetHashCode()]]);
  }
  ed.UpdateICS();

  //let's update the remaining structural data
  atom_list_ = ent_.GetAtomList();
  res_list_ = ent_.GetResidueList();
  num_atoms_ = atom_list_.size();
  num_residues_ = res_list_.size();

  //let's update the index mappers
  for(uint i = 0; i < num_atoms_; ++i){
    if(atom_index_mapper_.find(atom_list_[i].GetHashCode()) == atom_index_mapper_.end()){
      atom_index_mapper_[atom_list_[i].GetHashCode()] = i;
      continue;
    }
    //indices of atoms that have already been there must not be changed
    if(atom_index_mapper_[atom_list_[i].GetHashCode()] != i){
      throw ost::Error("Updating indices during merging process went horribly wrong!");
    }
  }

  for(uint i = 0; i != num_residues_; ++i){
    if(residue_index_mapper_.find(res_list_[i].GetHashCode()) == residue_index_mapper_.end()){
      residue_index_mapper_[res_list_[i].GetHashCode()] = i;
      continue;
    }
    //indices of residues that have already been there must not be changed
    if(residue_index_mapper_[res_list_[i].GetHashCode()] != i){
      throw ost::Error("Updating indices during merging process went horribly wrong!");
    }
  }

  atom_name_mapper_.clear(); //let's rebuild from scratch
  std::map<String,uint> per_residue_mapper;
  ost::mol::AtomHandleList residue_atom_list;
  for(uint i = 0; i < num_residues_; ++i){
    per_residue_mapper.clear();
    residue_atom_list = res_list_[i].GetAtomList();
    for(ost::mol::AtomHandleList::iterator j = residue_atom_list.begin();
        j != residue_atom_list.end(); ++j){
      per_residue_mapper[j->GetName()] = atom_index_mapper_[j->GetHashCode()];
    }
    atom_name_mapper_.push_back(per_residue_mapper);
  }
 
  //let's first generate an index mapper from source atom indices to the indices in
  //the destination entity
  ost::mol::AtomHandleList source_atoms = source_ent.GetAtomList();
  std::map<int,int> final_index_mapper;
  for(uint i = 0; i < source_atoms.size(); ++i){
    final_index_mapper[i] = atom_index_mapper_[added_atoms[index_mapper[source_atoms[i].GetHashCode()]].GetHashCode()];
  }

  //let's map over masses
  std::vector<Real> source_masses = p->GetMasses();

  for(uint i = 0; i < source_atoms.size(); ++i){
    atom_masses_.push_back(0.0);
  }
  for(uint i = 0; i < source_atoms.size(); ++i){
    atom_masses_[final_index_mapper[i]] = source_masses[i];
  }

  //let's map over the other per atom parameters if they're defined in the destination topology
  if(!charges_.empty()){
    std::vector<Real> source_charges = p->GetCharges();
    if(source_charges.empty()){
      throw ost::Error("Cannot merge topology without charges into a topology with defined charges!");
    }
    for(uint i = 0; i < source_charges.size(); ++i){
      charges_.push_back(0.0);
    }
    for(uint i = 0; i < source_charges.size(); ++i){
      charges_[final_index_mapper[i]] = source_charges[i];
    }
  }

  if(!sigmas_.empty()){
    std::vector<Real> source_sigmas = p->GetSigmas();
    if(source_sigmas.empty()){
      throw ost::Error("Cannot merge topology without lj sigmas into a topology with defined sigmas!");
    }
    for(uint i = 0; i < source_sigmas.size(); ++i){
      sigmas_.push_back(0.0);
    }
    for(uint i = 0; i < source_sigmas.size(); ++i){
      sigmas_[final_index_mapper[i]] = source_sigmas[i];
    }
  }

  if(!epsilons_.empty()){
    std::vector<Real> source_epsilons= p->GetEpsilons();
    if(source_epsilons.empty()){
      throw ost::Error("Cannot merge topology without lj epsilons into a topology with defined epsilons!");
    }
    for(uint i = 0; i < source_epsilons.size(); ++i){
      epsilons_.push_back(0.0);
    }
    for(uint i = 0; i < source_epsilons.size(); ++i){
      epsilons_[final_index_mapper[i]] = source_epsilons[i];
    }
  }

  if(!gbsa_radii_.empty()){
    std::vector<Real> source_radii= p->GetGBSARadii();
    if(source_radii.empty()){
      throw ost::Error("Cannot merge topology without gbsa radii into a topology with defined radii!");
    }
    for(uint i = 0; i < source_radii.size(); ++i){
      gbsa_radii_.push_back(0.0);
    }
    for(uint i = 0; i < source_radii.size(); ++i){
      gbsa_radii_[final_index_mapper[i]] = source_radii[i];
    }
  }

  if(!obc_scaling_.empty()){
    std::vector<Real> source_scaling= p->GetOBCScalings();
    if(source_scaling.empty()){
      throw ost::Error("Cannot merge topology without obc scaling into a topology with defined scaling!");
    }
    for(uint i = 0; i < source_scaling.size(); ++i){
      obc_scaling_.push_back(0.0);
    }
    for(uint i = 0; i < source_scaling.size(); ++i){
      obc_scaling_[final_index_mapper[i]] = source_scaling[i];
    }
  }

  //finally, all the interactions get mapped over

  const std::vector<std::pair<Index<2>, std::vector<Real> > >& harmonic_bonds = p->GetHarmonicBonds();
  if(!harmonic_bonds.empty()){
    for(std::vector<std::pair<Index<2>, std::vector<Real> > >::const_iterator i = harmonic_bonds.begin();
        i != harmonic_bonds.end(); ++i){
      this->AddHarmonicBond(final_index_mapper[i->first[0]],
                            final_index_mapper[i->first[1]],
                            i->second[0],i->second[1]);
    }
  }

  const std::vector<std::pair<Index<3>, std::vector<Real> > >& harmonic_angles = p->GetHarmonicAngles();
  if(!harmonic_angles.empty()){
    for(std::vector<std::pair<Index<3>, std::vector<Real> > >::const_iterator i = harmonic_angles.begin();
        i != harmonic_angles.end(); ++i){
      this->AddHarmonicAngle(final_index_mapper[i->first[0]],
                             final_index_mapper[i->first[1]],
                             final_index_mapper[i->first[2]],
                             i->second[0],i->second[1]);
    }
  }

  const std::vector<std::pair<Index<3>, std::vector<Real> > >& urey_bradley_angles = p->GetUreyBradleyAngles();
  if(!urey_bradley_angles.empty()){
    for(std::vector<std::pair<Index<3>, std::vector<Real> > >::const_iterator i = urey_bradley_angles.begin();
        i != urey_bradley_angles.end(); ++i){
      this->AddUreyBradleyAngle(final_index_mapper[i->first[0]],
                                final_index_mapper[i->first[1]],
                                final_index_mapper[i->first[2]],
                                i->second[0],i->second[1],
                                i->second[2],i->second[3]);
    }
  }

  const std::vector<std::pair<Index<4>, std::vector<Real> > >& periodic_dihedrals = p->GetPeriodicDihedrals();
  if(!periodic_dihedrals.empty()){
    for(std::vector<std::pair<Index<4>, std::vector<Real> > >::const_iterator i = periodic_dihedrals.begin();
        i != periodic_dihedrals.end(); ++i){
      this->AddPeriodicDihedral(final_index_mapper[i->first[0]],
                                final_index_mapper[i->first[1]],
                                final_index_mapper[i->first[2]],
                                final_index_mapper[i->first[3]],
                                int(i->second[0]),i->second[1],i->second[2]);
    }
  }

  const std::vector<std::pair<Index<4>, std::vector<Real> > >& periodic_impropers = p->GetPeriodicImpropers();
  if(!periodic_impropers.empty()){
    for(std::vector<std::pair<Index<4>, std::vector<Real> > >::const_iterator i = periodic_impropers.begin();
        i != periodic_impropers.end(); ++i){
      this->AddPeriodicImproper(final_index_mapper[i->first[0]],
                                final_index_mapper[i->first[1]],
                                final_index_mapper[i->first[2]],
                                final_index_mapper[i->first[3]],
                                int(i->second[0]),i->second[1],i->second[2]);
    }
  }

  const std::vector<std::pair<Index<4>, std::vector<Real> > >& harmonic_impropers = p->GetHarmonicImpropers();
  if(!harmonic_impropers.empty()){
    for(std::vector<std::pair<Index<4>, std::vector<Real> > >::const_iterator i = harmonic_impropers.begin();
        i != harmonic_impropers.end(); ++i){
      this->AddHarmonicImproper(final_index_mapper[i->first[0]],
                                final_index_mapper[i->first[1]],
                                final_index_mapper[i->first[2]],
                                final_index_mapper[i->first[3]],
                                i->second[0],i->second[1]);
    }
  }

  const std::vector<std::pair<Index<5>, std::vector<Real> > >& cmaps = p->GetCMaps();
  if(!cmaps.empty()){
    for(std::vector<std::pair<Index<5>, std::vector<Real> > >::const_iterator i = cmaps.begin();
        i != cmaps.end(); ++i){
      uint dimension = sqrt(i->second.size());
      this->AddCMap(final_index_mapper[i->first[0]],
                    final_index_mapper[i->first[1]],
                    final_index_mapper[i->first[2]],
                    final_index_mapper[i->first[3]],
                    final_index_mapper[i->first[4]],
                    dimension,i->second);
    }
  }

  const std::vector<std::pair<Index<2>, std::vector<Real> > >& ljpairs = p->GetLJPairs();
  if(!ljpairs.empty()){
    for(std::vector<std::pair<Index<2>, std::vector<Real> > >::const_iterator i = ljpairs.begin();
        i != ljpairs.end(); ++i){
      this->AddLJPair(final_index_mapper[i->first[0]],
                      final_index_mapper[i->first[1]],
                      i->second[0],i->second[1]);
    }
  }


  const std::vector<std::pair<Index<2>, std::vector<Real> > >& distance_constraints = p->GetDistanceConstraints();
  if(!distance_constraints.empty()){
    for(std::vector<std::pair<Index<2>, std::vector<Real> > >::const_iterator i = distance_constraints.begin();
        i != distance_constraints.end(); ++i){
      this->AddDistanceConstraint(final_index_mapper[i->first[0]],
                                  final_index_mapper[i->first[1]],
                                  i->second[0]);
    }
  }

  const std::vector<Index<2> >& exclusions = p->GetExclusions();
  if(!exclusions.empty()){
    for(std::vector<Index<2> >::const_iterator i = exclusions.begin();
        i != exclusions.end(); ++i){
      this->AddExclusion(final_index_mapper[(*i)[0]],
                          final_index_mapper[(*i)[1]]);
    }
  }

  const std::vector<std::pair<Index<1>,std::vector<Real> > >& harmonic_position_restraints = p->GetHarmonicPositionRestraints();
  if(!harmonic_position_restraints.empty()){
    for(std::vector<std::pair<Index<1>,std::vector<Real> > >::const_iterator i = harmonic_position_restraints.begin();
        i != harmonic_position_restraints.end(); ++i){
      geom::Vec3 ref_pos(i->second[0],i->second[1],i->second[2]);
      this->AddHarmonicPositionRestraint(final_index_mapper[i->first[0]],
                                         ref_pos,
                                         i->second[3],
                                         i->second[4],
                                         i->second[5],
                                         i->second[6]);
    }
  }

  const std::vector<std::pair<Index<2>,std::vector<Real> > >& harmonic_distance_restraints = p->GetHarmonicDistanceRestraints();
  if(!harmonic_distance_restraints.empty()){
    for(std::vector<std::pair<Index<2>,std::vector<Real> > >::const_iterator i = harmonic_distance_restraints.begin();
        i != harmonic_distance_restraints.end(); ++i){
      this->AddHarmonicDistanceRestraint(final_index_mapper[i->first[0]],
                                         final_index_mapper[i->first[1]],
                                         i->second[0],
                                         i->second[1]);
    }
  }

  const std::vector<uint>& position_constraints = p->GetPositionConstraints();
  if(!position_constraints.empty()){
    for(std::vector<uint>::const_iterator i = position_constraints.begin();
        i != position_constraints.end(); ++i){
      this->AddPositionConstraint(final_index_mapper[*i]);
    }
  }

  for(std::set<Index<2> >::iterator i = p->added_lj_pairs_.begin();
      i != p->added_lj_pairs_.end(); ++i){
    added_lj_pairs_.insert(Index<2>(final_index_mapper[(*i)[0]],
                                    final_index_mapper[(*i)[1]]));
  }

  for(std::set<Index<2> >::iterator i = p->added_distance_constraints_.begin();
      i != p->added_distance_constraints_.end(); ++i){
    added_distance_constraints_.insert(Index<2>(final_index_mapper[(*i)[0]],
                                                final_index_mapper[(*i)[1]]));
  }

  for(std::set<Index<2> >::iterator i = p->added_exclusions_.begin();
      i != p->added_exclusions_.end(); ++i){
    added_exclusions_.insert(Index<2>(final_index_mapper[(*i)[0]],
                                      final_index_mapper[(*i)[1]]));
  }

}


}}}//ns
