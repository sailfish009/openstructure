Topology
================================================================================

.. currentmodule:: ost.mol

The :class:`Topology` object is an abstract representation of a protein 
structure or any combination of particles that interact with each other in 
some way. Being initialized with a list of masses representing the particles, 
the topology allows to define arbitrary forces between these particles. The 
:class:`Topology` gets internally translated to a molecular mechanics system 
as soon as a :class:`Simulation` is set up. To make setting up a 
:class:`Topology` more convenient, the :class:`TopologyCreator` class allows 
to create a :class:`Topology` based on an mm :class:`Settings` object, 
that should suffice most needs.

.. class:: TopologyCreator

  .. method:: Create(entity, settings)

    Creates a topology given an :class:`EntityHandle` and a :class:`Settings`
    object. Note, that the :class:`EntityHandle` is passed by reference
    and gets altered by adding hydrogens and modification of terminal residues
    based on the information given by the :class:`Settings`/:class:`Forcefield`.

    :param entity:      :class:`EntityHandle`

    :param settings:    :class:`Settings`

    :raises:            :class:`RuntimeError` if settings has no 
                        :class:`Forcefield` assigned

    :raises:            :class:`RuntimeError` if there is a residue with no
                        appropriate :class:`BuildingBlock` in the 
                        :class:`Forcefield`

    :raises:            :class:`RuntimeError` if there is a 
                        :class:`ResidueHandle` not matching the
                        :class:`BuildingBlock` definition in the
                        :class:`Forcefield`

    :raises:            :class:`RuntimeError` if there is an interaction,
                        that has no parameters in the forcefield. This 
                        :class:`RuntimeError` can be supressed by the 
                        according flag in the :class:`Settings` object.

.. class:: Topology


  .. method:: Save(filename)

    Dump Toplogy into binary file

    :param filename:    :class:`str`


  .. method:: Load(filename)

    Load binary topology from disk

    :param filename:    :class:`str`

    :returns:           loaded :class:`Topology`

    :raises:            :class:`RuntimeError` when file can't be found


  .. method:: Merge(other)

    Merges in another :class:`Topology` and adds all their particles and interactions.
    Assuming the actual :class:`Topology` containing n particles, the indexing of the
    other :class:`Topology` gets altered by adding n everywhere.

    :param other:       :class:`Topology`

    :raises:            :class:`RuntimeError` if fudge parameters are not consistent

    :raises:            :class:`RuntimeError` if current :class:`Topology` 
                        contains charges but the other not

    :raises:            :class:`RuntimeError` if current :class:`Topology` 
                        contains sigmas but the other not

    :raises:            :class:`RuntimeError` if current :class:`Topology` 
                        contains epsilons but the other not

    :raises:            :class:`RuntimeError` if current :class:`Topology` 
                        contains GBSA Radii but the other not

    :raises:            :class:`RuntimeError` if current :class:`Topology` 
                        contains OBC scalings but the other not

  .. method:: Merge(ent, other, other_ent)

    Merges in another :class:`Topology` and adds all their particles and 
    interactions. Assuming the actual :class:`Topology` containing n particles, 
    the indexing of the other :class:`Topology` gets altered by adding n 
    everywhere. This version of the Merge function assumes, the current topology 
    to describe the :class:`EntityHandle` ent and the other :class:`Topology`
    to describe the :class:`EntityHandle` other_ent. During merging, all
    chains, residues, atoms and bonds from other ent are pulled into ent in
    a way, that the current :class:`Topology` is consistent with the
    :class:`EntityHandle` ent.

    :param ent:         :class:`EntityHandle`

    :param other:       :class:`Topology`

    :param other_ent:   :class:`EntityHandle`

    :raises:            :class:`RuntimeError` if fudge parameters are not consistent

    :raises:            :class:`RuntimeError` if current :class:`Topology` 
                        contains charges but the other not

    :raises:            :class:`RuntimeError` if current :class:`Topology` 
                        contains sigmas but the other not

    :raises:            :class:`RuntimeError` if current :class:`Topology` 
                        contains epsilons but the other not

    :raises:            :class:`RuntimeError` if current :class:`Topology` 
                        contains GBSA Radii but the other not

    :raises:            :class:`RuntimeError` if current :class:`Topology` 
                        contains OBC scalings but the other not

    :raises:            :class:`RuntimeError` if the number of atoms in one
                        of the entities is not consistent with their related
                        topologies.

    :raises:            :class:`RuntimeError` other_ent contains a chain with
                        a name already present in ent.


  .. method:: AddHarmonicBond(index_one, index_two, bond_length, force_constant)

    :param index_one:   :class:`int`

    :param index_two:   :class:`int`

    :param bond_length: :class:`float`

    :param force_constant: :class:`float`

    :returns:           :class:`int` index of added interaction

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         particles



  .. method:: AddHarmonicAngle(index_one, index_two, index_three, bond_length, 
                               force_constant)

    :param index_one:   :class:`int`

    :param index_two:   :class:`int`

    :param index_three: :class:`int`

    :param angle:       :class:`float`

    :param force_constant: :class:`float`

    :returns:           :class:`int` index of added interaction

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         particles


  .. method:: AddUreyBradleyAngle(index_one, index_two, index_three, 
                                  angle, angle_force_constant,
                                  bond_length, bond_force_constant)

    :param index_one:   :class:`int`

    :param index_two:   :class:`int`

    :param index_three: :class:`int`

    :param angle:       :class:`float`

    :param angle_force_constant: :class:`float`

    :param bond_length: :class:`float`

    :param bond_force_constant: :class:`float`

    :returns:           :class:`int` index of added interaction

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         particles


  .. method:: AddPeriodicDihedral(index_one, index_two, index_three, index_four,
                                  multiplicity, phase, force_constant)

    :param index_one:   :class:`int`

    :param index_two:   :class:`int`

    :param index_three: :class:`int`

    :param index_four:  :class:`int`

    :param multiplicity: :class:`int`

    :param phase:       :class:`float`

    :param force_constant: :class:`float`

    :returns:           :class:`int` index of added interaction

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         particles


  .. method:: AddPeriodicImproper(index_one, index_two, index_three, index_four,
                                  multiplicity, phase, force_constant)

    :param index_one:   :class:`int`

    :param index_two:   :class:`int`

    :param index_three: :class:`int`

    :param index_four:  :class:`int`

    :param multiplicity: :class:`int`

    :param phase:       :class:`float`

    :param force_constant: :class:`float`

    :returns:           :class:`int` index of added interaction

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         particles


  .. method:: AddHarmonicImproper(index_one, index_two, index_three, index_four,
                                  angle, force_constant)

    :param index_one:   :class:`int`

    :param index_two:   :class:`int`

    :param index_three: :class:`int`

    :param index_four:  :class:`int`

    :param angle:       :class:`float`

    :param force_constant: :class:`float`

    :returns:           :class:`int` index of added interaction

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         particles


  .. method:: AddCMap(index_one, index_two, index_three, index_four,
                      index_five, dimension, values)

    :param index_one:   :class:`int`

    :param index_two:   :class:`int`

    :param index_three: :class:`int`

    :param index_four:  :class:`int`

    :param index_five:  :class:`int`

    :param dimension:   :class:`int`

    :param values:      :class:`list` of :class:`float` of size 
                        dimension*dimension describing the correction
                        map. The values have to be ordered like
                        (phi1,psi1),(phi1,psi2)...(phi1,psin),
                        (phi2,psi1),(phi2,psi2)...(phi2,psin),
                        ...(phin,psin).
                        the range for phi and psi are [-pi,pi[

    :returns:           :class:`int` index of added interaction

    :raises:            :class:`RuntimeError` when size of values is not 
                        dimension * dimension or index exceeds number of 
                        particles.


  .. method:: AddLJPair(index_one, index_two, sigma, epsilon)

    :param index_one:   :class:`int`

    :param index_two:   :class:`int`

    :param sigma:       :class:`float`

    :param epsilon:     :class:`float`

    :returns:           :class:`int` index of added interaction

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         particles or when particular LJPair is already 
                         parametrized


  .. method:: AddDistanceConstraint(index_one, index_two, distance)

    :param index_one:   :class:`int`

    :param index_two:   :class:`int`

    :param distance:    :class:`float`

    :returns:           :class:`int` index of added interaction

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         particles or when particular distance constraint
                         is already parametrized


  .. method:: AddExclusion(index_one, index_two)

    :param index_one:   :class:`int`

    :param index_two:   :class:`int`

    :returns:           :class:`int` index of added interaction

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         particles or when particular exclusion is already set


  .. method:: AddPositionConstraint(index)

    :param index:       :class:`int`

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         particles


  .. method:: AddHarmonicPositionRestraint(index, reference_pos, force_constant,
                                           x_scale=1.0, y_scale=1.0, 
                                           z_scale=1.0)

    Adds a harmonic position restraint for a particle of the form:
    force_constant*(x_scale*(x-x0)^2+y_scale*(y-y0)^2+z_scale*(z-z0)^2)

    :param index:       :class:`int`

    :param reference_pos: :class:`ost.geom.Vec3` reference_position

    :param force_constant: :class:`float`

    :param x_scale: :class:`float`

    :param y_scale: :class:`float`

    :param z_scale: :class:`float`

    :returns:           :class:`int` index of added interaction

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         particles


  .. method:: AddHarmonicDistanceRestraint(index_one, index_two, length
                                           force_constant)

    Adds a force between two particles of the form:
    force_constant * (b-b0)^2

    :returns:           :class:`int` index of added interaction

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         particles


  .. method:: ResetPositionConstraints()

    Removes all position constraints


  .. method:: ResetExclusions()

    Removes all set exclusions


  .. method:: SetSigmas(sigmas)

    :param sigmas:      :class:`list` of :class:`float` defining sigma params
   
    :raises:            :class:`RuntimeError` when list size is inconsistent
                        with number of particles


  .. method:: SetSigma(index, sigma)

    :param index:       :class:`int`

    :parm sigma:        :class:`float`

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         particles


  .. method:: SetEpsilons(epsilons)

    :param epsilons:    :class:`list` of :class:`float` defining epsilon params
   
    :raises:            :class:`RuntimeError` when list size is inconsistent
                        with number of particles


  .. method:: SetEpsilon(index, epsilon)

    :param index:       :class:`int`

    :parm epsilon:        :class:`float`

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         particles


  .. method:: SetGBSARadii(radii)

    :param radii:      :class:`list` of :class:`float` defining radii
   
    :raises:            :class:`RuntimeError` when list size is inconsistent
                        with number of particles


  .. method:: SetGBSARadii(index, radius)

    :param index:       :class:`int`

    :parm radius:       :class:`float`

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         particles


  .. method:: SetOBCScalings(scalings)

    :param scalings:    :class:`list` of :class:`float` defining scaling params
   
    :raises:            :class:`RuntimeError` when list size is inconsistent
                        with number of particles


  .. method:: SetOBCScaling(index, scaling)

    :param index:       :class:`int`

    :parm scaling:      :class:`float`

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         particles


  .. method:: SetCharges(charges)

    :param charges:     :class:`list` of :class:`float` defining charge params
   
    :raises:            :class:`RuntimeError` when list size is inconsistent
                        with number of particles


  .. method:: SetCharge(index, charge)

    :param index:       :class:`int`

    :parm charge:       :class:`float`

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         particles

  .. method:: SetMasses(masses)

    :param masses:      :class:`list` of :class:`float` defining mass params
   
    :raises:            :class:`RuntimeError` when list size is inconsistent
                        with number of particles


  .. method:: SetMass(index, mass)

    :param index:       :class:`int`

    :parm mass:         :class:`float`

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         particles


  .. method:: SetFudgeQQ(fudge)

    :param fudge:       :class:`fudge`


  .. method:: SetFudgeLJ(fudge)

    :param fudge:       :class:`fudge`


  .. method:: GetHarmonicBondParameters(index)

    :param index:       :class:`int`

    :returns:           :class:`tuple` containing the indices of the involved
                        particles and the set parameters

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         set harmonic bonds


  .. method:: GetHarmonicAngleParameters(index)

    :param index:       :class:`int`

    :returns:           :class:`tuple` containing the indices of the involved
                        particles and the set parameters

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         set harmonic angles

  .. method:: GetUreyBradleyAngleParameters(index)

    :param index:       :class:`int`

    :returns:           :class:`tuple` containing the indices of the involved
                        particles and the set parameters

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         set urey-bradley angles

  .. method:: GetPeriodicDiheralParameters(index)

    :param index:       :class:`int`

    :returns:           :class:`tuple` containing the indices of the involved
                        particles and the set parameters

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         set periodic dihedrals

  .. method:: GetPeriodicImproperParameters(index)

    :param index:       :class:`int`

    :returns:           :class:`tuple` containing the indices of the involved
                        particles and the set parameters

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         set periodic impropers

  .. method:: GetHarmonicImproperParameters(index)

    :param index:       :class:`int`

    :returns:           :class:`tuple` containing the indices of the involved
                        particles and the set parameters

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         set harmonic impropers


  .. method:: GetCMapParameters(index)

    :param index:       :class:`int`

    :returns:           :class:`tuple` containing the indices of the involved
                        particles and the set parameters

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         set cmaps


  .. method:: GetLJPairParameters(index)

    :param index:       :class:`int`

    :returns:           :class:`tuple` containing the indices of the involved
                        particles and the set parameters

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         set lj pairs


  .. method:: GetDistanceConstraintParameters(index)

    :param index:       :class:`int`

    :returns:           :class:`tuple` containing the indices of the involved
                        particles and the set parameters

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         set distance constraints


  .. method:: GetHarmonicPositionRestraintParameters(index)

    :param index:       :class:`int`

    :returns:           :class:`tuple` containing the indices of the involved
                        particles and the set parameters

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         set harmonic position restraints


  .. method:: GetHarmonicDistanceRestraintParameters(index)

    :param index:       :class:`int`

    :returns:           :class:`tuple` containing the indices of the involved
                        particles and the set parameters

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         set harmonic distance restraints


  .. method:: SetHarmonicBondParameters(index, bond_length, force_constant)

    Replaces existing parameters for particular interaction

    :param index:       :class:`int`

    :param bond_length: :class:`float`

    :param force_constant: :class:`float`

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         set harmonic bonds


  .. method:: SetHarmonicAngleParameters(index, angle, force_constant)

    Replaces existing parameters for particular interaction

    :param index:       :class:`int`

    :param angle:       :class:`float`

    :param force_constant: :class:`float`

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         set harmonic angles

  .. method:: SetUreyBradleyAngleParameters(index, angle, angle_force_constant,
                                            bond_length, bond_force_constant)

    Replaces existing parameters for particular interaction

    :param index:       :class:`int`

    :param angle_force_constant: :class:`float`

    :param bond_length: :class:`float`

    :param bond_force_constant: :class:`float`

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         set urey-bradley angles

  .. method:: SetPeriodicDiheralParameters(index, multiplicity, phase, force_constant)

    Replaces existing parameters for particular interaction

    :param index:       :class:`int`

    :param multiplicity: :class:`int`

    :param phase:       :class:`float`

    :param force_constant: :class:`float`

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         set periodic dihedrals

  .. method:: SetPeriodicImproperParameters(index, multiplicity, phase, force_constant)

    Replaces existing parameters for particular interaction

    :param index:       :class:`int`

    :param multiplicity: :class:`int`

    :param phase:       :class:`float`

    :param force_constant: :class:`float`

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         set periodic impropers

  .. method:: SetHarmonicImproperParameters(index, angle, force_constant)

    Replaces existing parameters for particular interaction

    :param index:       :class:`int`

    :param angle:       :class:`float`

    :param force_constant: :class:`float`

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         set harmonic impropers


  .. method:: SetCMapParameters(index, dimension, map)

    Replaces existing parameters for particular interaction

    :param index:       :class:`int`

    :param dimension:   :class:`int`

    :param map:         :class:`list` of :class:`float` values in the format
                        described in the AddCMap function.

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         set cmaps or size of map is not dimension*dimension


  .. method:: SetLJPairParameters(index, sigma, epsilon)

    Replaces existing parameters for particular interaction

    :param index:       :class:`int`

    :param sigma:       :class:`float`

    :param epsilon:     :class:`float`

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         set lj pairs


  .. method:: SetDistanceConstraintParameters(index, distance)

    Replaces existing parameters for particular interaction

    :param index:       :class:`int`

    :param distance:    :class:`float`

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         set distance constraints


  .. method:: SetHarmonicPositionRestraintParameters(index, ref_position, k,
                                                     x_scale=1.0, y_scale=1.0, 
                                                     z_scale=1.0)

    Replaces existing parameters for particular interaction

    :param index:       :class:`int`

    :param ref_position: :class:`ost.geom.Vec3`

    :param k:           :class:`float`

    :param x_scale:     :class:`float`

    :param y_scale:     :class:`float`

    :param z_scale:     :class:`float`

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         set harmonic position restraints


  .. method:: SetHarmonicDistanceRestraintParameters(index, length, k)

    Replaces existing parameters for particular interaction

    :param index:       :class:`int`

    :param length:      :class:`float`

    :param k:           :class:`float`

    :raises:            :class:`RuntimeError` when index exceeds number of 
                         set harmonic distance restraints


  .. method:: GetSigmas()

    :returns:           :class:`list` of :class:`float` with previously set 
                        sigmas


  .. method:: GetEpsilons()

    :returns:           :class:`list` of :class:`float` with previously set 
                        epsilons


  .. method:: GetGBSARadii()

    :returns:           :class:`list` of :class:`float` with previously set 
                        gbsa radii


  .. method:: GetOBCScalings()

    :returns:           :class:`list` of :class:`float` with previously set 
                        obc scalings


  .. method:: GetCharges()

    :returns:           :class:`list` of :class:`float` with previously set 
                        gbsa charges


  .. method:: GetMasses()

    :returns:           :class:`list` of :class:`float` with previously set 
                        masses


  .. method:: GetSigma(particle_index)

    :param particle_index: :class:`int`

    :returns:           :class:`float`

    :raises:            :class:`RuntimeError` when particle_index exceeds
                        number of particles or sigmas have not been set


  .. method:: GetEpsilon(particle_index)

    :param particle_index: :class:`int`

    :returns:           :class:`float`

    :raises:            :class:`RuntimeError` when particle_index exceeds
                        number of particles or epsilons have not been set



  .. method:: GetGBSARadius(particle_index)

    :param particle_index: :class:`int`

    :returns:          :class:`float`

    :raises:            :class:`RuntimeError` when particle_index exceeds
                        number of particles or gbsa_radii have not been set


  .. method:: GetOBCScaling(particle_index)

    :param particle_index: :class:`int`

    :returns:           :class:`float`

    :raises:            :class:`RuntimeError` when particle_index exceeds
                        number of particles or obc scalings have not been set


  .. method:: GetCharge(particle_index)

    :param particle_index: :class:`int`

    :returns:           :class:`float`

    :raises:            :class:`RuntimeError` when particle_index exceeds
                        number of particles or charges have not been set


  .. method:: GetMass(particle_index)

    :param particle_index: :class:`int`

    :returns:           :class:`float`

    :raises:            :class:`RuntimeError` when particle_index exceeds
                        number of particles


  .. method:: GetHarmonicBondIndices(index_one, index_two)

    :param index_one:   :class:`int`

    :param index_two:   :class:`int`

    :returns:           :class:`list` of :class:`int` describing all harmonic
                        bonds acting on given particles

    :raises:            :class:`RuntimeError` when an index exceeds
                        number of particles in topology


  .. method:: GetHarmonicAngleIndices(index_one, index_two, index_three)

    :param index_one:   :class:`int`

    :param index_two:   :class:`int`

    :param index_three: :class:`int`

    :returns:           :class:`list` of :class:`int` describing all harmonic
                        angles acting on given particles

    :raises:            :class:`RuntimeError` when an index exceeds
                        number of particles in topology


  .. method:: GetUreyBradleyAngleIndices(index_one, index_two, index_three)

    :param index_one:   :class:`int`

    :param index_two:   :class:`int`

    :param index_three: :class:`int`

    :returns:           :class:`list` of :class:`int` describing all 
                        urey-bradley angles acting on given particles

    :raises:            :class:`RuntimeError` when an index exceeds
                        number of particles in topology


  .. method:: GetPeriodicDihedralIndices(index_one, index_two, index_three,
                                         index_four)

    :param index_one:   :class:`int`

    :param index_two:   :class:`int`

    :param index_three: :class:`int`

    :param index_four:  :class:`int`

    :returns:           :class:`list` of :class:`int` describing all periodic
                        dihedrals acting on given particles

    :raises:            :class:`RuntimeError` when an index exceeds
                        number of particles in topology


  .. method:: GetPeriodicImpropersIndices(index_one, index_two, index_three,
                                          index_four)

    :param index_one:   :class:`int`

    :param index_two:   :class:`int`

    :param index_three: :class:`int`

    :param index_four:  :class:`int`

    :returns:           :class:`list` of :class:`int` describing all periodic
                        impropers acting on given particles

    :raises:            :class:`RuntimeError` when an index exceeds
                        number of particles in topology


  .. method:: GetHarmonicImpropersIndices(index_one, index_two, index_three,
                                          index_four)

    :param index_one:   :class:`int`

    :param index_two:   :class:`int`

    :param index_three: :class:`int`

    :param index_four:  :class:`int`

    :returns:           :class:`list` of :class:`int` describing all harmonic
                        impropers acting on given particles

    :raises:            :class:`RuntimeError` when an index exceeds
                        number of particles in topology


  .. method:: GetCMapIndices(index_one, index_two, index_three,
                             index_four, index_five)

    :param index_one:   :class:`int`

    :param index_two:   :class:`int`

    :param index_three: :class:`int`

    :param index_four:  :class:`int`

    :param index_five:  :class:`int`

    :returns:           :class:`list` of :class:`int` describing all cmaps
                        acting on given particles

    :raises:            :class:`RuntimeError` when an index exceeds
                        number of particles in topology


  .. method:: GetLJPairIndex(index_one, index_two)

    :param index_one:   :class:`int`

    :param index_two:   :class:`int`

    :returns:           :class:`int` describing LJPair acting on particles,
                        -1 if there is none

    :raises:            :class:`RuntimeError` when an index exceeds
                        number of particles in topology


  .. method:: GetDistanceConstraintIndex(index_one, index_two)

    :param index_one:   :class:`int`

    :param index_two:   :class:`int`

    :returns:           :class:`int` describing distance constraint acting on 
                        particles, -1 if there is none

    :raises:            :class:`RuntimeError` when an index exceeds
                        number of particles in topology


  .. method:: GetHarmonicBondIndices(index)

    :param index:       :class:

    :returns:           :class:`list` of :class:`int` describing all harmonic
                        bonds this particular particle is involved in

    :raises:            :class:`RuntimeError` when index exceeds
                        number of particles in topology


  .. method:: GetHarmonicAngleIndices(index)

    :param index:       :class:

    :returns:           :class:`list` of :class:`int` describing all harmonic
                        angles this particular particle is involved in

    :raises:            :class:`RuntimeError` when index exceeds
                        number of particles in topology


  .. method:: GetUreyBradleyAngleIndices(index)

    :param index:       :class:

    :returns:           :class:`list` of :class:`int` describing all 
                        urey-bradley angles this particular particle 
                        is involved in

    :raises:            :class:`RuntimeError` when index exceeds
                        number of particles in topology


  .. method:: GetPeriodicDihedralIndices(index)

    :param index:       :class:

    :returns:           :class:`list` of :class:`int` describing all periodic
                        dihedrals this particular particle is involved in

    :raises:            :class:`RuntimeError` when index exceeds
                        number of particles in topology


  .. method:: GetPeriodicImproperIndices(index)

    :param index:       :class:

    :returns:           :class:`list` of :class:`int` describing all periodic
                        impropers this particular particle is involved in

    :raises:            :class:`RuntimeError` when index exceeds
                        number of particles in topology

  .. method:: GetHarmonicImproperIndices(index)

    :param index:       :class:

    :returns:           :class:`list` of :class:`int` describing all harmonic
                        impropers this particular particle is involved in

    :raises:            :class:`RuntimeError` when index exceeds
                        number of particles in topology


  .. method:: GetCMapIndices(index)

    :param index:       :class:

    :returns:           :class:`list` of :class:`int` describing all cmaps
                        this particular particle is involved in

    :raises:            :class:`RuntimeError` when index exceeds
                        number of particles in topology


  .. method:: GetLJPairIndices(index)

    :param index:       :class:

    :returns:           :class:`list` of :class:`int` describing all LJPairs
                        this particular particle is involved in

    :raises:            :class:`RuntimeError` when index exceeds
                        number of particles in topology


  .. method:: GetDistanceConstraintIndices(index)

    :param index:       :class:

    :returns:           :class:`list` of :class:`int` describing all distance
                        constraints this particular particle is involved in

    :raises:            :class:`RuntimeError` when index exceeds
                        number of particles in topology


  .. method:: GetHarmonicPositionRestraintIndices(index)

    :param index:       :class:

    :returns:           :class:`list` of :class:`int` describing all harmonic
                        position restraints this particular particle is 
                        involved in

    :raises:            :class:`RuntimeError` when index exceeds
                        number of particles in topology
  

  .. method:: GetHarmonicDistanceRestraintIndices(index)

    :param index:       :class:

    :returns:           :class:`list` of :class:`int` describing all harmonic
                        distance restraints this particular particle is 
                        involved in

    :raises:            :class:`RuntimeError` when index exceeds
                        number of particles in topology


  .. method:: GetNumParticles()

    :returns:           :class:`int`


  .. method:: GetNumHarmonicBonds()

    :returns:           :class:`int`


  .. method:: GetNumHarmonicAngles()

    :returns:           :class:`int`


  .. method:: GetNumUreyBradleyAngles()

    :returns:           :class:`int`


  .. method:: GetNumPeriodicDihedrals()

    :returns:           :class:`int`


  .. method:: GetNumPeriodicImpropers()

    :returns:           :class:`int`


  .. method:: GetNumHarmonicImpropers()

    :returns:           :class:`int`


  .. method:: GetNumCMaps()

    :returns:           :class:`int`


  .. method:: GetNumLJPairs()

    :returns:           :class:`int`


  .. method:: GetNumDistanceConstraints()

    :returns:           :class:`int`


  .. method:: GetNumPositionConstraints()

    :returns:           :class:`int`


  .. method:: GetNumHarmonicPositionRestraints()

    :returns:           :class:`int`


  .. method:: GetNumHarmonicDistanceRestraints()

    :returns:           :class:`int`


  .. method:: GetNumExclusions()

    :returns:           :class:`int`