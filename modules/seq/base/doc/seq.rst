:mod:`~ost.seq` -- Sequences and Alignments
================================================================================

.. module:: ost.seq
   :synopsis: Contains classes and functions to deal with sequences and 
              alignments

The :mod:`seq` module helps you working with sequence data of various kinds. It 
has classes for :class:`single sequences <SequenceHandle>`, :class:`lists of 
sequences <SequenceList>` and :class:`alignments <AlignmentHandle>` of two or
more sequences. 


.. _attaching-views:

Attaching Structures to Sequences
--------------------------------------------------------------------------------


As OpenStructure is a computational structural biology framework, it is not
surprising that the sequence classes have been designed to work together with
structural data. Each sequence can have an attached :class:`~mol.EntityView`
allowing for fast mapping between residues in the entity view and position in
the sequence.

.. _sequence-offset:

Sequence Offset
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When using sequences and structures together, often the start of the structure 
and the beginning of the sequence do not fall together. In the following case, 
the alignment of sequences B and C only covers a subsequence of structure A::

  A acefghiklmnpqrstuvwy
  B     ghiklm
  C     123-45
  
We would now like to know which residue in protein A is aligned to which residue 
in sequence C. This is achieved by setting the sequence offset of sequence C to 
4. In essence, the sequence offset influences all the mapping operations from 
position in the sequence to residue index and vice versa. By default, the 
sequence offset is 0.

Loading and Saving Sequences and Alignments
--------------------------------------------------------------------------------

The :mod:`io` module supports input and output of common sequence formats. 
Single  sequences can be loaded from disk with :func:`io.LoadSequence`,
alignments are loaded with :func:`io.LoadAlignment` and lists of sequences are loaded with :func:`io.LoadSequenceList`. In addition to the file based input 
methods, sequences can also be loaded from a string:

.. code-block:: python

  seq_string='''>sequence
  abcdefghiklmnop'''
  s=io.SequenceFromString(seq_string, 'fasta')
  print s.name, s # will print "sequence abcdefghiklmnop"
  
Note that, in that case specifying the format is mandatory.

The SequenceHandle
--------------------------------------------------------------------------------

.. function:: CreateSequence(name, sequence)

  Create a new :class:`SequenceHandle` with the given name and sequence. 

  :param name: name of the sequence
  :type  name: str
  :param sequence: String of characters representing the sequence. Only   
       'word' characters (no digits), '?', '-' and '.' are allowed. In an
       upcoming release, '?' and '.' will also be forbidden so its best to
       translate those to 'X' or '-'.
  :type sequence: str
  :raises InvalidSequence: When the sequence string contains forbidden
       characters. In the future, '?' and '.' will also raise this exception.

.. class:: SequenceHandle

  Represents a sequence. New instances are created with :func:`CreateSequence`.
  
  .. method:: GetPos(residue_index)
  
    Get position of residue with index in sequence. This is best illustrated in 
    the following example:
    
    .. code-block:: python
      
      s=seq.CreateSequence("A", "abc---def")
      print s.GetPos(1) # prints 1
      print s.GetPos(3) # prints 6
    
    The reverse mapping, that is from position in the sequence to residue index 
    can be achieved with :meth:`GetResidueIndex`.
  
  .. method:: GetResidueIndex(pos)
     
    Get residue index of character at given position. This method is the
    inverse of :meth:`GetPos`. If the sequence contains a gap at that position,
    an :exc:`Error` is raised. Admires the
    :ref:`sequence offset <sequence-offset>`.
    
    .. code-block:: python
      
      s=seq.CreateSequence("A", "abc--def")
      print s.GetResidueIndex(1) # prints 1
      print s.GetResidueIndex(6) # prints 4
      # the following line raises an exception of type
      # Error with the message "requested position contains 
      # a gap"
      print s.GetResidueIndex(3)

  .. method:: GetResidue(pos)
     
    As, :meth:`GetResidueIndex`, but directly returns the residue view. If no
    view is attached, or if the position is a gap, an invalid residue view
    is returned.
    
    :rtype: :class:`~ost.mol.ResidueView`
    
  .. method:: GetLastNonGap()
     
    Get position of last non-gap character in sequence. In case of an empty
    sequence, or, a sequence only consisting of hyphens, -1 is returned
     
  .. method:: GetFirstNonGap()
  
    Get position of first non-gap character in sequence. In case of an empty
    sequence, or, a sequence only consisting of hyphens, -1 is returned.

  .. method:: AttachView(view)
              AttachView(view, [chain_name])
    
    Attach an :class:`~mol.EntityView` to sequence. The first signature requires
    that the view contains one chain. If not, an :exc:`IntegrityError` is
    raised. The second signature will select the chain with the given name. If 
    no such chain exists, an :exc:`IntegrityError` is raised.
    
  .. method:: HasAttachedView()
  
    Returns True when the sequence has a view attached, False if not.
    
  .. method:: GetAttachedView()
  
    Returns the attached :class:`~mol.EntityView`, or an invalid
    :class:`~mol.EntityView` if no view has been attached. Also available as 
    the property :attr:`attached_view`.
    
  .. method:: GetName()
  
    Returns the name of the sequence. Also available as the property
    :attr:`name`
  
  .. method:: SetOffset()
  
    Set the :ref:`sequence offset <sequence-offset>`. By default, the offset is
    0. Also available as the property :attr:`offset`.
    
  .. method:: GetOffset()
    
    Returns the :ref:`sequence offset <sequence-offset>`. Also available as
    :attr:`offset`.
    
    
  .. method:: GetGaplessString()
     
    Returns a string version of this sequence with all hyphens removed. Also
    available as the property :attr:`gapless_string`.
     
  .. method:: Normalise()
     
    Remove '-' and '.' as gaps from the sequence and make it all upper case.
    Works in place.
   
  .. method:: SetName()
  
    Set name of the sequence. Also available as the property :attr:`name`.
  
  .. attribute:: gapless_string
     
    Shorthand for :meth:`GetGaplessString()`
     
  .. attribute:: name
  
    Shorthand for :meth:`GetName`/:meth:`SetName`
  
  .. attribute:: attached_view
  
    Shorthand for :meth:`GetAttachedView`.

  .. attribute:: offset
  
    Shorthand for :meth:`GetOffset`/:meth:`SetOffset`

  .. method:: __len__()
    
    Returns the length of the sequence (including insertions and deletions)
    
  .. method:: __str__()

    Returns the sequence as a string.

.. function:: Match(s1, s2)

  :param s1: The first sequence
  :param s2: The second sequence
  :type s1: :class:`SequenceHandle`, or :class:`str`
  :type s2: :class:`SequenceHandle`, or :class:`str`

  Check whether the two sequences s1 and s2 match. This function performs are
  case-insensitive comparison of the two sequences. The character  'X' is
  interpreted as a wild card character that always matches the other sequence.

The SequenceList    
--------------------------------------------------------------------------------

.. class:: SequenceList

  Represents a list of sequences. The class provides a row-based interface. New
  instances are created with :func:`CreateSequenceList`.


The AlignmentHandle   
--------------------------------------------------------------------------------

The :class:`AlignmentHandle` represents a list of aligned sequences. In
contrast to :class:`SequenceList`, an alignment requires all sequences to be of 
the same length. New instances of alignments are created with 
:func:`CreateAlignment` and :func:`AlignmentFromSequenceList`.

Typically sequence alignments are used column-based, i.e by looking at an  
aligned columns in the sequence alignment. To get a row-based (sequence) view
on the sequence list, use :meth:`GetSequenceList()`. 

All functions that operate on an alignment will again produce a valid alignment. 
This mean that it is not possible to change the length of one sequence, without  
adjusting the other sequences, too.

The following example shows how to iterate over the columns and sequences of
an alignment:

.. code-block:: python

  aln=io.LoadAlignment('aln.fasta')
  # iterate over the columns
  for col in aln:
    print col

  # iterate over the sequences
  for s in aln.sequences:
    print s

.. function:: CreateAlignment()

  Creates and returns a new :class:`AlignmentHandle` with no sequences.
  
.. function:: AlignmentFromSequenceList(sequences)
  
  Create a new alignment from the given list of sequences
  
  :param sequences: the list of sequences
  :type sequences: :class:`ConstSequenceList`
  
  :raises: :exc:`InvalidAlignment` if the sequences do not have the same length.

.. class:: AlignmentHandle
  
  .. note:: 
  
    Several of these methods just forward calls to the sequence. For more 
    detailed information, have a look at the :class:`SequenceHandle`
    documentation.
  
  .. method:: GetSequence(index)
  
    Returns the sequence at the given index, raising an IndexError when trying
    to access an inexistent sequence.
    
  .. method:: GetSequenceList()
  
    Returns a list of all sequence of the alignment.
    
  .. method:: GetLength()
  
    Returns the length of the alignment.
    
  .. method:: GetCount()
  
    Returns the number of sequences in the alignment.
  
  
  .. method:: ToString(width=80)
  
    Returns a formatted string version of the alignment. The sequences are 
    split into smaller parts to fit into the number columns specified. 
    
    .. code-block:: python
    
      aln=seq.CreateAlignment()
      aln.AddSequence(seq.CreateSequence("A", "abcdefghik"))
      aln.AddSequence(seq.CreateSequence("B", "1234567890"))
      # The following command will print the output given below
      print aln.ToString(7)
      # A abcde
      # B 12345
      #
      # A fghik
      # B 67890

  .. method:: FindSequence(name)
  
    Find sequence with given name. If the alignment contains several sequences
    with the same name, the first sequence is returned.
    
  .. method:: SetSequenceName(seq_index, name)
  
    Set the name of the sequence at index `seq_index` to name
    
  .. method:: SetSequenceOffset(seq_index, offset)
  
    Set the sequence offset of sequence at index `seq_index`
    
  .. method:: Copy()
    
    Create a deep copy of the alignment

  .. method:: GetPos(seq_index, res_index)
    
    Get position of residue with index equal to `res_index` in sequence at index
    `seq_index`.
    
  .. method:: GetResidueIndex(seq_index, pos)
  
    Get residue index of residue at position `pos` in sequence at index
    `seq_index`.
  
  .. method:: AttachView(seq_index, view)
              AttachView(seq_index, view, chain_name)
    
    Attach the given view to the sequence at index `seq_index`.
    
  .. method:: Cut(start, end)
  
    Removes the columns in the half-closed interval `start`, `end` from the
    alignment.
    
    .. code-block:: python
    
      aln=seq.CreateAlignment()
      aln.AddSequence(seq.CreateSequence("A", "abcd---hik"))
      aln.AddSequence(seq.CreateSequence("B", "1234567890"))
      aln.Cut(4, 7)
      
      print aln.ToString(80)
      # will print
      # A abcdhik
      # B 1234890
   
  .. method:: Replace(new_region, start, end)
  
    Replace the columns in the half-closed interval `start`, `end` with the
    columns in `new_region`.
    
    :param new_region: The region to be inserted
    :type new_region: :class:`AlignedRegion` or :class:`AlignmentHandle`
  
  
  .. method:: ShiftRegion(start, end, amount, master=-1)
  
    Shift columns in the half-closed interval `start`, `end`. If amount is a
    positive number, the columns are shifted to the right, if negative, the 
    columns are shifted to the left.
    
    If master is set to -1, all sequences in the region are affected, otherwise 
    only the sequence at index equal to master is shifted.
  
  .. method:: GetMatchingBackboneViews(index1=0, index2=1)
  
    Returns a tuple of entity views containing matching backbone atoms for the 
    two sequences at index1 and index2, respectively. For each aligned column in
    the alignment, backbone atoms are added to the view if both aligned residues 
    have them. It is guaranteed that the two views contain the same number of 
    atoms and that the order of the atoms in the two views is the same.
    
    The output of this function can be used to superpose two structures with
    :func:`~ost.mol.alg.SuperposeSVD`.
    
    
    :param index1: The index of the first sequence
    
    :param index2: The index of the second sequence.
    
    :raises: In case one of the two sequences doesn't have an attached view, a 
       :exc:`RuntimeError` is raised.

  .. method:: AddSequence(sequence)

    Append a sequence to the alignment. The sequence must have the same length as
    sequences already present in the alignment.

    :raises: :exc:`RuntimeError` if the sequence length does not match
    :param sequence: Sequence to be added
    :type sequence: :class:`ConstSequenceHandle`

  .. method:: GetSequenceOffset(index)
              SetSequenceOffset(index, offset)

    Get/set the offset for sequence at *index*.

    :param index: The index of the sequence
    :type index: :class:`int`
    :param offset: The new offset
    :type offset: :class:`int`
    :rtype: :class:`int`
  
  .. method:: GetSequenceRole(index)
              SetSequenceRole(index, role)

    Get/Set the sequence role for sequence at *index*.

    :param index: The index of the sequence
    :type index: :class:`int`
    :param role: The new role
    :type role: :class:`str`
    :rtype: :class:`str`

  .. method:: GetCoverage(index)

    Get coverage of sequence at *index* to the first sequence.

    :param index: The index of the sequence
    :type index: :class:`int`
    :returns: Coverage as a number between 0 and 1.
  
  .. method:: RemoveSequence(index)

    Remove sequence at *index* from the alignment.


Handling Hidden Markov Models
--------------------------------------------------------------------------------

The HMM provides a simple container for hidden markov models in form of
single columns containing amino acid frequencies and transition probabilities.

.. class:: HMMColumn

  .. method:: BLOSUMNullModel()

    Static method, that returns a new :class:`HMMColumn` with amino acid
    frequencies given from the BLOSUM62 substitution matrix.

  .. method:: GetFreq(aa)

    :param aa:  One letter code of standard amino acid
    :type aa:  :class:`str`

    :returns:  Frequency of aa

  .. method:: SetFreq(aa,freq)

    :param aa:  One letter code of standard amino acid
    :param freq:  The frequency of the given amino acid
    :type aa:  :class:`str`
    :type freq:  :class:`float`

  .. method:: GetTransitionFreq(from,to)

    :param from:  Current state of HMM (HMM_MATCH, HMM_INSERT or HMM_DELETE)
    :param to:  Next state

    :returns:  Frequency of given state transition

  .. method:: SetTransitionFreq(from,to,freq)

    :param from:  Current state of HMM (HMM_MATCH, HMM_INSERT or HMM_DELETE)
    :param to:  Next state
    :param freq:  Frequency of transition

  .. attribute:: one_letter_code

    One letter code, this column is associated to

  .. attribute:: entropy

    Shannon entropy based on the columns amino acid frequencies


.. class:: HMM

  .. method:: Load(filename)

    Static method to load an hmm in the hhm format as it is in use in the HHSuite.

    :param filename:  Name of file to load
    :type filename:  :class:`str`

  .. method:: AddColumn(col)

    Appends column in the internal column list.

    :param col:  Column to add
    :type col:  :class:`HMMColumn`

  .. attribute:: sequence

    Sequence of the columns

  .. attribute:: columns

    Iterable columns of the HMM

  .. attribute:: null_model

    Null model of the HMM

  .. attribute:: avg_entropy

    Average entropy of all the columns

.. class:: HMMDB

  A simple database to gather :class:`HMM` objects. It is possible
  to save them to disk in a compressed format with limited accuracy
  (4 digits for freq values).

  .. method:: Save(filename)

    :param filename:  Name of file that will be generated on disk.
    :type filename:  :class:`str`

  .. method:: Load(filename)

    Static loading method

    :param filename:  Name of file from which the database should be loaded.
    :type filename:  :class:`str`
    :returns:  The loaded database

  .. method:: AddHMM(name, hmm)

    :param name:  Name of HMM to be added
    :param hmm:  HMM to be added

    :type name:  :class:`str`
    :type hmm:  :class:`HMM`
    :raises:  :class:`Exception` when filename is longer than 255 characters.

  .. method:: GetHMM(name)

    :param name:  Name of HMM to be returned
    :type name:  :class:`str`
    :returns:  The requested :class:`HMM`
    :raises:  :class:`Exception` when no :class:`HMM` for **name** exists.

  .. method:: Size()

    :returns: Number of :class:`HMM` objects in the database

  .. method:: GetNames()

    :returns: A nonsorted list of the names of all :class:`HMM` objects in the database






