Unit Cell and symmetry information
================================================================================

This document describes the API to work with unit cells and symmetry information and shows how to use it together with molecular entities.


Unitcell
--------------------------------------------------------------------------------

The arrangement of atoms in a crystal can be described in terms of its unit cell. The unit cell contains one or more atoms. By replicating the unit cell in all 3 dimensions, the crystal is obtained. The atom positions are  described relative to a unit cell. The unit cell itself is defined by three edge lengths, a, b, and c as well as the angles between these base vectors. 

.. class:: UnitCell(vec_a, vec_b, vec_c)
           UnitCell(a, b, c, alpha, beta, gamma)
           
  Initialise a new unit cell, either by specifying the 3 base vectors directly 
  or giving lengths and angles (in radians) between the vectors. In the latter 
  case, the base vectors are constructed such that, a is always colinear to the 
  x-axis, b lies in the x-y plane. Together the 3 base vectors form a 
  rectangular system.
  
  .. attribute:: a
  
    The direction and length of the a basis vector
  
  .. attribute:: b

    The direction and length of the b basis vector
    
  .. attribute:: c

    The direction and length of the c basis vector
  
  .. attribute:: alpha
  
    The angle between b and c in radians
  
  .. attribute:: beta
  
    The angle between a and c in radians
    
  .. attribute:: gamma
  
    The angle between a and b in radians
    
  .. attribute:: ortho_tf
  
    The matrix converting from fractional coordinates to orthogonal 
    coordinates. As an example, the following code determines the center of the 
    unit cell.
    
    .. code-block:: python
    
      unit_cell=GetUnitCell()
      
      print 'CENTER: ' % unit_cell.ortho_tf*geom.Vec3(0.5,0.5,0.5)
    
  .. attribute:: fractional_tf
  
    The matrix converting orthogonal coordinates to fractional coordinates in 
    the unit cell. This is the inverse of :attr:`ortho_tf`


Symmetry
--------------------------------------------------------------------------------

.. class:: Symop(rot=geom.Mat3(), trans=geom.Vec3())
                
  Symop encapsulates a symmetry operation described by a rotation and a 
  translation in fractional coordinates. To apply the symmetry operation to an 
  object, e.g. an entity, the transformation has first to be transformed into a 
  transformation operating in orthogonal coordinates using :meth:`GetOrthoTF`. 
  In addition to initializing the symmetry operation directly with a matrix and 
  a translation vector, the symmetry operation can also be created from a 
  compact string representation with :meth:`FromString`.
  
  .. staticmethod:: FromString
  
    Creates a symmetry operation from the given string. The transformation 
    applied to the x-, y-, and z- coordinates are specified, separated by a 
    comma. Below are a few examples of common symmetry operations:
    
    
    .. code-block:: python
    
      # identity transformation.
      symop=Symop.FromString('x,y,z')
      
      # clockwise rotation of 90 degrees around the y axis
      symop=Symop.FromString('-z,y,x')
      
      # inversion around origin
      symop=Symop.FromString('-x,-y,-z')
      
      # rotation around y by ninety degrees, followed by shift of half a 
      # unit cell
      symop=Symop.FromString('-z,y+1/2,x')
  
  .. attribute:: rot
  
    The rotation matrix of the symmetry operation
    
    :type: :class:`~geom.Mat3`
    
  .. attribute:: trans
  
    The translation vector of the symmetry operation
    
    :type: :class:`~geom.Vec3`

  .. method:: GetOrthoTF(unit_cell)
  
    Returns the transformation in orthogonal coordinates to be applied to the 
    given unit cell.


.. class:: Symmetry

  Symmetry is a collection of symmetry operations that together make up a 
  crystallographic space group.
  
  
  .. attribute:: type
  
    The type of the space group, one of SYM_HEXAGONAL, SYM_ORTHORHOMBIC,
    SYM_MONOCLINIC, SYM_CUBIC, SYM_TETRAGONAL, SYM_TRICLINIC, SYM_HEXAGONAL,
    SYM_TRIGONAL, and SYM_UNKNOWN.
    
  .. attribute:: symops
  
    List of symmetry operations that make up this space group
    
  .. staticmethod:: FromHermannMauguinSymbol(symbol)
   
    Create a symmetry based on the hermann mauguin space group nomenclature. 
    This nomenclature is, for example, used by the CRYST1 records of the PDB. If 
    an invalid symbol is specified, None is returned.
    
  .. staticmethod:: FromCCP4Symbol(symbol)
  
    Create symmetry from ccp4 space group number. If the number doesn't exists, 
    None is returned.
