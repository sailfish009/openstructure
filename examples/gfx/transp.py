e=io.LoadPDB("../entity/sdh.pdb")
s=io.LoadSurface("../surf/sdh","msms")

scene.Add(gfx.Entity("x",gfx.HSC,e.Select("chain=A")))
scene["x"].SetColor(gfx.Color(0.9,0.95,1.0))
scene["x"].SetColor(gfx.Color(1.0,0.9,0.7),"rtype=helix")
scene["x"].SetColor(gfx.Color(0.5,1.0,0.7),"rtype=strand")

scene.Add(gfx.Surface("s",s))
scene["s"].SetOpacity(0.4)
scene["s"].SetMat(0,1,0,0)

scene.bg=gfx.WHITE

scene.SetRTC(geom.Mat4(-0.589889,-0.594876,0.546034,28.2245,
                        0.806165,-0.472502,0.356147,20.1685,
                        0.0461388,0.65028,0.758291,73.5665,
                        -3.52709,1.72253,-120,1))


