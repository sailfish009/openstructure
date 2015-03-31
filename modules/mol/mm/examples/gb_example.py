from ost.mol import mm
from PyQt4 import QtCore

class Anim(QtCore.QTimer):
    def __init__(self,sim,ent,go):
        QtCore.QTimer.__init__(self)
        self.ent=ent
        self.sim=sim
        self.go = go
        self.ed = ent.EditXCS(ost.mol.BUFFERED_EDIT)
        QtCore.QObject.connect(self, QtCore.SIGNAL("timeout()"), self.OnTimer)

        
    def OnTimer(self):
        self.sim.Steps(5)
        positions = sim.GetPositions()
        for a, pos in zip(self.ent.atoms,positions):
          self.ed.SetAtomPos(a,pos)
        self.ed.UpdateICS()
        self.go.UpdatePositions()


prot=ost.mol.CreateEntityFromView(io.LoadPDB('1CRN.pdb').Select('peptide=true'),True)

settings = mm.MMSettings()

settings.integrator = mm.LangevinIntegrator(310,1,0.002)
settings.constrain_bonds = True
settings.constrain_hbonds = True
settings.constrain_hangles = True
settings.add_gbsa = True
#settings.add_thermostat = True
#settings.thermostat_temperature = 310.0
#settings.thermostat_collision_frequency = 1
settings.init_temperature = 0
settings.forcefield = mm.LoadCHARMMForcefield()
settings.platform = mm.Platform.CPU
#settings.nonbonded_cutoff = 10.0
settings.nonbonded_method = mm.NonbondedMethod.CutoffNonPeriodic

sim = mm.Simulation(prot,settings)
sim.ApplyLBFGS(tolerance = 1.0, max_iterations = 200)
sim.UpdatePositions()

ent = sim.GetEntity()
go = gfx.Entity("crambin",ent)
scene.Add(go)

anim = Anim(sim,ent,go)
anim.start(1)

