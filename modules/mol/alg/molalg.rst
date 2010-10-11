:mod:`mol.alg <ost.mol.alg>` -- Algorithms for Structures
================================================================================

.. module:: ost.mol.alg
   :synopsis: Algorithms operating on molecular structures

.. function:: LocalDistTest(model, reference, tolerance, radius)
  
  This function calculates the agreement of local contacts between the model and 
  the reference structure. The overlap is a number between zero and one, where 
  one indicates complete agreement, zero indicates no agreement at all. This 
  score is similar to the GDT, but does not require any superposition of the 
  model and the reference.
  
  The distance of atom pairs in the reference structure that are closer than a 
  certain distance (radius) to each other is compared to the distance found in 
  the model. If the difference between these two distances is smaller than a 
  threshold value (tolerance), the model and the reference agree. Missing atoms 
  in the model are treated disagreement and thus lower the score.
  
  For residue with symmetric sidechains (GLU, ASP, ARG, VAL, PHE, TYR), the 
  naming of the atoms is ambigous. For these residues, the overlap of both 
  possible solutions to the fixed atoms, that is, everything that is not 
  ambigous is calculated. The solution that gives higher overlap is then used to 
  calculate the actual overlap score.