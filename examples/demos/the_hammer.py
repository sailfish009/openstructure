from PyQt4 import QtCore
import math
from ost import qa

# remove all objects from scene, just in case
scene.RemoveAll()

class Anim(QtCore.QTimer):
    def __init__(self, a, b):
        QtCore.QTimer.__init__(self)
        self.a=a
        self.b=b
        self.angle=0.0
        self.dir=0.01
        self.edi=self.a.view.handle.RequestXCSEditor(mol.EditMode.UNBUFFERED_EDIT)
        QtCore.QObject.connect(self, QtCore.SIGNAL("timeout()"), self.OnTimer)

        
    def OnTimer(self):
        self.angle+=self.dir
        if self.angle>math.pi/2:
          self.dir=-self.dir
        if self.angle<0:
          self.dir=-self.dir
        rot=geom.AxisRotation(geom.Vec3(0.0, 0.0, -1.0), self.angle)
        self.edi.SetTransform(geom.Mat4(rot))
        self.edi.UpdateICS()
        for a in self.b.view.atoms:
          score=qa.ClashScore(a.handle, self.a.view)
          a.SetFloatProp('clash', score)
        self.a.UpdatePositions()
        self.b.ReapplyColorOps()


def Hammer(off=geom.Vec3()):
  ent=mol.CreateEntity()
  edi=ent.RequestXCSEditor(mol.EditMode.BUFFERED_EDIT)
  chain=edi.InsertChain("A")
  res=edi.AppendResidue(chain, "QUAD")
  prop=mol.AtomProp()
  prop.radius=1.0
  a=edi.InsertAtom(res, "A", off+geom.Vec3(0.0, 0.0, 0.0), prop)
  b=edi.InsertAtom(res, "B", off+geom.Vec3(0.0, 4.0, 0.0), prop)  
  c=edi.InsertAtom(res, "C", off+geom.Vec3(2.0, 4.0, 0.0), prop)  
  d=edi.InsertAtom(res, "D", off+geom.Vec3(2.0, 5.0, 0.0), prop)    
  e=edi.InsertAtom(res, "E", off+geom.Vec3(-2.0, 5.0, 0.0), prop)      
  f=edi.InsertAtom(res, "F", off+geom.Vec3(-2.0, 4.0, 0.0), prop)        
  edi.Connect(a, b)
  edi.Connect(b, c)
  edi.Connect(c, d)
  edi.Connect(d, e)
  edi.Connect(e, f)
  edi.Connect(f, b)    
  return ent

def TheWall():
  ent=mol.CreateEntity()
  edi=ent.RequestXCSEditor(mol.EditMode.BUFFERED_EDIT)
  chain=edi.InsertChain("A")
  res=edi.AppendResidue(chain, "QUAD")
  prop=mol.AtomProp()
  prop.radius=0.25
  index=0
  for i in range(-10, 10):
    for j in range(-10, 10):
      index+=1
      edi.InsertAtom(res, "A%d" % index, geom.Vec3(4.0, -2.0, 0.0)+
                     geom.Vec3(i, 0, j)*0.25, prop)
  return ent

a=Hammer()
b=TheWall()

a_go=gfx.Entity("a", gfx.CUSTOM, a)
b_go=gfx.Entity("b", gfx.CUSTOM, b)
for a in a.atoms:
  a.SetFloatProp('clash', 0.0)

scene.Add(a_go)
scene.Add(b_go)

anim=Anim(a_go, b_go)
anim.start(20)
grad=gfx.Gradient()
grad.SetColorAt(0.0, gfx.Color(0.0, 1.0, 0.0))
grad.SetColorAt(0.5, gfx.Color(1.0, 0.0, 1.0))
grad.SetColorAt(1.0, gfx.Color(1.0, 0.0, 0.0))
b_go.ColorBy('clash', gfx.Color(0,1,0), gfx.Color(1,0,0), 0.0, 10.0, mol.Prop.Level.ATOM)
scene.CenterOn(b_go)
print 'Demo 7: Illustrating a clash score'
