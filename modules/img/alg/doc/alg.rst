:mod:`~ost.img.alg` - Image Processing Algorithms
================================================================================

.. module:: ost.img.alg
  :synopsis: Image processing algorithms
  
Applying algorithms
-------------------

While image properties are usually manipulated using method of the :class:'~ost.img.ImageHandle` class, their data content
is manipulated using image algorithms. Image algorithms are objects. Each of them is a class, and its methods are used to handle the algorithm parameters. Applying an algorithm to an image is then conceptually a two-step process. First, an instance of an algorithm class is created, yielding an algorithm object. In a second step, the algorithm object is applied to an image. An algorithm can be applied in-place (using the :meth:`ost.img.ImageHandle.Apply` method), modifying the image. or out-of-place, (using :meth:`~ost.img.ImageHandle.ApplyIP` ), leaving the original image untouched, and returning the result as a new image. 

Here is an example that uses the :class:`Randomize` algorithm. This algorithm feels the pixels of the image with random 
values between 0.0 and 1.0

.. code-block:: python
 
  # creates an algorithm object
  rand_alg = img.alg.Randomize() 
  # applies algorithm object in place, overwriting the image
  im.ApplyIP( rand_alg )

Now that we have some (noisy) data present, let us run another algorithm, this time a Gaussian filter with a sigma of 4 pixel.

im.ApplyIP( img.alg.GaussianFilter(4.0) ) # apply temporary algorithm object in-place
As you can see, it is not always necessary to create an independent algorithm instance first, in many cases a temporary object will suffice (this applies to the randomization algorithm as well, 'im.ApplyIP(alg.Randomize())' would have been fine). However, when used this way, the algorithm class will cease to exist as soon as the algorithm is applied. This can be important if the algorithm stores some values that need to be recovered later. For example:

stat=img.alg.Stat()
im.ApplyIP(stat)
mean=stat.GetMean()
Algorithms are stateful objects and can store values. The 'Stat' algorithm computes basic statistics about the image it is applied on (maximum and minimum values, standard deviations, etc). The data are stored within the algorithm instance and can be recovered using the algorithm's methods. It would obviously make very little sense not to create an instance of the 'Stat' algorithm. When the algorithms ceases to exist, all information would be lost.

Applying a Fourier Transform

An image is Fourier-transformed using the 'img.alg.FFT()' algorithm object:

im=io.LoadImage("imagename.tif") # load the image
# create an instance of the fft algorithm object
fft=img.alg.FFT() 
# do the actual Fourier transformation
im_ft=im.Apply(fft) 
# back-transform
im2 = im_ft.Apply(fft) 
# if this is run from within the dng graphical frontend, open viewers to look at the images
gui.CreateDataViewer(im)
gui.CreateDataViewer(im_ft)
gui.CreateDataViewer(im2)
It is not really necessary to use the 'fft' variable to store the 'im.alg.FFT()' instance, a temporary object can be used, since the 'FFT' algorithm object is stateless. In addition, the algorithm can be applied in-place to avoid the creation of a second image:

im=io.LoadImage("imagename.tif") # load the image
# do the actual Fourier transformation, in-place using temporary object
im.ApplyIP(alg.FFT()) 
# repeating this command will do the back-transform
im.ApplyIP(alg.FFT())
As said before, the 'alg.FFT()' algorithm does not require a direction to be given, this is implicitly determined by the active domain of the underlying image state: a 'SPATIAL' image will always be transformed to the 'FREQUENCY' domain, and vice-versa.
















































Usage of Image Algorithms
--------------------------------------------------------------------------------

Image algorithms are objects. To execute them, the algorithms are applied to an 
image by passing it to the :meth:`ost.img.ImageHandle.Apply` or 
:meth:`ost.img.ImageHandle.ApplyIP` method:

.. code-block:: python
  
  image=img.CreateImage(img.Size(200, 200))
  fft_image=image.Apply(img.alg.FFT())
  image.ApplyIP(img.alg.FFT())
  
  
.. class:: FFT

  Fast Fourier Transforms the image. The FFT algorithms is aware of the 
  image's domain. The following rules apply:
  
   * SPATIAL -> HALF_FREQUENCY
   * HALF_FREQUENCY -> SPATIAL
   * FREQUENCY -> COMPLEX_SPATIAL
   * COMPLEX_SPATIAL -> FREQUENCY

.. class:: GaussianFilter(sigma=1.0)

  Applies a gaussian filter to the supplied image. Sigma is given in pixels.

  Implemented after I.T.Young, L.J. van Vliet,"Recursive implementation of the
  Gaussian filter", Signal Processing, 44(1995), 139-151

Filters in Fourier Space
--------------------------------------------------------------------------------

The following filters operate in Fourier Space. If the image they are applied on is in spatial domain, they will first be converted to frequency domain and then converted back after the filter has been applied.
  
.. class:: LowpassFilter(freq_limit)

  Filters an image by masking out frequencies higher than
  `freg_limit`.

  .. method:: GetLimit()
    
    Returns the frequency limit
    
  .. method:: SetLimit(freq)
    
    Set the frequency limit
    
.. class:: HighpassFilter
 
  Filters an image by masking out frequences lower than `freq_limit`
  
  .. method:: GetLimit()
    
    Returns the frequency limit
    
  .. method:: SetLimit(freq)
    
    Set the frequency limit
  