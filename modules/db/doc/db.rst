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


Data Extraction
--------------------------------------------------------------------------------

Openstructure provides data extraction functionality for the following scenario:
There is a SEQRES container and an ATOMSEQ container 
(both: :class:`LinearCharacterContainer`).
Positions are stored in :class:`LinearPositionContainer`. 
The containers contain entries from the protein structure database
and sequence/position data is relative to the SEQRES of those entries. 
This means, if the SEQRES has more characters as there are resolved residues
in the structure, the entry in the position container(s) still contains the exact
number of SEQRES characters but some positions remain invalid. Thats where the
ATOMSEQ container comes in. It only contains matching residues to the SEQRES but
marks non-resolved residues with '-'. 

.. method:: ExtractValidPositions(entry_name, chain_name, indexer, \
                                  atomseq_container, position_container, \
                                  seq, positions)

  Iterates over all data for a chain specified by *entry_name* and *chain_name*.
  For every data point marked as valid in the *atomseq_container* (character at
  that position is not '-'), the character and the corresponding position are 
  added to *seq* and *positions*

  :param entry_name:    Name of assembly you want the data from
  :param chain_name:    Name of chain you want the data from
  :param indexer:       Used to access *atomseq_container* and 
                        *position_container*
  :param atomseq_container: Container that marks locations with invalid position
                            data with '-'
  :param position_container: Container containing position data
  :param seq:           Sequence with extracted valid positions gets stored
                        in here.
  :param positions:     The extracted valid positions get stored in here

  :type entry_name:     :class:`str`
  :type chain_name:     :class:`str`
  :type indexer:        :class:`LinearIndexer`
  :type atomseq_container: :class:`LinearCharacterContainer`
  :type position_container: :class:`LinearPositionContainer`
  :type seq:            :class:`ost.seq.SequenceHandle`
  :type positions:      :class:`ost.geom.Vec3List`

  :raises:              :exc:`ost.Error` if requested data is not present         


.. class:: DisCoDataContainers(indexer_path, \
                               seqres_container_path, \
                               atomseq_container_path, \
                               position_container_path)

  Thin wrapper to organize the data containers required in 
  :meth:`ExtractTemplateDataDisCo`. All containers are loaded
  from disk and are expected to be consistent.

  :param indexer_path: Path to indexer file to access the data containers 
                       (:class:`LinearIndexer`)
  :param seqres_container_path: Path to container with SEQRES data
                                (:class:`LinearCharacterContainer`)
  :param atomseq_container_path: Path to container with ATOMSEQ data
                                 (:class:`LinearCharacterContainer`)
  :param position_container_path: Path to container with CA position data
                                  (:class:`LinearPositionContainer`)

  :type indexer_path: :class:`str`
  :type seqres_container_path: :class:`str`
  :type atomseq_container_path: :class:`str`
  :type position_container_path: :class:`str`

  .. attribute:: indexer

  .. attribute:: seqres_container

  .. attribute:: atomseq_container

  .. attribute:: position_container 

.. method:: ExtractTemplateDataDisCo(entry_name, chain_name, aln, \
                                     data_containers, residue_numbers, \
                                     positions)

  Data extraction helper for the specific use of extracting data required
  to generate DisCoContainers in the QMEAN scoring function.
  Let's say we have a target-template alignment in *aln* (first seq: target, 
  second seq: template). This function extracts all valid template positions 
  and their residue numbers given the entry specified by *entry_name* and 
  *chain_name*. The template sequence in *aln* must match the sequence in 
  *seqres_container*. Again, the *atomseq_container* is used to identify valid 
  positions.

  :param entry_name:    Name of assembly you want the data from
  :param chain_name:    Name of chain you want the data from
  :param aln:           Target-template sequence alignment
  :param data_containers: Contains all required data containers
  :param residue_numbers: Residue numbers of extracted positions will be stored 
                          in here
  :param positions:     Extracted positions will be stored in here
  :type entry_name:     :class:`str`
  :type chain_name:     :class:`str`
  :type aln:            :ost.seq.SequenceHandle:
  :type data_containers: :class:`DisCoDataContainers`
  :type residue_numbers: :class:`list` of :class:`int`
  :type positions:       :class:`geom::Vec3List`

  :raises:              :exc:`ost.Error` if requested data is not present in 
                        the containers or if the template sequence in *aln*
                        doesn't match with the sequence in *seqres_container*




.. class:: GMQEDataContainers(indexer_path, \
                               seqres_container_path, \
                               atomseq_container_path, \
                               dssp_container_path, \
                               n_position_container_path, \
                               ca_position_container_path, \
                               c_position_container_path, \
                               cb_position_container_path, \)

  Thin wrapper to organize the data containers required in 
  :meth:`ExtractTemplateDataGMQE`. All containers are loaded
  from disk and are expected to be consistent.

  :param indexer_path: Path to indexer file to access the data containers 
                       (:class:`LinearIndexer`)
  :param seqres_container_path: Path to container with SEQRES data
                                (:class:`LinearCharacterContainer`)
  :param atomseq_container_path: Path to container with ATOMSEQ data
                                 (:class:`LinearCharacterContainer`)
  :param dssp_container_path: Path to container containing dssp states
                              (:class:`LinearCharacterContainer`)
  :param n_position_container_path: Path to container with N position data
                                    (:class:`LinearPositionContainer`)
  :param ca_position_container_path: Path to container with CA position data
                                    (:class:`LinearPositionContainer`)
  :param c_position_container_path: Path to container with C position data
                                    (:class:`LinearPositionContainer`)
  :param cb_position_container_path: Path to container with CB position data
                                     (:class:`LinearPositionContainer`)

  :type indexer_path: :class:`str`
  :type seqres_container_path: :class:`str`
  :type atomseq_container_path: :class:`str`
  :type dssp_container_path: :class:`str`
  :type n_position_container_path: :class:`str`
  :type ca_position_container_path: :class:`str`
  :type c_position_container_path: :class:`str`
  :type cb_position_container_path: :class:`str`

  .. attribute:: indexer

  .. attribute:: seqres_container

  .. attribute:: atomseq_container

  .. attribute:: dssp_container

  .. attribute:: n_position_container 

  .. attribute:: ca_position_container 

  .. attribute:: c_position_container 

  .. attribute:: cb_position_container 


.. method:: ExtractTemplateDataGMQE(entry_name, chain_name, aln, \
                                    data_containers, residue_numbers, \
                                    dssp, n_positions, ca_positions, \
                                    c_positions, cb_positions)

  Data extraction helper for the specific use of extracting data required
  to estimate GMQE as available in the QMEAN software package.
  Let's say we have a target-template alignment in *aln* (first seq: target, 
  second seq: template). This function extracts all valid template positions,
  their dssp states and their residue numbers given the entry specified by 
  *entry_name* and *chain_name*. The template sequence in *aln* must match the 
  sequence in *seqres_container*. Again, the *atomseq_container* is used to 
  identify valid positions.

  :param entry_name:    Name of assembly you want the data from
  :param chain_name:    Name of chain you want the data from
  :param aln:           Target-template sequence alignment
  :param data_containers: Contains all required data containers
  :param residue_numbers: Residue numbers of extracted positions will be stored 
                          in here
  :param dssp:          Extracted DSSP states will be stored in here 
  :param n_positions:   Extracted n_positions will be stored in here
  :param ca_positions:  Extracted ca_positions will be stored in here
  :param c_positions:   Extracted c_positions will be stored in here
  :param cb_positions:  Extracted cb_positions will be stored in here

  :type entry_name:     :class:`str`
  :type chain_name:     :class:`str`
  :type aln:            :ost.seq.SequenceHandle:
  :type data_containers: :class:`GMQEDataContainers`
  :type residue_numbers: :class:`list` of :class:`int`
  :type dssp:            :class:`str`
  :type n_positions:     :class:`geom::Vec3List`
  :type ca_positions:    :class:`geom::Vec3List`
  :type c_positions:     :class:`geom::Vec3List`
  :type cb_positions:    :class:`geom::Vec3List`

  :raises:              :exc:`ost.Error` if requested data is not present in 
                        the containers or if the template sequence in *aln*
                        doesn't match with the sequence in *seqres_container*
