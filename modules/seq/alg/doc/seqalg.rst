:mod:`seq.alg <ost.seq.alg>` -- Algorithms for Sequences
================================================================================

.. module:: ost.seq.alg
  :synopsis: Algorithms for sequences


.. function:: MergePairwiseAlignments(pairwise_alns, ref_seq)

  Merge a list of pairwise alignments into a multiple sequence alignments. This
  function uses the reference sequence as the anchor and inserts gaps where 
  needed. This is also known as the *star method*.

  The resulting multiple sequence alignment provides a simple way to map between 
  residues of pairwise alignments, e.g. to compare distances in two structural 
  templates.
  
  There are a few things to keep in mind when using this function:
  
   - The reference sequence mustn't contain any gaps
   
   - The first sequence of each pairwise alignments corresponds to the reference 
     sequence. Apart from the presence of gaps, these two sequences must be 
     completely identical.
   
   - The resulting multiple sequence alignment is by no means optimal. For 
     better results, consider using a multiple-sequence alignment program such 
     as MUSCLE or ClustalW.
   
   - Residues in columns where the reference sequence has gaps should not be 
     considered as aligned. There is no information in the pairwise alignment to 
     guide the merging, the result is undefined.

.. autofunction:: AlignToSEQRES

.. autofunction:: AlignmentFromChainView



.. function:: LocalAlign(seq1, seq2, subst_weight, gap_open=-5, gap_ext=-2)

  Performs a Smith/Waterman local alignment of *seq1* and *seq2* and returns
  the best-scoring alignments as a list of pairwise alignments.
  
  **Example:**
  
  .. code-block:: python
  
    seq_a=seq.CreateSequence('A', 'acdefghiklmn')
    seq_b=seq.CreateSequence('B', 'acdhiklmn')
    alns=seq.alg.LocalAlign(seq_a, seq_b, seq.alg.BLOSUM62)
    print alns[0].ToString(80)
    # >>> A acdefghiklmn
    # >>> B acd---hiklmn

  :param seq1: A valid sequence
  :type seq1: :class:`~ost.seq.ConstSequenceHandle`
  :param seq2: A valid sequence  
  :type seq2: :class:`~ost.seq.ConstSequenceHandle`
  
  :param subst_weigth: The substitution weights matrix
  :type subst_weight: :class:`SubstWeightMatrix`
  :param gap_open: The gap opening penalty. Must be a negative number
  :param gap_ext: The gap extension penalty. Must be a negative number
  :returns: list of best-scoring, non-overlapping alignments of *seq1* and 
     *seq2*. Since alignments always start with a replacement, the start is
     stored in the sequence offset of the two sequences.


.. function:: GlobalAlign(seq1, seq2, subst_weight, gap_open=-5, gap_ext=-2)

  Performs a Needleman/Wunsch global alignment of *seq1* and *seq2* and returns
  the best-scoring alignment.
  
  **Example:**
  
  .. code-block:: python
  
    seq_a=seq.CreateSequence('A', 'acdefghiklmn')
    seq_b=seq.CreateSequence('B', 'acdhiklmn')
    alns=seq.alg.GlobalAlign(seq_a, seq_b, seq.alg.BLOSUM62)
    print alns[0].ToString(80)
    # >>> A acdefghiklmn
    # >>> B acd---hiklmn

  :param seq1: A valid sequence
  :type seq1: :class:`~ost.seq.ConstSequenceHandle`
  :param seq2: A valid sequence  
  :type seq2: :class:`~ost.seq.ConstSequenceHandle`
  
  :param subst_weigth: The substitution weights matrix
  :type subst_weight: :class:`SubstWeightMatrix`
  :param gap_open: The gap opening penalty. Must be a negative number
  :param gap_ext: The gap extension penalty. Must be a negative number
  :returns: best-scoring alignment of *seq1* and *seq2*.
