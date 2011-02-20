import desost_util
e=io.LoadPDB("examples/demos/data/sh2.pdb")
desost_util.RunStride(e,True)
scene.Add(gfx.Entity("t",gfx.HSC,e))

