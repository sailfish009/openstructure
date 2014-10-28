#ifndef OST_SIMULATION_HH
#define OST_SIMULATION_HH

#include <OpenMM.h>

#include <vector>
#include <map>

#include <ost/mol/mol.hh>
#include <ost/mol/mm/system_creator.hh>
#include <ost/mol/mm/topology.hh>
#include <ost/mol/mm/topology_creator.hh>
#include <ost/mol/mm/mm_settings.hh>
#include <ost/mol/mm/mm_modeller.hh>
#include <ost/geom/vec3.hh>
#include <ost/mol/mm/mm_observer.hh>
#include <ost/mol/mm/state_extractor.hh>
#include <ost/mol/mm/steep.hh>
#include <ost/mol/mm/mm_interaction.hh>


#include <time.h>

namespace ost { namespace mol{ namespace mm{

class Simulation;

typedef boost::shared_ptr<OpenMM::Integrator> IntegratorPtr;
typedef boost::shared_ptr<OpenMM::VerletIntegrator> VerletIntegratorPtr;
typedef boost::shared_ptr<OpenMM::BrownianIntegrator> BrownianIntegratorPtr;
typedef boost::shared_ptr<OpenMM::LangevinIntegrator> LangevinIntegratorPtr;
typedef boost::shared_ptr<OpenMM::VariableVerletIntegrator> VariableVerletIntegratorPtr;
typedef boost::shared_ptr<OpenMM::VariableLangevinIntegrator> VariableLangevinIntegratorPtr;
typedef boost::shared_ptr<OpenMM::Context> ContextPtr;
typedef boost::shared_ptr<OpenMM::AndersenThermostat> AndersenThermostatPtr;
typedef boost::shared_ptr<OpenMM::MonteCarloBarostat> MonteCarloBarostatPtr;

typedef boost::shared_ptr<Simulation> SimulationPtr;

class Simulation {

public:

  Simulation(const ost::mol::EntityHandle& handle, 
             const MMSettingsPtr settings);

  Simulation(const ost::mol::mm::TopologyPtr top,
             const MMSettingsPtr settings);


  ost::mol::EntityHandle GetEntity() { return top_->GetEntity(); }

  geom::Vec3List GetPositions(bool enforce_periodic_box = false, bool in_angstrom = true);

  geom::Vec3List GetVelocities();

  geom::Vec3List GetForces();

  void SetPositions(geom::Vec3List& positions, bool in_angstrom = true);

  void SetVelocities(geom::Vec3List& velocities);

  void UpdateTopologyPositions(bool enforce_periodic_box = false);

  void MinimizeEnergy(const String& type = "steep", Real tolerance = 1.0, int max_iterations = 1000);

  Real GetEnergy();

  Real GetPotentialEnergy();

  Real GetKineticEnergy();

  TopologyPtr GetTopology() { return top_; }

  void Steps(int steps);

  void Register(MMObserverPtr o);

  void ResetHarmonicBond(uint index, Real bond_length, Real force_constant);

  void ResetHarmonicAngle(uint index, Real angle, Real force_constant);

  void ResetUreyBradleyAngle(uint index, Real angle, Real angle_force_constant, Real bond_length, Real bond_force_constant);

  void ResetPeriodicDihedral(uint index, int multiplicity, Real phase, Real force_constant);

  void ResetPeriodicImproper(uint index, int multiplicity, Real phase, Real force_constant);

  void ResetHarmonicImproper(uint index, Real phase, Real force_constant);

  void ResetLJPair(uint index, Real sigma, Real epsilon);

  void ResetDistanceConstraint(uint index, Real constraint_length);

  void ResetHarmonicPositionRestraint(uint index, const geom::Vec3& ref_position, Real k,
                                      Real x_scale = 1.0, Real y_scale = 1.0, Real z_scale = 1.0);

  void ResetHarmonicDistanceRestraint(uint index, Real length, Real force_constant);

  void ResetLJ(uint index, Real sigma, Real epsilon);

  void ResetGBSA(uint index, Real radius, Real scaling);

  void ResetCharge(uint index, Real charge);

  void ResetMass(uint index, Real mass);

  void AddPositionConstraint(uint index);

  void AddPositionConstraints(const std::vector<uint>& index);

  void ResetPositionConstraints();

  geom::Vec3 GetPeriodicBoxExtents();

  void SetPeriodicBoxExtents(geom::Vec3& vec);

private:
  void Init(const ost::mol::mm::TopologyPtr top, 
            const MMSettingsPtr settings);

  int TimeToNextNotification();

  SystemPtr system_;
  IntegratorPtr integrator_;
  ContextPtr context_;
  TopologyPtr top_;
  std::vector<MMObserverPtr> observers_;
  std::vector<int> time_to_notify_;
  std::map<FuncType,unsigned int> system_force_mapper_;
  std::vector<Real> original_masses_;
};

}}} //ns

#endif
