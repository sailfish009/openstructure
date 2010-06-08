:mod:`img` Images and Density Maps
================================================================================
  
.. module:: mol
   :synopsis: Images and Density Maps
              
Introduction : The ImageHandle
--------------------------------------------------------------------------------

OpenStructure offers extensive processing capabilities for planar 2d images and
3d maps using the img module. Images are manipulated through the use of
dox[ost::img::ImageHandle|ImageHandles].

ImageHandles provide a clean and efficient interface to interact with images and 
maps. An :class:`ImageHandle` can store an image in either real ('SPATIAL') or 
Fourier ('FREQUENCY') space and always keep track of the currently active 
domain. This means,for example that one can apply a Fourier Transformation to an 
ImageHandle containing a 'SPATIAL' image and the ImageHandle will correctly 
identify the new active domain as 'FREQUENCY'. The ImageHandle also understands, 
for example, that applying a Fourier Transform to a centrosymmetric 'FREQUENCY' 
image results in a real 'SPATIAL' image, but applying it to a 
non-centrosymmetric one results in a complex 'SPATIAL' image.

Furthermore, the ImageHandle will make sure that real and Fourier space 
information about the image are always in sync. If, for example, the pixel 
sampling is changed while the current active domain is real space, the pixel 
sampling in Fourier space will be adjusted accordingly, and vice versa.

Moreover, the ImageHandle allows the extraction of both complex and real numeric 
values from images in any active domain. If the domain is complex in nature, but 
a real numeric value is requested, the amplitude of the complex number will be 
returned. If the numerical nature of the domain is real and a complex number is 
requested, the complex part will be set to 0.

Creating  and visualizing ImageHandles
--------------------------------------------------------------------------------
As a first step, enter the following lines in the OpenStructure python console:

  .. code-block:: python
  
    im=img.CreateImage(img.Size(200,200))

This will create an empty, 2D image, with a height and width of 200 pixels, whose
origin (ie the pixel with the coordinates <0,0>) is in the top-left corner.

  .. code-block:: python
  
    v=gui.CreateDataViewer(im)

A viewer window will pop up (see below), showing a white frame on a black background.
The inner area of the white frame is the image, which is empty.

Reading and writing into an image
-------------------------------------------------------------------------------

Data can be read and written from and into an image using the following commands:

  .. code-block:: python
  
    # writes the real value 23.4 into pixel 10,10
    im.SetReal(img.Point(10,10),23.4)
    # reads the value in pixel 10,10
    val=im.GetReal(img.Point(10,10))
    
The complex equivalents are also available    

  .. code-block:: python
  
    # writes the complex value value 2+3j into pixel 10,10
    im.SetComplex(img.Point(10,10),2+3j)
    # reads the value in pixel 10,10
    val=im.GetComplex(img.Point(10,10))

The image knows in which domain it is, and will adjust the type of data being written
accordingly. For example, if one writes a complex value in a 'SPATIAL' image, the value
will be automatically converted to a real one by taking the amplitude of the complex number
On the other hand, if one writes a real value in a 'FREQUENCY' image, the value is automatically
converted to complex by setting the imaginary part to 0.

## Applying algorithms

Let us fill the image with random values.

  .. code-block:: python
  
    rand_alg = img.alg.Randomize() # create algorithm object
    im.ApplyIP( rand_alg ) # apply algorithm object in-place

As you can see, applying an algorithm is conceptually a two-step process. First,
an instance of an algorithm class is created, yielding an algorithm object (in 
this case 'rand\_alg'). In a second step, the algorithm object is applied to an 
image, either in-place, modifying the image, or out-of-place, leaving the 
original image untouched, and returning the result as a new image. Note that the 
in-place/out-of-place logic is decoupled from the algorithm object.

Now that we have some (noisy) data present, let us run another algorithm, this 
time a Gaussian filter with a sigma of 4 pixel.

  .. code-block:: python
  
    im.ApplyIP( img.alg.GaussianFilter(4.0) ) # apply temporary algorithm object in-place

As you can see, it is not always necessary to create an independent algorithm 
instance first, in many cases a temporary object will suffice (this applies to 
the randomization algorithm as well, 'im.ApplyIP(alg.Randomize())' would have 
been fine). However, when used this way, the algorithm class will cease to exist 
as soon as the algorithm is applied. This can be important if the algorithm 
stores some values that need to be recovered later. For example:

  .. code-block:: python
  
    stat=img.alg.Stat()
    im.ApplyIP(stat)
    mean=stat.GetMean()
    
Algorithms are stateful objects and can store values. The 'Stat' algorithm 
computes basic statistics about the image it is applied on (maximum and minimum 
values, standard deviations, etc). The data are stored within the algorithm 
instance and can be recovered using the algorithm's methods. It would obviously 
make very little sense not to create an instance of the 'Stat' algorithm. When 
the algorithms ceases to exist, all information would be lost.

Applying a Fourier Transform
--------------------------------------------------------------------------------

An image is Fourier-transformed using the 'img.alg.FFT()' algorithm object:

  .. code-block:: python
  
    im=io.LoadImage("imagename.tif") # load the image
    # create an instance of the fft algorithm object
    fft=img.alg.FFT() 
    # do the actual Fourier transformation
    im_ft=im.Apply(fft) 
    # back-transform
    im2 = im_ft.Apply(fft) 
    # if this is run from within the dng graphical frontend, open viewers to
    # look at the images
    gui.CreateDataViewer(im)
    gui.CreateDataViewer(im_ft)
    gui.CreateDataViewer(im2)
 
It is not really necessary to use the 'fft' variable to store the 'im.alg.FFT()' 
instance, a temporary object can be used, since the 'FFT' algorithm object is stateless. In addition, the algorithm can be applied in-place to avoid the 
creation of a second image:

  .. code-block:: python
  
    im=io.LoadImage("imagename.tif") # load the image
    # do the actual Fourier transformation, in-place using temporary object
    im.ApplyIP(alg.FFT()) 
    # repeating this command will do the back-transform
    im.ApplyIP(alg.FFT()) 

As said before, the 'alg.FFT()' algorithm does not require a direction to be given, this is implicitly
determined by the active domain of the underlying image state: a 'SPATIAL' image will always be
transformed to the 'FREQUENCY' domain, and vice-versa.

Extracting and Pasting Images
--------------------------------------------------------------------------------

An image can be extracted and pasted into another image using the 'Extract()' 
and 'Paste()' member functions:

  .. code-block:: python
  
    # load the image
    im=io.LoadImage("imagename.tif")
    # generate a subimage from the region going from (10,10) to (30,30)
    im2=im.Extract(img.Extent(img.Point(10,10),img.Point(30,30)))
    # generate an empty image with the same size as the original image
    im3=img.CreateImage(im.GetExtent())
    # paste the subimage into the empty image
    im3.Paste(im2)

Note that the extent is fully honored for the paste operation, i.e. only the
region where the pasted-to and the pasted-in image overlap will be affected.



  ..
     |                                                   |                                                                     |                                                                          |
     |:-------------------------------------------------:|:-------------------------------------------------------------------:|:------------------------------------------------------------------------:|
     |![Empty Image] (docs/tut/dv1.jpg "Empty Image")    | ![After Randomization] (docs/tut/dv2.jpg "After Randomization")     | ![After Gaussian Filtering] (docs/tut/dv3.jpg "After Randomization")  |
     |Empty Image                                        | After Randomization                                                 | After Gaussian Filtering                                                 |
     