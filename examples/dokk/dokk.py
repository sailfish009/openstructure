import glwin
from ost import gfx
from ost import io

prot=io.LoadPDB('datafiles/2CF8_prot.pdb')
prot_go=gfx.Entity('prot', prot)
scene.Add(prot_go)

surf=io.LoadSurface('datafiles/2CF8_surf','msms')
surf.Attach(prot,5)
surf_go=gfx.Surface('surf',surf)
scene.Add(surf_go)

lig=io.LoadSDF('datafiles/2CF8_lig.sdf')
lig_go=gfx.Entity('lig', lig)
lig_go.SetRenderMode(gfx.CUSTOM)
lig_go.SetColor(gfx.GREEN,'ele=C')
scene.Add(lig_go)

dokk_win=glwin.DokkGLWin()

scene.CenterOn(lig_go)

if '--fullscreen' in sys.argv:
  dokk_win.Show(fullscreen=True)
else:
  dokk_win.Show(fullscreen=False)
