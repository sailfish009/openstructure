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
    # turn on full contribution from light
    scene.SetLightProp(1,1,1)
    
    # use a material with 10% global ambient light,
    # 90% diffuse light, and full specular with small
    # hilights
    scene["surf"].SetMat(0.1,0.9,1,128)

    # turn on ambient occlusion
    # this may take seconds to calculate
    scene["surf"].AmbientOcclusion(True)

    # blend the ambient occlusion 100% into the surface,
    # reducing the color intensity
    scene["surf"].AmbientOcclusionWeight(1)
    # blend the local color 30% percent into the surface
    scene["surf"].AmbientLocalWeight(0.3)

    scene.AutoAutoslab(False)
    scene.Autoslab()

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
