from ost import gfx
class Protein:
  def __init__(self, prot):
    self.handle=prot
    for a in self.handle.atoms:
      a.SetGenericFloatProperty('clash', 0.0)    
    self.prot_go_=gfx.Entity("Prot", self.handle)
    gfx.Scene().Add(self.prot_go_)
    
  def Close(self):
    gfx.Scene().Remove(self.prot_go_)
    del(self.prot_go_)