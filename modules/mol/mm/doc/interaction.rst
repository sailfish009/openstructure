Interactions
================================================================================

.. currentmodule:: ost.mol.mm

The interaction object is intended to build a basic container tat can be used
in various scenarios. At the initialization a FuncType has to be defined.
Whenever names, types or parameters are set, the number of parameters
gets checked according to the FuncType. A name or type can also consist of
a wildcard expressed as X.


The FuncType enum
--------------------------------------------------------------------------------

Every Interaction is linked to one particular *FuncType* describing the types 
of interactions supported by OpenStructure. There are a 15 possible values with
Invalid defining an unknown interaction:

  HarmonicBond, HarmonicAngle, UreyBradleyAngle, PeriodicDihedral, 
  PeriodicImproper, HarmonicImproper, CMap, LJ, LJPair, GBSA, 
  DistanceConstraint, Exclusion, HarmonicPositionRestraint,
  HarmonicPositionRestraint, Invalid

The implementation guarantees, that the parameter related to atom types,
names, parameters are checked regarding their dimension (e.g. 3 atoms for
a harmonic angle) 

--------------------------------------------------------------------------------


.. class:: Interaction(func_type)

  :param func_type:     :class:`FuncType` defining a particular interaction

  ..method:: SetTypes(types)

    Sets the forcefield specific atom types.

    :param types:   A list of strings describing the force field specific atom
                    types.

    :type types:    :class:`list`

    :raises: :exc:` RuntimeError` when size of input is not consistent with
                                  the interactions functype


  ..method:: SetNames(names)

    Sets atom names.

    :param types:   A list of strings describing the atom names.

    :type names:    :class:`list`

    :raises: :exc:` RuntimeError` when size of input is not consistent with
                                  the interactions functype


  ..method:: SetParam(names)

    Sets interaction specific parameters.

    :param types:  A list of real values .

    :type names:   :class:`list`

    :raises: :exc:`RuntimeError` when size of input is not consistent with
                                  the interactions functype


  ..method:: GetTypes()

    Get Previously set types

    :returns:      :class:`list`



  ..method:: GetNames()

    Get Previously set names

    :returns:      :class:`list`


  ..method:: GetParam()

    Get Previously set parameters

    :returns:      :wtf:`bool`


  ..method:: GetAtoms(residue)

    Returns an AtomHandleList containing the atoms in residue matching
    the atom names

    :param residue :class:`~mol.ResidueHandle`
    :returns:      :class:`~mol.AtomHandleList`
    :raises: :exc:`RuntimeError` when atom cannot be found in residue


  ..method:: GetFuncType()

    Returns the functype enum of the interaction

    :returns:      :enum:`mol.mm.FuncType`  


  ..method:: ReplaceAtom(name, new_name, new_type)

    Searches for given atom name. If found, this particular atom name
    gets changed to new_name. If the atom types are set as well, the
    associated atom type gets also reset to new_type.

    :param name    :class:`string`
    :param new_name:class:`string`
    :param new_type:class:`string`
    :returns:      :wtf:`bool` whether replacement was successful or not


  ..method::MatchTypes(atom_types)

    Checks, whether the given types match the internal types. The match
    can be "as is" or in reversed order

    :param atom_types    :class:`list`
    :returns:     `:wtf:`bool`


  ..method::MatchNames(atom_names)

    Checks, whether the given names match the internal names. The match
    can be "as is" or in reversed order

    :param atom_names    :class:`list`
    :returns:     `:wtf:`bool`


  ..method::HasName(name)

    Checks, whether the given name is present in the internal names

    :param name    :class:`string`
    :returns:      :wtf:`bool`


  ..method::HasType(type)

    Checks, whether the given type is present in the internal types

    :param type    :class:`string`
    :returns:      :wtf:`bool`


  ..IsParametrized()

    Checks, whether the parameters have been set

    :returns:      :wtf:`bool`


  ..HasTypeWildcard()

    Checks, whether one of the types is a wildcard (X)

    :returns:      :wtf:`bool`


  ..HasNameWildcard()

    Checks, whether one of the names is a wildcard (X)

    :returns:      :wtf:`bool`
