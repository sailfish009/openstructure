import sys
import ost.img.alg
if len(sys.argv)<2:
images=io.LoadImageList(['square.png'])  
else:
  images=io.LoadImageList(sys.argv[1:])
viewers=[]
i=1
for im in images:
   im.CenterSpatialOrigin()
   im.ApplyIP(img.alg.DFT()) 
   v=gui.CreateDataViewer(im)
   v.SetName(sys.argv[i])
   viewers.append(v)
   i+=1
