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
  HarmonicDistanceRestraint, Invalid

The implementation guarantees, that the parameter related to atom types,
names, parameters are checked regarding their dimension (e.g. 3 atoms and 2 
parameters for a harmonic angle) 


Interaction Parametrization
--------------------------------------------------------------------------------

* HarmonicBond, Interacting Particles: 2, Num Parameters: 2

  #. :class:`float` bond length in nm
  #. :class:`float` force constant in kJ/mol/nm^2

* HarmonicAngle, Interacting Particles: 3, Num Parameters: 2 

  #. :class:`float` angle in radians
  #. :class:`float` force constant in kJ/mol/radian^2 

* UreyBradleyAngle, Interacting Particles: 3, Num Parameters: 4

  #. :class:`float` angle in radians
  #. :class:`float` angle force constant in kJ/mol/radian^2
  #. :class:`float` bond length in nm
  #. :class:`float` bond force constant in kJ/mol/nm^2 

* PeriodicDihedral, Interacting Particles: 4, Num Parameters: 3

  #. :class:`int` multiplicity
  #. :class:`float` phase in radians
  #. :class:`float` force constant in kJ/mol/radian^2

* PeriodicImproper, Interacting Particles: 4, Num Parameters: 3

  #. :class:`int` multiplicity
  #. :class:`float` phase in radians
  #. :class:`float` force constant in kJ/mol/radian^2

* HarmonicImproper, Interacting Particles: 4, Num Parameters: 2

  #. :class:`float` angle in radians
  #. :class:`float` force constant in 
  #. :class:`float` force constant in kJ/mol/radian^2

* CMap, Interacting Particles: 5, Num Parameters: n*n+1 

  #. :class:`list` of :class:`float` of size n*n+1, first value describes the dimension n of the cmap, the other n*n values describe the correction map in kJ/mol in the ordering (phi1,psi1),(phi1,psi2)... and torsion angles in [-pi,pi] (will be converted to the format OpenMM prefers)

* LJ, Interacting Particles: 1, Num Parameters: 2

  #. :class:`float` sigma in nm
  #. :class:`float` epsilon in kJ/mol

* LJPair, Interacting Particles: 2, Num Parameters: 2

  #. :class:`float` sigma in nm
  #. :class:`float` epsilon in kJ/mol

* GBSA, Interacting Particles: 1, Num Parameters: 2

  #. :class:`float` GBSA radius in nm
  #. :class:`float` OBC scaling factor

* DistanceConstraint, Interacting Particles: 2, Num Parameters: 1

  #. :class:`float` constraint distance

* Exclusion, Interacting Particles: 2, Num Parameters: 0

* HarmonicPositionRestraint, Interacting Particles: 1, Num Parameters: 7

  #. :class:`float` ref pos x
  #. :class:`float` ref pos y
  #. :class:`float` ref pos z
  #. :class:`float` force constant
  #. :class:`float` x_scale
  #. :class:`float` y_scale
  #. :class:`float` z_scale


* HarmonicDistanceRestraint, Interacting Particles: 2, Num Parameters: 2

  #. :class:`float` restraint length
  #. :class:`float` force constant



.. class:: Interaction(func_type)

  :param func_type:     :class:`FuncType` defining a particular interaction


  .. method:: SetTypes(types)

    Sets the forcefield specific atom types.

    :param types:       :class:`list` of :class:`str` A list of strings 
                        describing the force field specific atom types.

    :raises:            :class:`RuntimeError` when size of input is not consistent with
                                  the interactions functype


  .. method:: SetNames(names)

    Sets atom names.

    :param types:       :class:`list` of :class:`str` describing the atom names.

    :raises:            :class:`RuntimeError` when size of input is not 
                        consistent with the interactions functype


  .. method:: SetParam(names)

    Sets interaction specific parameters.

    :param types:  A :class:`list` of :class:`float` values .

    :type names:   :class:`list`

    :raises: :exc:`RuntimeError` when size of input is not consistent with
                                  the interactions functype


  .. method:: GetTypes()

    Get Previously set types

    :returns:      :class:`list`



  .. method:: GetNames()

    Get Previously set names

    :returns:      :class:`list`


  .. method:: GetParam()

    Get Previously set parameters

    :returns:      :class:`bool`


  .. method:: GetAtoms(residue)

    Returns an AtomHandleList containing the atoms in residue matching
    the atom names of the interaction

    :param residue:     :class:`mol.ResidueHandle`

    :returns:           :class:`mol.AtomHandleList`

    :raises:            :class:`RuntimeError` when atom cannot be found in 
                        residue

  .. method:: GetFuncType()

    Returns the functype enum of the interaction

    :returns:      :class:`FuncType`  


  .. method:: ReplaceAtom(name, new_name, new_type)

    Searches for given atom name. If found, this particular atom name
    gets changed to new_name. If the atom types are set as well, the
    associated atom type gets also reset to new_type.

    :param name:        class:`str`
    :param new_name:    class:`str`
    :param new_type:    class:`str`
    :returns:           :class:`bool` whether replacement was successful or not


  .. method:: MatchTypes(atom_types)

    Checks, whether the given types match the internal types. The match
    can be "as is" or in reversed order

    :param atom_types:  :class:`list` of :class:`str`
    :returns:           :class:`bool`


  .. method:: MatchNames(atom_names)

    Checks, whether the given names match the internal names. The match
    can be "as is" or in reversed order

    :param atom_names:  :class:`list` of :class:`str`
    :returns:           :class:`bool`


  .. method:: HasName(name)

    Checks, whether the given name is present in the internal names

    :param name:        :class:`string`
    :returns:           :class:`bool`


  .. method:: HasType(type)

    Checks, whether the given type is present in the internal types

    :param type:        :class:`string`
    :returns:           :class:`bool`


  .. method:: IsParametrized()

    Checks, whether the parameters have been set

    :returns:           :class:`bool`


  .. method:: HasTypeWildcard()

    Checks, whether one of the types is a wildcard ('X')

    :returns:           :class:`bool`


  .. method:: HasNameWildcard()

    Checks, whether one of the names is a wildcard ('X')

    :returns:           :class:`bool`
