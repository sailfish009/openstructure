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
