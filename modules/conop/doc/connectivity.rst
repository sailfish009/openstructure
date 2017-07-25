Connectivity
================================================================================

.. currentmodule:: ost.conop


Motivation
--------------------------------------------------------------------------------


The connectivity of atoms is notoriously difficult to come by for biological 
macromolecules. PDB files, the de-factor standard exchange format for structural 
information allows bonds to be specified in CONECT records. However, they are not
mandatory. Many programs, especially the ones not requiring on connectivity of 
atoms, do not write CONECT records. As a result, programs and structural biology 
frameworks can't rely on connectivity information to be present. The connectivity
information needs to be derived in the program itself.


Loader heuristics are great if you are the one that implemented them but are 
problematic if you are just the user of a software that has them. As time goes 
on, these heuristics become buried in thousands of lines of code and they are 
often hard yet impossible to trace back.

Different clients of the framework have different requirements. A visualisation 
software wants to read in a PDB files as is without making any changes. A 
script in an automated pipeline, however, does want to either strictly reject 
files that are incomplete or fill-in missing structural features. All these 
aspects are implemented in the conop module, separated from the loading of the 
PDB file, giving clients a fine grained control over the loading process. The
conop logic can thus be reused in code requiring the presence of 

The conop module defines a :class:`Processor` interface, to run connectivity 
algorithms, that is to connect the atoms with bonds and perform basic clean up 
of erroneous structures. The clients of the conop module can specify how the 
Processor should treat unknown amino acids, missing atoms and chemically 
infeasible bonds.

Processors
--------------------------------------------------------------------------------

The exact behaviour for a processor is implementation-specific. So far, two
classes implement the processor interface: A heuristic and a rule-based processor. 
The processor mainly differ in the source of their connectivity information. The
Heuristicprocessor uses a hard-coded heuristic connectivity table for the 20 
standard amino acids as well as nucleotides.For other compounds such as ligands
the HeuristicProcessor runs a distance-based connectivity algorithm that connects
two atoms if they are closer than a  certain threshold. The RuleBasedProcessor
uses a connectivity library containing  all molecular components present in the
PDB files on PDB.org. The library can  easily be extended with custom 
connectivity information, if required. If a :doc:`compound library <compoundlib>` is 
present, the :class:`RuleBasedProcessor` is enabled by default, otherwise the 
:class:`HeuristicProcessor` is used as a fallback.


The Processor base class
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. class:: Processor

  .. attribute:: connect

    Whether to connect atoms by bonds. Enabled by default. This also sets the
    :attr:`~ost.mol.ResidueHandle.is_protein` property of residues when peptide
    bonds are created. Turn this off if you would like to speed up the loading
    process and do not require connectivity information to be present in your
    structures.

    :type: :class:`bool`

  .. attribute:: zero_occ_treatment

    Controls the behaviour of importing atoms with zero occupancy. By default, this 
    is set to silent.

    :type: :class:`str`

  .. attribute:: check_bond_feasibility

    Whether an additional bond feasibility check is performed. Disabled by default.
    Turn this on, if you would like to connect atoms by bonds only if they are 
    within a reasonable distance.
    See also :func:`IsBondFeasible`.

    :type: :class:`bool`

  .. attribute:: assign_torsions

    Whether backbone torsions should be added to the backbone. Disabled by default.
    Set to true, to assign PHI, PSI and OMEGA torsions to the peptide residues.
    See also :func:`AssignBackboneTorsions`.

    :type: :class:`bool`


  .. method:: Process(ent)
  
    Processess the entity *ent* according to the current options.


The RuleBasedProcessor class
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. class:: RuleBasedProcessor(compound_lib)

   :param compound_lib: The compound library
   :type compound_lib: :class:`CompoundLib`
   
   The :class:`RuleBasedProcessor` implements the :class:`Processor` interface.
   Refer to its documentation for methods and accessors common to all processor.

   
  .. attribute:: strict_hydrogens

    Whether to use strict hydrogen naming rules outlined in the compound library.
    Disabled by default.

    :type: :class:`bool`

  .. attribute:: unk_atom_treatment

    Treatment upon encountering an unknown atom. Default: 'warn'.


    :type: :class:`str`

  .. attribute:: unk_res_treatment

    Treatment upon encountering an unknown residue

    :type: :class:`str`

  .. attribute:: fix_elements

    Whether the element of the atom should be changed to the atom defined in the 
    compound library. Enabled by default.

    :type: :class:`bool`


