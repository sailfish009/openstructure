:mod:`~ost.bindings.tmtools` - Structural superposition
================================================================================

.. module:: ost.bindings.tmtools
  :synopsis: Sequence dependent and independent structure superposition

The :mod:`~ost.bindings.tmtools` module provides access to the structural 
superposition programs TMscore, Tmalign and MMalign developed by Y. Zhang 
and J. Skolnick. These programs superpose a model onto a reference structure, 
using the positions of the Calpha atoms only. While at their core, these 
programs essentially use the same algorithm, they differ on how the Calphas are 
paired. TMscore pairs the Calpha atom based on the residue number, TMalign 
calculates an optimal pairing of Calpha atom based on heuristics.

Citation:

  Yang Zhang and Jeffrey Skolnick, Proteins 2004 57: 702-710
  Y. Zhang and J. Skolnick, Nucl. Acids Res. 2005 33, 2302-9

Besides using the standalone TM-align program, ost also provides a wrapper 
around TM-align as published in:

  Sha Gong, Chengxin Zhang, Yang Zhang, Bioinformatics 2019 

The advantage is that no intermediate files must be generated, a wrapper on the
c++ layer is used instead. However, only the basic TM-align superposition
functionality is available.



Distance measures used by TMscore
--------------------------------------------------------------------------------

There are many different ways to describe the structural similarity of two 
protein structures at the Calpha level. TMscore calculate several of these 
measures. The most common is to describe the difference in terms of the root 
mean square deviation of the Calpha positions, the RMSD. Despite its common use, 
RMSD has several drawbacks when working with incomplete models. Since the RMSD 
highly depends on the set of included atoms, it is relatively easy to obtain a 
smaller RMSD by omitting flexible parts of a protein structure. This has lead to 
the introduction of the global distance test (GDT). A model is compared to a 
reference by calculating the fraction of Calpha atoms that can be superposed 
below a certain cutoff, e.g. 1Å. The fractions of several such cutoffs are 
combined into the GDT_TS (1, 2, 4 and 8Å) and GDT_HA (0.5, 1, 2, 4Å) and divided 
by four to obtain the final measure. In contrast to RSMD, GDT is an agreement 
measure. The higher the value, the more similar the two structures are. TM-score 
(not to be confused by TMscore, the program), additionally adds a size 
dependences to the GDT measure by taking the protein length into account. As 
with GDT, the bigger the value, the more similar the two structures are.

Common Usage
--------------------------------------------------------------------------------

The following example shows how to use TMscore to superpose two protein 
structures and print the RMSD as well as the GDT_TS and GDT_HA similarity measures.

.. code-block:: python

  from ost.bindings import tmtools
  
  pdb1=io.LoadPDB('1ake.pdb', restrict_chains='A')
  pdb2=io.LoadPDB('4ake.pdb', restrict_chains='A')
  result=tmtools.TMScore(pdb1, pdb2)
  print(result.rmsd_below_five) # 1.9
  print(result.gdt_ha) # 0.41
  print(result.gdt_ts) # 0.56

Usage of TMalign
--------------------------------------------------------------------------------

.. autofunction:: ost.bindings.tmtools.TMAlign


Usage of TMscore
--------------------------------------------------------------------------------

.. autofunction:: ost.bindings.tmtools.TMScore

.. autoclass:: ost.bindings.tmtools.TMScoreResult


TMalign C++ wrapper
--------------------------------------------------------------------------------

.. currentmodule:: ost.bindings

Instead of calling the TMalign executable, ost also provides a wrapper around
its C++ implementation. The advantage is that no intermediate files need to be 
generated in order to call the executable.

.. code-block:: python

  from ost import bindings
  
  pdb1=io.LoadPDB('1ake.pdb').Select("peptide=true")
  pdb2=io.LoadPDB('4ake.pdb').Select("peptide=true")
  result = bindings.WrappedTMAlign(pdb1.chains[0], pdb2.chains[0], 
                                   fast=True)
  print(result.tm_score)
  print(result.alignment.ToString(80))


.. class:: TMAlignResult(rmsd, tm_score, aligned_length, transform, alignment)

  All parameters of the constructor are available as attributes of the class

  :param rmsd:          RMSD of the superposed residues
  :param tm_score:      TMScore of the superposed residues
  :param aligned_length: Number of superposed residues
  :param transform:     Transformation matrix to superpose first chain onto 
                        reference
  :param alignment:     The sequence alignment given the structural superposition
  :type rmsd:           :class:`float`
  :type tm_score:       :class:`float`
  :type aligned_length: :class:`int`
  :type transform:      :class:`geom.Mat4`
  :type alignment:      :class:`ost.seq.AlignmentHandle`

.. method:: WrappedTMAlign(chain1, chain2, [fast=False])

  Takes two chain views and runs TMalign with *chain2* as reference.
  The positions and sequences are directly extracted from the chain
  residues for every residue that fulfills:
  
    * peptide linking
    * valid one letter code(no '?')
    * valid CA atom

  :param chain1:        Chain from which position and sequence are extracted
                        to run TMalign.
  :param chain2:        Chain from which position and sequence are extracted
                        to run TMalign, this is the reference.
  :param fast:          Whether to apply the *fast* flag to TMAlign
  :type chain1:         :class:`ost.mol.ChainView`
  :type chain2:         :class:`ost.mol.ChainView`
  :type fast:           :class:`bool`
  :rtype:               :class:`ost.bindings.TMAlignResult`


.. method:: WrappedTMAlign(pos1, pos2, seq1, seq2 [fast=False])

  Similar as described above, but directly feeding in raw data.

  :param pos1:          CA positions of the first chain
  :param pos2:          CA positions of the second chain, this is the reference.
  :param seq1:          Sequence of first chain
  :param seq2:          Sequence of second chain
  :param fast:          Whether to apply the *fast* flag to TMAlign
  :type pos1:           :class:`ost.geom.Vec3List`
  :type pos2:           :class:`ost.geom.Vec3List`
  :type seq1:           :class:`ost.seq.SequenceHandle`
  :type seq2:           :class:`ost.seq.SequenceHandle`
  :type fast:           :class:`bool`
  :rtype:               :class:`ost.bindings.TMAlignResult`
  :raises:              :class:`ost.Error` if pos1 and seq1, pos2 and seq2 
                        respectively are not consistent in size.

