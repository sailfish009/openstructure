scene.SetFog(False)

eh = io.LoadEntity("sdh.pdb")
ev = eh.Select("cname=A")
ec=gfx.Entity("p",ev)
ec.SetRenderMode(gfx.CPK)
scene.Add(ec)
scene.SetCenter(ec.GetCenter())
ec.Hide()

s = mol.BuildSurface(ev,1.5,15.0/57.3)
gs=gfx.Surface("s",s)
scene.Add(gs)

# for debug reasons
rs=mol.RSurf(1.5)
for a in ev.GetAtomList():
    rs.AddSphere(a.GetPos(),a.GetRadius(),str(a))
rs.Build()
rs.Triangulate(60.0*3.1415/180.0)
gr=gfx.RSurface("r",rs)
scene.Add(gr)
gr.Hide()



