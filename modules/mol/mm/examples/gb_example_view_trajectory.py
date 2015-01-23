from ost.gui import trajectory_viewer

t = io.LoadCHARMMTraj("gb_example_traj.pdb","gb_example_traj.dcd")
go = gfx.Entity("gb",t.GetEntity())
scene.Add(go)
w = trajectory_viewer.TrajWidget([t],[go])
w.show()
