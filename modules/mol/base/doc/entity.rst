The Molecular Entity
================================================================================

.. currentmodule:: ost.mol

This document describes the :class:`EntityHandle` and the related classes.

The Handle Classes
--------------------------------------------------------------------------------

.. function:: CreateEntity()

   Creates a new entity. The created entity is empty, that is, it does not
   contain any atoms, residues, chains, bonds or torsions. To populate the
   entity, use an :doc:`entity editor <editors>`.
   
   :returns: The newly created :class:`EntityHandle`
   
.. class:: EntityHandle

  The entity class represents a molecular structure. Such a structure is in
  general made up of one or more chains of residues, which in turn are
  formed by one or more atoms.

  The interface of entities is tailored to biological macromolecules, but this
  does not prevent it to be used for molecules in general: An entity also
  represent a ligand or a collection of water molecules - hence the rather
  generic name.

  .. attribute:: chains
    
    List of all chains of this entity. The chains are in the same 
    order they have been added, for example the order they have been listed 
    in a PDB file.
    
    Also available as :meth:`GetChainList`. To access a single chain, use 
    :meth:`FindChain`.
    
    This property is read-only.
    
  .. attribute:: residues
    
    List of all residues of this entity. The returned residues are first 
    sorted by chain and then from N- to C-terminus.
    
    This property is read-only.
    
    **Example usage:**
    
    .. code-block:: python
    
      for res in ent.residues:
        print res.name, res.atom_count
    
    Also available as :meth:`GetResidueList`. To access a single residue, use 
    :meth:`FindResidue`. 
  
     :returns: A list of :class:`residue handles<ResidueHandle>`
  
  .. attribute:: atoms
  
     Get list of all atoms of this entity. To access a single atom, use
     :meth:`FindAtom`.
     
     This property is read-only. Also available as :meth:`GetAtomList`
  
     :type: A list of :class:`atom handles<AtomHandle>`
  
  .. attribute:: bounds
  
    Axis-aligned bounding box of the entity. Read-only
    
    :type: :class:`ost.geom.AlignedCuboid`

  .. attribute:: mass
    
    The total mass of the entity in Dalton. Also available as :meth:`GetMass`.
    
    :type: float

  .. attribute:: center_of_mass
  
    Center of mass. Also available as :meth:`GetCenterOfMass`
    
    :type: :class:`~ost.geom.Vec3`
    
  .. attribute:: center_of_atoms
  
    Center of atoms (not mass-weighted). Also available as 
    :meth:`GetCenterOfAtoms`.
    
    :type: :class:`~ost.geom.Vec3`
  
  .. method:: FindChain(chain_name)
    
    Get chain by name. See also :attr:`chains`
    
    :param chain_name:  Chain identifier, e.g. "A"
    :type  chain_name:  str
    :returns:           A valid :class:`ChainHandle`, if the entity contains a
                        chain with the given name, an invalid
                        :class:`ChainHandle` otherwise.
                        
  .. method:: GetChainList()
    
    See :attr:`chains`
    
  .. method:: FindResidue(chain_name, res_num)
    
    Get residue by chain name and residue number. See also 
    :meth:`GetResidueList`
    
    :param chain_name:  Chain identifier, e.g. "A"
    :type  chain_name:  str
    :param    res_num:  residue number
    :type     res_num:  mol.ResNum
    
    :returns:           A valid :class:`ResidueHandle` if the chain exists and
                        the chain contains a residue of the given residue
                        number, an invalid :class:`ResidueHandle` otherwise.
    
  .. method:: GetResidueList()
    
    See :attr:`residues`
    
  .. method:: FindAtom(chain_name, res_num, atom_name)
    
    Get atom by chain name, residue number and atom name. See also
    :attr:`atoms`
    
    :param chain_name:  Chain identifier, e.g. "A"
    :type  chain_name:  str
    :param    res_num:  residue number
    :type     res_num:  mol.ResNum
    :param  atom_name:  atom name, e.g. CA
    :type   atom_name:  str
    
    :returns:           A valid :class:`AtomHandle` if an atom matching the
                        parameters could be found, an invalid
                        :class:`AtomHandle` otherwise
    
  .. method:: GetAtomList()
    
    See :attr:`atoms`
    
  .. method:: EditXCS([edit_mode=mol.EditMode.BUFFERED_EDIT])
    
    Request :class:`XCSEditor` for editing the external coordinate system. This
    call will fail when there are pending changes of the internal coordinate
    system.
    
    :param edit_mode: Must be EditMode.BUFFERED_EDIT or
                      EditMode.UNBUFFERED_EDIT. For more details, see the
                      editor documentation.
    :type edit_mode: mol.EditMode
    
    :returns: :class:`XCSEditor`
    
  .. method:: EditICS([edit_mode=mol.EditMode.BUFFERED_EDIT])
    
    Request :class:`ICSEditor` for editing the internal coordinate system, such
    as torsions, bond lengths and angle between two bonds. This call will fail
    when there are pending changes of the external coordinate system.
    
    :param edit_mode: Must be EditMode.BUFFERED_EDIT or
                      EditMode.UNBUFFERED_EDIT. For more details, see the
                      editor documentation.
    :type edit_mode: mol.EditMode
    
    :returns: :class:`ICSEditor`
    
  .. method:: Select(query, flags=0)
    
    Perform a selection on the entity. The result of the selection is an 
    :class:`EntityView` which (usually) contains only a subset of chains,
    residues, atoms and bonds of the original entity.
    
    **Example Usage:**
    
    .. code-block::python
      # select calpha atoms of peptides
      calphas=ent.Select('aname=CA and peptide=true')
      
      # select atoms in a box of size 10, centered at the origin
      in_box=ent.Select('x=-5:5 and y=-5:5 and z=-5:5')
    
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
    method is usually the starting point for manual entity view creation, e.g.:
    
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
    
    :returns: :class:`~ost.geom.Vec3`
    
  .. method:: GetCenterOfMass()
    
    Calculates the center of mass of the entity. Use :meth:`GetCenterOfAtoms`
    to calculate the non-mass-weighted center of the entity.
    
    :returns: :class:`~ost.geom.Vec3`
    
  .. method:: GetGeometricCenter()
  
    Calculates the mid-point of the axis aligned bounding box of the entity.
    
    :returns: :class:`~ost.geom.Vec3`
    
  .. method:: GetMass()
  
    See :attr:`mass`
    
    
  .. method:: FindWithin(pos, radius)
  
    Find all atoms in sphere of given radius centered at *pos*. To turn the
    returned list of atoms into an :class:`EntityView`, use
    :func:`CreateViewFromAtomList`.
    
    :param pos: Center of sphere
    :type pos: :class:`~ost.geom.Vec3`
    :param radius: The radius of the sphere
    :type radius: float
    
    :returns: A list of :class:`atom handles<AtomHandle>`
    
.. class:: ChainHandle

  A chain of one or more :class:`residues <ResidueHandle>`. Chains are always 
  part of an entity.
  
  .. attribute:: name
  
     The chain name. The name uniquely identifies the chain in the entity. In 
     most cases, the chain name is one character. This is restriction of the PDB 
     file format. However, you are free to use longer names as long as you don't 
     want to save them as PDB files
     
     This property is read-only. To change the name, use an :class:`XCSEditor`. 

     Also available as :meth:`GetName`
     
     :type: str

  .. attribute:: type

     Describes the type of the chain. See :ref:`chaintype`.

  .. attribute:: description

     Details about the chain. Not categorised, just text.

  .. attribute:: residues
   
     List of all residues of this chain. The residues are sorted from N- to 
     C-terminus. Usually the residue numbers are in ascending order 
     (see :attr:`in_sequence`).
   
     This property is read-only.
   
     **Example usage:**
   
     .. code-block:: python
     
       chain=ent.FindChain("A")
       for res in chain.residues:
         print res.name, res.atom_count
   
     Also available as :meth:`GetResidueList`. To access a single residue, use 
     :meth:`FindResidue`. 

     :returns: A list of :class:`residue handles<ResidueHandle>`
  
  .. attribute:: in_sequence
  
     Whether the residue numbers are in ascending order. For example:
     
     .. code-block:: python
     
       chain=ent.FindChain("A")
       print chain.residues # [A.GLY1, A.GLY2, A.GLY4A, A.GLY4B]
       print chain.in_sequence # prints true
       
       chain=ent.FindChain("B")
       print chain.residues # [B.GLY1, B.GLY4, B.GLY3]
       print chain.in_sequence # prints false

  .. attribute:: atoms

     Get list of all atoms of this chain. To access a single atom, use
     :meth:`FindAtom`.
   
     This property is read-only. Also available as :meth:`GetAtomList`

     :type: A list of :class:`atom handles<AtomHandle>`

  .. attribute:: bounds

    Axis-aligned bounding box of the chain. Read-only

    :type: :class:`ost.geom.AlignedCuboid`
          
  .. attribute:: mass
  
    The total mass of this chain in Dalton. Also available as :meth:`GetMass`
  
    :type: float

  .. attribute:: center_of_mass

    Center of mass. Also available as :meth:`GetCenterOfMass`
  
    :type: :class:`~ost.geom.Vec3`
  
  .. attribute:: center_of_atoms
    
    Center of atoms (not mass weighted). Also available as 
    :meth:`GetCenterOfAtoms`.
    
    :type: :class:`~ost.geom.Vec3`
    
  .. method:: FindResidue(res_num)
   
    Get residue by residue number. See also :attr:`residues`
    
    :param    res_num:  residue number
    :type     res_num:  mol.ResNum
    
    :returns:           A valid :class:`ResidueHandle` if the chain contains
                        a residue with matching residue number, an invalid
                        :class:`ResidueHandle` otherwise.
                        
  .. method:: GetResidueList()

    Get list of all residues of this chain. For peptide chains, the residues
    are usually ordered from N- to C-terminus.To access a single residue, use     
    :meth:`FindResidue`.
    
    :returns: A list of :class:`residue handles<ResidueHandle>`

  .. method:: FindAtom(res_num, atom_name)

    Get atom by residue number and atom name. See also :attr:`atoms`

    :param    res_num:  residue number
    :type     res_num:  mol.ResNum
    :param  atom_name:  atom name, e.g. CA
    :type   atom_name:  str

    :returns:           A valid :class:`AtomHandle` if an atom matching the
                        parameters could be found, an invalid
                        :class:`AtomHandle` otherwise

  .. method:: GetAtomList()
    
    See :attr:`atoms`

  .. method:: GetName()
  
    See :attr:`name`

  .. method:: GetType()

    See :attr:`type`

  .. method:: GetDescription()

    See :attr:`description`

.. class:: ResidueHandle

  The residue is either used to represent complete molecules or building blocks 
  in a polymer, e.g. in a protein, DNA or RNA. A residue consists of one or 
  more :class:`atoms <AtomHandle>`. Residues are always part of a 
  :class:`ChainHandle`, even if they are ligands or water molecules where the 
  concept of a chain does not apply.
  
  .. attribute:: name
  
    The residue name is usually a str of 3 characters, e.g. `GLY` for 
    glycine or `ALA` for alanine, but may be shorter, e.g. `G` for guanosine, 
    or longer for structures loaded from formats other than PDB.
    
    This property is read-only. To change the name of the residue, use
    :meth:`EditorBase.SetResidueName`.
  
  .. attribute:: number
  
    The number of this residue. The residue number has a numeric part and an
    insertion-code. This property is read-only. Also available as 
    :meth:`GetNumber`.
    
    :type: :class:`ResNum`
  
  .. attribute:: one_letter_code
  
    For amino acids, and nucleotides the `one_letter_code` is an alpha-numeric 
    character. For unknown or more *exotic* residues, the one letter code is set 
    to '?'.
    
    **Example**
    
    This code-snippet shows how to get the sequence string from a list of 
    residues.
    
    .. code-block:: python
    
      print ''.join([r.one_letter_code for r in chain.residues])
    
    :type: str
    
  .. attribute:: bounds
  
    Axis-aligned bounding box of the residue. Read-only.
    
    :type: :class:`ost.geom.AlignedCuboid`
    
  .. attribute:: mass
  
    The total mass of this residue in Dalton. Also available as :meth:`GetMass`.
  
    :type: float

  .. attribute:: center_of_mass

    Center of mass. Also available as :meth:`GetCenterOfMass`
  
    :type: :class:`~ost.geom.Vec3`
  
  .. attribute:: center_of_atoms
    
    Center of atoms (not mass weighted). Also available as 
    :meth:`GetCenterOfAtoms`.
    
    :type: :class:`~ost.geom.Vec3`

  .. attribute:: chain
  
    The chain this residue belongs to. Read-only. Also available as 
    :meth:`GetChain`
    
    :type: :class:`ChainHandle`
  
  .. attribute:: phi_torsion
  
    The PHI dihedral angle between this residue and the next. For residues 
    that are not amino acids, residues that do not have all atoms required or 
    residues that do not have bonds between the four atoms involved in the 
    torsion, the PHI torsion is an invalid handle.
    
    Read-only. Also available as :meth:`GetPhiTorsion`
    
    :type: :class:`TorsionHandle`
  
  .. attribute:: psi_torsion
  
    The PSI dihedral angle between this residue and the previous. For residues 
    that are not amino acids, residues that do not have all atoms required or 
    residues that do not have bonds between the four atoms involved in the 
    torsion, the PSI torsion is an invalid handle.
    
    Read-only. Also available as :meth:`GetPsiTorsion`
    
    :type: :class:`TorsionHandle`
  
  .. attribute:: chem_class
  
    The chemical class of a residue is used to broadly categorize residues based 
    on their chemical properties. For example, peptides belong  to the 
    `L_PEPTIDE_LINKING` or `D_PEPTIDE_LINKING` classes.

  .. attribute:: chem_type

    The chemical type of a residue is a classification of all compounds
    obtained from the PDB component dictionary. For example, ions belong to the
    class `ChemType::IONS`, amino acids to `ChemType::AMINOACIDS`.
  
  .. attribute:: sec_structure
  
    The secondary structure of the residue.
  
  .. attribute:: is_ligand
  
    Whether the residue is a ligand. When loading PDB structures, this property 
    is set based on the HET records. This also means, that this property will 
    most likely not be set properly for all except PDB files coming from 
    pdb.org.

  .. method:: FindAtom(atom_name)

    Get atom by atom name. See also :attr:`atoms`

    :param  atom_name:  atom name, e.g. CA
    :type   atom_name:  str

    :returns:           A valid :class:`AtomHandle` if an atom with the given
                        name could be found, an invalid :class:`AtomHandle`
                        otherwise

  .. method:: GetAtomList()

    Get list of all atoms of this residue. To access a single atom, use
    :meth:`FindAtom`.
    
  .. method:: GetChain()
  
    See :attr:`chain`
  
  .. method:: GetCenterOfAtoms()
    
    See :attr:`center_of_atoms`
    
  .. method:: GetCenterOfMass()
  
    See :attr:`center_of_mass`
  
  .. method:: GetPhiTorsion()
    
    See :attr:`phi_torsion`
  
  .. method:: GetPsiTorsion()
  
    See :attr:`psi_torsion`
  
  .. method:: GetChemType()
    
    See :attr:`chem_type`
  

.. class:: AtomHandle

  Represents an atom in a molecular structure. Atoms are always part of a 
  residue.
  
  .. attribute:: name
  
    The atom name, usually a capitalized string consisting of the element and
    an position indicator, e.g. `G1`. In general, the atom name uniquely 
    identifies an atom within a residue. However, this is not enforced and there
    may be more than one atoms with the same name. 
    
    :type: str
  
  .. attribute:: qualified_name
  
     The qualified name consists of the atom name as well as a unique residue
     identifier and chain name. For CA of a glycine with residue number 2 of
     chain A, the qualified name is "A.GLY2.CA".
     
     :type: str
     
  .. attribute:: element
  
    The atom's element. Note that this may return an empty string. Also 
    available as :meth:`GetElement`. Read-only.
    
    :type: str
    
  .. attribute:: mass
  
    The atom's mass in Dalton. Also available as :meth:`GetMass`. Read-only.
    
    :type: float
    
  .. attribute:: pos
  
    The atom's position in global coordinates, transformed by the entity 
    transformation. Also available as :meth:`GetPos`.
    
    Read-only.
    
    :type: :class:`~ost.geom.Vec3`
    
  .. attribute:: original_pos
  
    The atom's untransformed position in global coordinates. Also available as 
    :meth:`GetOriginalPos`.
    
    Read-only.
    
    :type: :class:`~ost.geom.Vec3`
    
  .. attribute:: radius
  
    The van-der-Waals radius of the atom. Also available as :meth:`GetRadius`. 
    Read/write.
    
    :type: float
  
  .. attribute:: occupancy
  
    The atom's occupancy in the range 0 to 1. Read/write. Also available as 
    meth:`GetOccupancy`, :meth:`SetOccupancy`.
    :type: float
    
  .. attribute:: b_factor
  
    The atom's temperature factor. Read/write. Also available as 
    :meth:`GetBFactor`, :meth:`SetBFactor`.
    
    :type: float    

  .. attribute:: charge
    
    The atom's charge. Also available as :meth:`GetCharge`, :meth:`SetCharge`.
    
    :type: float

  .. attribute:: residue
  
    The residue this atom belongs to. Read-only.
    
    :type: :class:`ResidueHandle`
  
  .. attribute:: is_hetatom
  
    Indicates whether this atom is a *HETATM*. When loading a structure from a 
    PDB file, all non-standard aminoacid and nucleotide atoms as well as ligands 
    are marked as HETATM. 
    
  .. attribute:: bonds
    
    List of bonds this atom is involved in. The bonds are  in no particular 
    order. Also available as :meth:`GetBondList`.
    
    :type: list of :class:`bond handles<BondHandle>`
  
  .. method:: FindBondToAtom(other_atom)

    Finds and returns the bond formed between this atom and `other_atom`. If no 
    bond exists between the two atoms, an invalid :class:`BondHandle` is 
    returned.
    
    :param other_atom: The other atom
    :type  other_atom: :class:`AtomHandle`
    :rtype: :class:`BondHandle`



  .. method:: GetBondCount()
    
    :rtype: int

  .. method:: GetBondList()
    
    See :attr:`bonds`
    
    :rtype: :class:`BondHandleList`

  .. method:: GetBondPartners()
    
    Get list of atoms this atom is bonded with. This method exists as a shortcut 
    to determine all the bonding partners of an atom and avoids code like this: 
    
    .. code-block:: python
    
      bond_parters=[]
      for bond in atom.bonds:
        if bond.first==atom:
          bond_partners.append(bond.second)
        else:
          bond_partners.append(bond.first)

    :rtype: :class:`AtomHandleList`

  .. method:: GetCharge()
  
    See :attr:`charge`
  
    :rtype: float

  .. method:: GetElement()
    
    See :attr:`element`
    
    :rtype: str

  .. method:: GetEntity()
    
    The entity this atom belongs to
    
    :rtype: :class:`EntityHandle`

  .. method:: GetHashCode()
    
    Returns a unique identifier for this atom.
    :rtype: int

  .. method:: GetIndex()
    
    Returns the index of the atom.
    
    :rtype: int

  .. method:: GetMass()
    
    See :attr:`mass`
    
    :rtype: float

  .. method:: GetName()

    See :attr:`name`
    
    :rtype: str

  .. method:: GetOriginalPos()

    :rtype: :class:`~ost.geom.Vec3`

  .. method:: GetPos()

    See :attr:`pos`
    
    :rtype: :class:`~ost.geom.Vec3`

  .. method:: GetQualifiedName()
    
    See :attr:`qualified_name`
    
    :rtype: str

  .. method:: GetRadius()
    
    See :attr:`radius`
    
    :rtype: float

  .. method:: GetResidue()
    
    See :attr:`residue`
    
    :rtype: :class:`ResidueHandle`

  .. method:: IsHetAtom()
    
    See :attr:`is_hetatom`
    
    :rtype: bool

  .. method:: IsValid()
  
    See :attr:`valid`
  
    :rtype: bool
  
The View Classes
--------------------------------------------------------------------------------

.. class:: EntityView

  An entity view represents a structural subset of an :class:`EntityHandle`. For 
  an introduction ,see :doc:`../../intro-01`.
  
  .. attribute:: chains
   
    List of all chains of this entity. The chains are in the same 
    order they have been added.
    
    Also available as :meth:`GetChainList`. To access a single chain, use 
    :meth:`FindChain`.
    
    This property is read-only.
    
  .. attribute:: residues
    
    List of all residues of this entity. The returned residues are first 
    sorted by chain and then from N- to C-terminus.
    
    This property is read-only.
    
    **Example usage:**
    
    .. code-block:: python
    
      for res in ent.residues:
        print res.name, res.atom_count
    
    Also available as :meth:`GetResidueList`. To access a single residue, use 
    :meth:`FindResidue`. 
    
    :type: A list of :class:`ResidueViews <ResidueView>`

  .. attribute:: atoms

    Get list of all atoms of this entity. To access a single atom, use
    :meth:`FindAtom`.
    
    This property is read-only. Also available as :meth:`GetAtomList`
    
    :type: A list of :class:`AtomViews <AtomView>`

  .. attribute:: bounds
  
    Axis-aligned bounding box of the entity view. Read-only.
    
    :type: :class:`ost.geom.AlignedCuboid`

  .. attribute:: handle
  
     The underlying :class:`handle <EntityHandle>` of the entity view. Also 
     available as :meth:`GetHandle`.
     
     :type: :class:`EntityHandle`

  .. method:: CreateEmptyView()
  
    See :meth:`EntityHandle.CreateEmptyView`

    :rtype: EntityView

  .. method:: CreateFullView()

    Returns a copy of this entity. Provided for `duck-typing <http://en.wikipedia.org/wiki/Duck_typing>` purposes.
    
    :rtype: :class:`EntityView`

  .. method:: AddChain(chain_handle[, view_add_flags])

    Add chain to view. By default, only the chain is added to the view, but not 
    its residues and atoms. This behaviour can be changed by passing in an
    appropriate set of `view_add_flags`.

    The following example just adds a chain without residues and atoms:

    .. code-block:: python

        pdb = ost.io.LoadPDB(<PDB file name>)
        v = pdb.CreateEmptyView()
        v.AddChain(pdb.chains[0])

    To **copy** a whole chain, go like:

    .. code-block:: python

        pdb = ost.io.LoadPDB(<PDB file name>)
        v = pdb.CreateEmptyView()
        v.AddChain(pdb.chains[0], ost.mol.INCLUDE_ALL)

    :param chain_handle:
    :type  chain_handle: ChainHandle
    :param view_add_flags: An ORed together combination of `view_add_flags`.
    :type  view_add_flags: :class:`int`
    :rtype: :class:`ChainView`

  .. method:: AddResidue(residue_handle[, view_add_flags])

    Add residue to view. If the residue's chain is not already part of the view, 
    it will be added. By default, only the residue is added, but not its atoms. 
    This behaviour can be modified by passing in an appropriate combination of 
    `view_add_flags`.
    
    :param residue_handle:
    :type  residue_handle: ResidueHandle
    :param view_add_flags:
    :type  view_add_flags: int
    :rtype: :class:`ResidueView`

  .. method:: AddAtom(atom_handle[, view_add_flags])

    Add the atom to view. The chain and residue of the atom are added to the 
    view if they haven't already been added.
    
    
    :param atom_handle: The atom handle
    :type  atom_handle: AtomHandle
    :param view_add_flags: An ORed together combination of ViewAddFlags
    :type  view_add_flags: int
    :rtype: class:`AtomView`

  .. method:: AddBond(bond_handle)

    Add bond to the view.
    
    :param bond_handle: Bond to be added
    :type  bond_handle: BondHandle
    :returns: True if the bond has been added, false if the bond was already 
       present in the view.
    :rtype: bool

  .. method:: AddAllInclusiveBonds()
  
     Convenience method to add all bonds to the view for which both of the bond 
     partners are in the view. This method is useful when manually assembling 
     views.
     
  .. method:: RemoveChain(chain)
    
    Remove chain and all its residues from the entity view.
    
    :param chain: The chain to be removed. May be invalid
    :type  chain: ChainView

  .. method:: RemoveResidue(residue)

    Remove residue from view. 
    
    :param residue: The residue to be removed. May be invalid
    
    :type  residue: ResidueView
    
  .. method:: RemoveAtom(atom)
  
    Remove atom from view

    :param atom: The atom to be removed. May be invalid
    
    :type  atom: AtomView

  .. method:: GetAngle(atom1, atom2, atom3)

    :param atom1:
    :type  atom1: AtomHandle
    :param atom2:
    :type  atom2: AtomHandle
    :param atom3:
    :type  atom3: AtomHandle
    :rtype: float

  .. method:: FindWithin(pos, radius)

    Find all atoms that are within radius of the given position. See 
    :meth:`EntityHandle.FindWithin`.
    
    :param pos:
    :type  pos: :class:`~ost.geom.Vec3`
    :param radius:
    :type  radius: float
    :rtype: class:`AtomViewList`

  .. method:: FindChain(chain_name)

    Find chain by name.
    
    :param chain_name:
    :type  chain_name: str
    :returns: The chain if present in the view, an invalid :class:`ChainView` 
       otherwise
    :rtype: class:`ChainView`

  .. method:: FindResidue(residue)
    
    Find residue view of pointing to the given handle.
      
    :param residue: Residue handle
    :type  residue: ResidueHandle
    :returns: The residue view pointing the the handle, or an invalid handle if  the residue is not part of the view
    :rtype: class:`ResidueView`

  .. method:: FindAtom(chain_name, res_num, atom_name)

    :param chain_name: The chain name
    :type  chain_name: str
    :param res_num: The residue number
    :type  res_num: ResNum or int
    :param atom_name: The name of the atom
    :type  atom_name: str
    :rtype: class:`AtomView`

  .. method:: Select(query[, flags])

    Perform selection on entity view. See :meth:`EntityHandle.Select`.
    
    :param query: The query
    :type  query: (:class:`Query` or str)
    :param flags: An ORed together combination of ViewAddFlags
    :type  flags: int
    
    :rtype: EntityView

  .. method:: Copy()
    
    Returns a deep copy of the entity view. The effect is identical to calling
    
    .. code-block:: python
      
      the_copy=view.Select(')
    
    :rtype: EntityView

  .. method:: GetMass()

    Get total mass of view.

    :rtype: float

  .. method:: GetCenterOfMass()

    Get the center of mass. For a non-mass-weighted center, see 
    :meth:`GetCenterOfAtoms`.
    
    :rtype: :class:`~ost.geom.Vec3`

  .. method:: GetGeometricCenter()
    
    Get the geometric center (the center of the axis-aligned 
    bounding box).
    
    :rtype: :class:`~ost.geom.Vec3`

  .. method:: GetGeometricStart()
    
    :rtype: :class:`~ost.geom.Vec3`

  .. attribute:: chain_count
    
    Number of chains. Read-only. See :meth:`GetChainCount`.
    
    :type: int
    
  .. attribute:: residue_count
    
    Number of residues. Read-only. See :meth:`GetResidueCount`.
    
    :type: int
    
  .. attribute:: atom_count
    
    Number of atoms. Read-only. See :meth:`GetAtomCount`.
    
    :type: int
    
  .. method:: GetCenterOfAtoms()
    
    Calculates the center of atoms. For a mass-weighted center, use 
    :meth:`GetCenterOfMass`.
    
    :rtype: :class:`~ost.geom.Vec3`

  .. method:: GetAtomList()

    See :attr:`atoms`
    
    :rtype: class:`AtomViewList`

  .. method:: GetBondCount()

    Get number of bonds
    :rtype: int

  .. method:: GetChainCount()

    Get number chains. See :attr:`chain_count`
    
    :rtype: int

  .. method:: GetResidueCount()
    
    See :attr:`residue_count`
    
    :rtype: int

  .. method:: GetBondList()

    See :attr:`bonds`
    
    :rtype: :class:`BondHandleList`

  .. method:: GetHandle()
  
    See :attr:`handle`

    :rtype: class:`EntityHandle`

  .. method:: GetResidueList()

    :rtype: class:`ResidueViewList`

  .. method:: GetGeometricEnd()

    :rtype: :class:`~ost.geom.Vec3`

  .. method:: GetChainList()

    See :attr:`chains`
    
    :rtype: class:`ChainViewList`
    
  .. method:: GetAtomCount()
    
    Get number of atoms. See :attr`atom_count`.
    :rtype: int

.. class:: ChainView

  .. attribute:: name
  
     The chain name. The name uniquely identifies the chain in the entity. In 
     most cases, the chain name is one character. This is restriction of the PDB 
     file format. However, you are free to use longer names as long as you don't 
     want to save them as PDB files
     
     This property is read-only. To change the name, use an :class:`XCSEditor`. 
     
     Also available as :meth:`GetName`
     
     :type: str

  .. attribute:: residues
   
     List of all residues of this chain. The residues are sorted from N- to 
     C-terminus. Usually the residue numbers are in ascending order 
     (see :attr:`in_sequence`).
   
     This property is read-only.
   
     **Example usage:**
   
     .. code-block:: python
     
       chain=ent.FindChain("A")
       for res in chain.residues:
         print res.name, res.atom_count
   
     Also available as :meth:`GetResidueList`. To access a single residue, use 
     :meth:`FindResidue`. 

     :type: A list of :class:`residue views<residueView>`
  
  .. attribute:: in_sequence
  
     Whether the residue numbers are in ascending order. For example:
     
     .. code-block:: python
     
       chain=ent.FindChain("A")
       print chain.residues # [A.GLY1, A.GLY2, A.GLY4A, A.GLY4B]
       print chain.in_sequence # prints true
       
       chain=ent.FindChain("B")
       print chain.residues # [B.GLY1, B.GLY4, B.GLY3]
       print chain.in_sequence # prints false

  .. attribute:: atoms

     Get list of all atoms of this chain. To access a single atom, use
     :meth:`FindAtom`.
   
     This property is read-only. Also available as :meth:`GetAtomList`

     :type: A list of :class:`atom handles<AtomHandle>`

  .. attribute:: bounds

    Axis-aligned bounding box of the chain. Read-only
 
    :type: :class:`ost.geom.AlignedCuboid`
    
  .. attribute:: mass
  
    The total mass of this chain in Dalton. Also available as :meth:`GetMass`
  
    :type: float

  .. attribute:: center_of_mass

    Center of mass. Also available as :meth:`GetCenterOfMass`
  
    :type: :class:`~ost.geom.Vec3`
  
  .. attribute:: center_of_atoms
    
    Center of atoms (not mass weighted). Also available as 
    :meth:`GetCenterOfAtoms`.
    
    :type: :class:`~ost.geom.Vec3`

  .. attribute:: handle

    The chain handle this view points to. Also available as :meth:`GetHandle`.
 
    :type: :class:`ChainHandle`
   
  .. attribute:: in_sequence
  
    Whether the residue numbers are in ascending order. Note that the value of 
    `in_sequence` is independent from the value of 
    :attr:`ChainHandle.in_sequence`.
    
    :type: bool

  .. method:: AddAtom(atom_handle[, view_add_flags])

    Add atom to the view. If the residue of the atom is not already part of the 
    view, it will be added. If the atom does not belong to chain, the result is
    undefined. 
    
    :param atom_handle: The atom to be added
    :type  atom_handle: :class:`AtomHandle`
    :param view_add_flags: An ORed together combination of ViewAddFlags
    :type  view_add_flags: int
    :rtype: :class:`AtomView`

  .. method:: AddResidue(residue_handle[, view_add_flags])

    Add residue to the view. If the atom does not belong to chain, the result is
    undefined. By default, only the residue, but no atoms are added to the view. 
    To change the behavior, pass in a suitable combination of `view_add_flags`.
    
    :param residue_handle:
    :type  residue_handle: :class:`ResidueHandle`
    :param view_add_flags:
    :type  view_add_flags: int
    :rtype: :class:`ResidueView`

  .. method:: FindAtom(res_num, atom_name)

    Find atom with the given residue number and atom name. 
    
    :param res_num: The residue number
    :type  res_num: :class:`ResNum`
    :param atom_name: The atom name
    :type  atom_name: str
    :returns: The atom view, or an invalid atom view if no atom with the given 
       parameters is part of the view.
    
    :rtype: :class:`AtomView`

  .. method:: FindResidue(res_num)

    Find residue by residue number.
    
    :param res_num:
    :type  res_num: :class:`ResNum`
    :rtype: :class:`ResidueView`
    :returns: The residue view, or an invalid residue view if no residue with 
       the given residue number is in the view.

  .. method:: GetCenterOfAtoms()

    See :attr:`center_of_atoms`
    
  .. method:: GetCenterOfMass()

    See :attr:`center_of_mass`
    
  .. method:: GetEntity()
    
    See :attr:`entity`

  .. method:: GetGeometricCenter()
    
    See :attr:`geometric_center`

  .. method:: GetHandle()

    See :attr:`handle`

  .. method:: GetMass()

    See :attr:`mass`

  .. method:: GetName()

    See :attr:`name`

  .. method:: GetResidueByIndex(index)
    
    Returns the residue view at `index`. If index is negative or bigger than 
    the number of residues minus one, an invalid ResidueView is returned.
    
    :param index: The index
    :type  index: int
    :rtype: :class:`ResidueView`

  .. method:: GetResidueList()

    See :attr:`residues`

    :rtype: bool

  .. method:: InSequence()
  
    See :attr:`in_sequence`

  .. method:: IsValid()

    See :attr:`valid`

  .. method:: RemoveResidue(residue)

    Remove residue from chain. If the residue is not part of the view, the 
    chain is left unchanged.
    
    :param residue: The residue view. May be invalid
    :type  residue: :class:`ResidueView`
    :rtype: None

  .. method:: RemoveResidues()
   
    Remove all residues from this chain view

  .. method:: Select(query[, flags])

    Perform query on chain. This will return an entity view containing atoms 
    and bonds of the residue view that match the residue. In case no atom 
    matches the predicate, an empty view is returned.
    
    :param query: The query
    :type  query: :class:`Query` or str
    :param flags: An ORed together combination of query flags
    :type  flags: int
    :rtype: :class:`EntityView`

.. class:: ResidueView

  .. attribute:: handle

    The residue handle this view points to. Also available as 
    :meth:`GetHandle`.

    :type: :class:`ResidueHandle`

  .. attribute:: name

    The residue name is usually a str of 3 characters, e.g. `GLY` for 
    glycine or `ALA` for alanine, but may be shorter, e.g. `G` for guanosine, 
    or longer for structures loaded from formats other than PDB.
  
    This property is read-only. To change the name of the residue, use
    :meth:`EditorBase.SetResidueName`.

  .. attribute:: number

    The number of this residue. The residue number has a numeric part and an
    insertion-code. This property is read-only. Also available as 
    :meth:`GetNumber`.
  
    :type: :class:`ResNum`

  .. attribute:: one_letter_code

    For amino acids, and nucleotides the `one_letter_code` is an alpha-numeric 
    character. For unknown or more *exotic* residues, the one letter code is set 
    to '?'.
  
    **Example**
  
    This code-snippet shows how to get the sequence string from a list of 
    residues.
  
    .. code-block:: python
  
      print ''.join([r.one_letter_code for r in chain.residues])
  
    :type: str
  
  .. attribute:: bounds

    Axis-aligned bounding box of the residue view. Read-only

    :type: :class:`ost.geom.AlignedCuboid`

  .. attribute:: mass

    The total mass of this residue in Dalton. Also available as :meth:`GetMass`.

    :type: float

  .. attribute:: center_of_mass

    Center of mass. Also available as :meth:`GetCenterOfMass`

    :type: :class:`~ost.geom.Vec3`

  .. attribute:: center_of_atoms
  
    Center of atoms (not mass weighted). Also available as 
    :meth:`GetCenterOfAtoms`.
  
    :type: :class:`~ost.geom.Vec3`

  .. attribute:: chain

    The chain this residue belongs to. Read-only. Also available as 
    :meth:`GetChain`
  
    :type: :class:`ChainView`

  .. attribute:: handle
  
    The residue handle this view points to
    
    :type: :class:`ResidueHandle`
    
  .. method:: RemoveAtom(atom_view)
  
    Remove atom from residue and all associated bonds. If the atom is not part 
    of the view, the residue view is left unchanged.
    
    :param atom_view: The atom to be removed. May be an invalid handle
    :type  atom_view: :class:`AtomView`
    :rtype: None

  .. method:: GetHandle()

    See :attr:`handle`

  .. method:: GetMass()

    See :attr:`mass`

  .. method:: GetChain()

    See :attr:`chain`

  .. method:: FindAtom(atom_name)
    
    Find atom by name. Returns the atom, or an invalid atom if no atoms with 
    the given name is part of the view.
    
    :param atom_name:
    :type  atom_name: str
    :rtype: :class:`AtomView`

  .. method:: GetIndex()
    
    See :attr:`index`

  .. method:: GetCenterOfMass()

    See :attr:`center_of_mass`

  .. method:: IsAtomIncluded(atom_handle)

    Returns true if the given atom is part of the view, false if not.
    
    :param atom_handle:
    :type  atom_handle: :class:`AtomHandle`
    :rtype: bool

  .. method:: GetGeometricCenter()

    See :attr:`geometric_center`

  .. method:: AddAtom(atom_handle[, flags])

    Add atom to the view. 
    :param atom_handle:
    :type  atom_handle: :class:`AtomHandle`
    :param flags: An ORed together combination of ViewAddFlags.
    :type  flags: int
    :rtype: :class:`AtomView`

  .. method:: GetCenterOfAtoms()

    See :attr:`center_of_atoms`

  .. method:: GetAtomList()

    See :attr:`atoms`
    
  .. method:: Select(query[, flags])
   
    Perform selection on residue view. This method will return an entity view 
    containing all atoms and bonds of the residue matching the predicate. In 
    case no atom matches the predicate, this will return an empty view.
    
    :param query: The query
    :type  query: :class:`Query` or str
    :param flags: An ORed together combination of query flags
    :type  flags: int
    :rtype: :class:`EntityView`


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

.. function:: CreateEntityFromView(view, include_exlusive_atoms, handle=EntityHandle())
 
  This function behaves exactly like :meth:`EntityHandle.Copy`, except that only
  atoms, residues, chains and bonds that are present in the view will be 
  copied.
   
  :param view: is the view to be converted to a handle
  :param include_exlusive_atoms: if true, atoms that are part of an exclusive
       bond (only one bond partner is included in the view) will also be included
       in the new entity handle.
  :param handle: If invalid a new entity will be created. If valid, the atoms, 
       residues, chains, bonds and torsions will be added to handle. This is 
       useful to combine several entities into one.

  :returns :class:`EntityHandle`
  
.. _chaintype:

ChainType
--------------------------------------------------------------------------------

A ChainType fills the :attr:`ChainHandle.type` attribute. Different types are
described in the :ref:`ChainType enum <chaintype_enum>`. Functions for setting
a type belong to the :class:`EditorBase` class, getting is provided by the
:class:`ChainHandle` class, further convenience functions are described here.

.. _chaintype_enum:

The ChainType enum
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The ChainType enum enumerates all types defined by the PDB for the MMCif file
format. Following values are supported:

  ``CHAINTYPE_POLY``, ``CHAINTYPE_NON_POLY``, ``CHAINTYPE_WATER``,
  ``CHAINTYPE_POLY_PEPTIDE_D``, ``CHAINTYPE_POLY_PEPTIDE_L``,
  ``CHAINTYPE_POLY_DN``, ``CHAINTYPE_POLY_RN``, ``CHAINTYPE_POLY_SAC_D``,
  ``CHAINTYPE_POLY_SAC_L``, ``CHAINTYPE_POLY_DN_RN``,
  ``CHAINTYPE_UNKNOWN``, ``CHAINTYPE_N_CHAINTYPES``  

Where ``CHAINTYPE_N_CHAINTYPES`` holds the number of different types available.

Setter & Getter functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

:func:`EditorBase.SetChainType`, :func:`ChainHandle.GetType`


ChainType functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. function:: ChainTypeFromString(identifier)

   Create a ChainType item for a given string.

   :param identifier: String to request a type for.
   :type identifier: :class:`str`
   :raises: :class:`runtime_error` if **identifier** is unrecognised.

   :returns: :class:`ChainType`

.. function:: StringFromChainType(type)

   Return the String identifier for a given **type**.

   :param type: To be translated
   :type type: :class:`ChainType`
   :raises: :class:`runtime_error` if **type** is unrecognised.

   :returns: :class:`str`
