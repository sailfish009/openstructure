import glwin
from ost import io, geom, gfx, mol

class World:
  def __init__(self):
    self.atom_string=mol.CreateEntity()
    edi=self.atom_string.RequestXCSEditor(mol.EditMode.BUFFERED_EDIT)
    chain=edi.InsertChain("A")
    r=edi.AppendResidue(chain, "STRING")
    prev_atom=mol.AtomHandle()
    for i in range(-5, 5):
      atom=edi.InsertAtom(r, "X%02d" % (i+5), geom.Vec3(i, 0, 0))
      if prev_atom.IsValid():
        edi.Connect(prev_atom, atom)
      prev_atom=atom
    self.go=gfx.Entity("GO", gfx.CUSTOM, self.atom_string)
    self.go.custom_options.SetBondRad(0.1)
    self.go.custom_options.SetSphereRad(0.4)
    scene.Add(self.go)
    scene.CenterOn(self.go)
    self.last_positions=[]
    for atom in self.atom_string.atoms:
      self.last_positions.append(atom.pos)
    self.forces=[]
    for i in range(len(self.last_positions)):
      self.forces.append(geom.Vec3())
  def AddForce(self, atom, force):
    for index, aa in enumerate(self.atom_string.atoms):
      if aa==atom:
        self.forces[index]+=force
        break
  def Update(self):
    last_atom=mol.AtomHandle()
    for index, atom in enumerate(self.atom_string.atoms):
      if last_atom.IsValid():
        diff=last_atom.pos-atom.pos
        length=geom.Length(diff)
        diff/=length
        force=(1.0-length)**2*diff
        self.forces[index-1]-=force
        self.forces[index]+=force
      last_atom=atom
    edi=self.atom_string.RequestXCSEditor(mol.EditMode.BUFFERED_EDIT)
    for force, atom in zip(self.forces, self.atom_string.atoms):
      edi.SetAtomPos(atom, atom.pos+force*0.01)
    self.go.Rebuild()
    for i in range(len(self.last_positions)):
      self.forces[i]=geom.Vec3()

dokk_win=glwin.DokkGLWin()

world=World()
dokk_win.canvas_.world=world
dokk_win.Show(fullscreen=('--fullscreen' in sys.argv))
