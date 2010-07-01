:mod:`~ost.conop` -- Connectivity and Topology of Molecules
================================================================================

.. module:: ost.conop
   :synopsis: The conop modules implement different strategies to derive
               connectivity information of molecules.

The main task of the conop module is to connect atoms with bonds. While the 
bond class is also part of the base module, the conop module deals with setting
up the correct bonds between atoms.

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

The Builder interface
--------------------------------------------------------------------------------

The conop module defines a :class:`Builder` interface, to run connectivity 
algorithms, that is to connect the atoms with bonds and perform basic clean up 
of errorneous structures. The clients of the conop module can specify how the 
Builder should treat unknown amino acids, missing atoms and chemically 
infeasible bonds.

The exact behaviour for a builder is implementation-specific. So far, two
classes implement the Builder interface: A heuristic and a  rule-based builder. The builders mainly differ in the source of their connectivity information. The
HeuristicBuilder uses a hard-coded heuristic connectivity table for the 20
standard amino acids as well as nucleotides.For other compounds such as ligands
the HeuristicBuilder runs a distance-based connectivity algorithm that connects
two atoms if they are closer than a  certain threshold. The RuleBasedBuilder
uses a connectivity library containing  all molecular components present in the
PDB files on PDB.org. The library can  easily be extended with custom 
connectivity information, if required. By default the heuristic builder is used,
however the builder may be switched by setting the !RuleBasedBuilder as the 
default. To do so, one has first to create a new instance of a RuleBasedBuilder 
and register it in the builder registry of the conop module. In Python, this can 
be achieved with

.. code-block:: python

  from ost import conop
  compound_lib=conop.CompoundLib.Load('...')
  rbb=conop.RuleBasedBuilder(compound_lib)
  conop.Conopology.Instance().RegisterBuilder(rbb,'rbb')
  conop.Conopology.Instance().SetDefaultBuilder('rbb')

All subsequent calls to :func:`ost.io.LoadEntity` will make use of the
RuleBasedBuilder  instead of the heuristic builder. See 
:ref:`here <mmcif-convert>` for more  information on how to create the necessary 
files to use the rule-based builder.


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
     

.. class:: RuleBasedBuilder
   
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
        
Connecting atoms
--------------------------------------------------------------------------------

A single function call to :func:`ConnectAll` is sufficient to assign residue and atoms properties as well as to connect atoms with bonds.


.. code-block:: python

  # Suppose that BuildRawModel is a function that returns a protein structure
  # with no atom properties assigned and no bonds formed.
  ent=BuildRawModel(...)
  print ent.bonds  # will return an empty list
  # Call ConnectAll() to assign properties/connect atoms
  conop.ConnectAll(ent)
  print ent.bonds  # will print a list containing many bonds

For fine grained control, the :class:`Builder` interface may be used directly.


.. _mmcif-convert:

Convert MM CIF dictionary
--------------------------------------------------------------------------------

The CompoundLib may be created from a MM CIF dictionary. The latest dictionary 
can be found on the `wwPDB site <http://www.wwpdb.org/ccd.html>`_. 

After downloading the file in MM CIF use the :program:`chemdict_tool` to convert
the MM CIF  dictionary into our internal format.

.. code-block:: bash
  
  chemdict_tool create <components.cif> <compounds.chemlib>
