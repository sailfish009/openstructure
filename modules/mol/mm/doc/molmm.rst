The mm Module
================================================================================

.. currentmodule:: ost.mol

Introduction
--------------------------------------------------------------------------------

The mol.mm module provides a wrapper around the 
`OpenMM <http://openmm.org>`_ molecular mechanics library to provide
basic molecular dynamics (MD) capabilities fully embedded into the OpenStructure universe.
The heart of every simulation is the :class:`Topology` describing how the 
particles of an :class:`EntityHandle` interact. The :class:`Simulation` class connects the :class:`EntityHandle` with a :class:`Topology` and allows you to perform energy minimizations 
or move the simulation through time using an :class:`Integrator`. 
A :class:`Topology` can either be built from scratch by adding
one interaction after the other or automatically using the
:class:`TopologyCreator`. The process of :class:`Topology` building
and setting up a :class:`Simulation` gets controlled with the :class:`Settings`.

Latest Publication of `OpenMM <http://openmm.org>`_:
P. Eastman, M. S. Friedrichs, J. D. Chodera, R. J. Radmer, C. M. Bruns, J. P. Ku, K. A. Beauchamp, T. J. Lane, L.-P. Wang, D. Shukla, T. Tye, M. Houston, T. Stich, C. Klein, M. R. Shirts, and V. S. Pande. "OpenMM 4: A Reusable, Extensible, Hardware Independent Library for High Performance Molecular Simulation." J. Chem. Theor. Comput. 9(1):461-469. (2013)




Installation
--------------------------------------------------------------------------------

OpenStructure does not come by default with 
`OpenMM <http://openmm.org>`_ support.
You have to install it as an additional dependency and
recompile OpenStructure to dive into the amazing world of GPU accelerated
molecular mechanics. Once installed, you have to pass cmake additional
flags to allow compilation with OpenMM support. e.g.:

  .. code-block:: bash
    
    cmake . -DENABLE_MM=1 -DOPEN_MM_INCLUDE_DIR=/path/to/openmm/include
            -DOPEN_MM_LIBRARY=/path/to/openmm/lib/libOpenMM.so
            -DOPEN_MM_PLUGIN_DIR=/path/to/openmm/lib/plugins


Setting up a simple simulation
--------------------------------------------------------------------------------


  .. code-block:: python
    
    from ost.mol import mm

    prot=io.LoadPDB('1crn',remote=True)

    #set up the simulation
    settings = mm.Settings()
    settings.integrator = mm.LangevinIntegrator(310,1,0.002)
    settings.forcefield = mm.LoadCHARMMForcefield()
    sim = mm.Simulation(prot,settings)

    #minimize it
    sim.ApplySD(tolerance = 1.0, max_iterations = 200)

    #create a trajectory observer and register it to the simulation
    #every 10 steps, the actual positions will be written down to disk
    observer = mm.TrajWriter(10,"example_traj.pdb","example_traj.dcd")
    sim.Register(observer)

    #run the simulation
    sim.Steps(10000)

    #Trajectory Observer needs to finalize, otherwise you might get a corrupt dcd file
    observer.Finalize()


Doing more sophisticated stuff
--------------------------------------------------------------------------------

You want to create your own :class:`BuildingBlock` to parameterize custom 
residues? Or even generate your own custom :class:`Forcefield`? 
Check out the mm dir in the examples/code_fragments directory. 

Documentation
--------------------------------------------------------------------------------

.. toctree::
  interaction
  buildingblock
  forcefield
  settings
  topology
  observers
  integrators
  simulation

