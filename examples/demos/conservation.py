from ost.seq import alg
scene.RemoveAll()
#-------------------------------------------------------------------------------
# Loading structure and alignment
#-------------------------------------------------------------------------------
m=io.LoadPDB('data/sh2.pdb')
mp=m.Select('ishetatm=false')
aln=io.LoadAlignment('data/sh2.aln')
aln.AttachView(0, mp)
aln.SetSequenceOffset(0, 1)
#-------------------------------------------------------------------------------
# Calculate conservation of alignment
# First we set all properties to zero, then let alg.Conservation assign the
# conservation scores to the residues
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

s.Attach(mp.Select('ishetatm=false'), 8.0)

gr=gfx.Gradient()
gr.SetColorAt(0.0, gfx.Color(0.1, 0.1, 0.8))
gr.SetColorAt(1.0, gfx.Color(0.8, 0.1, 0.1))

gs.ColorBy('cons', gr, 0.8, 1.0, 
           mol.Prop.Level.RESIDUE)
g.SetRenderMode(gfx.HSC, 
                m.Select('ishetatm=false'))
g.SetRenderMode(gfx.CUSTOM, 
                m.Select('ishetatm=true'))
g.SetColor(gfx.YELLOW, 'ishetatm=true')

#-------------------------------------------------------------------------------
# Create alignment viewer and show it on the screen
#-------------------------------------------------------------------------------
seq_viewer=gui.SequenceViewer()
seq_viewer.AddAlignment(aln)
seq_viewer.Show()


