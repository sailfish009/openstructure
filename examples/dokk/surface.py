from ost import gfx
from ost import mol
class Surface:
  def __init__(self, surf):
    self.handle=surf
    self.go=gfx.Surface("Surf", self.handle)

    gfx.Scene().Add(self.go)
    grad=gfx.Gradient()
    grad.SetColorAt(0.0, gfx.Color(1.0, 1.0, 1.0))
    grad.SetColorAt(0.7, gfx.Color(1.0, 1.0, 0.0))
    grad.SetColorAt(1.0, gfx.Color(1.0, 0.0, 0.0))    
    self.go.ColorBy('clash', grad, 0.0, 10.0, mol.Prop.Level.ATOM)        

  def Close(self):
    gfx.Scene().Remove(self.go)
    del(self.go)
