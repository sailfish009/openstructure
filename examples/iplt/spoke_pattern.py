import math
import sys
import ost.iplt.alg

if len(sys.argv)!=2:
  raise RuntimeError('Wrong number of command line arguments. The name of the output file should be the only argument.')

# Tweakable parameters
wedge_start = 80 # Width of wedge at the narrower point
wedge_end = 2000 # Width of wedge at the broader point
number_of_bands = 11 # Number of alternating bands (must be odd)
size_of_image_x = 2000 # Size of the image (X)
size_of_image_y = 2000 # Size of the image (Y)
pixel_sampling = 1.0 * Units.A # Pixel width
threshold = 2.0 * Units.A # Threshold for low pass filtering
amplitude = 255 # amplitude of the obscillations

# Image is created
image=iplt.CreateImage(iplt.Size(size_of_image_x,size_of_image_y))
image.CenterSpatialOrigin()
image.SetSpatialSampling(pixel_sampling)

# For a cleaner code, 4 shorthands are defined
image_extent=image.GetExtent()
start_y=image_extent.GetStart()[1]
end_y=image_extent.GetEnd()[1]
start_x=image_extent.GetStart()[0]
end_x=image_extent.GetEnd()[0]

# Wedge is written in the image
for y in range (start_y,end_y+1):
  wedge_width=wedge_start+(y-start_y)*(wedge_end-wedge_start)/(end_y-start_y)
  for x in range (start_x,end_x+1):
    half_wedge=wedge_width/2.0
    outer_bands=(number_of_bands-1)/2
    factor=(0.5+outer_bands)*math.pi/half_wedge
    if float(x)>-half_wedge and float(x)<half_wedge:
      value=255*math.cos(float(x)*factor)
      image.SetReal(iplt.Point(x,y),value)

# Image is low-pass filtered
filter=ost.iplt.alg.GaussianLowPassFilter(threshold)
image.ApplyIP(filter)

# Image is saved
io.SaveImage(image,sys.argv[1])

# Viewer is launched to show the result
v=gui.CreateDataViewer(image)