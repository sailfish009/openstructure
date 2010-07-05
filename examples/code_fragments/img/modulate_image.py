import math
import ost.img.alg

class modulator(img.RealFunction):
    def __init__(self,f):
        img.RealFunction.__init__(self)
        self.f = f
        
    def Func(self,point):
        return math.sin(point[0]*self.f)*math.sin(point[1]*self.f)

im = img.CreateImage(img.Size(400,200))
im.ApplyIP(img.alg.Randomize())

im2 = im * modulator( 0.1 )

im2.SetSpatialOrigin(img.Point(0,200))

im3 = img.CreateImage(img.Size(400,400))
im3.Paste(im)
im3.Paste(im2)

v=gui.CreateDataViewer(im3)
main_area=gui.GostyApp.Instance().perspective.main_area
main_area.AddWidget("Modulated Image", v)