The Molecular Entity
================================================================================

.. currentmodule:: ost.mol

This document describes the :class:`EntityHandle` and related classes.


.. function:: CreateEntity()

   Creates a new entity. The created entity is empty, that is, it does not
   contain any atoms, residues, chains, bonds or torsions. To populate the
   entity, use an :doc:`editors`.
   
   :returns: The newly created :class:`EntityHandle`
   
.. class:: EntityHandle

  The entity class represents a molecular structure. Such a structure is in
  general made up of one or more linear chains of residues, which in turn are
  formed by one or more atoms.

  The interface of entities is tailored to biological macromolecules, but this
  does not prevent it to be used for molecules in general: An entity also
  represent a ligand or a collection of water molecules - hence the rather
  generic name.

  .. method:: FindChain(chain_name)

     Get chain by name. See also :meth:`GetChainList`

     :param chain_name:  Chain identifier, e.g. "A"
     :type  chain_name:  string
     :returns:           A valid :class:`ChainHandle`, if the entity contains a
                         chain with the given name, an invalid
                         :class:`ChainHandle` otherwise.
                         
  .. method:: GetChainList()
     
     Get list of all chains of this entity. To access a single chain, use
     :meth:`FindChain`.
     
     :returns: A list of :class:`ChainHandles<ChainHandle>`
    
  .. method:: FindResidue(chain_name, res_num)
     
     Get residue by chain name and residue number. See also 
     :meth:`GetResidueList`
     
     :param chain_name:  Chain identifier, e.g. "A"
     :type  chain_name:  string
     :param    res_num:  residue number
     :type     res_num:  mol.ResNum
     
     :returns:           A valid :class:`ResidueHandle` if the chain exists and
                         the chain contains a residue of the given residue
                         number, an invalid :class:`ResidueHandle` otherwise.

  .. method:: GetResidueList()
  
     Get list of all residues of this entity. To access a single residue, use
     :meth:`FindResidue`.
  
     :returns: A list of :class:`ResidueHandles<ResidueHandle>`
     
  .. method:: FindAtom(chain_name, res_num, atom_name)
  
     Get atom by chain name, residue number and atom name. See also
     :meth:`GetAtomList`
     
     :param chain_name:  Chain identifier, e.g. "A"
     :type  chain_name:  string
     :param    res_num:  residue number
     :type     res_num:  mol.ResNum
     :param  atom_name:  atom name, e.g. CA
     :type   atom_name:  string
     
     :returns:           A valid :class:`AtomHandle` if an atom matching the
                         parameters could be found, an invalid
                         :class:`AtomHandle` otherwise

  .. method:: GetAtomList()
  
     Get list of all atoms of this entity. To access a single atom, use
     :meth:`FindAtom`
  
     :returns: A list of :class:`AtomHandles<AtomHandle>`

  .. method:: RequestXCSEditor([edit_mode=mol.EditMode.BUFFERED_EDIT])

     Request :class:`XCSEditor` for editing the external coordinate system. This
     call will fail when there are pending changes of the internal coordinate
     system.
     
     :param edit_mode: Must be EditMode.BUFFERED_EDIT or
                       EditMode.UNBUFFERED_EDIT. For more details, see the
                       editor documentation.
     :type edit_mode: mol.EditMode
     
     :returns: :class:`XCSEditor`
  .. method:: RequestICSEditor([edit_mode=mol.EditMode.BUFFERED_EDIT])
  
     Request :class:`ICSEditor` for editing the internal coordinate system, such
     as torsions, bond lengths and angle between two bonds. This call will fail
     when there are pending changes of the external coordinate system.
     
     :param edit_mode: Must be EditMode.BUFFERED_EDIT or
                       EditMode.UNBUFFERED_EDIT. For more details, see the
                       editor documentation.
     :type edit_mode: mol.EditMode
     
     :returns: :class:`ICSEditor`
     
  .. method:: Select(query, flags)
  
     Perform a selection on the entity. The result of the selection is an 
     :class:`EntityView` which contains usually only a subset of chains,
     residues, atoms and bonds of the original entity.
     
     :param query: The query to be executed. See :class:`Query` for details.
     :type  query: string or :class:`Query`
     :param flags: An ORed combination of :class:`QueryFlags`.
     :type  flags: int
     :returns:     An :class:`entity view <EntityView>`.
     :raises:      :class:`QueryError` when the query could not be executed due
                   to syntactic errors.
  
  .. method:: CreateFullView()
  
     Creates  an entity view containing all chains, residues, atoms and bonds of
     this entity.
     
     .. code-block:: python
     
       # these two lines are identical
       full=ent.Select('')
       full=ent.CreateFullView()

     :returns: :class:`EntityView`
  
  .. method:: CreateEmptyView()
  
     Creates an entity view pointing to this entity, but otherwise empty. This
     method is usually the starting point for manual entity view creation, e.g.
     
     .. code-block:: python
     
       view=ent.CreateEmtpyView()
       for atom in ent.atoms:
         if ComplicatedPredicate(atom):
            view.AddAtom(atom)

     :returns: :class:`EntityView`

  .. method:: Copy()
    
    Creates a deep copy of the entity.
    
    :returns: A new :class:`EntityHandle` that is an exact copy of this entity.
    
    .. note::
      
      alternative atom positions are not handled yet.

  .. method:: GetCenterOfAtoms()
    
    Get center of atoms, that is the average atom position of the entity. Use
    :meth:`GetCenterOfMass` to calculate the mass-weighted center of the entity.
    
    :returns: :class:`geom.Vec3`
    
  .. method:: GetCenterOfMass()
     
     Calculates the center of mass of the entity. Use :meth:`GetCenterOfAtoms`
     to calculate the non-mass-weighted center of the entity.
     
     :returns: :class:`geom.Vec3`
     
  .. method:: GetGeometricCenter()
  
    Calculates the mid-point of the axis aligned bounding box of the entity.
    
    :returns: :class:`geom.Vec3`
    
  .. method:: GetMass()
  
    Calculates the total mass of the entity by summing up the masses of all
    atoms.
    
    :returns: The mass in Dalton
    
  .. method:: Apply(visitor)
  
     Apply EntityVisitor to the entity. In Python, you shouldn't bother using
     this function. EntityVisitory are mainly used in C++ to implement
     algorithms that operate are able to operate on EntityViews or
     EntityHandles.
     
  .. method:: FindWithin(pos, radius)
  
     Find all atoms in sphere of given radius centered at *pos*. To turn the
     returned list of atoms into an :class:`EntityView`, use
     :func:`CreateViewFromAtomList`.
     
     :param pos: Center of sphere
     :type pos: :class:`geom.Vec3`
     :param radius: The radius of the sphere
     :type radius: float
     
     :returns: A list of :class:`AtomHandles<AtomHandle>`
     
.. class:: ChainHandle

  .. method:: FindResidue(res_num)
   
    Get residue by residue number. See also :meth:`GetResidueList`
    
    :param    res_num:  residue number
    :type     res_num:  mol.ResNum
    
    :returns:           A valid :class:`ResidueHandle` if the chain contains
                        a residue with matching residue number, an invalid
                        :class:`ResidueHandle` otherwise.
                        
  .. method:: GetResidueList()

    Get list of all residues of this chain. For peptide chains, the residues
    are usually ordered from N- to C-terminus.To access a single residue, use     
    :meth:`FindResidue`.
    
    :returns: A list of :class:`ResidueHandles<ResidueHandle>`

  .. method:: FindAtom(res_num, atom_name)

    Get atom by residue number and atom name. See also :meth:`GetAtomList`

    :param    res_num:  residue number
    :type     res_num:  mol.ResNum
    :param  atom_name:  atom name, e.g. CA
    :type   atom_name:  string

    :returns:           A valid :class:`AtomHandle` if an atom matching the
                        parameters could be found, an invalid
                        :class:`AtomHandle` otherwise

  .. method:: GetAtomList()

    Get list of all atoms of this chain. To access a single atom, use
    :meth:`FindAtom`

    :returns: A list of :class:`AtomHandles<AtomHandle>`

.. class:: ResidueHandle

  .. method:: FindAtom(atom_name)

   Get atom by atom name. See also :meth:`GetAtomList`

   :param  atom_name:  atom name, e.g. CA
   :type   atom_name:  string

   :returns:           A valid :class:`AtomHandle` if an atom with the given
                       name could be found, an invalid :class:`AtomHandle`
                       otherwise

  .. method:: GetAtomList()

   Get list of all atoms of this residue. To access a single atom, use
   :meth:`FindAtom`.

.. class:: AtomHandle

.. class:: EntityView
   
   Represents a subpart of an :class:`EntityHandle`.
   
Functions
--------------------------------------------------------------------------------


Boolean Operators
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. function:: Intersection(view_a, view_b)

  Calculates and returns the intersection of `view_a` and `view_b`. `view_a`
  and `view_b`  must be views of the same entity.

  :param view_a:    first view
  :type view_a:     EntityView
  :param view_b:    second view
  :type view_b:     EntityView

.. function:: Difference(view_a, view_b)

  Calculates and returns the difference between `view_a` and `view_b`. `view_a`
  and `view_b`  must be views of the same entity.The  returned view will
  contain atoms, residues, chains and bonds that are in `view_a` and not in
  `view_b`.

  :param view_a:    first view
  :type view_a:     EntityView
  :param view_b:    second view
  :type view_b:     EntityView 

.. function:: Union(view_a, view_b)

  Calculates and returns the union of `view_a` and `view_b`. `view_a`
  and `view_b`  must be views of the same entity.The returned view will contain
  all atoms, residues, chains and bonds that are either in `view_a`, `view_b`
  or part of both views.

  :param view_a:    first view
  :type view_a:     EntityView
  :param view_b:    second view
  :type view_b:     EntityView


Other Entity-Related Functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. function:: CreateViewFromAtomList(atom_list)

   Returns a view made up of the atoms in *atom_list*. All atoms are required to
   be atoms of the same entity. Duplicate atoms are only added to the view once.
   
   :param atom_list: the atoms
   :type atom_list: :class:`AtomHandleList` or :class:`AtomViewList`
   :raises: :class:`IntegrityError` if atoms of different entities are
            encountered
   
   :returns: :class:`EntityView`