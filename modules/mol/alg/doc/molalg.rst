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
  

.. function:: SuperposeFrames(frames, sel, from=0, to=-1, ref=-1)

  This function superposes the frames of the given coord group and returns them
  as a new coord group.
  
  :param frames: The source coord group.
  :param from: index of the first frame
  :param to: index of the last frame plus one. If set to -1, the value is set to 
     the number of frames in the coord group
  :param sel: An entity view containing the selection of atoms to be used for     
    superposition. If set to an invalid view, all atoms in the coord group are 
    used.
  :param ref: The index of the reference frame to use for superposition. If set 
     to -1, the each frame is superposed to the previous frame.
     
  :returns: A newly created coord group containing the superposed frames.
  
  
Steric Clashes
--------------------------------------------------------------------------------

The following function detects steric clashes in atomic structures. Two atoms are clashing if their euclidian distance is smaller than a threshold value. The threshold values are calculated from high-resolution X-ray structures for each possible element pair. The value is chosen such that 99.99% of observed distances between 0 and 5 Angstroem are above the threshold.


.. function:: FilterClashes(ent, tolerance=0.0)

  This function filters out residues with clashing atoms. If the clashing atom 
  is a backbone atom, the complete residue is removed, if the atom is part of 
  the sidechain, only the sidechain atoms are removed.
  
  Hydrogen and deuterium atoms are ignored.
  
  :param ent: The input entity
  :type ent: :class:`~ost.mol.EntityView` or :class:`~ost.mol.EntityHandle`
  :param tolerance: The tolerance in (Angstroem) is substracted from the 
     thresholds calculated from high resolution X-ray structures to make the 
     function less pedantic. Negative values are also allowed and make the 
     function more stringent.

  :returns: The filtered :class:`~ost.mol.EntityView`


