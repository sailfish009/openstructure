import glwin
from ost import gfx
from ost import io
sdh=io.LoadPDB('../entity/sdh.pdb')
go=gfx.Entity("sdh", sdh)
scene.Add(go)

dokk_win=glwin.DokkGLWin()

scene.CenterOn(go)

if '--fullscreen' in sys.argv:
  dokk_win.Show(fullscreen=True)
else:
  dokk_win.Show(fullscreen=False)