from ost.mol.mm import *

prot=io.LoadPDB('1CRN.pdb')

#set up the simulation
settings = MMSettings()
settings.integrator = LangevinIntegrator(310,1,0.002)
settings.add_gbsa = True
settings.forcefield = LoadCHARMMForcefield()
settings.nonbonded_cutoff = 8.0
settings.nonbonded_method = NonbondedMethod.CutoffNonPeriodic
settings.platform = Platform.CPU

sim = Simulation(prot,settings)

#minimize it
sim.ApplySD(tolerance = 1.0, max_iterations = 200)

#create a trajectory observer and register it to the simulation
observer = TrajWriter(10,"gb_example_traj.pdb","gb_example_traj.dcd")
sim.Register(observer)

#run the simulation
sim.Steps(10000)

#Trajectory Observer needs to finalize, otherwise you might get a corrupt dcd file
observer.Finalize()




