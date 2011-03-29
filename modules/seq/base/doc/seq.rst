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
the alignment of sequences B and C only covers a subpart of structure A::

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
  s=io.LoadSequenceFromString(seq_string, 'fasta')
  print s.name, s # will print "sequence abcdefghiklmnop"
  
Note that, in that case specifying the format is mandatory.

The SequenceHandle
--------------------------------------------------------------------------------

.. function:: CreateSequence(name, sequence)

  Create a new :class:`SequenceHandle` with the given name and sequence. 
  
  :param name: name of the sequence
  :type  name: str
  :param sequence: String of characters representing the sequence. Only   
       alphanumerical characters and '-' are allowed.
  :type sequence: str
  :raises InvalidSequence: When the sequence string contains forbidden
       characters, that is anything that is not alphanumeric or a hyphen.

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
    an :exc:`Error` is raised.
    
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
     
   
  .. method:: SetName()
  
    Set name of the sequence. Also available as the property :attr:`name`.
  
  .. attribute:: gapless_string
     
    Shorthand for :meth:`GetGaplessString()`
     
  .. attribute:: name
  
    Shorthand for :meth:`GetName`/:meth:`SetName`
  
  .. attribute:: attached_view
  
    Shorthand for :meth:`GetAttachedView`.

  .. attribute:: offset
  
    Shorthand for :meth:`GetSequenceOffset`/:meth:`SetSequenceOffset`

  .. method:: __len__()
    
    Returns the length of the sequence (including insertions and deletions)
    
  .. method:: __str__()

    Returns the sequence as a string.


The SequenceList    
--------------------------------------------------------------------------------

.. class:: SequenceList

  Represents a list of sequences. The class provides a row-based interface. New
  instances are created with :func:`CreateSequenceList`.


The AlignmentHandle   
--------------------------------------------------------------------------------

The :class:`AlignmentHandle` represents a list of aligned sequences. In
constrast to :class:`SequenceList`, an alignment requires all sequences to be of 
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
