import glwin
from ost import io, geom, gfx

from level import Level

dokk_win=glwin.DokkGLWin()


level=Level('thrombin')
dokk_win.SetLevel(level)
dokk_win.Show(fullscreen=('--fullscreen' in sys.argv))

