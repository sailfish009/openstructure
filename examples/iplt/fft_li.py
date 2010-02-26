import sys
import ost.iplt.alg

images=io.LoadImageList(sys.argv[1:])
viewers=[]
i=1
for im in images:
   im.CenterSpatialOrigin()
   im.ApplyIP(iplt.alg.DFT()) 
   v=gui.CreateDataViewer(im)
   v.SetName(sys.argv[i])
   viewers.append(v)
   i+=1
