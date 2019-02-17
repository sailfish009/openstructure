Linear Database
===============================================================================

.. currentmodule:: ost.db


Many applications require to load lots of structures. Expecially on distributed
file systems, io becomes a problem.
OST provides a linear database to dump position data, e.g. CA positions, or 
character data, e.g. sequences, to allow fast retrieval.
The actual data container behave like linear data arrays and the idea is to
use an indexer to keep track of where to find data for a certain entry.

.. class:: LinearIndexer()

  The idea of the :class:`LinearIndexer` is to keep track of locations
  of data assuming a linear memory layout. The level of entries in the indexer
  are assemblies that can contain an arbitrary number of chains with varying
  length. Whenever a new assembly is added, a range enclosing all residues of
  that assembly is defined that is subsequent to the range of the previously
  added assembly. It is then not only possible to access the range of the full
  assembly, but also the range of single chains. 
  Whenever an assembly with *n* residues is deleted, the ranges of all 
  assemblies that have been added later on are reduced by *n*.

  .. method:: Load(filename)

    Loads indexer from file

    :param filename:    Path to file to be loaded
    :type filename:     :class:`str`
    :returns:           The loaded indexer
    :rtype:             :class:`LinearIndexer`
    :raises:            :exc:`ost.Error` if *filename* cannot be
                        opened


  .. method:: Save(filename)

    Saves indexer to file

    :param filename:    Path to file where the indexer is stored
    :type filename:     :class:`str`
    :raises:            :exc:`ost.Error` if *filename* cannot be       
                        created


  .. method:: AddAssembly(name, chain_names, chain_lenths)

    Adds a new assembly to the indexer. The range assigned to that assembly is 
    subsequent to the previously added assembly.

    :param name:        Name of the added assembly
    :param chain_names: Names of all chains of the added assembly
    :param chain_lengths: The according lengths of the chains
    :type name:         :class:`str`
    :type chain_names:  :class:`list` of :class:`str`
    :type chain_lengths: :class:`list` of :class:`int`

    :raises:            :exc:`ost.Error` if lengths of 
                        *chain_names* and *chain_lengths* is inconsistent


  .. method:: RemoveAssembly(name)

    Removes an assembly from the indexer. Assuming that assembly contains
    a total of *n* residues, all ranges of the subsequent assemblies are
    reduced by *n*.

    :param name:        Name of the assembly to be removed
    :type param:        :class:`str`

    :raises:            :exc:`ost.Error` if *name* is not present

      
  .. method:: GetAssemblies()

    :returns:           The names of all added assemblies
    :rtype:             :class:`list` of :class:`str`
    :raises:            :exc:`ost.Error` if *name* is not present

  
  .. method:: GetChainNames(name)

    :param name:        Name of assembly from which you want the chain names
    :type name:         :class:`str`

    :returns:           The chain names of the specified assembly
    :rtype:             :class:`list` of :class:`str`

    :raises:            :exc:`ost.Error` if *name* is not present


  .. method:: GetChainLengths(name)

    :param name:        Name of assembly from which you want the chain lengths
    :type name:         :class:`str`

    :returns:           The chain lengths of the specified assembly
    :rtype:             :class:`list` of :class:`int`

    :raises:            :exc:`ost.Error` if *name* is not present


  .. method:: GetDataRange(name)

    Get the range for a full assembly

    :param name:        Name of the assembly from which you want the range
    :type name:         :class:`str`
    :returns:           Two values defining the range as [from, to[
    :rtype:             :class:`tuple` of :class:`int`

    :raises:            :exc:`ost.Error` if *name* is not present


  .. method:: GetDataRange(name, chain_name)

    Get the range for a chain of an assembly

    :param name:        Name of the assembly from which you want the range
    :param chain_name:  Name of the chain from which you want the range
    :type name:         :class:`str`
    :type chain_name:   :class:`str`
    :returns:           Two values defining the range as [from, to[
    :rtype:             :class:`tuple` of :class:`int`

    :raises:            :exc:`ost.Error` if *name* is not present
                        or the according assembly has no chain with specified
                        chain name


  .. method:: GetNumResidues()

    :returns:           The total number of residues in all added assemblies
    :rtype:             :class:`int`



.. class:: LinearCharacterContainer()

  The :class:`LinearCharacterContainer` stores characters in a linear memory 
  layout that can represent sequences such as SEQRES or ATOMSEQ. 
  It can be accessed using range parameters and the idea is to keep it in sync 
  with a :class:`LinearIndexer`.

  .. method:: Load(filename)

    Loads container from file

    :param filename:    Path to file to be loaded
    :type filename:     :class:`str`
    :returns:           The loaded container
    :rtype:             :class:`LinearCharacterContainer`
    :raises:            :exc:`ost.Error` if *filename* cannot be
                        opened


  .. method:: Save(filename)

    Saves container to file

    :param filename:    Path to file where the container is stored
    :type filename:     :class:`str`
    :raises:            :exc:`ost.Error` if *filename* cannot be       
                        created

  .. method:: AddCharacters(characters)

    Adds *characters* at the end of the internal data. Call this function with
    appropriate data whenever you add an assembly to the associated
    :class:`LinearIndexer`

    :param characters:  Characters to be added
    :type characters:   :class:`str`


  .. method:: ClearRange(range)

    Removes all characters specified by *range* in form [*from*, *to* [ 
    from the internal data. 
    The internal data layout is linear, all characters starting from *to*
    are shifted to the location defined by *from*.
    Call this function with appropriate range whenever you remove an assembly
    from the associated :class:`LinearIndexer`

    :param range:       Range to be deleted in form [from, to[
    :type range:        :class:`tuple` of :class:`int`

    :raises:            :exc:`ost.Error` if *range* does not
                        specify a valid range

 
  .. method:: GetCharacter(idx) 

    :returns:           The character at the specified location
    :rtype:             :class:`str`
    :raises:            :exc:`ost.Error` if *idx* does not
                        specify a valid position


  .. method:: GetCharacters(range) 

    :returns:           The characters from the specified range
    :rtype:             :class:`str`
    :raises:            :exc:`ost.Error` if *range* does not
                        specify a valid range
 
  .. method:: GetNumElements()

    :returns:           The number of stored characters
    :rypte:             :class:`int`
   


.. class:: LinearPositionContainer()

  The :class:`LinearPositionContainer` stores positions in a linear memory 
  layout. 
  It can be accessed using range parameters and the idea is to keep it in sync 
  with a :class:`LinearIndexer`. In order to save some memory, a lossy 
  compression is applied that results in a limited accuracy of two digits.
  if the absolute value of your added position is very large (> ~10000),
  the accuracy is further lowered to one digit. This is all handled internally.

  .. method:: Load(filename)

    Loads container from file

    :param filename:    Path to file to be loaded
    :type filename:     :class:`str`
    :returns:           The loaded container
    :rtype:             :class:`LinearPositionContainer`
    :raises:            :exc:`ost.Error` if *filename* cannot be
                        opened


  .. method:: Save(filename)

    Saves container to file

    :param filename:    Path to file where the container is stored
    :type filename:     :class:`str`
    :raises:            :exc:`ost.Error` if *filename* cannot be       
                        created


  .. method:: AddPositions(positions)

    Adds *positions* at the end of the internal data. Call this function with
    appropriate data whenever you add an assembly to the associated
    :class:`LinearIndexer`

    :param positions:   Positions to be added
    :type positions:    :class:`ost.geom.Vec3List`


  .. method:: ClearRange(range)

    Removes all positions specified by *range* in form [*from*, *to* [ 
    from the internal data. 
    The internal data layout is linear, all positions starting from *to*
    are shifted to the location defined by *from*.
    Call this function with appropriate range whenever you remove an assembly
    from the associated :class:`LinearIndexer`

    :param range:       Range to be deleted in form [from, to[
    :type range:        :class:`tuple` of :class:`int`

    :raises:            :exc:`ost.Error` if *range* does not
                        specify a valid range

 
  .. method:: GetPosition(idx, pos) 

    Extracts a position at specified location. For efficiency reasons,
    the function requires the position to be passed as reference.

    :param idx:         Specifies location
    :param pos:         Will be altered to the desired position
    :type idx:          :class:`int`
    :type pos:          :class:`ost.geom.Vec3`

    :raises:            :exc:`ost.Error` if *idx* does not
                        specify a valid position


  .. method:: GetPositions(range, positions) 

    Extracts positions at specified range. For efficiency reasons, the function
    requires the positions to be passed as reference.

    :param range:       Range in form [from,to[ that defines positions 
                        to be extracted
    :param positions:   Will be altered to the desired positions
    :type range:        :class:`tuple` of :class:`int`
    :type positions:    :class:`ost.geom.Vec3List`

    :raises:            :exc:`ost.Error` if *range* does not
                        specify a valid range
 
  .. method:: GetNumElements()

    :returns:           The number of stored positions
    :rypte:             :class:`int`

