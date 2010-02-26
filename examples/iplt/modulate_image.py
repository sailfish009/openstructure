import math
import ost.iplt.alg

class modulator(iplt.RealFunction):
    def __init__(self,f):
        iplt.RealFunction.__init__(self)
        self.f = f
        
    def Func(self,point):
        return math.sin(point[0]*self.f)*math.sin(point[1]*self.f)

im = iplt.CreateImage(iplt.Size(400,200))
im.ApplyIP(iplt.alg.Randomize())

im2 = im * modulator( 0.1 )

im2.SetSpatialOrigin(iplt.Point(0,200))

im3 = iplt.CreateImage(iplt.Size(400,400))
im3.Paste(im)
im3.Paste(im2)

v=gui.CreateDataViewer(im3)