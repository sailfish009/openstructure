Connectivity
================================================================================

.. currentmodule:: ost.conop


Motivation
--------------------------------------------------------------------------------
Traditionally the connectivity between atoms has not been reliably described in
a PDB file. Different programs adopted various ways of finding out if two atoms
are connected. One way chosen is to rely on proper naming of the atoms. For 
example, the backbone atoms of the standard amino acids are named as N, CA, C 
and O and if atoms with these name appear in the same residue they are shown 
connected. Another way is to apply additional heuristics to find out if a
peptide bond between two consecutive residues is formed. Breaks in the backbone
are indicated, e.g., by introducing a discontinuity in the numbering of the residue.

Loader heuristics are great if you are the one that implemented them but are 
problematic if you are just the user of a software that has them. As time goes 
on, these heuristics become buried in thousands of lines of code and they are 
often hard yet impossible to trace back.

Different clients of the framework have different requirements. A visualisation 
software wants to read in a PDB files as is without making any changes. A 
script in an automated pipeline, however, does want to either strictly reject 
files that are incomplete or fill-in missing structural features. All these 
aspects are implemented in the conop module, separated from the loading of the 
PDB file, giving clients a fine grained control over the loading process.

The conop module defines a :class:`Builder` interface, to run connectivity 
algorithms, that is to connect the atoms with bonds and perform basic clean up 
of erroneous structures. The clients of the conop module can specify how the 
Builder should treat unknown amino acids, missing atoms and chemically 
infeasible bonds.

The high-level interface
--------------------------------------------------------------------------------


.. autofunction:: ConnectAll()



A call to :func:`ConnectAll` is sufficient to assign residue and atoms 
properties as well as to connect atoms with bonds.


.. code-block:: python

  # Suppose that BuildRawModel is a function that returns a protein structure
  # with no atom properties assigned and no bonds formed.
  ent=BuildRawModel(...)
  print ent.bonds  # will return an empty list
  # Call ConnectAll() to assign properties/connect atoms
  conop.ConnectAll(ent)
  print ent.bonds  # will print a list containing many bonds

For a more fine-grained control, consider using the :class:`Builder` interface.

The builder interface
--------------------------------------------------------------------------------

The exact behaviour for a builder is implementation-specific. So far, two
classes implement the Builder interface: A heuristic and a rule-based builder. The builders mainly differ in the source of their connectivity information. The
HeuristicBuilder uses a hard-coded heuristic connectivity table for the 20
standard amino acids as well as nucleotides.For other compounds such as ligands
the HeuristicBuilder runs a distance-based connectivity algorithm that connects
two atoms if they are closer than a  certain threshold. The RuleBasedBuilder
uses a connectivity library containing  all molecular components present in the
PDB files on PDB.org. The library can  easily be extended with custom 
connectivity information, if required. If a :doc:`compound library <compoundlib>` is present, the :class:`RuleBasedBuilder` is enabled by default, otherwise the :class:`HeuristicBuilder` is used as a fallback.

The following 3 functions give you access to builders known to OpenStructure, 
and allow you to set the default builder:


.. autofunction:: GetBuilder()

.. autofunction:: RegisterBuilder()
  
.. autofunction:: SetDefaultBuilder()

The Builder baseclass
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. class:: Builder

  .. method:: CompleteAtoms(residue)
  
    add any missing atoms to the residue based on its key, with coordinates set
    to zero.
    
    :param residue: must be a valid residue
    :type  residue: mol.ResidueHandle
    
  .. method:: CheckResidueCompleteness(residue)
  
    verify that the given residue has all atoms it is supposed to have based on
    its key.
    
    :param residue: must be a valid residue
    :type  residue: mol.ResidueHandle
    
  .. method:: IsResidueComplete(residue)
  
    Check whether the residue has all atoms it is supposed to have. Hydrogen
    atoms are not required for a residue to be complete.
    
    :param residue: must be a valid residue
    :type  residue: mol.ResidueHandle
    
  .. method::   IdentifyResidue(residue)
  
    attempt to identify the residue based on its atoms, and return a suggestion
    for the proper residue key.
    
    :param residue: must be a valid residue
    :type  residue: mol.ResidueHandle
    
  .. method:: ConnectAtomsOfResidue(residue)
  
     Connects atoms of residue based on residue and atom name. This method does
     not establish inter-residue bonds. To connect atoms that belong to 
     different residues, use :meth:`ConnectResidueToPrev`, or
     :meth:`ConnectResidueToNext`.
     
     :param residue: must be a valid residue
     :type  residue: mol.ResidueHandle
     
  .. method:: ConnectResidueToPrev(residue, prev)
  
     Connect atoms of residue to previous. The order of the parameters is
     important. In case of a polypeptide chain, the residues are thought to be
     ordered from N- to C- terminus.
     
     :param residue: must be a valid residue
     :type  residue: mol.ResidueHandle
     :param prev: valid or invalid residue
     :type  prev: mol.ResidueHandle
     
     
  .. method:: DoesPeptideBondExist(n, c)
  
     Check if peptide bond should be formed between the `n` and `c` atom. This
     method is called by ConnectResidueWithNext() after making sure that
     both residues participating in the peptide bond are peptide linking
     components.
     
     By default, :meth:`IsBondFeasible` is used to check whether the two atoms
     form a peptide bond.
     
     :param n: backbone nitrogen atom (IUPAC name `N`). Must be valid.
     :type  n: mol.AtomHandle
     :param c: backbone C-atom (IUPAC name `C`). Must be valid.
     :type  c: mol.AtomHandle
     
  .. method:: IsBondFeasible(atom_a, atom_b)
  
    Overloadable hook to check if bond between to atoms is feasible. The
    default implementation uses a distance-based check to check if the
    two atoms should be connected. The atoms are connected if they are in
    the range of 0.8 to 1.2 times their van-der-WAALS radius.
    
    :param atom_a: a valid atom
    :type  atom_b: mol.AtomHandle
    :param atom_a: a valid atom
    :type  atom_b: mol.AtomHandle
    
  .. method:: GuessAtomElement(atom_name, hetatm)
  
    guess element of atom based on name and hetatm flag
    
    :param atom_name: IUPAC atom name, e.g. `CA`, `CB` or `N`.
    :type  atom_name: string
    :param    hetatm: Whether the atom is a hetatm or not
    :type     hetatm: bool
    
  .. method:: AssignBackboneTorsionsToResidue(residue)
  
     For :meth:`peptide-linking residues <mol.ResidueHandle.IsPeptideLinking>`,
     residues, assigns phi, psi and omega torsions to amino acid.
     
     :param residue: must be a valid residue
     :type  residue: mol.ResidueHandle
     
  .. method:: GuessChemClass(residue)

    Guesses the chemical class of the residue based on its atom and
    connectivity.

    So far, the method only guesses whether the residue is a peptide. A residue
    is a peptide if all the backbone atoms N,CA,C,O are present, have the right
    element and are in a suitable orientation to form bonds.


The RuleBasedBuilder class
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. class:: RuleBasedBuilder(compound_lib)

   :param compound_lib: The compound library
   :type compound_lib: :class:`CompoundLib`
   
   The :class:`RuleBasedBuilder` implements the :class:`Builder` interface.
   Refer to its documentation for a basic description of the methods.
   
   .. method:: CheckResidueCompleteness(residue)
   
      By using the description of the chemical compound, the completeness of
      the residue is verified. The method distinguishes between required atoms
      and atoms that are optional, like `OXT` that is only present, if not
      peptide bond is formed. Whenever an unknown atom is encountered,
      :meth:`OnUnknownAtom` is invoked. Subclasses of the
      :class:`RuleBasedBuilder` may implement some additional logic to deal with
      unknown atom. Likewise, whenever a required atom is missing,
      :meth:`OnMissingAtom` is invoked. Hydrogen atoms are not considered as
      required by default.
      
      :param residue: must be a valid residue
      :type  residue: mol.ResidueHandle
    
   .. method:: IdentifyResidue(residue)
    
      Looks-up the residue in the database of chemical compounds and returns
      the name of the residue or "UNK" if the residue has not been found in the
      library.
   
      :param residue: must be a valid residue
      :type  residue: mol.ResidueHandle
   
   
   .. method:: OnUnknownAtom(atom)
   
      Invoked whenever an unkknown atom has been encountered during a residue
      completeness check.
      
      The default implementation guesses the atom properties based on the name 
      and returns false, meaning that it should be treated as an unknown atom.
      
      Custom implementations of this method may delete the atom, or modify it.
      
      :param atom: the unknown atom
      :type  atom: mol.AtomHandle
      
   .. method:: OnMissingAtom(atom)
    
      Invoked whenever an atom is missing. It is up to the overloaded method
      to deal with the missing atom, either by ignoring it or by inserting a
      dummy atom.
      
      :param atom: The missing atom's name
      :type  atom: string
    
   .. method:: GetUnknownAtoms(residue)
   
     Returns the unknown atoms of this residue, that is all atoms that 
     are not part of the compound lib definition.
     
     :rtype: list of :class:`~ost.mol.AtomHandle` instances

Changing the default builder
---------------------------------------------------------------------------------

The default builder can be specified with :func:`SetDefaultBuilder`. Before being
able to set a builder, it needs to be registered with :func:`RegisterBuilder`.
By default, there is always a builder called "HEURISTIC" registered. If, for some
reason your are currently using the :class:`RuleBasedBuilder` and you would like
to switch to that builder, call

.. code-block:: python

  conop.SetDefaultBuilder("HEURISTIC")
