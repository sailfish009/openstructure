from ost import gfx
class Protein:
  def __init__(self, prot):
    self.handle=prot
    for a in self.handle.atoms:
      a.SetGenericFloatProperty('clash', 0.0)    
    self.prot_go_=gfx.Entity("Prot", self.handle)
    gfx.Scene().Add(self.prot_go_)