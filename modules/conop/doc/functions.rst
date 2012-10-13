Conop Functions
=======================================================================


.. function:: AssignBackboneTorsions(prev, res, next)
              AssignBackboneTorsions(chain)
              AssignBackboneTorsions(residues)
   
   Assigns the backbone torsions PHI, PSI and OMEGA. The backbone atoms 
   are required to be connected for the torsions to be added. In addition, 
   only residues for which :meth:`mol.ResidueHandle.IsPeptideLinking` returns 
   true are considered.

   The first signature assigns the torsions to *res*, assuming prev is 
   the amino acid before, and *next* is the amino acid next to *res*.  
   Both *next* and *prev* may be invalid residues.

   The second and third signature assign the torsions to all peptidic 
   residues of the chain/the residue list. The residues in the chain, 
   the residue list are thought to run from N to C terminus.

   :param prev: The amino acid before *res*
   :type prev: :class:`~mol.ResidueHandle`
   :type next: :class:`~mol.ResidueHandle`
   :param next: The amino acid after *res*
   :type res: :class:`~mol.ResidueHandle`
   :type res: The central amino acid. Must be a valid handle

.. function:: GetUnknownAtomsOfResidue(residue, compound, strict_hydrogens=False) 

  Returns the list of atoms present in *residue* that are not part of the 
  atom specifications in compound.

  :param strict_hydrogens: When set to true, hydrogen atoms are checked as well.
