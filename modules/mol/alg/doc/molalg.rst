:mod:`mol.alg <ost.mol.alg>` -- Algorithms for Structures
================================================================================

.. module:: ost.mol.alg
   :synopsis: Algorithms operating on molecular structures

.. function:: LocalDistTest(model, reference, tolerance, radius, local_ldt_property_string="")
  
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

  If a string is passed as last parameter, the function computes the overlap score for
  each residue and saves it as a float property in the ResidueHandle, with the passed string
  as property name
  
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

.. autofunction:: ParseAtomNames

.. autofunction:: MatchResidueByNum

.. autofunction:: MatchResidueByIdx

.. autofunction:: MatchResidueByLocalAln

.. autofunction:: MatchResidueByGlobalAln

.. autofunction:: Superpose


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


Trajectory Analyses
--------------------------------------------------------------------------------

This is a set of funcitons used for basic trajectory analysis such as extracting positions, 
distances, angles and RMSDs. The organization is such that most functions have their counterpart 
at the individual frame level (CoordFrame) so that they can alsobe called on one frame instead 
of the whole trajectory.

All these functions have a "stride" argument that defaults to stride=1, which is used to skip frames in the anlysis.


.. function:: AnalyzeAtomPos(traj, atom1, stride=1)

  This function extracts the position of an atom from a trajectory. It returns 
  a vector containing the position of the atom for each analyzed frame.
  
  :param traj: The trajectory to be analyzed.
  :type traj: :class:`~ost.mol.CoordGroupHandle`
  :param atom1: The :class:`~ost.mol.AtomHandle`.
  :param stride: Size of the increment of the frame's index between two consecutive frames analyzed.
  
  
.. function:: AnalyzeCenterOfMassPos(traj, sele, stride=1)

  This function extracts the position of the center-of-mass of a selection 
  (:class:`~ost.mol.EntityView`) from a trajectory and returns it as a vector.
  
  :param traj: The trajectory to be analyzed.
  :type traj: :class:`~ost.mol.CoordGroupHandle`
  :param sele: The selection from which the center of mass is computed
  :type sele: :class:`~ost.mol.EntityView`.
  :param stride: Size of the increment of the frame's index between two consecutive frames analyzed.
  

.. function:: AnalyzeDistanceBetwAtoms(traj, atom1, atom2, stride=1)

  This function extracts the distance between two atoms from a trajectory 
  and returns it as a vector.
  
  :param traj: The trajectory to be analyzed.
  :type traj: :class:`~ost.mol.CoordGroupHandle`
  :param atom1: The first :class:`~ost.mol.AtomHandle`.
  :param atom2: The second :class:`~ost.mol.AtomHandle`.
  :param stride: Size of the increment of the frame's index between two consecutive frames analyzed.  


.. function:: AnalyzeAngle(traj, atom1, atom2, atom3, stride=1)

  This function extracts the angle between three atoms from a trajectory 
  and returns it as a vector. The second atom is taken as being the central
  atom, so that the angle is between the vectors (atom1.pos-atom2.pos) and
  (atom3.pos-atom2.pos).
  
  :param traj: The trajectory to be analyzed.
  :type traj: :class:`~ost.mol.CoordGroupHandle`
  :param atom1: The first :class:`~ost.mol.AtomHandle`.
  :param atom2: The second :class:`~ost.mol.AtomHandle`.
  :param atom3: The third :class:`~ost.mol.AtomHandle`.
  :param stride: Size of the increment of the frame's index between two consecutive frames analyzed.


.. function:: AnalyzeDihedralAngle(traj, atom1, atom2, atom3, atom4, stride=1)

  This function extracts the dihedral angle between four atoms from a trajectory 
  and returns it as a vector. The angle is between the planes containing the first 
  three and the last three atoms.
  
  :param traj: The trajectory to be analyzed.
  :type traj: :class:`~ost.mol.CoordGroupHandle`
  :param atom1: The first :class:`~ost.mol.AtomHandle`.
  :param atom2: The second :class:`~ost.mol.AtomHandle`.
  :param atom3: The third :class:`~ost.mol.AtomHandle`.
  :param atom4: The fourth :class:`~ost.mol.AtomHandle`.
  :param stride: Size of the increment of the frame's index between two consecutive frames analyzed.

.. function:: AnalyzeDistanceBetwCenterOfMass(traj, sele1, sele2, stride=1)

  This function extracts the distance between the center-of-mass of two selections 
  (:class:`~ost.mol.EntityView`) from a trajectory and returns it as a vector.

  :param traj: The trajectory to be analyzed.
  :type traj: :class:`~ost.mol.CoordGroupHandle`
  :param sele1: The selection from which the first center of mass is computed
  :type sele1: :class:`~ost.mol.EntityView`.
  :param sele2: The selection from which the second center of mass is computed
  :type sele2: :class:`~ost.mol.EntityView`.
  :param stride: Size of the increment of the frame's index between two consecutive frames analyzed.


.. function:: AnalyzeRMSD(traj, reference_view, sele_view)

  This function extracts the rmsd between two :class:`~ost.mol.EntityView` and returns it as a vector
  The views don't have to be from the same entity. The reference positions are taken directly
  from the reference_view, evaluated only once. The positions from the sele_view are evaluated for
  each frame.
  If you want to compare to frame i of the trajectory t, first use t.CopyFrame(i) for example:
  eh=io.LoadPDB(...),t=io.LoadCHARMMTraj(eh,...);sele=eh.Select(...);t.CopyFrame(0);mol.alg.AnalyzeRMSD(t,sele,sele)

  :param traj: The trajectory to be analyzed.
  :type traj: :class:`~ost.mol.CoordGroupHandle`
  :param reference_view: The selection used as reference structure
  :type reference_view: :class:`~ost.mol.EntityView`.
  :param sele_view: The selection compared to the reference_view
  :type sele_view: :class:`~ost.mol.EntityView`.
  :param stride: Size of the increment of the frame's index between two consecutive frames analyzed.









