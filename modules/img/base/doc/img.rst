:mod:`~ost.img` Images and Density Maps
================================================================================
  
.. module:: ost.img
   :synopsis: Images and density maps
              
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

Image properties
-----------------

Every 2D or 3D image in OpenStructure has some basic properties, which are discussed in detail in the next paragraphs.

Point
^^^^^^^^

This class represents an image pixel. It is defined using three integer numbers, corresponding to the pixel’s indexes along the weight and depth dimensions respectively. For 2-dimensional images, the 3rd index can be omitted.

  .. class:: ost.img.Point(x=0, y=0, z=0)¶
  .. class:: ost.img.Point(x=0, y=0)

Size
^^^^^^^^

This property describes the size of an image. It is defined using three integer numbers: When depth is not specified, the size is assumed to refer to a 2D image two-dimensional (depth=1).

  .. class:: ost.img.Size(width, height, depth)¶
  .. class:: ost.img.Size(width, height)

The Size class can be passed to the CreateImage function to define the size of the image being created.

  .. code-block:: python 

    # uses the size class to create an image
    s=img.Size(40,20,30)
    i=img.CreateImage(s)
    # uses a temporary instance of the Size class
    i=img.CreateImage(img.Size(40,20,30))

Extent
^^^^^^^^

This property describes the extent of an image in pixels. The image extent is a rectangle in 2d and a cuboid in 3d, and can be defined in a number of ways: giving the first and the last point of the extent, specifying the first point and the size of the extent, or listing the size of the extent and its central point.

.. code-block:: python 

  .. class:: ost.img.Extent(first and last point)¶
  .. class:: ost.img.Extent(first point, size)
  .. class:: ost.img.Extent(size, central point)

Some examples:

  .. code-block:: python 

    # defines a rectangular extent that goes from pixel (2,4) to pixel (5,10)  
    s=img.Extent(img.Point(2,4),img.Point(5,10))
    # defines the same extent using its first point and its size
    s=img.Extent(img.Point(2,4),img.Size(4,7))
    # again the same extent using its size and central pixel
    s=img.Extent(img.Size(4,7),img.Point(3,7))

The Extent class can be passed to the CreateImage function to create an image with a specific extent.

  .. code-block:: python 

    # uses the Extent class to create an image
    e=img.Extent(img.Point(2,4),img.Size(4,7))
    i=img.CreateImage(e)
    # uses a temporary instance of the Extent class
    i=img.CreateImage(img.Extent(img.Point(2,4),img.Size(4,7)))

Given an Extent, is it possible to recover its full size, and also the length of each of its dimensions separately.

Examples:

  .. code-block:: python 

    # gets the size of an extent (e is a 3D extent)
    s=e.GetSize()
    # gets the three dimensions separately
    w=e.GetWidth()
    h=e.GetHeight()
    d=e.GetDepth()

When one needs to visit all the image pixels belonging to an Extent in sequence, one can use an ExtentIterator:

  .. code-block:: python 

    # defines an iterator over an extent e
    ei=ExtentIterator(e)
    # visits all the pixels in the extent and
    # prints out their values
    for pixel in ei:
      print i.GetReal(pixel)

.. _spatial_point

DataType
^^^^^^^^

The DataType of an image represents the nature of the data it contains. An image can contain 'REAL' or 'COMPLEX' values.

.. _data-domain

DataDomain
^^^^^^^^^^

The DataDomain of an image specifies wether the image contains data in the 'SPATIAL' domain or in the 'FREQUENCY' domain. A 'HALF_FREQUENCY' domain also exists,
representing centrosymmetric frequency data (such as the data coming from the Fourier transform of an image from the real spatial domain)

SpatialOrigin
^^^^^^^^^^^^^^

Type: :class:`Point`

The Spatial Origin of an image is the first pixel of its extent. Specifically, this is the top left pixel for 2D images and the top-top-left for 3D images.

.. _absolute-origin

AbsoluteOrigin
^^^^^^^^^^^^^^^^

Type: :class:`~ost.geom.Vec3`

The Absolute Origin of an image describes the coordinates, in the absolute reference system used by OpenStructure, of the pixel in with all indexes equal to 0. Please notice that the pixel does not necessarily need to belong to the extent of the image.

.. _pixel-sampling

PixelSampling
^^^^^^^^^^^^^^^^^

Type: :class:`~ost.geom.Vec3`

The PixelSampling property of an image describes the size of its pixels. For the same image, the size of pixels in the 'SPATIAL' and in the 'FREQUENCY' :ref:`data-domain` are obviously interdependent. OpenStructure takes care of the transformation and allows access to both pixel sampling irrespective of the current image domain.

.. _data-type




ImageHandle
-----------

The public interface of the ImageHandle class provides many ways to manipulate image properties. What follows is a brief description of the most important methods of the
ImageHandle class.

.. class:: ImageHandle

  .. method:: Apply(algorithm)

     Applies an algorithm on an image and returns a new ImageHandle containing the modified image. See :doc:`doc/img/alg` 

     :param algorithm: algorithm
     :type  algorithm: Instance of an algorithm class (:doc:`doc/img/alg`)
     :rtype: :class:`ImageHandle`

  .. method:: ApplyIP(algorithm)

     Applies an algorithm on an image, overwriting the current content. See :doc:`doc/img/alg` 

     :param algorithm: algorithm
     :type  algorithm: Instance of an algorithm class (:doc:`doc/img/alg`)

  .. method:: CenterSpatialOrigin()

     Sets the :ref:`spatial-origin` of an image in such a way that the central pixel has all 0 indexes, e.g. (0,0) or (0,0,0) for 3d images 
    
  .. method:: CoordToIndex(coord)

     Returns the indexes of an image corresponding to the specified absolute coordinates. (See :ref:`absolute-origin'). A given set of absolute coordinates will almost never fall
     exactly at the center of a pixel, so this method return fractional indexes. 

     :param coord: Absolute coordinates
     :type  coord: :class:`~ost.geom.Vec3`
     :rtype: Vec3

  .. method:: Copy()
     
     Creates a and returns a copy of the ImageHandle. The new handle does not point to the same underlying data as the old one, but to a complete and separate copy of the
     original data.

     :rtype: :class:`ImageHandle`

  .. method:: Extract(extent)
     
     Creates and returns a new image that contains a copy of a portion of the original image. The extracted image keeps the same :ref:`data-type` of the original image,
     but extractions from images in the 'FREQEUNCY' or 'HALF FREQUENCY' domains result in 'COMPLEX' 'SPATIAL' images. This transformation is necessary, since the there is no
     guarantee that the extracted 'FREQUENCY' sub-image is centered around the origin and hence back-transformable to 'SPATIAL'.

     :param extent:
     :type  extent: :class:`Extent`
     :rtype: :class:`ImageHandle`

  .. method:: FractionalIndexToCoord(frac_pixel)

     Same as :method:`IndexToCoord(pixel)`, but introduces subpixel precision by accepting fractional numbers for pixel indexes.
     
     :param frac_pixel: Fractional pixel indexes
     :type  frac_pixel: :class:`~ost.geomVec3`
     :rtype: :class:`~ost.geom.Vec3`

  .. method:: GetAbsoluteOrigin()
    
     Returns the :ref:`absolute-origin` of an image

     :rtype: :class:`~ost.geom.Vec3`

  .. method:: GetComplex(pixel)

     Returns the complex value of the specified image pixel. If the image holds data of the 'REAL' :ref:`data_type`, the method return s complex value with the pixel content
     as real part and a null imaginary part.     

     :param pixel:
     :type  pixel: :class:`Point`
     :rtype: complex

  .. method:: GetDomain()
    
     Returns the :ref:`data-domain` of an image ('SPATIAL', 'FREQUENCY' or 'HALF_FREQUENCY')  

     :rtype: DataDomain     ???????????

  .. method:: GetExtent()

     Returns the :class:`Extent` of an image.

     :rtype: class:`Extent`

  .. method:: GetFrequencySampling()
     
     Returns the :ref`pixel-sampling` of the image in the 'FREQUENCY' :ref:`DataDomain`
       
     :rtype: :class:`~ost.geom.Vec3`

  .. method:: GetIntpolComplex(frac_pixel)

     Returns the interpolated complex value of the virtual pixel corresponding to the specified fractional indexes. This is computed by calculating a weighted vector sum of 
     the values of the surrounding pixels. If the image holds data of the 'REAL' :ref:`data_type`, the method computes the interpolated value using bilinear interpolation (trilinear
     for 3D images), then returns a complex value with the interpolated value as real part and a null imaginary part.

     :param frac_pixel: Fractional pixel indexes
     :type  frac_pixel: :class:`~ost.geom.Vec3`
     :rtype: complex

  .. method:: GetIntpolReal(frac_pixel)

     Returns the interpolated value of the virtual pixel corresponding to the specified fractional indexes. This is computed by using bilinear interpolation (trilinear
     for 3D images). If the image holds data of the 'COMPLEX' :ref:`data_type`, the method computes the interpolated value as a weighted vector sum of 
     the values of the surrounding pixels, then returns the amplitude of the interpolated value. 

     :param frac_pixel: Fractional pixel indexes
     :type  frac_pixel: :class:`~ost.geom.Vec3` 
     :rtype: float

  .. method:: GetPixelSampling()
   
     Returns the :ref:`pixel-sampling` of the image in the current :ref:`data-domain`.

     :rtype: :class:`~ost.geom.Vec3`

  .. method:: GetReal(pixel)

     Returns the value of the specified image pixel. If the image holds data of the 'COMPLEX' :ref:`data_type`, the method return the amplitude of the pixel content.

     :param arg2:
     :type  arg2: :class:`Point`
     :rtype: float

  .. method:: GetSize()

     Returns the :class:`Size` of the image.

     :rtype: :class:`Size`

  .. method:: GetSpatialOrigin()

     Returns the :ref:`spatial-origin` of the image.

     :rtype: :class:`Point`

  .. method:: GetSpatialSampling()

     Return the :ref:`pixel-sampling' of the image in the 'SPATIAL' :ref:`data-domain`.

    :rtype: :class:`~ost.geom.Vec3`

  .. method:: GetType()
     
     Returns the :ref:`data-type` of the image ('REAL' or 'COMPLEX')
  
     :rtype: DataType  ???????????

  .. method:: IndexToCoord(pixel)
 
     Returns the absolute coordinates (See :ref:`absolute-origin`) corresponding to the pixel with the specified indexes. Please note this method always returns the coordinates
     corresponding to the center of the pixel. When sub-pixel precision is needed, the method :method:`eFractionalIndexToCoord(frac_Pixel) can be used.
    
    :param pixel:
    :type  pixel: :class:`Point`
    :rtype: :class:`~ost.geom.Vec3`

  .. method:: IsFrequency()

     Returns true if the :ref:`data-domain` of the image is 'FREQUENCY` or `HALF-FREQUENCY'     

     :rtype: bool

  .. method:: IsSpatial()

     Returns true if the :ref:`data-domain` of the image is 'SPATIAL'     

     :rtype: bool

  .. method:: IsValid()

     ??????????? 

    :rtype: bool

  .. method:: Paste(source_image)
   
     Copies the content of a different image into the current one, overwriting pre-existing data . The values of pixels with given indexes in the source image are copied into pixels
     with the same indexes in the target image. If the two images have different extents (see :class:`Extent`), pixels that do not exist in both are ignored. Please notice
     that this method only copies the pixel content: all other properties of the image are left untouched. 

     :param source_image:
     :type  data: :class:`ImageHandle`

  .. method:: SetAbsoluteOrigin(coord)
  
     Sets the :ref:`absolute-origin` of the image to the specified coordinates  
  
     :param coord:
     :type  coord: :class:`~ost.geom.Vec3`
 
  .. method:: SetComplex(pixel, value)

     Sets the content of the specified pixel to the provided value.  If the image holds data of the 'REAL' :ref:`data_type`, the method sets the pixel to the amplitude of the provided
     value.

     :param pixel:
     :type  pixe;: Point
     :param value:
     :type  value: complex
 
  .. method:: SetPixelSampling(sampling)

     Sets the :ref:`pixel-sampling` of the image to the provided values in the current :ref:`data-domain`. 

     :param sampling:
     :type  sampling: :class:`~ost.geom.Vec3`

  .. method:: SetReal(pixel, value)

     Sets the content of the specified pixel to the provided value.  If the image holds data of the 'COMPLEX' :ref:`data_type`, the method sets the pixel to a value has a real
     part equal to the provided value and a null complex part.
 
     :param pixel:
     :type  pixel: :class:`Point`
     :param value:
     :type  value: float

  .. method:: SetSpatialOrigin(pixel_indexes)

     Sets the :ref:`spatial-origin` of the image to the provided indexes.

     :param pixel_indexes:
     :type  pixel_indexes: :class:`Point`
     :rtype: None

  .. method:: SetSpatialSampling(arg2)

     Sets the :ref:`pixel-sampling` if the image to the provided values in the spatial :ref:`data-domain`

     :param sampl:
     :type  sampl: :class:`~ost.geom.Vec3`
 
  
