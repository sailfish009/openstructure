:mod:`mol.alg <ost.mol.alg>` -- Algorithms for Structures
================================================================================

.. module:: ost.mol.alg
   :synopsis: Algorithms operating on molecular structures


.. function:: LocalDistDiffTest(model, reference, tolerance, radius, local_ldt_property_string="")
  
  This function calculates the agreement of local contacts between a model and 
  a reference structure (Local Distance Difference Tests). The overlap is a number 
  between zero and one, where one indicates complete agreement, zero indicates no 
  agreement at all. This score is similar to the GDT, but does not require any 
  superposition between the model and the reference.
  
  The distance of atom pairs in the reference structure, when shorter than a 
  certain predefined distance (inclusion radius), is compared with the same distance in 
  the model. If the difference between these two distances is smaller than a 
  threshold value (tolerance), the distance is considered conserverd in the model. Missing atoms 
  in the model lead to non-conserved distances and thus lower the score.
  
  The function only processes standard residues in the first chains of the model and of the reference
  For residues with symmetric sidechains (GLU, ASP, ARG, VAL, PHE, TYR), the 
  naming of the atoms is ambigous. For these residues, the function computes the Local Distance Difference 
  Test score that each naming convention would generate when considering all non-ambigous surrounding atoms.
  The solution that gives higher score is then picked to compute the final Local Difference
  Distance Score for the whole model.

  If a string is passed as last parameter to the function, the function computes the Local Difference Distance
  Test score for each residue and saves it as a float property in the ResidueHandle, with the passed string
  as property name. Additionally, the actual residue-based counts of the total checked distances and of 
  the distances conserved in the model are stored as integer properties in the ResidueHandle. The property
  names are respectively <passed string>_total and <passed string>_conserved.

  :param model: the model structure
  :type model: :class:`~ost.mol.EntityView`
  :param reference: the reference structure
  :type reference: :class:`~ost.mol.EntityView`
  :param tolerance: the tolerance threshold used to determine distance conservation
  :param radius: the inclusion radius in Angstroms
  :param local_ldt_property_string: the base name for the ResidueHandle properties that store the local scores

  :returns: the Local Distance Difference Test score

  
.. function:: LocalDistDiffTest(model, distance_list, tolerance_list, sequence_separation=0,  local_ldt_property_string="")
  
  This function counts the conserved local contacts between the model and the reference structure
  (these are the values needed to compute the Local Distance Difference Test score, see description of 
  the previous function). It shares the same properties as the previous function, with some differences:
  the thresholds can be more than one (the return counts are then the average over all thresholds), and
  the input is not the reference structure, but already a list of distances to be checked for conservation

  A sequence separation parameter can be passed to the function. If this happens, only distances between residues
  whose separation is higher than the provided parameter are considered when computing the score

  If a string is passed as the last parameter, residue-based counts and the value of the residue-based Local
  Distance Difference Test score are saved in each ResidueHandle as int and float properties, as detailed in
  the description of the previous function. 

  :param model: the model structure
  :type model: :class:`~ost.mol.EntityView`
  :param distance_list: the list of distances to check for conservation
  :type distance_list: :class:`~ost.mol.alg.GlobalRDMap`
  :param tolerance_list: a list of thresholds used to determine distance conservation
  :param sequence_separation: sequence separation parameter used when computing the score
  :param local_ldt_property_string: the base name for the ResidueHandle properties that store the local scores

  :returns: a tuple containing the counts of the conserved distances in the model and of all the checked 
            distances


.. function::  LocalDistDiffTest(alignment, tolerance, radius, ref_index=0, mdl_index=1);

  Calculates the Local Distance Difference Test score (see previous functions) starting from an
  alignment between a reference structure and a model. The AlignmentHandle parameter used to provide the 
  alignment to the function needs to have the two structures attached to it. By default the first structure in the
  alignment is considered to be the reference structure, and the second structure is taken as the model. This
  can however be changed by passing the indexes of the two structures in the AlignmentHandle as parameters to the
  function. 

  BEWARE: This function uses the old implementation of the Local Distance Difference Test algorithm and
  will give slightly different results from the new one.  

  :param alignment: an alignment containing the sequences of the reference and of the model structures, with the structures themselves
                    attached
  :type alignment: :class:`~ost.seq.AlignmentHandle`
  :param tolerance: a list of thresholds used to determine distance conservation
  :param radius: the inclusion radius in Angstroms (to determine which distances are checked for conservation)
  :param ref_index: index of the reference structure in the alignment 
  :param mdl_index: index of the model in the alignment

  :returns: the Local Distance Difference Test score


.. function::  LDDTHA(model, distance_list, sequence_separation=0);

  This function calculates the Local Distance Difference Test - High Accuracy score (see previous functions).
  The High Accuracy name comes from the fact that the tolerance levels used by this function are the same
  as the thresholds used by GDT-HA (0.5, 1, 2, and 4 Angstrom). 

  The function only compares the input distance list to the first chain of the model structure

  The local Local Distance Difference Test score values are stored in the ResidueHandles of the model passed to the 
  function in a float property called "locallddt" 

  A sequence separation parameter can be passed to the function. If this happens, only distances between residues
  whose separation is higher than the provided parameter are considered when computing the score

  :param model: the model structure
  :type model: :class:`~ost.mol.EntityView`
  :param distance_list: the list of distances to check for conservation
  :type distance_list: :class:`~ost.mol.alg.GlobalRDMap`
  :param sequence_separation: sequence separation parameter used when computing the score

  :returns: the Local Distance Difference Test score


.. function:: CreateDistanceList(reference, radius);
.. function:: CreateDistanceListFromMultipleReferences(reference_list, tolerance_list, sequence_separation, radius);

  Both these functions create lists of distances to be checked during a Local Distance Difference Test
  (see description of the functions above). 

  Both functions process only standard residues present in the first chain of the reference structures.

  The only difference between the two functions is that one takes a single reference structure and the other
  a list of reference structures. The structures in the list have to be properly prepared before being passed 
  to the function. Corresponding residues in the structures must have the same residue number, the same chain name,
  etc. Gaps are allowed and automatically dealt with: if information about a distance is present in at least one of
  the structures, it will be considered.  

  If a distance between two atoms is shorter than the inclusion radius in all structures in which the two atoms are
  present, it is included in the list. However, if the distance is longer than the inclusion radius in at least
  one of the structures, it is not considered to be a local interaction and is excluded from the list.

  The multiple-reference function takes care of residues with ambigous symmetric sidechains. To decide which naming
  convention to use, the function computes a Local Distance Difference Test score for  each reference against the
  first reference structure in the list, using only non ambigously-named atoms. It picks then the naming convention
  that gives the highest score, guaranteeing that all references are processed with the correct atom names.

  The cutoff list that will later be used to compute the Local Distance Difference Test score and the sequence
  separation parameter must be passed to the multi-reference function. These parameters do not influence the output
  distance list, which always includes all distances within the provided radius (to make it consistent with the
  single-reference corresponding function). However, the parameters are used when dealing with the naming convention
  of residues with ambiguous nomenclature.

  :param reference: a reference structure from which distances are derived
  :type reference: :class:`~ost.mol.EntityView`
  :param reference_list: a list of  of reference structures from which distances are derived
  :type reference_list: list of :class:`~ost.mol.EntityView`
  :param tolerance_list: a list of thresholds used to determine distance conservation when computing the LDDT score
  :param sequence_separation: sequence separation parameter used when computing the LDDT score
  :param radius: inclusion radius (in Angstroms) used to determine the distances included in the list
  
  :returns: class `~ost.mol.alg.GlobalRDMap`

  
.. class:: UniqueAtomIdentifier

  Object containing enough information to uniquely identify an atom in a structure

  .. method:: UniqueAtomIdentifier(chain,residue_number,residue_name,atom_name)
    
    Creates an UniqueAtomIdentifier object starting from relevant atom information

    :param chain: a string containing the name of the chain to which the atom belongs
    :param residue_number: the number of the residue to which the atom belongs 
    :type residue_number: :class:`~ost.mol.ResNum`
    :param residue_name: a string containing the name of the residue to which the atom belongs
    :param atom_name: a string containing the name of the atom

  .. method:: GetChainName() 

    Returns the name of the chain to which the atom belongs, as a String  

  .. method:: GetResNum() 

    Returns the number of the residue the atom belongs to, as a :class:`~ost.mol.ResNum` object

  .. method:: GetResidueName()
    
     Returns the name of the residue to which the atom belongs, as a String
 
  .. method:: GetAtomName()

     Returns the name of the atom, as a String

  .. method:: GetQualifiedAtomName()

     Returns the qualified name of the atom (the chain name, followed by a unique residue identifier and the atom name. For example:  “A.GLY2.CA”)


.. class:: ResidueRDMap

  Dictionary-like object containing the a list of distances that originate from the a single residue residue, to
  check during a run of the Local Distance Difference Test algorithm 


.. class:: GlobalRDMap

  Dictionary-like object containing all the :class:`~ost.mol.alg.ResidueRDMap` objects related to residues
  of a single structure

  
.. function: PrintResidueRDMap(residue_distance_list)

  Prints to standard output all the distances contained in a :class:`~ost.mol.ResidueRDMap` object 


.. function: PrintGlobalRDMap(global_distance_list)

  Prints to standard output all the distances contained in each of the :class:`~ost.mol.ResidueRDMap` objects that
  make up a :class:`~ost.mol.GlobalRDMap` object


.. _steric-clashes:

Steric Clashes
--------------------------------------------------------------------------------

The following function detects steric clashes in atomic structures. Two atoms are clashing if their euclidian distance is smaller than a threshold value (minus a tolerance offset). 


.. function:: FilterClashes(entity, clashing_distances, always_remove_bb=False)

  This function filters out residues with non-bonded clashing atoms. If the clashing atom 
  is a backbone atom, the complete residue is removed from the structure, if the atom is part of 
  the sidechain, only the sidechain atoms are removed. This behavior is changed 
  by the always_remove_bb flag: when the flag is set to True the whole residue is removed even if
  a clash is just detected in the side-chain.

  The function returns a view containing all elements (residues, atoms) that have not been removed from the 
  input structure, plus a :class:`~ost.mol.alg.ClashingInfo` object containing information about the 
  detected clashes.
  
  Two atoms are defined as clashing if their distance is shorter than the reference distance minus a tolerance
  threshold. The information about the clashing distances and the tolerance thresholds for all possible pairs of 
  atoms is passed to the function as a parameter

  Hydrogen and deuterium atoms are ignored by this function.
  
  :param entity: The input entity
  :type entity: :class:`~ost.mol.EntityView` or :class:`~ost.mol.EntityHandle`
  :param clashing_distances: information about the clashing distances
  :type clashing_distances: :class:`~ost.mol.alg.ClashingDistances`
  :param always_remove_bb: if set to True, the whole residue is removed even if the clash happens in the side-chain

  :returns: A tuple of two elements: The filtered :class:`~ost.mol.EntityView`, and a :class:`~ost.mol.alg.ClashingInfo` object


.. function:: CheckStereoChemistry(entity,bond_stats,angle_stats,bond_tolerance,angle_tolerance,always_remove_bb=False)

  This function filters out residues with severe stereo-chemical violations. If the violation 
  involves a backbone atom, the complete residue is removed from the structure, if it involves an atom that is 
  part of the sidechain, only the sidechain is removed. This behavior is changed 
  by the always_remove_bb flag: when the flag is set to True the whole residue is removed even if
  a violation is just detected in the side-chain

  The function returns a view containing all elements (residues, atoms) that have not been removed from the input structure, plus a :class:`~ost.mol.alg.StereoChemistryInfo` object containing information about the detected stereo-chemical violations.
    
  A violation is defined as a bond length that lies outside of the range: [mean_length-std_dev*bond_tolerance <-> meanlength+std_dev*bond_tolerance] or an angle width lying outside of the range [mean_width-std_dev*angle_tolerance <-> mean_width+std_dev*angle_tolerance ]. The information about the mean lengths and widths and the corresponding standard deviations is passed to the function using two parameters.

  Hydrogen and deuterium atoms are ignored by this function.

  :param entity: The input entity
  :type entity: :class:`~ost.mol.EntityView` or :class:`~ost.mol.EntityHandle`
  :param bond_stats: statistics about bond lengths
  :type bond_stats: :class:`~ost.mol.alg.StereoChemicalParams`
  :param angle_stats: statistics about angle widths
  :type angle_stats: :class:`~ost.mol.alg.StereoChemicalParams`
  :param bond_tolerance: tolerance for bond lengths (in standard deviations)
  :param angle_tolerance: tolerance for angle widths (in standard deviations)£
  :param always_remove_bb: if set to True, the whole residue is removed even if a violation in just detected in the side-chain

  :returns: A tuple of two elements: The filtered :class:`~ost.mol.EntityView`, and a :class:`~ost.mol.alg.StereoChemistryInfo` object


.. class:: ClashingInfo

  This object is returned by the FilterClashes function, and contains information about the clashes detected by the function. 

  .. method:: GetClashCount()

     This method returns the number of clashes between non-bonded atoms detected in the input structure

  .. method:: GetAverageOffset()

     This methods returns a value in Angstroms representing the average offset by which clashing atoms lie closer than the minimum acceptable distance (which of course differs for each possible pair of elements)

     :returns: the average offset, in Angstroms

  .. method:: GetClashList()

     Returns the list of detected inter-atomic clashes

     :returns: a list of :class:`~ost.mol.alg.ClashEvent` objects


.. class:: ClashEvent

  This object contains all the information relative to a single clash detected by the FilterClashes function

  .. method:: GetFirstAtom()
  .. method:: GetSecondAtom()

     These two methods return the two atoms which clash

     :returns: :class:`~ost.mol.alg.UniqueAtomIdentifier`

  .. method:: GetModelDistance()

     This method returns the distance between the two clashing atoms as observed in the model

     :returns: the distance in Angstroms between the two atoms

  .. method:: GetAdjustedReferenceDistance()

     This method returns the minimum acceptable distance between the two atoms involved in the clash, as defined in the :class:`~ost.mol.alg.ClashingDistances` class

     :returns: the minimum acceptable distance in Angstroms


.. class:: StereoChemistryInfo

  This object is returned by the CheckStereoChemistry function, and contains information about bond lengths and planar angle widths in the structure that diverge from the parameters tabulated by Engh and Huber in the International Tables of Crystallography. Only elements that diverge from the tabulated value by a minimum  number of standard deviations (defined when the CheckStereoChemistry function is called) are reported.

  .. method:: GetBadBondCount()

     This method returns the number of bonds where a serious violation was detected

  .. method:: GetBondCount()

     This method returns the total number of bonds in the structure checked by the CheckStereoChemistry function

  .. method GetAvgZscoreBonds()

     This method returns the average z-score of all the bond lengths in the structure, computed using Engh and Huber's mean and standard deviation values.

     :returns: The average z-score of bond lengths

  .. method:: GetBadAngleCount()  

     This method returns the number of planar angles where a serious violation was detected

  .. method:: GetAngleCount()

     This method returns the total number of planar angles in the structure checked by the CheckStereoChemistry function

  .. method:: GetAvgZscoreAngles()

     This method returns the average z-score of all the planar angle widths, computed using Engh and Huber's mean and standard deviation values.

     :returns: The average z-score of planar angle widths

  .. method:: GetBondViolationList()

     Returns the list of bond length violations detected in the structure

     :returns: a list of :class:`~ost.mol.alg.StereoChemicalBondViolation` objects

  .. method:: GetAngleViolationList()

     Returns the list of angle width violations detected in the structure

     :returns: a list of :class:`~ost.mol.alg.StereoChemicalAngleViolation` objects


.. class:: StereoChemicalBondViolation

  This object contains all the information relative to a single detected violation of stereo-chemical parameters in a bond length

  .. method:: GetFirstAtom()

     Returns the first atom of the bond

     :returns: :class:`~ost.mol.alg.UniqueAtomIdentifier`

  .. method:: GetSecondAtom()

     Returns the first atom of the bond

     :returns: :class:`~ost.mol.alg.UniqueAtomIdentifier`

  .. method:: GetBondLength()

     Returns the length of the bond as observed in the model

     :returns: the bond length in Angstroms

  .. method:: GetAllowedRange()

     Returns the allowed range of bond lengths, according to the Engh and Huber's tabulated parameters and the tolerance threshold used when CheckStereoChemistry function was called

     :returns: a tuple containing the minimum and maximum allowed bond lengths in Angstroms


.. class:: StereoChemicalAngleViolation

  This object contains all the information relative to a single detected violation of stereo-chemical parameters in a planar angle width

  .. method:: GetFirstAtom()

     Returns the first atom that defines the planar angle 

     :returns: :class:`~ost.mol.alg.UniqueAtomIndentifier`

  .. method:: GetSecondAtom()

     Returns the vertex atom of the planar angle

     :returns: :class:`~ost.mol.alg.UniqueAtomIdentifier`

  .. method:: GetThirdAtom()

     Returns the third atom that defines the planar angle

     :returns: :class:`~ost.mol.alg.UniqueAtomIdentifier`

  .. method:: GetAngleWidth()

     Returns the width of the planar angle as observed in the model

     :returns: the angle width in degrees

  .. method:: GetAllowedRange()

     Returns the allowed range of angle widths, according to the Engh and Huber's tabulated parameters and the tolerance threshold used when the CheckStereoChemistry function was called

     :returns: a tuple containing the minimum and maximum allowed angle widths in degrees


.. class:: ClashingDistances

  Object containing information about clashing distances between non-bonded atoms

  .. method:: ClashingDistances()

    Creates an empty distance list

  .. method:: SetClashingDistance(ele1,ele2, clash_distance, tolerance)

    Adds or replaces an entry in the list

    :param ele1: string containing the first element's name 
    :param ele2: string containing the second element's name 
    :param clash_distance: minimum clashing distance (in Angstroms)
    :param tolerance: tolerance threshold (in Angstroms)

  .. method:: GetClashingDistance()

     Recovers a reference distance and a tolerance threshold from the list

    :param ele1: string containing the first element's name 
    :param ele2: string containing the second element's name 

    :returns: a tuple containing the minimum clashing distance and the tolerance threshold

  .. method:: GetAdjustedClashingDistance()

     Recovers a reference distance from the list, already adjusted by the tolerance threshold

    :param ele1: string containing the first element's name
    :param ele2: string containing the second element's name

    :returns: the adjusted minimum clashing distance

  .. method::  GetMaxAdjustedDistance()
 
    Returns the longest clashing distance in the list, after adjustment with tolerance threshold    

  .. method:: IsEmpty()

    Returns True if the list is empty (i.e. in an invalid, useless state)
 
  .. method:: PrintAllDistances()

    Prints all distances in the list to standard output


.. class:: StereoChemicalParams

  Object containing stereo-chemical information about bonds and angles. For each item (bond or angle
  in a specific residue), stores the mean and standard deviation 

  .. method:: StereoChemicalParams()

    Creates an empty parameter list

  .. method:: SetParam(item, residue, mean, standard_dev)

    Adds or replaces an entry in the list

    :param item: string defining a bond (format: X-Y) or an angle (format:X-Y-Z), where X,Y an Z are atom names 
    :param residue: string containing the residue type the information pertains to 
    :param mean: mean bond length or angle width
    :param standard_dev: standard deviation of the bond length or of the angle width

  .. method GetParam(item,residue)

     Recovers an entry from the list, 

    :param item: string defining a bond (format: X-Y) or an angle (format:X-Y-Z), where X,Y an Z are atom names 
    :param residue: string containing the residue type the item information pertains to 

    :returns: a tuple containing the mean length or width and the standard deviation

  .. method ContainsParam(item,residue)

    Checks if a specific entry is present in the list 

    :param item: string defining a bond (format: X-Y) or an angle (format:X-Y-Z), where X,Y an Z are atom names 
    :param residue: string containing the residue type the information pertains to 

    :returns: True if the list contains an entry corresponding to the correct item and residue, False if it does not

  .. method:: IsEmpty()

    Returns True if the list is empty (i.e. in an invalid, useless state)
 
  .. method:: PrintAllParameters()

    Prints all distances in the list to standard output  


.. function:: FillClashingDistances(file_content)
.. function:: FillBondStereoChemicalParams(file_content)
.. function:: FillAngleStereoChemicalParams(file_content)

  These three functions fill a list of reference clashing distances, a list of stereo-chemical parameters for 
  bonds and a list of stereo-chemical parameters for angles, respectively, starting from a the content of 
  parameter file. The content of the file is passed to the function as a list of strings, each containing
  a line from the parameter file

  :returns: :class:`~ost.mol.alg.ClashingDistances` and :class:`~ost.mol.alg.StereoChemicalParams` respectively


.. function:: FillClashingDistancesFromFile(filename)
.. function:: FillBondStereoChemicalParamsFromFile(filename)
.. function:: FillAngleStereoChemicalParamsFromFile(filename)

  These three functions fill a list of reference clashing distances, a list of stereo-chemical parameters for 
  bonds and a list of stereo-chemical parameters for angles, respectively, starting from a file. The filename 
  passed to the function can be a full path.

  :returns: :class:`~ost.mol.alg.ClashingDistances` and :class:`~ost.mol.alg.StereoChemicalParams` respectively


.. function:: DefaultClashingDistances()
.. function:: DefaultBondStereoChemicalParams()
.. function:: DefaultAngleStereoChemicalParams()

  These three functions fill a list of reference clashing distances, a list of stereo-chemical parameters for 
  bonds and a list of stereo-chemical parameters for angles, respectively, using the default parameter file
  distributed with OpenStructure.

  :returns: :class:`~ost.mol.alg.ClashingDistances` and :class:`~ost.mol.alg.StereoChemicalParams` respectively


.. function:: ResidueNamesMatch(probe,reference)

  The function requires a reference structure and a probe structure. The function checks that all the 
  residues in the reference structure that appear in the probe structure (i.e., that have the 
  same ResNum) are of the same residue type. Chains are comapred by order, not by chain name 
  (i.e.: the first chain of the reference will be compared with the first chain of the probe 
  structure, etc.)

  :param probe: the structure to test
  :type probe: :class:`~ost.mol.EntityView`
  :param reference: the reference structure
  :type reference: :class:`~ost.mol.EntityView`

  :returns: true if the residue names are the same, false otherwise


.. _traj-analysis:

Trajectory Analysis
--------------------------------------------------------------------------------

This is a set of functions used for basic trajectory analysis such as extracting 
positions, distances, angles and RMSDs. The organization is such that most 
functions have their counterpart at the individual :class:`frame level 
<ost.mol.CoordFrame>` so that they can also be called on one frame instead 
of the whole trajectory.

All these functions have a "stride" argument that defaults to stride=1, which is 
used to skip frames in the analysis.


.. function:: AnalyzeAtomPos(traj, atom1, stride=1)

  This function extracts the position of an atom from a trajectory. It returns 
  a vector containing the position of the atom for each analyzed frame.
  
  :param traj: The trajectory to be analyzed.
  :type traj: :class:`~ost.mol.CoordGroupHandle`
  :param atom1: The :class:`~ost.mol.AtomHandle`.
  :param stride: Size of the increment of the frame's index between two 
      consecutive frames analyzed.
  
  
.. function:: AnalyzeCenterOfMassPos(traj, sele, stride=1)

  This function extracts the position of the center-of-mass of a selection 
  (:class:`~ost.mol.EntityView`) from a trajectory and returns it as a vector.
  
  :param traj: The trajectory to be analyzed.
  :type traj: :class:`~ost.mol.CoordGroupHandle`
  :param sele: The selection from which the center of mass is computed
  :type sele: :class:`~ost.mol.EntityView`.
  :param stride: Size of the increment of the frame's index between two 
     consecutive frames analyzed.
  

.. function:: AnalyzeDistanceBetwAtoms(traj, atom1, atom2, stride=1)

  This function extracts the distance between two atoms from a trajectory 
  and returns it as a vector.
  
  :param traj: The trajectory to be analyzed.
  :type traj: :class:`~ost.mol.CoordGroupHandle`
  :param atom1: The first :class:`~ost.mol.AtomHandle`.
  :param atom2: The second :class:`~ost.mol.AtomHandle`.
  :param stride: Size of the increment of the frame's index between two 
      consecutive frames analyzed.  


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
  :param stride: Size of the increment of the frame's index between two 
     consecutive frames analyzed.


.. function:: AnalyzeDihedralAngle(traj, atom1, atom2, atom3, atom4, stride=1)

  This function extracts the dihedral angle between four atoms from a trajectory 
  and returns it as a vector. The angle is between the planes containing the  
  first three and the last three atoms.
  
  :param traj: The trajectory to be analyzed.
  :type traj: :class:`~ost.mol.CoordGroupHandle`
  :param atom1: The first :class:`~ost.mol.AtomHandle`.
  :param atom2: The second :class:`~ost.mol.AtomHandle`.
  :param atom3: The third :class:`~ost.mol.AtomHandle`.
  :param atom4: The fourth :class:`~ost.mol.AtomHandle`.
  :param stride: Size of the increment of the frame's index between two 
      consecutive frames analyzed.

.. function:: AnalyzeDistanceBetwCenterOfMass(traj, sele1, sele2, stride=1)

  This function extracts the distance between the center-of-mass of two 
  selections (:class:`~ost.mol.EntityView`) from a trajectory and returns it as 
  a vector.

  :param traj: The trajectory to be analyzed.
  :type traj: :class:`~ost.mol.CoordGroupHandle`
  :param sele1: The selection from which the first center of mass is computed
  :type sele1: :class:`~ost.mol.EntityView`.
  :param sele2: The selection from which the second center of mass is computed
  :type sele2: :class:`~ost.mol.EntityView`.
  :param stride: Size of the increment of the frame's index between two 
     consecutive frames analyzed.


.. function:: AnalyzeRMSD(traj, reference_view, sele_view, stride=1)

  This function extracts the rmsd between two :class:`~ost.mol.EntityView` and 
  returns it as a vector. The views don't have to be from the same entity. The 
  reference positions are taken directly from the reference_view, evaluated only 
  once. The positions from the sele_view are evaluated for each frame.
  If you want to compare to frame i of the trajectory t, first use 
  t.CopyFrame(i) for example:
 
  .. code-block:: python
     
    eh=io.LoadPDB(...)
    t=io.LoadCHARMMTraj(eh,...)
    sele=eh.Select(...)
    t.CopyFrame(0)
    mol.alg.AnalyzeRMSD(t,sele,sele)

  :param traj: The trajectory to be analyzed.
  :type traj: :class:`~ost.mol.CoordGroupHandle`
  :param reference_view: The selection used as reference structure
  :type reference_view: :class:`~ost.mol.EntityView`.
  :param sele_view: The selection compared to the reference_view
  :type sele_view: :class:`~ost.mol.EntityView`.
  :param stride: Size of the increment of the frame's index between two 
      consecutive frames analyzed.

.. function:: AnalyzeMinDistance(traj, view1, view2, stride=1)

  This function extracts the minimal distance between two sets of atoms 
  (view1 and view2) for each frame in a trajectory and returns it as a vector.

  :param traj: The trajectory to be analyzed.
  :type traj: :class:`~ost.mol.CoordGroupHandle`
  :param view1: The first group of atoms
  :type view1: :class:`~ost.mol.EntityView`.
  :param view2: The second group of atoms
  :type view2: :class:`~ost.mol.EntityView`.
  :param stride: Size of the increment of the frame's index between two 
     consecutive frames analyzed.
     
.. function:: AnalyzeMinDistanceBetwCenterOfMassAndView(traj, view_cm, view_atoms, stride=1)

  This function extracts the minimal distance between a set of atoms 
  (view_atoms) and the center of mass of a second set of atoms (view_cm) 
  for each frame in a trajectory and returns it as a vector.

  :param traj: The trajectory to be analyzed.
  :type traj: :class:`~ost.mol.CoordGroupHandle`
  :param view_cm: The group of atoms from which the center of mass is taken
  :type view_cm: :class:`~ost.mol.EntityView`.
  :param view_atoms: The second group of atoms
  :type view_atoms: :class:`~ost.mol.EntityView`.
  :param stride: Size of the increment of the frame's index between two 
     consecutive frames analyzed.

.. function:: AnalyzeAromaticRingInteraction(traj, view_ring1, view_ring2, stride=1)

  This function is a crude analysis of aromatic ring interactions. For each frame in a trajectory, it calculates
  the minimal distance between the atoms in one view and the center of mass of the other
  and vice versa, and returns the minimum between these two minimal distances.
  Concretely, if the two views are the heavy atoms of two rings, then it returns the minimal
  center of mass - heavy atom distance betweent he two rings

  :param traj: The trajectory to be analyzed.
  :type traj: :class:`~ost.mol.CoordGroupHandle`
  :param view_ring1: First group of atoms
  :type view_ring1: :class:`~ost.mol.EntityView`.
  :param view_ring2: Second group of atoms
  :type view_ring2: :class:`~ost.mol.EntityView`.
  :param stride: Size of the increment of the frame's index between two 
     consecutive frames analyzed.  

.. function:: SuperposeFrames(frames, sel, from=0, to=-1, ref=-1)

  This function superposes the frames of the given coord group and returns them
  as a new coord group.
  
  :param frames: The source coord group.
  :type frames: :class:`~ost.mol.CoordGroupHandle`
  :param sel: An entity view containing the selection of atoms to be used for     
    superposition. If set to an invalid view, all atoms in the coord group are 
    used.
  :type sel: :class:`ost.mol.EntityView`
  :param from: index of the first frame
  :param to: index of the last frame plus one. If set to -1, the value is set to 
     the number of frames in the coord group
  :param ref: The index of the reference frame to use for superposition. If set 
     to -1, the each frame is superposed to the previous frame.
     
  :returns: A newly created coord group containing the superposed frames.

.. function:: SuperposeFrames(frames, sel, ref_view, from=0, to=-1)

  Same as SuperposeFrames above, but the superposition is done on a reference
  view and not on another frame of the trajectory.
  
  :param frames: The source coord group.
  :type frames: :class:`~ost.mol.CoordGroupHandle`
  :param sel: An entity view containing the selection of atoms of the frames to be used for     
    superposition.
  :type sel: :class:`ost.mol.EntityView`
  :param ref_view: The reference view on which the frames will be superposed. The number
    of atoms in this reference view should be equal to the number of atoms in sel.
  :type ref_view: :class:`ost.mol.EntityView`
  :param from: index of the first frame
  :param to: index of the last frame plus one. If set to -1, the value is set to 
     the number of frames in the coord group     
  
  :returns: A newly created coord group containing the superposed frames.

.. autofunction:: ParseAtomNames

.. autofunction:: MatchResidueByNum

.. autofunction:: MatchResidueByIdx

.. autofunction:: MatchResidueByLocalAln

.. autofunction:: MatchResidueByGlobalAln

.. autofunction:: Superpose

