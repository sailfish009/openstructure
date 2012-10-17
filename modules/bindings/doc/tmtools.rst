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
  print result.rmsd_below_five # 1.9
  print result.gdt_ha # 0.41
  print result.gdt_ts # 0.56

Usage of TMalign
--------------------------------------------------------------------------------

.. autofunction:: ost.bindings.tmtools.TMAlign

.. autoclass:: ost.bindings.tmtools.TMAlignResult

Usage of TMscore
--------------------------------------------------------------------------------

.. autofunction:: ost.bindings.tmtools.TMScore

.. autoclass:: ost.bindings.tmtools.TMScoreResult
