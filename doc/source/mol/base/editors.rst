Editors
================================================================================

.. currentmodule:: ost.mol

The structure, topology and connectivity of entities is edited via editors. This 
includes operations such as changing atom positions, connecting atoms with bonds 
as well as adding and removing chains, residues and atoms. 

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


The editors follow the RIAA (resource allocation is initialisation) principle: 
Whenever an editor is requested an internal reference counter is incremented. In 
the destructor, this reference count is decremented. When the count drops to 
zero, the dependent infomation is updated.

In Python, one can not rely on the destructors being called. It is adviced to 
always put a call to :meth:`XCSEditor.UpdateICS` or 
:meth:`ICSEditor.UpdateXCS` when the editing is finished. Alternatively, starting from Python version 2.6, one can use the 
`with <http://docs.python.org/reference/compound_stmts.html#with>`_  statement
to make sure the destructor are called and the dependent information is updated.


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
  
  .. method:: InsertAtom(residue, atom_name, pos, [prop=mol.AtomProp()])
  
    Insert new atom and add it to residue. For atoms with alternative atom
    locations use :meth:`InsertAltAtom`.
    
    :param residue:   is the parent residue and must be valid
    :type residue:    :class:`ResidueHandle`
    :param atom_name: is the atom name. While free to choose a name, it is
                      adviced  to properly name the atoms according to IUPAC
                      rules as several algorithms as well as most
                      :class:`builders <conop.Builder>` in the :mod:`conop`
                      module rely on proper naming.
    :type atom_name:  string
    :param pos:       is the position of the atom in global coordinates
    :type pos:        :class:`geom.Vec3`
    :param prop:      are the atom's properties such as element, van der Waals 
                      radius charge and so on. The default set of atom
                      properties is rather meaningless.
    :type prop:       class:`AtomProp`
    :returns:         :class:`AtomHandle`

Editor for the External Coordinate System
--------------------------------------------------------------------------------

The XCSEditor defines the interface for manipulating the external coordinate 
system. The external coordinate system directly operates on atom positions in 
euclidian space. 

.. class:: XCSEditor
   
  Inherits :class:`EditorBase`
   
  .. method:: ApplyTransform(transform)
  
     Apply a transformation the entity transform. The entity transform is a
     global transformation applied to all atoms.
     
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
     :type  pos: :class:`geom.Vec3`
     
  .. method:: SetOriginalAtomPos(atom, pos)
     
     Set the origininal (untransformed) position of the atom. This method will
     also update the transformed position by applying the entity transform to
     the original pos.
     
     :param atom: must be a valid atom handle
     :type  atom: :class:`ost.mol.AtomHandle`
     :param pos: The new untransformed position
     :type  pos: :class:`geom.Vec3`
  
Editor for the Internal Coordinate System
--------------------------------------------------------------------------------
The :class:`ICSEditor` is used to manipulate the internal coordinate system that
is defined by bond lengths and angles. By default the internal coordinate system
is  not calculates. However, upon requesting an :class:`ICSEditor` for the first
time, the internal coordinate system is initialized. This involves the build-up
of a  directed-graph for the bond network as well as calculating the internal 
coordinate matrices.

The use :class:`XCSEditor` and :class:`ICSEditor` are mutually exclusive. This
means that whenever a :class:`XCSEditor` has pending changes, the results of
using an :class:`ICSEditor` is undefined and vice versa.

.. class:: ICSEditor
   
   Inherits :class:`EditorBase`
