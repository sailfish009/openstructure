The Molecular Mechanis Settings
================================================================================

.. currentmodule:: ost.mol

The :class:`Settings` define all parameters to control the buildup of a 
:class:`Topology` in the :class:`TopologyCreator` and the final setup
of the :class:`Simulation` object.

.. class:: Settings

  .. attribute::        add_bonds  

    Flag, whether bonds should be parametrized
    in :class:`TopologyCreator`. default: True

  .. attribute::        add_angles 

    Flag, whether angles should be parametrized
    in :class:`TopologyCreator`. default: True

  .. attribute::        add_dihedrals 

    Flag, whether dihedrals should be parametrized
    in :class:`TopologyCreator`. default: True

  .. attribute::        add_impropers 

    Flag, whether impropers should be parametrized
    in :class:`TopologyCreator`. default: True

  .. attribute::        add_cmaps 

    Flag, whether cmaps should be parametrized
    in :class:`TopologyCreator`. default: True

  .. attribute::        add_exclusions 

    Flag, whether exclusions should be parametrized
    in :class:`TopologyCreator`. default: True

  .. attribute::        add_nonbonded

    Flag, whether nonbonded interactions should be parametrized
    in :class:`TopologyCreator`. default: True

  .. attribute::        add_gbsa 

    Flag, whether GBSA interactions should be parametrized
    in :class:`TopologyCreator`. default: False

  .. attribute::        constrain_hbonds

    Flag, whether bonds involving hydrogens should be constraint
    in :class:`TopologyCreator`. default: False

  .. attribute::        constrain_bonds

    Flag, whether all bonds should be constraint in 
    :class:`TopologyCreator`. default: False

  .. attribute::        rigid_water

    Flag, whether water molecules should be made rigid in
    :class:`TopologyCreator`. This is achieved by adding
    a distance constraint on the the O-H bonds and an additional
    one between the two hydrogens. default: False

  .. attribute::        strict_interactions

    Makes sure, that an error is thrown when a particular interaction cannot
    be parametrized with the given forcefield in the :class:`TopologyCreator`. 
    By setting it to False, these cases get just ignored. default: True

  .. attribute::        ideal_bond_length_constraints

    Flag, whether the ideal bond length from the :class:`Forcefield` should be
    taken for distance constraints in the :class:`TopologyCreator`. 
    :class:`Forcefield` should be taken. The actual distances from the 
    :class:`EntityHandle` get taken otherwise. default: True

  .. attribute::        fix_heavy_atoms

    Flag, whether all heavy atom positions (non hydrogens) should be fixed in
    space in the :class:`TopologyCreator`. default: False

  .. attribute::        kill_electrostatics

    Flag, whether all charges should just be set to zero in the
    :class:`TopologyCreator`. default: False

  .. attribute::        generate_disulfid_bonds

    Flag, whether disulfid bonds based on a simple geometric criterion 
    (SG-SG dist < 2.5A) should be built in :class:`TopologyCreator`. 
    default: True

  .. attribute::        nonbonded_method

    Nonbonded method set up at the creation of the :class:`Simulation`.
    must be one of mm.NoCutoff, mm.CutoffNonPeriodic, mm.CutoffPeriodic
    mm.Ewald or mm.PME. default: mm.NoCutoff

  .. attribute::        nonbonded_cutoff

    Nonbonded cutoff set when simulation is set up. default: 10.0

  .. attribute::        remove_cmm_motion

    Whether a center of mass remover object should be attached to the
    :class:`Simulation`. default: True

  .. attribute::        cmm_frequency

    Frequency regarding simulationsteps when the cmm remover should be applied.
    default: 1

  .. attribute::        periodic_box_extent

    :class:`ost.geom.Vec3` describing the X,Y and Z extents of the rectangular
    simulation_box defined when setting up the :class:`Simulation`. 
    default: (0,0,0)

  .. attribute::        init_temperature

    Initial simulation velocities get set according to a Boltzman distribution
    controlled by init_temperature(Kelvin). default: 0.0

  .. attribute::        forcefield

    :class:`Forcefield` that is used in :class:`TopologyCreator`. default: None

  .. attribute::        termini_exceptions

    Use termini other than the defaults set in the :class:`Forcefield` when
    using :class:`TopologyCreator`. Has to be a :class:`TerminiExceptions`
    object. default: None

  .. attribute::        platform

    Platform used by OpenMM to do the calculations. Must be one of mm.Reference,
    mm.CPU, mm.CUDA or mm.OpenCL. If anything else than the reference platform 
    is used, the attribute openmm_plugin_directory has to be set accordingly.
    default: mm.Reference

  .. attribute::        add_thermostat

    Flag, whether an Andersen thermostat should be attached when settings up
    the :class:`Simulation`. default: False

  .. attribute::        thermostat_temperature

    Temperature for the Andersen thermostat. default: NaN

  .. attribute::        thermostat_collision_frequency

    Collision frequency of the Andersen thermostat. default: NaN

  .. attribute::        add_barostat

    Flag, whether an  MonteCarlo barostat should be attached when settings up
    the :class:`Simulation`. default: False

  .. attribute::        barostat_temperature

    Temperature for the MonteCarlo Barostat. default: NaN

  .. attribute::        barostat_pressure

    Pressure for the MonteCarlo Barostat. default: NaN

  .. attribute::        barostat_frequency

    Frequency of the MonteCarlo Barostat. default: 25

  .. attribute::        integrator

    Integrator to move the simulation forward in time. OpenMM offers following
    Integrators: :class:`VerletIntegrator`, :class:`BrownianIntegrator`,
    :class:`LangevinIntegrator`, :class:`VariableVerletIntegrator` and
    :class:`VariableLangevinIntegrator`. default: None

  .. attribute::        solvent_dielectric

    Solvent dielectric constant. This is used for the GBSA force when the 
    :class:`Simulation` gets set up. default: 78.3

  ..  attribute::       solute_dielectric

    Solute dielectric constant. This is used for the GBSA force when the
    :class:`Simulation` gets set up. default: 1.0

  .. attribute::        reaction_field_dielecric

    Sets the reaction field dielectric for the Nonbonded Force when setting
    up the :class:`Simulation`. default: 78.3

  .. attribute::        use_dispersion_correction

    Flag, whether the dispersion correction should be used when setting
    up the Nonbonded Force in the :class:`Simulation`. default: True

  .. attribute::        keep_ff_specific_naming

    When running through the :class:`TopologyCreator`, the given ent gets 
    renamed to the forcefield specific naming. If set to true, the naming
    stays like that, if not the entity gets renamed to PDB standard.
    default: True

  .. attribute::        openmm_plugin_directory

    Path, where OpenMM specific plugins are searched. Especially if you want
    to use other platforms than reference, this has to be set.
    default: "/usr/local/openmm/lib/plugins"

  .. attribute::        custom_plugin_directory

    Path, where custom plugins are searched. 
    default: "/usr/local/openmm/lib/plugins"




.. class:: TerminiExceptions
  
  Can be used to define exceptions from the standard assignments
  assigned in the :class:`Forcefield`. 

  .. method::           SetException(res, exception_name)

    :param res:         :class:`ResidueHandle` for which the exception
                        should be set.

    :param exception_name:    :class:`str` describing the name of the
                        Blockmodifier in the forcefield that should be
                        applied in case of a termini

  .. method::           HasException(res)

    :param res:         :class:`ResidueHandle` that should be checked for 
                        exceptions

    :returns:           :class:`bool` True if res has an exception

  .. method::           GetException(res)

    :param res:         :class:`ResidueHandle` containing the exception

    :raises:            :class:`RuntimeError` if res has no associated exception

    :returns:           :class:`str` describint the name of the Blockmodifier in
                        the forcefield, that should be applied in case of a 
                        termini
