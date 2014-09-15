from ost.gui import traj

t = io.LoadCHARMMTraj("gb_example_traj.pdb","gb_example_traj.dcd")
w = traj.TrajWidget(t)
w.show()
