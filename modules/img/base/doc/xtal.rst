Crystallographic datasets
================================================================================

.. currentmodule:: ost.img

The image handles themselves are only partially suited to deal with X-ray crystallographic data. While they support non-orthogonal coordinate systems, they are finite in extent. Crystallography density maps on the other hand, are infinite in extent. To deal with the special features of density maps, the :mod:`~ost.img` module contains a class called :class:`XtalMap`. The class allows you to reconstruct any region in 3D space from the symmetry and unit cell information.

The XtalMap class
--------------------------------------------------------------------------------

The XtalMap class is used to represent a crystal. It only stores one asymmetric unit (ASU) and reconstructs the symmetry related copies of the crystal based on symmetry operators and repeating the unit cells along all 3 major crystal axes. Usually, the a new density map is created by loading it from disk with :func:`~ost.io.LoadXtalMap`. As of now, OpenStructure only supports loading crystallographic density maps from MRC and CCP4 map files.


.. class:: XtalMap(asu, unit_cell, unit_cell_size)

  Create a new crystallographic density map from the image handle holding at 
  least one asymmetric unit, the unit cell parameters and the size of the unit 
  cell.
  
  
  .. method:: Extract(extent)
  
    Creates and returns a :class:`ImageHandle` for the given region defined 
    by extent.
    
  .. method:: IndexToCoord(point)
  
    Returns the coordinate in 3D space for the given point.
    
    :rtype: :class:`~ost.geom.Vec3`
    
    
  .. method:: CoordToIndex(coord)
  
    Converts the coordinate in global coordinates to the index in the map.
    
    :rtype: :class:`~ost.geom.Vec3`
    
  .. attribute:: unit_cell
  
    Holds the unit cell information of this density map
    
    :type: :class:`~ost.UnitCell`
    
  .. attribute :: unit_cell_size
  
    The size of the map for a full unit cell.
    
    :type: :class:`Size`
    


Visualizing crystallographic density maps
--------------------------------------------------------------------------------

To visualize the density map in the 3D viewer, use the :class:`~ost.gfx.MapIso` class:


.. code-block:: python

  xtal_map=io.LoadXtalMap('crystal.ccp4')
  map_iso=gfx.MapIso('XTAL', xtal_map, 0.5)
  scene.Add(map_iso)
  # set the visible extent to an area of 64x64x64 pixels, centered 
  # on (0,0,0)
  map_iso.SetVisibleExtent(img.Extent(img.Size(64, 64, 64)))
  
  