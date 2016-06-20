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

   - If the reference sequence has an offset, the first sequence of each pairwise alignment 
     must have the same offset. This offset is inherited by the first sequence of the final
     output alignment.
   
   - The resulting multiple sequence alignment is by no means optimal. For 
     better results, consider using a multiple-sequence alignment program such 
     as MUSCLE or ClustalW.
   
   - Residues in columns where the reference sequence has gaps should not be 
     considered as aligned. There is no information in the pairwise alignment to 
     guide the merging, the result is undefined.

.. autofunction:: ValidateSEQRESAlignment

.. autofunction:: AlignToSEQRES

.. autofunction:: AlignmentFromChainView

.. function:: Conservation(aln, assign=true, prop_name="cons", ignore_gap=false)

  Calculates conservation scores for each column in the alignment, according to
  the ConSurf method (Armon et al., J. Mol. Biol. (2001) 307, 447-463).
  
  The conservation score is a value between 0 and 1. The bigger the number 
  the more conserved the aligned residues are. 
  
  :param aln: An alignment handle
  :type aln: :class:`~ost.seq.AlignmentHandle`
  :param assign: If true, the conservation scores are assigned to attached 
      residues. The name of the property can be changed with the *prop_name* 
      parameter. Useful when coloring entities based on sequence conservation.
  :param prop_name: The property name for assigning the conservation to 
      attached residues. Defaults to 'cons'.
  :param ignore_gap: If true, the dissimilarity between two gaps is increased to
      6.0 instead of 0.5 as defined in the original version. Without this, a
      stretch where in the alignment there is only one sequence which is
      aligned to only gaps, is considered highly conserved (depending on the
      number of gap sequences).

.. function:: LocalAlign(seq1, seq2, subst_weight, gap_open=-5, gap_ext=-2)

  Performs a Smith/Waterman local alignment of *seq1* and *seq2* and returns
  the best-scoring alignments as a list of pairwise alignments.
  
  **Example:**
  
  .. code-block:: python
  
    seq_a = seq.CreateSequence('A', 'acdefghiklmn')
    seq_b = seq.CreateSequence('B', 'acdhiklmn')
    alns = seq.alg.LocalAlign(seq_a, seq_b, seq.alg.BLOSUM62)
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
  :returns: A list of best-scoring, non-overlapping alignments of *seq1* and 
     *seq2*. Since alignments always start with a replacement, the start is
     stored in the sequence offset of the two sequences.


.. function:: GlobalAlign(seq1, seq2, subst_weight, gap_open=-5, gap_ext=-2)

  Performs a Needleman/Wunsch global alignment of *seq1* and *seq2* and returns
  the best-scoring alignment.
  
  **Example:**
  
  .. code-block:: python
  
    seq_a = seq.CreateSequence('A', 'acdefghiklmn')
    seq_b = seq.CreateSequence('B', 'acdhiklmn')
    alns = seq.alg.GlobalAlign(seq_a, seq_b, seq.alg.BLOSUM62)
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
  :returns: Best-scoring alignment of *seq1* and *seq2*.

.. function:: ShannonEntropy(aln, ignore_gaps=True)

  Returns the per-column Shannon entropies of the alignment. The entropy
  describes how conserved a certain column in the alignment is. The higher
  the entropy is, the less conserved the column. For a column with no amino 
  aids, the entropy value is set to NAN.

  :param aln: Multiple sequence alignment
  :type aln: :class:`~ost.seq.AlignmentHandle`
  :param ignore_gaps: Whether to ignore gaps in the column.
  :type ignore_gaps: bool

  :returns: List of column entropies

.. function:: SemiGlobalAlign(seq1, seq2, subst_weight, gap_open=-5, gap_ext=-2)

  Performs a semi-global alignment of *seq1* and *seq2* and returns the best-
  scoring alignment. The algorithm is Needleman/Wunsch same as GlobalAlign, but
  without any gap penalty for starting or ending gaps. This is prefereble 
  whenever one of the sequences is significantly shorted than the other.
  This make it also suitable for fragment assembly.
  
  **Example:**
  
  .. code-block:: python
  
    seq_a=seq.CreateSequence('A', 'abcdefghijklmnok')
    seq_b=seq.CreateSequence('B', 'cdehijk')
    alns=seq.alg.GlobalAlign(seq_a, seq_b, seq.alg.BLOSUM62)
    print alns[0].ToString(80)
    # >>> A abcdefghijklmnok
    # >>> B --cde--hijk-----

  :param seq1: A valid sequence
  :type seq1: :class:`~ost.seq.ConstSequenceHandle`
  :param seq2: A valid sequence  
  :type seq2: :class:`~ost.seq.ConstSequenceHandle`
  
  :param subst_weigth: The substitution weights matrix
  :type subst_weight: :class:`SubstWeightMatrix`
  :param gap_open: The gap opening penalty. Must be a negative number
  :param gap_ext: The gap extension penalty. Must be a negative number
  :returns: best-scoring alignment of *seq1* and *seq2*.


.. _contact-prediction:

Contact Prediction
--------------------------------------------------------------------------------

This is a set of functions for predicting pairwise contacts from a multiple
sequence alignment (MSA). The core method here is mutual information which uses 
coevolution to predict contacts. Mutual information is complemented by two other 
methods which score pairs of columns of a MSA from the likelyhood of certain
amino acid pairs to form contacts (statistical potential) and the likelyhood
of finding certain substitutions of aminio-acid pairs in columns of the MSA
corresponding to interacting residues.

.. class:: ContactPredictionScoreResult
  
  Object containing the results form a contact prediction. 

  .. attribute:: matrix

    An *NxN* :class:`~ost.FloatMatrix` where *N* is the length of the alignment.
    The element *i,j* corresponds to the score of the corresponding
    columns of the MSA. High scores correspond to high likelyhood of
    a contact.

  .. attribute:: sorted_indices

    List of all indices pairs *i,j*, containing (N*N-1)/2 elements,
    as the **matrix** is symmetrical and elements in the diagonal
    are ignored. The indices are sorted from the pair most likely to form
    a contact to the least likely one.

  .. method:: GetScore(i,j)

    returns **matrix(i,j)**

    :param i: First index
    :param j: Second index
    :type i:  :class:`int`
    :type j:  :class:`int`

  .. method:: SetScore(i,j,score)

    Sets **matrix(i,j)** to **score**

    :param i: First index
    :param j: Second index
    :param score: The score
    :type i:  :class:`int`
    :type j:  :class:`int`
    :type score:  :class:`float`

.. autofunction:: PredictContacts

.. function:: CalculateMutualInformation(aln,weights=LoadConstantContactWeightMatrix(),
            apc_correction=true,zpx_transformation=true,small_number_correction=0.05)

    Calculates the mutual information (MI) from a multiple sequence alignemnt. Contributions of each pair of amino-acids are weighted using the matrix **weights** (weighted mutual information). The average product correction (**apc_correction**) correction and transformation into Z-scores (**zpx_transofrmation**) increase prediciton accuracy by reducing the effect of phylogeny and other noise sources. The small number correction reduces noise for alignments with small number of sequences of low diversity.

    :param aln: The multiple sequences alignment
    :type aln:  :class:`~ost.seq.AlignmentHandle`
    :param weights: The weight matrix
    :type weights:  :class`ContactWeightMatrix`
    :param apc_correction: Whether to use the APC correction
    :type apc_correction:  :class:`bool`
    :param zpx_transformation:  Whether to transform the scores into Z-scores
    :type zpx_transformation: :class:`bool`
    :param small_number_correction: initial values for the probabilities of having a given pair of amino acids *p(a,b)*.
    :type small_number_correction: :class:`float`

.. function:: CalculateContactScore(aln,weights=LoadDefaultContactWeightMatrix())
  
  Calculates the Contact Score (*CoSc*) from a multiple sequence alignment. For each pair of residues *(i,j)* (pair of columns in the MSA), *CoSc(i,j)* is the average over the values of the **weights** corresponding to the amino acid pairs in the columns.

  :param aln: The multiple sequences alignment
  :type aln:  :class:`~ost.seq.AlignmentHandle`
  :param weights: The contact weight matrix
  :type weights:  :class`ContactWeightMatrix`

.. function:: CalculateContactSubstitutionScore(aln,ref_seq_index=0,
                            weights=LoadDefaultPairSubstWeightMatrix())

  Calculates the Contact Substitution Score (*CoEvoSc*) from a multiple sequence alignment. For each pair of residues *(i,j)* (pair of columns in the MSA), *CoEvoSc(i,j)* is the average over the values of the **weights** corresponding to substituting the amino acid pair in the reference sequence (given by **ref_seq_index**) with all other pairs in columns *(i,j)* of the **aln**.

  :param aln: The multiple sequences alignment
  :type aln:  :class:`~ost.seq.AlignmentHandle`
  :param weights: The pair substitution weight matrix
  :type weights:  :class`ContactWeightMatrix`

.. function:: LoadDefaultContactWeightMatrix()
  
  :returns: *CPE*, a :class:`ContactWeightMatrix` that was calculated from a large (>15000) set of
    high quality crystal structures as *CPE=log(CF(a,b)/NCF(a,b))* and then normalised so that all its elements are comprised between 0 and 1. *CF(a,b)* is the frequency of amino acids *a* and *b* for pairs of contacting residues and *NCF(a,b)* is the frequency of amino acids *a* and *b* for pairs of non-contacting residues. Apart from weights for the standard amino acids, this matrix gives a weight of 0 to all pairs for which at least one amino-acid is a gap.

.. function:: LoadConstantContactWeightMatrix()
  
  :returns: A :class:`ContactWeightMatrix`. This matrix gives a weight of one to all pairs of
   standard amino-acids and a weight of 0 to pairs for which at least one amino-acid is a gap.

.. function:: LoadDefaultPairSubstWeightMatrix()
  
  :returns: *CRPE*, a :class:`PairSubstWeightMatrix` that was calculated from a large (>15000) set of
    high quality crystal structures as *CRPE=log(CRF(ab->cd)/NCRF(ab->cd))* and then normalised so that all its elements are comprised between 0 and 1. *CRF(ab->cd)* is the frequency of replacement of a pair of amino acids  *a* and *b* by a pair *c* and *d* in columns of the MSA corresponding to contacting residues and *NCRF(ab->cd)* is the frequency of replacement of a pair of amino acids  *a* and *b* by a pair *c* and *d* in columns of the MSA corresponding to non-contacting residues. Apart from weights for the standard amino acids, this matrix gives a weight of 0 to all pair substitutions for which at least one amino-acid is a gap.


.. class:: PairSubstWeightMatrix(weights, aa_list)

  This class is used to associate a weight to any substitution from one amino-acid pair *(a,b)* to any other pair *(c,d)*.

  .. attribute:: weights

    A :class:`~ost.FloatMatrix4` of size *NxNxNxN*, where *N=len(aa_list)*

  .. attribute:: aa_list

    A :class:`CharList` of one letter codes of the amino acids for which weights are found in the **weights** matrix.

.. class:: ContactWeightMatrix(weights, aa_list)

  This class is used to associate a weight to any pair of amino-acids.

  .. attribute:: weights

    A :class:`~ost.FloatMatrix` of size *NxN*, where *N=len(aa_list)*

  .. attribute:: aa_list

    A :class:`CharList` of one letter codes of the amino acids for which weights are found in the **weights** matrix.

