#ifndef OST_MM_SETTINGS_HH
#define OST_MM_SETTINGS_HH

#include <vector>

#include <boost/shared_ptr.hpp>
#include <limits>
#include <ost/mol/residue_handle.hh>
#include <ost/mol/mm/forcefield.hh>

namespace OpenMM{
  class Integrator;//hacky way of telling the Integrator us around
                   //will be included in source file to avoid
                   //dependencies on external libraries
}

namespace ost { namespace mol{ namespace mm{

struct MMSettings;
class TerminiExceptions;
typedef boost::shared_ptr<MMSettings> MMSettingsPtr;
typedef boost::shared_ptr<TerminiExceptions> TerminiExceptionsPtr;

enum Platform{
  Reference,
  OpenCL,
  CUDA,
  CPU
};

enum NonbondedMethod{
  NoCutoff,
  CutoffNonPeriodic,
  CutoffPeriodic,
  Ewald,
  PME
};

class TerminiExceptions{

public:

  TerminiExceptions(){ }

  void SetException(const ost::mol::ResidueHandle& res, const String& exception_name){
    exceptions_[res.GetQualifiedName()] =  exception_name;
  }

  bool HasException(const ost::mol::ResidueHandle& res) const{
    return exceptions_.find(res.GetQualifiedName()) != exceptions_.end();
  }

  String GetException(const ost::mol::ResidueHandle& res) const {
    if(!this->HasException(res)){
      std::stringstream ss;
      ss<<"Tried to get termini exceptions of res "<<res<<" without defined exception!";
      throw ost::Error(ss.str());
    }
    std::map<String, String>::const_iterator i = exceptions_.find(res.GetQualifiedName());
    return i->second;
  }
private:
  std::map<String, String> exceptions_;

};

struct MMSettings{

  MMSettings(): add_bonds(true),
                add_angles(true),
                add_dihedrals(true),
                add_impropers(true),
                add_cmaps(true),
                add_exclusions(true),
                add_nonbonded(true),
                add_gbsa(false),
                constrain_hbonds(false),
                constrain_bonds(false),
                rigid_water(false),
                strict_interactions(true),
                ideal_bond_length_constraints(true),
                fix_heavy_atoms(false),
                kill_electrostatics(false),
                generate_disulfid_bonds(true),
                nonbonded_method(NoCutoff),
                nonbonded_cutoff(10.0),
                remove_cmm_motion(true),
                cmm_frequency(1),
                periodic_box_extents(0,0,0),
                init_temperature(0.0),
                forcefield(), //note, that we don't create an empty forcefield at this point =>force the user
                              //to assign a forcefield
                termini_exceptions(new TerminiExceptions),
                platform(Reference),
                add_thermostat(false),
                thermostat_temperature(std::numeric_limits<Real>::quiet_NaN()),
                thermostat_collision_frequency(std::numeric_limits<Real>::quiet_NaN()),
                add_barostat(false),
                barostat_temperature(std::numeric_limits<Real>::quiet_NaN()),
                barostat_pressure(std::numeric_limits<Real>::quiet_NaN()),
                barostat_frequency(25),
                integrator(),
                solvent_dielectric(78.3), //this is for GBSA
                solute_dielectric(1.0), //this is for GBSA
                reaction_field_dielectric(78.3),
                use_dispersion_correction(true),
                openmm_plugin_directory("/usr/local/openmm/lib/plugins"),
                custom_plugin_directory("/usr/local/openmm/lib/plugins")

                  {   }

  //
  bool add_bonds;
  bool add_angles;
  bool add_dihedrals;
  bool add_impropers;
  bool add_cmaps;
  bool add_exclusions;
  bool add_nonbonded;
  bool add_gbsa;
  bool constrain_hbonds;
  bool constrain_bonds;
  bool rigid_water;
  bool strict_interactions;
  bool ideal_bond_length_constraints;
  bool fix_heavy_atoms;
  //set all charges to zero
  bool kill_electrostatics;
  //automatically generate disulfid bonds
  bool generate_disulfid_bonds;
  //see export_openmm.cc for all possibilities
  NonbondedMethod nonbonded_method;
  Real nonbonded_cutoff;
  bool remove_cmm_motion;
  int cmm_frequency;
  //currently only boxes defined by base vectors are supported 
  geom::Vec3 periodic_box_extents;
  //at the beginning of the simulation, the initial velociities are
  //randomly set according to a boltzmann distribution
  Real init_temperature;
  ForcefieldPtr forcefield;
  TerminiExceptionsPtr termini_exceptions;
  Platform platform;
  bool add_thermostat;
  Real thermostat_temperature;
  Real thermostat_collision_frequency;
  bool add_barostat;
  Real barostat_temperature;
  Real barostat_pressure;
  int barostat_frequency;
  boost::shared_ptr<OpenMM::Integrator> integrator;
  Real solvent_dielectric;
  Real solute_dielectric;
  Real reaction_field_dielectric;
  bool use_dispersion_correction;
  String openmm_plugin_directory;
  String custom_plugin_directory;
};

}}} //namespace

#endif