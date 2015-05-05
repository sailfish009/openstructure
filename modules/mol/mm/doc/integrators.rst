Integrators
================================================================================

.. currentmodule:: ost.mol

The Integrators are a wrapper around the integrators provided by OpenMM and
have to be attached to the :class:`Settings` when setting up a simulation
to provide a way of making the thing shake.



Verlet Dynamics
--------------------------------------------------------------------------------

.. class:: VerletIntegrator(step_size)

  Implementation of Verlet dynamics

  :param step_size:     :class:`float` defining timestep in ps.



.. class:: VariableVerletIntegrator(error_tol)

  Implementation of Verlet dynamics with variable time steps

  :param error_tol:     :class:`float` error tolerance

  .. method:: GetErrorTolerance()

    :returns:           :class:`float`

  .. method:: SetErrorTolerance(tol)

    :param tol:         :class:`float`


Langevin Dynamics
--------------------------------------------------------------------------------

.. class:: LangevinIntegrator(temperature, friction_coeff, step_size)

  Implementation of the Langevin dynamics

  :param temperature:   :class:`float` temperature of heat bath in K

  :param friction_coeff: :class:`float` friction coefficient coupling the
                         system to the heat bath in 1/ps

  :param step_size:     :class:`float` defining timestep in ps.

  .. method:: GetTemperature()

    :returns:         :class:`float`

  .. method:: SetTemperature(temperature)

    :param temperature: :class:`float`

  .. method:: GetFriction()

    :returns:           :class:`float`

  .. method:: SetFriction(friction)

    :param friction:    :class:`float`

  .. method:: GetRandomNumberSeed()

    :returns:          :class:`int`

  .. method:: SetRandomNumberSeed(seed)

    :param seed:        :class:`int`




.. class:: VariableLangevinIntegrator(temperature, friction_coeff, error_tol)

  Implementation of Langevin dynamics with variable time steps

  :param temperature:   :class:`float` temperature of heat bath in K

  :param friction_coeff: :class:`float` friction coefficient coupling the
                         system to the heat bath in 1/ps

  :param error_tol:     :class:`float` error_tolerance.

  .. method:: GetTemperature()

    :returns:         :class:`float`

  .. method:: SetTemperature(temperature)

    :param temperature: :class:`float`

  .. method:: GetFriction()

    :returns:           :class:`float`

  .. method:: SetFriction(friction)

    :param friction:    :class:`float`

  .. method:: GetRandomNumberSeed()

    :returns:          :class:`int`

  .. method:: SetRandomNumberSeed(seed)

    :param seed:        :class:`int`

  .. method:: GetErrorTolerance()

    :returns:           :class:`float`

  .. method:: SetErrorTolerance()

    :param tol:         :class:`float`


Brownian Dynamics
--------------------------------------------------------------------------------

.. class:: BrownianIntegrator(temperature, friction_coeff, step_size)

  Implementation of Brownian dynamics

  :param temperature:   :class:`float` temperature of heat bath in K

  :param friction_coeff: :class:`float` friction coefficient coupling the
                         system to the heat bath in 1/ps

  :param step_size:     :class:`float` defining timestep in ps.

  .. method:: GetTemperature()

    :returns:           :class:`float`

  .. method:: SetTemperature(temperature)

    :param temperature: :class:`float`

  .. method:: GetFriction()

    :returns:           :class:`float`

  .. method:: SetFriction(friction)

    :param friction:    :class:`float`

  .. method:: GetRandomNumberSeed()

    :returns:          :class:`int`

  .. method:: SetRandomNumberSeed(seed)

    :param seed:        :class:`int`
