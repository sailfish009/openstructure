#ifndef OST_MM_TOPOLOGY_HH
#define OST_MM_TOPOLOGY_HH

#include <vector>
#include <map>
#include <set>

#include <boost/shared_ptr.hpp>

#include <ost/message.hh>
#include <ost/mol/atom_handle.hh>
#include <ost/mol/residue_handle.hh>
#include <ost/mol/entity_handle.hh>
#include <ost/mol/mm/index.hh>
#include <ost/mol/mm/buildingblock.hh>
#include <ost/mol/mm/block_modifiers.hh>
#include <ost/mol/mm/forcefield.hh>
#include <ost/mol/mm/mm_settings.hh>
#include <ost/mol/mm/index.hh>
#include <ost/mol/mm/mm_interaction.hh>
#include <ost/mol/xcs_editor.hh>
#include <ost/mol/bond_handle.hh>

#include <time.h>



namespace ost { namespace mol{ namespace mm{

class Topology;
typedef boost::shared_ptr<Topology> TopologyPtr;


class Topology{

public:

  Topology(const ost::mol::EntityHandle& ent, const std::vector<Real>& masses);

  uint AddHarmonicBond(uint index_one,
                       uint index_two, 
                       Real bond_length,
                       Real force_constant);

  uint AddHarmonicAngle(uint index_one,
                        uint index_two,
                        uint index_three,
                        Real angle,
                        Real force_constant);

  uint AddUreyBradleyAngle(uint index_one,
                           uint index_two,
                           uint index_three,
                           Real angle,
                           Real angle_force_constant,
                           Real bond_length,
                           Real bond_force_constant);

  uint AddPeriodicDihedral(uint index_one,
                           uint index_two,
                           uint index_three,
                           uint index_four,
                           int multiplicity,
                           Real phase,
                           Real force_constant);

  uint AddPeriodicImproper(uint index_one,
                           uint index_two,
                           uint index_three,
                           uint index_four,
                           int multiplicity,
                           Real phase,
                           Real force_constant);

  uint AddHarmonicImproper(uint index_one,
                           uint index_two,
                           uint index_three,
                           uint index_four,
                           Real angle,
                           Real force_constant);

  uint AddCMap(uint index_one,
               uint index_two,
               uint index_three,
               uint index_four,
               uint index_five,
               int dimension,
               std::vector<Real> values);

  uint AddLJPair(uint index_one, 
                 uint index_two,
                 Real sigma,
                 Real epsilon);


  uint AddDistanceConstraint(uint index_one, 
                             uint index_two,
                             Real distance);

  uint AddExclusion(uint index_one,
                    uint index_two);

  void AddPositionConstraint(uint index);

  void ResetPositionConstraints() { position_constraints_.clear(); }

  void ResetExclusions() { exclusions_.clear(); }

  uint AddHarmonicPositionRestraint(uint index, const geom::Vec3& ref_position, Real k, 
                                    Real x_scale = 1.0, Real y_scale = 1.0, Real z_scale = 1.0);

  uint AddHarmonicDistanceRestraint(uint index_one, uint index_two, 
                                    Real length, Real force_constant);

  //Single atom parameters are expected to be set at once...
  void SetSigmas(const std::vector<Real>& sigmas);

  void SetSigma(uint index, Real sigma);

  void SetEpsilons(const std::vector<Real>& epsilons);

  void SetEpsilon(uint index, Real epsilon);

  void SetGBSARadii(const std::vector<Real>& gbsa_radii);

  void SetGBSARadius(uint index, Real radius);

  void SetOBCScalings(const std::vector<Real>& obc_scaling);

  void SetOBCScaling(uint index, Real scaling);

  void SetCharges(const std::vector<Real>& charges);

  void SetCharge(uint index, Real charge);

  void SetMasses(const std::vector<Real>& masses);

  void SetMass(uint index, Real mass);

  void SetFudgeQQ(Real fudge) { fudge_qq_ = fudge; }

  void SetFudgeLJ(Real fudge) { fudge_lj_ = fudge;} 


  void GetHarmonicBondParameters(uint index, uint& index_one, uint& index_two,
                                 Real& bond_length, Real& force_constant) const;

  void GetHarmonicAngleParameters(uint index, uint& index_one, uint& index_two, uint& index_three,
                                  Real& angle, Real& force_constant) const;

  void GetUreyBradleyAngleParameters(uint index, uint& index_one, uint& index_two, uint& index_three,
                                     Real& angle, Real& angle_force_constant, Real& bond_length, Real& bond_force_constant) const;

  void GetPeriodicDihedralParameters(uint index, uint& index_one, uint& index_two, uint& index_three, uint& index_four,
                                     int& multiplicity, Real& phase, Real& force_constant) const;

  void GetPeriodicImproperParameters(uint index, uint& index_one, uint& index_two, uint& index_three, uint& index_four,
                                     int& multiplicity, Real& phase, Real& force_constant) const;

  void GetHarmonicImproperParameters(uint index, uint& index_one, uint& index_two, uint& index_three, uint& index_four,
                                     Real& angle, Real& force_constant) const;

  void GetCMapParameters(uint index, uint& index_one, uint& index_two, uint& index_three, uint& index_four, uint& index_five,
                         int& dimension, std::vector<Real>& map) const;

  void GetLJPairParameters(uint index, uint& index_one, uint& index_two,
                           Real& sigma, Real& epsilon) const;

  void GetDistanceConstraintParameters(uint index, uint& index_one, uint& index_two,
                                       Real& distance) const;

  void GetHarmonicPositionRestraintParameters(uint index, uint& atom_index, geom::Vec3& ref_position,
                                               Real& k, Real& x_scale, Real& y_scale, Real& z_scale) const;

  void GetHarmonicDistanceRestraintParameters(uint index, uint& atom_one, uint& atom_two, Real& length,
                                               Real& force_constant) const;

  void SetHarmonicBondParameters(uint index, const Real& bond_length, const Real& force_constant);

  void SetHarmonicAngleParameters(uint index, const Real& angle, const Real& force_constant);

  void SetUreyBradleyAngleParameters(uint index, const Real& angle, const Real& angle_force_constant, const Real& bond_length, const Real& bond_force_constant);

  void SetPeriodicDihedralParameters(uint index, const int& multiplicity, const Real& phase, const Real& force_constant);

  void SetPeriodicImproperParameters(uint index, const int& multiplicity, const Real& phase, const Real& force_constant);

  void SetHarmonicImproperParameters(uint index, const Real& angle, const Real& force_constant);

  void SetCMapParameters(uint index, const int& dimension, const std::vector<Real>& map);

  void SetLJPairParameters(uint index, const Real& sigma, const Real& epsilon);

  void SetDistanceConstraintParameters(uint index, const Real& distance);

  void SetHarmonicPositionRestraintParameters(uint index, const geom::Vec3& ref_position, Real k, 
                                               Real x_scale = 1.0, Real y_scale = 1.0, Real z_scale = 1.0);

  void SetHarmonicDistanceRestraintParameters(uint index, Real length, Real force_constant);

  ost::mol::EntityHandle GetEntity() const { return ent_; };

  uint GetAtomIndex(const ost::mol::AtomHandle& at) const;

  uint GetAtomIndex(uint residue_index, const String& atom_name) const;

  uint GetResidueIndex(const ost::mol::ResidueHandle& res) const;

  const std::vector<std::pair<Index<2>, std::vector<Real> > >& GetHarmonicBonds() const { return harmonic_bonds_; }
 
  const std::vector<std::pair<Index<3>, std::vector<Real> > >& GetHarmonicAngles() const { return harmonic_angles_; }
 
  const std::vector<std::pair<Index<3>, std::vector<Real> > >& GetUreyBradleyAngles() const { return urey_bradley_angles_; }
 
  const std::vector<std::pair<Index<4>, std::vector<Real> > >& GetPeriodicDihedrals() const { return periodic_dihedrals_; }
 
  const std::vector<std::pair<Index<4>, std::vector<Real> > >& GetPeriodicImpropers() const { return periodic_impropers_; }
 
  const std::vector<std::pair<Index<4>, std::vector<Real> > >& GetHarmonicImpropers() const { return harmonic_impropers_; }
 
  const std::vector<std::pair<Index<5>, std::vector<Real> > >& GetCMaps() const { return cmaps_; }
 
  const std::vector<std::pair<Index<2>, std::vector<Real> > >& GetLJPairs() const { return lj_pairs_; }
 
  const std::vector<std::pair<Index<2>, std::vector<Real> > >& GetDistanceConstraints() const { return distance_constraints_; }

  const std::vector<std::pair<Index<1>, std::vector<Real> > >&GetHarmonicPositionRestraints() const { return harmonic_position_restraints_; }
 
  const std::vector<std::pair<Index<2>, std::vector<Real> > >&GetHarmonicDistanceRestraints() const{ return harmonic_distance_restraints_; }

  const std::vector<Index<2> >& GetExclusions() const { return exclusions_; }
 
  const std::vector<uint>& GetPositionConstraints() const { return position_constraints_; }

  std::vector<Real> GetSigmas() const { return sigmas_; }

  std::vector<Real> GetEpsilons() const { return epsilons_; }
 
  std::vector<Real> GetGBSARadii() const { return gbsa_radii_; }

  std::vector<Real> GetOBCScalings() const { return obc_scaling_; }
 
  std::vector<Real> GetCharges() const { return charges_; }

  Real GetCharge(uint index) const;

  Real GetMass(uint index) const;

  Real GetSigma(uint index) const;

  Real GetEpsilon(uint index) const;

  Real GetGBSARadius(uint index) const;

  Real GetOBCScaling(uint index) const;

  std::vector<Real> GetMasses() const { return atom_masses_; }

  Real GetFudgeQQ() const { return fudge_qq_; }

  Real GetFudgeLJ() const { return fudge_lj_; }

  std::vector<uint> GetHarmonicBondIndices(uint index_one,
                                            uint index_two) const;

  std::vector<uint> GetHarmonicAngleIndices(uint index_one,
                                             uint index_two,
                                             uint index_three) const;

  std::vector<uint> GetUreyBradleyAngleIndices(uint index_one,
                                                uint index_two,
                                                uint index_three) const;

  std::vector<uint> GetPeriodicDihedralIndices(uint index_one,
                                                uint index_two,
                                                uint index_three,
                                                uint index_four) const;

  std::vector<uint> GetPeriodicImproperIndices(uint index_one,
                                                uint index_two,
                                                uint index_three,
                                                uint index_four) const;

  std::vector<uint> GetHarmonicImproperIndices(uint index_one,
                                                uint index_two,
                                                uint index_three,
                                                uint index_four) const;

  std::vector<uint> GetCMapIndices(uint index_one,
                                    uint index_two,
                                    uint index_three,
                                    uint index_four,
                                    uint index_five) const;

  int GetLJPairIndex(uint index_one,
                     uint index_two) const;

  int GetDistanceConstraintIndex(uint index_one,
                                 uint index_two) const;



  std::vector<uint> GetHarmonicDistanceRestraintIndices(uint index_one,
                                                        uint index_two) const;


  std::vector<uint> GetHarmonicBondIndices(uint atom_index) const;

  std::vector<uint> GetHarmonicAngleIndices(uint atom_index) const;

  std::vector<uint> GetUreyBradleyAngleIndices(uint atom_index) const;

  std::vector<uint> GetPeriodicDihedralIndices(uint atom_index) const;

  std::vector<uint> GetPeriodicImproperIndices(uint atom_index) const;

  std::vector<uint> GetHarmonicImproperIndices(uint atom_index) const;

  std::vector<uint> GetCMapIndices(uint atom_index) const;

  std::vector<uint> GetLJPairIndices(uint atom_index) const;

  std::vector<uint> GetDistanceConstraintIndices(uint atom_index) const;

  std::vector<uint> GetHarmonicPositionRestraintIndices(uint atom_index) const;

  std::vector<uint> GetHarmonicDistanceRestraintIndices(uint atom_index) const;

  uint GetNumAtoms() { return num_atoms_; }

  uint GetNumResidues() { return num_residues_; }

  uint GetNumHarmonicBonds() { return harmonic_bonds_.size(); }

  uint GetNumHarmonicAngles() { return harmonic_angles_.size(); }

  uint GetNumUreyBradleyAngles() { return urey_bradley_angles_.size(); }

  uint GetNumPeriodicDihedrals() { return periodic_dihedrals_.size(); }

  uint GetNumPeriodicImpropers() { return periodic_impropers_.size(); }

  uint GetNumHarmonicImpropers() { return harmonic_impropers_.size(); }

  uint GetNumCMaps() { return cmaps_.size(); }

  uint GetNumLJPairs() { return lj_pairs_.size(); }

  uint GetNumDistanceConstraints() { return distance_constraints_.size(); }

  uint GetNumPositionConstraints() { return position_constraints_.size(); }

  uint GetNumHarmonicPositionRestraints() { return harmonic_position_restraints_.size(); }

  uint GetNumHarmonicDistanceRestraints() { return harmonic_distance_restraints_.size();}

  uint GetNumExclusions() { return exclusions_.size(); }

  void Merge(TopologyPtr p);

  void SetEntityPositions(const geom::Vec3List& positions);

private:

  ost::mol::EntityHandle ent_;
  ost::mol::AtomHandleList atom_list_;
  ost::mol::ResidueHandleList res_list_;

  uint num_atoms_;
  uint num_residues_;

  std::map<long,uint> atom_index_mapper_;
  std::map<long,uint> residue_index_mapper_;
  std::vector<std::map<String,uint> > atom_name_mapper_;

  //fudge parameters for lj 1,4 pairs
  Real fudge_qq_;
  Real fudge_lj_;

  //single atom parameters
  std::vector<Real> atom_masses_;
  std::vector<Real> sigmas_;
  std::vector<Real> epsilons_;
  std::vector<Real> gbsa_radii_;
  std::vector<Real> obc_scaling_;
  std::vector<Real> charges_;
  std::vector<uint> position_constraints_;

  //interactions with multiple atoms involved
  std::vector<std::pair<Index<2>,std::vector<Real> > > harmonic_bonds_;
  std::vector<std::pair<Index<3>,std::vector<Real> > > harmonic_angles_;
  std::vector<std::pair<Index<3>,std::vector<Real> > > urey_bradley_angles_;
  std::vector<std::pair<Index<4>,std::vector<Real> > > periodic_dihedrals_;
  std::vector<std::pair<Index<4>,std::vector<Real> > > periodic_impropers_;
  std::vector<std::pair<Index<4>,std::vector<Real> > > harmonic_impropers_;
  std::vector<std::pair<Index<5>,std::vector<Real> > > cmaps_;
  std::vector<std::pair<Index<2>,std::vector<Real> > > lj_pairs_;
  std::vector<std::pair<Index<2>,std::vector<Real> > > distance_constraints_;
  std::vector<Index<2> > exclusions_;
  std::vector<std::pair<Index<1>,std::vector<Real> > > harmonic_position_restraints_;
  std::vector<std::pair<Index<2>,std::vector<Real> > > harmonic_distance_restraints_;

  //the atoms of the interactions, that should be unique get tracked in here
  //note, that this is waste of memory, needs better implementation
  std::set<Index<2> > added_lj_pairs_;
  std::set<Index<2> > added_distance_constraints_;
  std::set<Index<2> > added_exclusions_;

};


}}} //ns

#endif
