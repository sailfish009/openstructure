from ost import gfx, geom
from ost.mol import alg
from ost import mol
class Ligand:
  def __init__(self, ligand, config=None):
    self.handle=ligand
    self.solution=ligand.Copy().CreateFullView()
    try:
      show_sol = bool(int(config.Level["SHOW_SOLUTION"]))
    except KeyError:
      show_sol = True
    if show_sol:
      self.sol_go=gfx.Entity("Solution", gfx.SIMPLE, self.solution)
      self.sol_go.SetColor(gfx.GREEN, 'ele=C')
    else:
      self.sol_go = None
    gfx.Scene().Add(self.sol_go)
    self.go=gfx.Entity("Ligand", gfx.CPK, self.handle)
    self.go.SetColor(gfx.GREEN, 'ele=C')
    self.go.cpk_options.SetSphereDetail(7)
    gfx.Scene().Add(self.go)
    gfx.Scene().SetCenter(self.go.center)
    bbox=self.go.GetBoundingBox()
    self.radius=geom.Length(bbox.GetMax()-bbox.GetMin())+3.0
    self.pivot_=mol.AtomHandle()
    self.start_tf_=geom.Mat4()
    self.config = config
    self.box_max = geom.Vec3(float(self.config.Box["XMAX"]),
                            float(self.config.Box["YMAX"]),
                            float(self.config.Box["ZMAX"]))
    self.box_min = geom.Vec3(float(self.config.Box["XMIN"]),
                            float(self.config.Box["YMIN"]),
                            float(self.config.Box["ZMIN"]))
    self.Reset()
    
  def SetPivot(self, pivot):
    self.pivot_=pivot

  def GetCenter(self):
    return self.handle.GetGeometricCenter()

  def RotateAxis(self, axis, angle):
    edi=self.handle.RequestXCSEditor()
    rot=geom.Mat4(geom.AxisRotation(axis, angle))
    trans=geom.Mat4()    
    center=self.handle.GetGeometricCenter()
    if self.pivot_.IsValid():
      center=self.pivot_.GetPos()
    trans.PasteTranslation(-center)
    trans2=geom.Mat4()
    trans2.PasteTranslation(center)
    full_tf=trans2*rot*trans
    edi.ApplyTransform(full_tf)
    self.go.UpdatePositions()

  def Shift(self, vec):
    if self.__IsInside(vec):
      trans=geom.Mat4()
      trans.PasteTranslation(vec)
      edi=self.handle.RequestXCSEditor()    
      edi.ApplyTransform(trans)
      self.go.UpdatePositions()
  
  def ApplyTF(self, tf):
    edi=self.handle.RequestXCSEditor()
    rot = geom.Mat4(tf.GetRot())
    trans=geom.Mat4()
    center=self.handle.GetGeometricCenter()
    if self.pivot_.IsValid():
      center=self.pivot_.GetPos()
    trans.PasteTranslation(-center)
    trans2=geom.Mat4()
    trans2_vec = ((tf.GetTrans())*gfx.Scene().GetTransform().GetRot())
    if self.__IsInside(trans2_vec):
      trans2_vec = center + trans2_vec
      trans2.PasteTranslation(trans2_vec)
      full_tf = trans2*rot*trans
      edi.ApplyTransform(full_tf)
      self.go.UpdatePositions()


  def RMSDToSolution(self):
    return alg.CalculateRMSD(self.handle.CreateFullView(), 
                             self.solution)

  def Solve(self):
    edi=self.handle.RequestXCSEditor()
    edi.SetTransform(self.start_tf_)
    self.go.UpdatePositions()
    
  def Reset(self):
    if self.config is not None:
      edi=self.handle.RequestXCSEditor()
      shift_vec = geom.Vec3(float(self.config.start["POSX"]),
                            float(self.config.start["POSY"]),
                            float(self.config.start["POSZ"]))
      transf = mol.Transform()
      transf.SetTrans(shift_vec)
      edi.SetTransform(transf.GetMatrix())
      self.RotateAxis(geom.Vec3(1,0,0), float(self.config.start["ROTX"]))
      self.RotateAxis(geom.Vec3(0,1,0), float(self.config.start["ROTY"]))
      self.RotateAxis(geom.Vec3(0,0,1), float(self.config.start["ROTZ"]))
      self.go.UpdatePositions()
    
  def Close(self):
    if self.sol_go:
      gfx.Scene().Remove(self.sol_go)
    gfx.Scene().Remove(self.go)
    del(self.go)
    
  def __IsInside(self, vec):
    center = self.GetCenter()
    rad = 1
    if center[0] + rad + vec[0] < self.box_max[0] and center[0] - rad + vec[0] > self.box_min[0] and \
     center[1] + rad + vec[1] < self.box_max[1] and center[1] - rad + vec[1] > self.box_min[1] and \
     center[2] + rad + vec[2] < self.box_max[2] and center[2] - rad + vec[2] > self.box_min[2] :
      return True
    return False