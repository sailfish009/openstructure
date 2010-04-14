def test1():
    global surf
    prot=io.LoadPDB("fragment.pdb")
    surf=io.LoadSurface("fragment","msms")
    surf.Attach(prot,5.0)
    scene.Add(gfx.Surface("surf",surf))
    scene["surf"].SetColor(gfx.RED,"ele=O")
    scene["surf"].SetColor(gfx.BLUE,"ele=N")
    scene.CenterOn("surf")
    scene.SetFog(False)
    # blend the ambient occlusion 40% into the surface
    # hence reduce diffuse to 60%
    # give 100% to specular
    scene.SetLightProp(0.6,0.4,1)
    # blend the local color 25% percent into the surface
    # keep diffuse at max
    # add specular component at 80% with a strong hilight
    scene["surf"].SetMat(0.25,1,0.8,128)
    scene.AutoAutoslab(False)
    scene.Autoslab()
    scene["surf"].AmbientOcclusion(True)

def test2():
    global plist
    plist=gfx.PrimList("t")
    plist.AddPoint(geom.Vec3(-1,-1,0), gfx.RED)
    plist.AddPoint(geom.Vec3(1,-1,0), gfx.GREEN)
    plist.AddPoint(geom.Vec3(1,1,0), gfx.YELLOW)
    plist.AddPoint(geom.Vec3(-1,1,0), gfx.BLUE)
    plist.AddPoint(geom.Vec3(0,0,4), gfx.MAGENTA)
    plist.SetMat(1,1,1,64)
    scene.SetLightProp(0,1,0)
    plist.SetRadius(1.5)
    plist.SetSphereDetail(4)
    plist.SetRenderMode(gfx.CUSTOM)
    plist.AmbientOcclusion(True)
    scene.Add(plist)
    scene.CenterOn(plist)
    scene.AutoAutoslab(False)
    scene.Autoslab()
    plist.AmbientOcclusion(True)


test1()
