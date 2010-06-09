:mod:`~ost.img.alg` - Image Processing Algorithms
================================================================================

.. module:: ost.img.alg
  :synopsis: Image processing algorithms
  
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
  