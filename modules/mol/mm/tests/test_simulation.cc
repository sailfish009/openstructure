#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <ost/mol/mm/mm_settings.hh>
#include <ost/mol/mm/forcefield.hh>
#include <ost/mol/mm/mm_interaction.hh>
#include <ost/mol/mm/buildingblock.hh>
#include <ost/mol/mm/gromacs_block_modifiers.hh>
#include <ost/message.hh>
#include <ost/mol/builder.hh>
#include <ost/mol/xcs_editor.hh>
#include <ost/io/mol/pdb_reader.hh>
#include <ost/mol/mm/simulation.hh>
#include <ost/mol/mm/topology.hh>


using namespace ost::mol::mm;

BOOST_AUTO_TEST_SUITE( mol_mm );

BOOST_AUTO_TEST_CASE(test_simulation_basics){

  String pdb_name = "1CRN.pdb";

  ost::io::PDBReader reader(pdb_name, ost::io::IOProfile());
  ost::mol::EntityHandle test_ent = ost::mol::CreateEntity();
  reader.Import(test_ent);

  ost::conop::ProcessorPtr processor(new ost::conop::HeuristicProcessor);
  processor->Process(test_ent);

  MMSettingsPtr settings(new MMSettings);
  ForcefieldPtr forcefield = Forcefield::Load("CHARMM27.dat");
  settings->forcefield = forcefield;
  settings->add_gbsa = true;

  TopologyPtr top = TopologyCreator::Create(test_ent,settings);
  //lets add interactions, that are not set when using charmm
  top->AddHarmonicAngle(1,2,3,10.0,10.0);
  top->AddPeriodicImproper(1,2,3,4,2,10.0,10.0);
  top->AddDistanceConstraint(1,2,10.0);
  top->AddHarmonicPositionRestraint(1,geom::Vec3(0.0,0.0,0.0),10.0);
  top->AddHarmonicDistanceRestraint(1,2,10.0,10.0);

  //try to set up simulation without an integrator
  BOOST_CHECK_THROW(Simulation sim(test_ent,settings),ost::Error);

  settings->integrator = IntegratorPtr(new OpenMM::VerletIntegrator(0.002));

  Simulation sim(top, test_ent, settings);


  //we check, wether the reset functions have the desired effect on the topology
  //we cannot really check the effect on the openmm system...
  sim.ResetHarmonicBond(42,5.0,6.0);
  sim.ResetHarmonicAngle(0,8.0,9.0);
  sim.ResetUreyBradleyAngle(42,5.0,6.0,7.0,8.0);
  sim.ResetPeriodicDihedral(42,4,5.0,6.0);
  sim.ResetPeriodicImproper(0,5,8.0,9.0);
  sim.ResetHarmonicImproper(42,5.0,6.0);
  sim.ResetLJPair(42,5.0,6.0);
  sim.ResetDistanceConstraint(0,10.0);
  sim.ResetHarmonicPositionRestraint(0,geom::Vec3(1.0,1.0,1.0),5.0);
  sim.ResetHarmonicDistanceRestraint(0,5.0,10.0);
  sim.ResetLJ(42,2.0,3.0);
  sim.ResetGBSA(42,5.0,6.0);
  sim.ResetCharge(42,100.0);
  sim.ResetMass(42,200.0);

  uint ui1,ui2,ui3,ui4;
  int i1;
  Real r1,r2,r3,r4;
  geom::Vec3 pos;


  top->GetHarmonicBondParameters(42,ui1,ui2,r1,r2);
  BOOST_CHECK(r1 == 5.0 && r2 == 6.0);

  top->GetHarmonicAngleParameters(0,ui1,ui2,ui3,r1,r2);
  BOOST_CHECK(r1 == 8.0 && r2 == 9.0);

  top->GetUreyBradleyAngleParameters(42,ui1,ui2,ui3,r1,r2,r3,r4);
  BOOST_CHECK(r1 == 5.0 && r2 == 6.0 && r3 == 7.0 && r4 == 8.0);

  top->GetPeriodicDihedralParameters(42,ui1,ui2,ui3,ui4,i1,r1,r2);
  BOOST_CHECK(i1 == 4 && r1 == 5.0 && r2 == 6.0);

  top->GetPeriodicImproperParameters(0,ui1,ui2,ui3,ui4,i1,r1,r2);
  BOOST_CHECK(i1 == 5 && r1 == 8.0 && r2 == 9.0);

  top->GetLJPairParameters(42,ui1,ui2,r1,r2);
  BOOST_CHECK(r1 == 5.0 && r2 == 6.0);

  top->GetDistanceConstraintParameters(0,ui1,ui2,r1);
  BOOST_CHECK(r1 == 10.0);

  top->GetHarmonicPositionRestraintParameters(0, ui1, pos,r1, r2, r3, r3);
  BOOST_CHECK(pos == geom::Vec3(1.0,1.0,1.0) && r1 == 5.0);

  top->GetHarmonicDistanceRestraintParameters(0,ui1,ui2, r1, r2);
  BOOST_CHECK(r1 == 5.0 && r2 == 10.0);

  r1 = top->GetSigma(42);
  r2 = top->GetEpsilon(42);
  BOOST_CHECK(r1 == 2.0 && r2 == 3.0);

  r1 = top->GetGBSARadius(42);
  r2 = top->GetOBCScaling(42);
  BOOST_CHECK(r1 == 5.0 && r2 == 6.0);

  r1 = top->GetCharge(42);
  r2 = top->GetMass(42);
  BOOST_CHECK(r1 == 100.0 && r2 == 200.0);
}


BOOST_AUTO_TEST_CASE(test_simulation_energy_calculations){


  String pdb_name = "1CRN.pdb";

  ost::io::PDBReader reader(pdb_name, ost::io::IOProfile());
  ost::mol::EntityHandle test_ent = ost::mol::CreateEntity();
  reader.Import(test_ent);

  ost::conop::ProcessorPtr processor(new ost::conop::HeuristicProcessor);
  processor->Process(test_ent);

  MMSettingsPtr settings(new MMSettings);
  ForcefieldPtr forcefield = Forcefield::Load("CHARMM27.dat");
  settings->forcefield = forcefield;

  TopologyPtr top = TopologyCreator::Create(test_ent,settings);
  //lets add interactions, that are not set when using charmm

  settings->integrator = IntegratorPtr(new OpenMM::VerletIntegrator(0.002));

  Simulation sim(top, test_ent,settings);

  sim.MinimizeEnergy("steep",1.0,200);
  sim.UpdatePositions();

  ost::mol::EntityHandle minimized_ent = sim.GetEntity();

  MMSettingsPtr harmonic_bond_settings(new MMSettings);
  harmonic_bond_settings->add_angles = false;
  harmonic_bond_settings->add_dihedrals = false;
  harmonic_bond_settings->add_impropers = false;
  harmonic_bond_settings->add_nonbonded = false;
  harmonic_bond_settings->add_cmaps = false;
  harmonic_bond_settings->integrator = IntegratorPtr(new OpenMM::VerletIntegrator(0.002));
  harmonic_bond_settings->forcefield = forcefield;

  MMSettingsPtr urey_bradley_angle_settings(new MMSettings);
  urey_bradley_angle_settings->add_bonds = false;
  urey_bradley_angle_settings->add_dihedrals = false;
  urey_bradley_angle_settings->add_impropers = false;
  urey_bradley_angle_settings->add_nonbonded = false;
  urey_bradley_angle_settings->add_cmaps = false;
  urey_bradley_angle_settings->integrator = IntegratorPtr(new OpenMM::VerletIntegrator(0.002));
  urey_bradley_angle_settings->forcefield = forcefield;

  MMSettingsPtr periodic_dihedral_settings(new MMSettings);
  periodic_dihedral_settings->add_bonds = false;
  periodic_dihedral_settings->add_angles = false;
  periodic_dihedral_settings->add_impropers = false;
  periodic_dihedral_settings->add_nonbonded = false;
  periodic_dihedral_settings->add_cmaps = false;
  periodic_dihedral_settings->integrator = IntegratorPtr(new OpenMM::VerletIntegrator(0.002));
  periodic_dihedral_settings->forcefield = forcefield;

  MMSettingsPtr harmonic_improper_settings(new MMSettings);
  harmonic_improper_settings->add_bonds = false;
  harmonic_improper_settings->add_angles = false;
  harmonic_improper_settings->add_dihedrals = false;
  harmonic_improper_settings->add_nonbonded = false;
  harmonic_improper_settings->add_cmaps = false;
  harmonic_improper_settings->integrator = IntegratorPtr(new OpenMM::VerletIntegrator(0.002));
  harmonic_improper_settings->forcefield = forcefield;

  MMSettingsPtr cmap_settings(new MMSettings);
  cmap_settings->add_bonds = false;
  cmap_settings->add_angles = false;
  cmap_settings->add_dihedrals = false;
  cmap_settings->add_nonbonded = false;
  cmap_settings->add_impropers = false;
  cmap_settings->integrator = IntegratorPtr(new OpenMM::VerletIntegrator(0.002));
  cmap_settings->forcefield = forcefield;

  MMSettingsPtr nonbonded_settings(new MMSettings);
  nonbonded_settings->add_bonds = false;
  nonbonded_settings->add_angles = false;
  nonbonded_settings->add_dihedrals = false;
  nonbonded_settings->add_cmaps = false;
  nonbonded_settings->add_impropers = false;
  nonbonded_settings->integrator = IntegratorPtr(new OpenMM::VerletIntegrator(0.002));
  nonbonded_settings->forcefield = forcefield;

  Simulation sim_harmonic_bond(minimized_ent,harmonic_bond_settings);
  Simulation sim_urey_bradley_angle(minimized_ent,urey_bradley_angle_settings);
  Simulation sim_periodic_dihedral(minimized_ent,periodic_dihedral_settings);
  Simulation sim_harmonic_improper(minimized_ent,harmonic_improper_settings);
  Simulation sim_cmap(minimized_ent,cmap_settings);
  Simulation sim_nonbonded(minimized_ent, nonbonded_settings);

  Real e_harmonic_bond = sim_harmonic_bond.GetPotentialEnergy();
  Real e_urey_bradley_angle = sim_urey_bradley_angle.GetPotentialEnergy();
  Real e_periodic_dihedral = sim_periodic_dihedral.GetPotentialEnergy();
  Real e_harmonic_improper = sim_harmonic_improper.GetPotentialEnergy();
  Real e_cmap = sim_cmap.GetPotentialEnergy();
  Real e_nonbonded = sim_nonbonded.GetPotentialEnergy();


  //the energy values have been compared with the results from the
  //CHARMM27 forcefield in gromacs

  //differenecs in energy(kJ/mol):
  //harmonic_bond:      0.00021344800001
  //urey_bradley_angle: 0.00021319300003
  //periodic_dihedral:  0.00018275799993
  //harmonic_improper:  0.00001410630000
  //cmap:               0.00192255100001
  //nonbonded:          0.00204422999968

  //You'll most likely get different results when repeating the comparison with gromacs
  //this is because of the precision.
  //Right now we have full real precision in the positions.
  //When you write down the pdb, precision will be lowered to 3 digits.
  //Another digit gets lost, when you create a gromacs topology...
  //Gromacs writes down the positions using nm instead of A, but
  //also has a precision of 3 digits.

  BOOST_CHECK_CLOSE(e_harmonic_bond, 150.238, Real(1e-3));
  BOOST_CHECK_CLOSE(e_urey_bradley_angle, 499.690, Real(1e-3));
  BOOST_CHECK_CLOSE(e_periodic_dihedral, 879.744, Real(1e-3));
  BOOST_CHECK_CLOSE(e_harmonic_improper, 15.7698, Real(1e-3));
  BOOST_CHECK_CLOSE(e_cmap, -303.904, Real(1e-3));
  BOOST_CHECK_CLOSE(e_nonbonded, -3626.539, Real(1e-3));
}



BOOST_AUTO_TEST_SUITE_END();