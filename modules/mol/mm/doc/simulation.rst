Simulation
================================================================================

.. currentmodule:: ost.mol



.. class:: Simulation

  The :class:`mm.Simulation` class is used to run a simulation. 

  .. method:: Save(filename)

    :param filename: :class:`str`

  .. method:: Load(filename)

    :param filename: :class:`str`

  .. method:: ApplyLBFGS([tolerance=1.0,max_iterations=1000])

    Run minimization using the Limited-memory Broyden–Fletcher–Goldfarb–Shanno (L-BFGS) method.
   
    :param tolerance:  :class:`float`
    :param max_iterations:  :class:`int`
  

  .. method:: ApplySD(tolerance,max_iterations)

    Run minimization using the Steepes-Descend method.
   
    :param tolerance:  :class:`float`
    :param max_iterations:  :class:`int`

  .. method:: Steps(steps)

    Runs the simulation for **steps** number of steps.
   
    :param steps:  :class:`int`

  .. method:: GetPositions([enforce_periodic_box = false, in_angstrom = true])

    :param enforce_periodic_box: :class:`bool`
    :param in_angstrom: :class:`bool`

    :returns: A :class:`~ost.geom.Vec3List` of all the positions of the atoms in the Entity.

  .. method:: SetPositions(positions,[,in_angstrom = true])

    :param positions: :class:`~ost.geom.Vec3List`
    :param in_angstrom: :class:`bool`

  .. method:: UpdatePositions([enforce_periodic_box=false])

    Sets the positions of the atoms in the :class:`EntityHandle` to the current positions of the simulation.

    :param enforce_periodic_box: :class:`bool`

  .. method:: GetVelocities()

    :returns: A :class:`~ost.geom.Vec3List` of all the velocities of the atoms in the Entity.
  
  .. method:: GetForces()

    :returns: A :class:`~ost.geom.Vec3List` of all the resulting force for each the atom in the Entity.

  .. method:: SetVelocities(velocities)

    :param velocities: :class:`~ost.geom.Vec3List`

  .. method:: GetEntity()

    :returns: :class:`EntityHandle`

  .. method:: GetEnergy()

    :returns: :class:`float`

  .. method:: GetPotentialEnergy()

    :returns: :class:`float`

  .. method:: GetKineticEnergy()

    :returns: :class:`float`

  .. method:: GetTopology()

    :returns: :class:`Topology`

  .. method:: GetPeriodicBoxExtents()

    :returns: :class:`~ost.geom.Vec3List`

  .. method:: SetPeriodicBoxExtents(vec)

    :param vec: :class:`~ost.geom.Vec3List`

  .. method:: Register(observer)

    Adds an observer to the simulation object. This can be used for example to write out the trajectory during the simulation.

    :param observer: :class:`Observer`

  .. method:: AddPositionConstraint(index)

    Fixes the position of the atom with index given in the argument.

    :param index: :class:`int`

  .. method:: AddPositionConstraints(indices)

    Fixes the position of the atoms with indices given in in the argument.

    :param indices: :class:`~ost.IntList`

  .. method:: ResetPositionConstraints()

    Removes all position constraints.

  .. method:: ResetHarmonicBond(index,bond_length, force_constant)

    :param index: :class:`int`
    :param bond_length: :class:`float`
    :param force_constant: :class:`float`

  .. method:: ResetHarmonicAngle()

    :param index: :class:`int`
    :param angle: :class:`float`
    :param force_constant: :class:`float`

  .. method:: ResetUreyBradleyAngle(index, angle, angle_force_constant, bond_length, bond_force_constant)

    :param index: :class:`int`
    :param angle: :class:`float`
    :param force_constant: :class:`float`
    :param bond_length: :class:`float`
    :param force_constant: :class:`float`

  .. method:: ResetPeriodicDihedral(index, multiplicity, phase, force_constant)

    :param index: :class:`int`
    :param multiplicity: :class:`int`
    :param phase: :class:`float`
    :param force_constant: :class:`float`


  .. method:: ResetPeriodicImproper(index, multiplicity, phase, force_constant)

    :param index: :class:`int`
    :param multiplicity: :class:`int`
    :param phase: :class:`float`
    :param force_constant: :class:`float`

  .. method:: ResetHarmonicImproper(index, phase, force_constant)

    :param index: :class:`int`
    :param phase: :class:`float`
    :param force_constant: :class:`float`

  .. method:: ResetLJPair(index,sigma,epsilon)

    :param index: :class:`int`
    :param sigma: :class:`float`
    :param epsilon: :class:`float`


  .. method:: ResetDistanceConstraint(index, constraint_length)
    
    :param index: :class:`int`
    :param constraint_length: :class:`float`  

  .. method:: ResetHarmonicPositionRestraint(index,ref_position, force_constant,x_scale,y_scale,z_scale)

    :param index: :class:`int`
    :param ref_position: :class:`~ost.geom.Vec3`
    :param force_constant: :class:`float`
    :param x_scale: :class:`float`
    :param y_scale: :class:`float`
    :param z_scale: :class:`float`

  .. method:: ResetHarmonicDistanceRestraint(index,bond_length, force_constant)

    :param index: :class:`int`
    :param bond_length: :class:`float`
    :param force_constant: :class:`float`

  .. method:: ResetLJ(index,sigma,epsilon)

    :param index: :class:`int`
    :param sigma: :class:`float`
    :param epsilon: :class:`float`

  .. method:: ResetGBSA(index,radius,scaling)

    :param index: :class:`int`
    :param radius: :class:`float`
    :param scaling: :class:`float`

  .. method:: ResetCharge(index,charge)

    :param index: :class:`int`
    :param charge: :class:`float`


  .. method:: ResetMass(index,mass)

    :param index: :class:`int`
    :param mass: :class:`float`




