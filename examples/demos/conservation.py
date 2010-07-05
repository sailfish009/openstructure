from ost.seq import alg
#-------------------------------------------------------------------------------
# Loading structure and alignment
#-------------------------------------------------------------------------------
m=io.LoadPDB('data/sh2.pdb')
mp=m.Select('ishetatm=false')
aln=io.LoadAlignment('data/sh2.aln')
aln.AttachView(0, mp)
#-------------------------------------------------------------------------------
# Calculate conservation of alignment
#-------------------------------------------------------------------------------
for r in m.residues:
  r.SetFloatProp('cons', 0)
alg.Conservation(aln)
#-------------------------------------------------------------------------------
# Setup Graphical Objects for Rendering
#-------------------------------------------------------------------------------
g=gfx.Entity('SH2', m)
s=io.LoadSurface('data/sh2.vert')
gs=gfx.Surface('SH2-surf', s)
scene.Add(gs)
scene.Add(g)
scene.CenterOn(g)

s.Attach(mp.Select('ishetatm=false'), 5.0)

gr=gfx.Gradient()
gr.SetColorAt(0.0, gfx.BLUE)
gr.SetColorAt(0.5, gfx.WHITE)
gr.SetColorAt(1.0, gfx.RED)
gs.ColorBy('cons', gr, 0.8, 1.0, 
           mol.Prop.Level.RESIDUE)
g.SetRenderMode(gfx.HSC, 
                m.Select('ishetatm=false'))
g.SetRenderMode(gfx.CUSTOM, 
                m.Select('ishetatm=true'))
g.SetColor(gfx.YELLOW, 'ishetatm=true')


seq_viewer=gui.SequenceViewer()
seq_viewer.AddAlignment(aln)
seq_viewer.Show()

print 'Colouring the active site according to the level of conservation found inthe sequences shown in the sequence viewer. Highly conserved regions are shown in blue, variable regions in red.'
