Editors
================================================================================

.. currentmodule:: ost.mol

The structure, topology and connectivity of :class:`entities <EntityHandle>` is 
edited via editors. This includes operations such as changing atom positions, 
connecting atoms with bonds as well as adding and removing chains, residues and 
atoms. There are two flavors of editors, one for the internal coordinate system (:class:`ICSEditor`) and one for the external coordinate system (:class:`XCSEditor`).

Edit Modes
--------------------------------------------------------------------------------

Editors support two modes: An unbuffered edit mode and a buffered edit mode. In
the unbuffered edit mode, dependent information such as the spatial organizer 
and the internal coordinate system (in case of the XCSEditor) are updated after 
every change. In buffered edit mode, the updates are delayed until one of the 
following happens:

 * The last editor goes out of scope.
 * :meth:`XCSEditor.UpdateICS` or :meth:`ICSEditor.UpdateXCS` is called
   explicitly.


The editors follow the RIAA (resource allocation is initialization) principle: 
Whenever an editor is requested an internal reference counter is incremented. In 
the destructor, this reference count is decremented. When the count drops to 
zero, the dependent information is updated.

In Python, one can not rely on the destructors being called. It is advised to 
always put a call to :meth:`XCSEditor.UpdateICS` or 
:meth:`ICSEditor.UpdateXCS` when the editing is finished. Alternatively, 
starting from Python version 2.6, one can use the \
`with <http://docs.python.org/reference/compound_stmts.html#with>`_  
statement to make sure the destructor are called and the dependent information 
is updated.


Basic Editing Operations
--------------------------------------------------------------------------------

The basic functionality of editors is implemented in the EditorBase class. 

.. class::  EditorBase
  
  Inherited by :class:`XCSEditor`, :class:`ICSEditor`.
  
  .. method:: InsertChain(chain_name)
  
     Add new chain to the entity
  
     :param chain_name: The chain's name. In the scope of an entity, chain names
                        are unique. If a chain of the same already exists an
                        IntegrityError is raised.
     :type  chain_name: string
     :returns:          :class:`ChainHandle`

  .. method:: AppendResidue(chain, residue_name, [res_num])
  
     Append residue to the end of the chain. If res_num is not given, the
     residue number will be set to the residue number of the last added residue
     plus one. The insertion code is the same.

     :param chain: Must be a valid chain
     :type  chain: :class:`ChainHandle`
     :param residue_name: 3-letter-code of the residue, e.g. ``GLY``.
     :type  residue_name: string
     :returns:     :class:`ResidueHandle`

  .. method:: RenameResidue(residue, new_name)

     Change the name of residue to new_name. Just overwrites the 3-letter-code
     without changing anything else. Be aware that the sequence/ 1-letter-code
     will not change automatically.

     :param residue:  Must be a valid residue
     :type residue:   :class:`ResidueHandle`
     :param new_name: is the new name. Free to choose and not verified to be a
                      valid identifier.
     :type new_name:  string

  .. method:: RenameChain(chain, new_name)

     Change the name of a chain to new_name while avoiding duplicated
     identifiers. If new_name is already in use by any chain, an exception will
     be generated.

     :param chain:    Must be a valid chain
     :type chain:     :class:`ChainHandle`
     :param new_name: is the new name
     :type new_name:  string

  .. method:: InsertAtom(residue, atom_name, pos, 
                         element="", occupancy=1.0, b_factor=0.0,
                         is_hetatm=False)
  
    Insert new atom and add it to residue. For atoms with alternative atom
    locations use :meth:`InsertAltAtom`. If the element parameter is a valid 
    element, the atom properties mass, charge, and radius are set to default 
    values for that element. If element is an empty string (or an invalid 
    element), the properties are set to rather meaningless default values. You 
    may later set any of the properties manually.
    
    :param residue:   is the parent residue and must be valid
    :type residue:    :class:`ResidueHandle`
    :param atom_name: is the atom name. While free to choose a name, it is
                      advised  to properly name the atoms according to IUPAC
                      rules as several algorithms as well as most
                      :class:`builders <conop.Builder>` in the :mod:`conop`
                      module rely on proper naming.
    :type atom_name:  string
    :param pos:       is the position of the atom in global coordinates
    :type pos:        :class:`~ost.geom.Vec3`
    :param element:   is the atom's element. If set to a a valid element,
                      atom properties such as mass, charge, radius are set 
                      based on default values for that element. If the element 
                      string is empty, or unknown, the properties are filled 
                      with rather meaningless default values.
    :type element:    class:`string`
    :param occupancy: The occupancy of the atom. between 0 and 1
    :type occupancy:  float
    :param b_factor:  temperature factor.
    :type  b_factor:  float
    :param is_hetatm: whether the atom is an atom coming from a HETATM record.
    :type is_hetatm:  bool
    :returns:         :class:`AtomHandle`

  .. method:: AddTorsion(name, atom1, atom2, atom3, atom4)
  
    Add a named torsion to the entity. The atoms must have bonds between 
    atom1-atom2-atom3-atom4.

    :param name: The torsion name, e.g. PHI or PSI
    :type name: :class:`str`

    :param atom1: First atom. must be valid
    :type atom1: :class:`AtomHandle`

    :param atom2: Second atom. must be valid
    :type atom2: :class:`AtomHandle`

    :param atom3: Third atom. must be valid
    :type atom3: :class:`AtomHandle`

    :param atom4: Fourth atom. must be valid
    :type atom4: :class:`AtomHandle`
      
  .. method:: DeleteAtom(atom)
  
    Deletes the atom from the entity and removes all bonds and torsions this 
    atom is involved.
    
    :param atom: A valid atom
    :type atom: :class:`EntityHandle`
    
  .. method:: DeleteAllAtoms(residue)
  
    Deletes all atoms of this residue from the entity and remove all bonds and 
    torsions for where an atom of the residue is involved.
    
    :type residue: :class:`ResidueHandle`
    :param residue: A valid residue
    
    
    :type atom: The atom to be deleted
    
    :type atom: :class:`EntityHandle`
  
  .. method:: DeleteResidue(residue)
  
    Deletes the residue, it's atoms and removes all bonds and torsion where one
    atom of the residue is involved
    
    :type residue: :class:`ResidueHandle`
    :param residue: A valid residue

  .. method:: DeleteChain(chain)
  
    Delete the given chain, and all its residues
    
    :param chain: `A valid chain`
    :type chain: :class:`ChainHandle`

  .. method:: ReorderResidues(chain)
              ReorderResidues()
              
    Reorder residues of the chain (the entity) such that their residues numbers 
    are continuously increasing. This function might be useful in cases of PDB 
    files that do not list the residues from N to C terminus but rather use the 
    residue number to describe their position in the chain.
    
    :param chain: `A valid chain`
    :type chain: :class:`ChainHandle`

  .. method:: RenameAtom(atom, new_name)

     Change the name of atom to new_name without changing anything else.

     :param atom:     Must be a valid atom
     :type atom:      :class:`AtomHandle`
     :param new_name: is the new name. Free to choose and not verified to be a
                      valid atom identifier.
     :type new_name:  string


Editor for the External Coordinate System
--------------------------------------------------------------------------------

The XCSEditor defines the interface for manipulating the external coordinate 
system. The external coordinate system directly operates on atom positions in 
Euclidian space. 

.. class:: XCSEditor
   
  Inherits :class:`EditorBase`
   
  .. method:: ApplyTransform(transform)
  
     Apply a transformation to the entity. The transformation is applied to all 
     atoms positions.
     
     :param transform: The transformation to be applied
     :type  transform: :class:`geom.Mat4`
     
  .. method:: SetTransform(transform)
  
     Set the entity transformation. See also :meth:`ApplyTransform`
     
     :param transform: The transformation to be applied
     :type  transform: :class:`geom.Mat4`
     
  .. method:: SetAtomPos(atom, pos)
  
     Set the (transformed) position of atom. This method will also update the
     original position of the atom by applying the inverse of the entity
     transform.
     
     :param atom: must be a valid atom handle
     :type  atom: :class:`ost.mol.AtomHandle`
     :param pos: The new position
     :type  pos: :class:`~ost.geom.Vec3`
     
  .. method:: SetOriginalAtomPos(atom, pos)
     
     Set the original (untransformed) position of the atom. This method will
     also update the transformed position by applying the entity transform to
     the original pos.
     
     :param atom: must be a valid atom handle
     :type  atom: :class:`ost.mol.AtomHandle`
     :param pos: The new untransformed position
     :type  pos: :class:`~ost.geom.Vec3`
  
Editor for the Internal Coordinate System
--------------------------------------------------------------------------------

The :class:`ICSEditor` is used to manipulate the internal coordinate system that
is defined by bond lengths and angles. You can create an editor with the 
:class:`~EntityHandle.EditICS` method of the :class:`EntityHandle`.

The use :class:`XCSEditor` and :class:`ICSEditor` are mutually exclusive. This
means that whenever a :class:`XCSEditor` has pending changes, the results of
using an :class:`ICSEditor` is undefined and vice versa.

.. note:: 

  For speed reasons, the internal coordinate system is not initialised until
  the first call to :meth:`EntityHandle.EditICS`. This involves the build-up of
  a directed-graph for the bond network as well as calculating the internal
  coordinate matrices.

.. class:: ICSEditor
   
  Inherits :class:`EditorBase`
  
  .. method:: SetTorsionAngle(torsion, angle)
    
    Set the angle of the given torsion. If the edit mode of the editor is set 
    to buffered, the external coordinates remain unchanged. If set to 
    unbuffered, the external coordinates are immediately recalculated.
    
    :see: :meth:`UpdateXCS`
    
    :param torsion: A valid torsion
    
    :type torsion: :class:`TorsionHandle`
    
    :param angle: The angle in radians
    
    :type angle: :class:`float`
    
    :raises: :exc:`RuntimeError` when the torsion handle is invalid
  
  .. method:: UpdateXCS()
  
    Apply all remaining changes to the internal coordinate system and 
    recalculate external coordinates. In unbuffered edit mode, calling this 
    method has no effect.
    
    
  .. method:: SetBondLength(bond, length)
  
    Sets the length of a bond. If the edit mode of the editor is set 
    to buffered, the external coordinates remain unchanged. If set to 
    unbuffered, the external coordinates are immediately recalculated.
    
    :see: :meth:`UpdateICS`
    
    :param bond: A valid bond handle
    
    :type bond: :class:`BondHandle`
    
    :param length: The bond length in Angstrom.
    
    :type length: :class:`float`
    
    :raises: :exc:`RuntimeError` when the bond handle is invalid

  .. method:: SetAngle(atom1, atom2, atom3, angle)
  
    Sets the angle between 3 atoms. The first atom must be connected to the 
    second, the second to the third with a bond. If the edit mode of the editor 
    is set to buffered, the external coordinates remain unchanged. If set to 
    unbuffered, the external coordinates are immediately recalculated.
    
    :see: :meth:`UpdateICS`
    
    :param atom1: The first atom. Must be valid
    :type atom1: :class:`AtomHandle`
    
    :param atom2: The second atom. Must be valid
    :type atom2: :class:`AtomHandle`
    
    :param atom3: The third atom. Must be valid
    :type atom3: :class:`AtomHandle`
    
    :param angle: The angle in radians
    
    :raises: :exc:`RuntimeError` when one of the atoms is invalid or there is
             no bond between atom1 and atom2 or atom2 and atom3.
