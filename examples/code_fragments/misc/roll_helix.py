import math

e=io.LoadEntity('../entity/sdh.pdb')
c=e.FindChain('A')

edi=e.EditICS(mol.EditMode.BUFFERED_EDIT)
for r in c.GetResidueList():
  phi=r.GetPhiTorsion()
  psi=r.GetPsiTorsion()
  if phi.IsValid(): 
    edi.SetTorsionAngle(phi, math.radians(-60))    
  if psi.IsValid(): 
    edi.SetTorsionAngle(psi, math.radians(-45))    
  
edi.UpdateXCS()

for r in c.GetResidueList():
  phi=r.GetPhiTorsion()
  psi=r.GetPsiTorsion()
  if phi.IsValid(): 
    print 'PHI %3.0f' % math.degrees(phi.GetAngle())
  if psi.IsValid(): 
    print 'PSI %3.0f'  % math.degrees(psi.GetAngle())

g=gfx.Entity('helix', e.Select('aname=CA,N,O,C,CB'))
#g=gfx.Entity('helix', e)
scene.Add(g)
scene.SetCenter(g.GetCenter())



