Blocks
================================================================================

.. currentmodule:: ost.mol

The most basic type of a residue description is the BuildingBlock. It contains
information of atom names and their corresponding types, charges and
optionally also their masses. Interactions for all particles can also
be defined even though they are optional, except the bonds giving
information about the connectivity. 
You often need to manipulate building blocks or
the residues they refer to in an automated manner. That's where the 
Blockmodifiers come in, with the GromacsBlockModifier as a specific
implementation. As a special case there also exist HydrogenConstructors.


.. class:: BuildingBlock

  .. method:: Match(residue, match_connectivity)

    Checks, wether the given residue matches the atom names in the 
    BuildingBlock. The Connectivity gets optionally checked.

    :param residue:    
    :param match_connectivity: If set to true, the function checks the bonds
                               in the residue with the defined bonds in the
                               buildingblock 

    :type residue:    :class:`ResidueHandle`
    :type match_connectivity:    :class:`bool`


  .. method:: Connect(residue, editor)

    Connects residue based on the bond definitions of the BuildingBlock.

    :param residue:     residue to be connected
    :param editor:      editor associated to the residues entity

    :type residue:      :class:`ResidueHandle`
    :type editor:       :class:`XCSEditor`

    :raises:            :class:`RuntimeError` when required atom can not be found in 
                        residue


  .. method:: AddAtom(name, type, charge, mass = None)

    :param name:        name of atom
    :param type:        its corresponding forcefield type
    :param charge:      its charge
    :param mass:        its mass


  .. method:: RemoveAtom(name)

    removes atom from building block with all its associated values and 
    interactions

    :param name:        name of atom to be removed


  .. method:: ReplaceAtom(name, new_name, new_type, new_charge, new_mass = None)

    Replace given atom with and resets type, charge and mass. All interactions
    containing that atom get adapted as well

    :param name:        name of atom to be replaced
    :param new_name:    new name of atom
    :param new_type:    new type of atom
    :param new_charge:  new charge of atom
    :param new_mass:    new mass of atom


  .. method:: RemoveInteractionsToPrev()

    Removes all interactions associated to an atom of a previous residues.
    This gets indicated by an atom name prefixed by a *-*


  .. method:: RemoveInteractionsToNext()

    Removes all interactions associated to an atom of a next residues.
    This gets indicated by an atom name prefixed by a *+*


  .. method:: AddBond(bond, replace_existing = False)

    :param bond:        Bond to be added
    :param replace_existing: Whether potentially already existing bond for the
                             same atoms should be replaced.

    :type bond:         :class:`Interaction`
    :type replace_exisiting: bool


  .. method:: AddAngle(angle, replace_existing = False)

    :param angle:        Angle to be added
    :param replace_existing: Whether potentially already existing angle for the
                             same atoms should be replaced.

    :type angle:         :class:`Interaction`
    :type replace_exisiting: bool


  .. method:: AddDihedral(dihedral, replace_existing = False)

    :param dihedral:    Dihedral to be added
    :param replace_existing: Whether potentially already existing dihedral for the
                             same atoms should be replaced.

    :type dihedral:     :class:`Interaction`
    :type replace_exisiting: bool


  .. method:: AddImproper(improper, replace_existing = False)

    :param improper:    Improper to be added
    :param replace_existing: Whether potentially already existing improper for the
                             same atoms should be replaced.

    :type improper:     :class:`Interaction`
    :type replace_exisiting: bool


  .. method:: AddExclusion(exclusion, replace_existing = False)

    :param exclusion:        Exclusion to be added
    :param replace_existing: Whether potentially already existing Exclusion for the
                             same atoms should be replaced.

    :type exclusion:         :class:`Interaction`
    :type replace_exisiting: bool


  .. method:: AddCMap(cmap, replace_existing = False)

    :param cmap:        CMap to be added
    :param replace_existing: Whether potentially already existing cmap for the
                             same atoms should be replaced.

    :type cmap:         :class:`Interaction`
    :type replace_exisiting: bool


  .. method:: AddConstraint(constraint, replace_existing = False)

    :param constraint:  Constraint to be added
    :param replace_existing: Whether potentially already existing constraint for the
                             same atoms should be replaced.

    :type constraint:         :class:`Interaction`
    :type replace_exisiting: bool 


  .. method:: GetType(name)

    Gets forcefield type from atom with given name

    :param name:        Name from atom you want the type from

    :type name:         :class:`str`

    :raises: :exc: `RuntimeError` when atom can not be found in 
                                  BuildingBlock    

  .. method:: GetCharge(name)

    Gets charge from atom with given name

    :param name:        Name from atom you want the charge from

    :type name:         :class:`str`

    :raises: :class:`RuntimeError` when atom can not be found in 
                                   BuildingBlock 


  .. method:: GetMass(name)

    Gets mass from atom with given name

    :param name:        Name from atom you want the mass from

    :type name:         :class:`str`

    :raises: :class:`RuntimeError` when atom can not be found in 
                                   BuildingBlock 


  .. method:: GetBonds()
    :returns: list of all bonds


  .. method:: GetAngles()
    :returns: list of all angles


  .. method:: GetDihedrals()
    :returns: list of all dihedrals


  .. method:: GetImpropers()
    :returns: list of all impropers


  .. method:: GetCMaps()
    :returns: list of all cmaps


  .. method:: GetExclusions()
    :returns: list of all exlusions


  .. method:: GetConstraints()
    :returns: list of all constraints

Automated modification of :class:`BuildingBlock` and :class:`ResidueHandle`
--------------------------------------------------------------------------------

.. class:: GromacsBlockModifier

  .. method:: ApplyOnBuildingBlock(block)

    Applies all defined rules on the given :class:`BuildingBlock`

    :param block:       :class:`BuildingBlock`

  .. method:: ApplyOnResidue(residue)

    Applies all defined rules on the given :class:`ResidueHandle`

    :param residue:     :class:`ResidueHandle`

  .. method:: AddReplaceRule(name, new_name, new_type, new_charge)

    Rule, that basically renames and an atom and also resets its type and charge
    in a :class:`BuildingBlock`. A simple renaming occurs in a :class:`ResidueHandle`.

    :param name:        :class:`str` name of the atom to be changed
    :param new_name:    :class:`str`
    :param new_type:    :class:`str`
    :param new_charge:  :class:`float`

  .. method:: AddAddRule(number, method, atom_names, anchors, type, charge)

    A rule to add new atoms the gromacs way, see
    `Gromacs Manual <http://www.gromacs.org/Documentation/Manual>`_ 
    for the exact definition of the parameters. A :class:`BuildingBlock`
    gets modified by adding the new atom definitions and also the
    corresponding bonds describing the connectivity. In case of 
    :class:`ResidueHandle` the new Atoms with connectivity get added with 
    their positions as defined by the gromacs adding rule. 

    :param number:      :class:`int` number of atoms to be added
    :param method:      :class:`int` gromacs adding rule
    :param atom_names:  :class:`list` of strings containing the new atom names
    :param anchors:     :class:`list` of strings containing atom names used as anchor
    :param type:        :class:`str` the type the atoms will have
    :param charge:      :class:`float` the charge the atoms will have

  .. method:: AddDeleteAtom(name)

    Defines an atom that has to be removed. In case of the 
    :class:`BuildingBlock` this removes this particular atom plus all 
    interactions connected to it, in case of :class:`ResidueHandle`, the
    atom simply gets deleted.

    :param name:        :class:`str` atom to be deleted

  .. method:: AddBond(bond)

    Adds a bond, this only has effect on :class:`BuildingBlock`, not
    on :class:`ResidueHandle`

    :param bond:        :class:`Interaction` bond to be added

  .. method:: AddAngle(angle)

    Adds an angle, this only has effect on :class:`BuildingBlock`, not
    on :class:`ResidueHandle`

    :param angle:        :class:`Interaction` angle to be added

  .. method:: AddDihedral(dihedral)

    Adds a dihedral, this only has effect on :class:`BuildingBlock`, not
    on :class:`ResidueHandle`

    :param dihedral:        :class:`Interaction` dihedral to be added

  .. method:: AddImproper(improper)

    Adds an improper, this only has effect on :class:`BuildingBlock`, not
    on :class:`ResidueHandle`

    :param improper:        :class:`Interaction` improper to be added

  .. method:: AddCMap(cmap)

    Adds a cmap, this only has effect on :class:`BuildingBlock`, not
    on :class:`ResidueHandle`

    :param cmap:        :class:`Interaction` cmap to be added


.. class:: GromacsHydrogenConstructor

  The :class:`GromacsHydrogenConstructor` is the gromacs way of adding
  hydrogens to a structure.

  .. method:: ApplyOnBuildingBlock(block)

    Guess what it does: !!ABSOLUTELY NOTHING!! just there for consistency

    :param block:       :class:`BuildingBlock` block that won't be changed at all!
                                               Isn't that awesome?

  .. method:: ApplyOnResidue(residue)

    Constructs hydrogens based on the defined hydrogen adding rules

    :param residue:     :class:`ResidueHandle` to be modified

  .. method:: AddHydrogenRule(number, method, hydrogen_names, anchors)

    Adds a hydrogen building rule as defined in 
    `Gromacs Manual <http://www.gromacs.org/Documentation/Manual>`_ 
    
    :param number:      :class:`int` number of hydrogens to be added
    :param method:      :class:`int` gromacs adding rule
    :param hydrogen_names:  :class:`list` of strings containing the hydrogen names
    :param anchors:     :class:`list` of strings containing atom names used as anchor


.. class:: HeuristicHydrogenConstructor

  As soon as we leave the well defined world of gromacs residue definitions,
  we have to find new ways for constructing hydrogens. The 
  :class:`HeuristicHydrogenConstructor` takes a :class:`BuildingBlock` at
  initialization and builds heuristic rules to build hydrogens based on 
  the connecticity defined in the given block.

  .. method:: ApplyOnBuildingBlock(block)

    Guess what it does: !!ABSOLUTELY NOTHING!! just there for consistency

    :param block:       :class:`BuildingBlock` block that won't be changed at all!
                                               Isn't that awesome?

  .. method:: ApplyOnResidue(residue)

    Constructs hydrogen based on heuristic rules

    :param residue:     :class:`ResidueHandle` to be modified

