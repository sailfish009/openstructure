Integrators
================================================================================

.. currentmodule:: ost.mol

The Integrators are a wrapper around the integrators provided by OpenMM and
have to be attached to the :class:`Settings` when setting up a simulation
to provide a way of making the thing shake.

.. class:: VerletIntegrator

  Implementation of Verlet dynamics



.. class:: BrownianIntegrator

  Implementation of Brownian dynamics

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




.. class:: LangevinIntegrator

  Implementation of the Langevin dynamics

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



.. class:: VariableVerletIntegrator

  Implementation of Verlet dynamics with variable time steps

  .. method:: GetErrorTolerance()

    :returns:           :class:`float`

  .. method:: SetErrorTolerance(tol)

    :param tol:         :class:`float`



.. class:: VariableLangevinIntegrator

  Implementation of Langevin dynamics with variable time steps

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
