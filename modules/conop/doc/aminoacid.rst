Functions and classes for standard amino acids
================================================================================

.. currentmodule:: ost.conop

This document describes functions and classes to work with the 20 standard amino acids. The functions convert between different representations, e.g. one-letter-code, three-letter-code or the AminoAcid *enum*


The AminoAcid enum
--------------------------------------------------------------------------------

The amino acid enum enumerates all 20 standard amino acid and the special value 
XXX, to signify an unknown amino acid. The amino acid enum supports the 
following values:

  ALA, ARG, ASN, ASP, GLN, GLU, LYS, SER, CYS, MET
  TRP, TYR, THR, VAL, ILE, LEU, GLY, PRO, HIS, PHE
  XXX

Converter functions
--------------------------------------------------------------------------------
.. function:: ResidueToAminoAcid(residue)
              ResidueNameToAminoAcid(rname) 
              OneLetterCodeNameToAminoAcid(olc) 

  Get amino acid from residue, residue name (three-letter-code)
  or one-letter-code. Returns XXX if residue, residue name or 
  one-letter-code is not one of the 20 standard amino acids.

.. function:: OneLetterCodeToResidueName(olc)
              AminoAcidToResidueName(amino_acid)

  Get residue name from one-letter-code or amino_acid. For invalid 
  one-letter-codes or XXX, 'UNK' is returned.

.. function:: ResidueNameToOneLetterCode(rname)

  Get one-letter-code for the given residue name. Returns 'X' if residue name is 
  not one of the 20 standard amino acids.

.. class:: AminoAcidSet

  A set of amino acids, with constant-time access
  
  .. staticmethod:: CreatePolarSet()
                    CreateAromaticSet()
                    CreateApolarSet()

    Returns a set containing all polar, aromatic or apolar amino acids, 
    respectively.
  


  .. method:: Add(amino_acid)
  
    Add amino acid to the set.
    
  .. method:: Remove(amino_acid)
  
    Remove amino acid from the set
    
  .. method Contains(amino_acid)
  
    Whether the set contains the given amino acid. 
    
  .. method:: Empty()
  
    Whether the set is empty, i.e. doesn't contain any amino acids.


Mapping functions
--------------------------------------------------------------------------------

The following functions help to convert one residue into another by reusing as
much as possible from the present atoms. They are mainly meant to map from
standard amino acid to other standard amino acids or from modified amino acids
to standard amino acids.

.. function:: CopyResidue(src_res, dst_res, editor)

  Copies the atoms of ``src_res`` to ``dst_res`` using the residue names
  as guide to decide which of the atoms should be copied. If ``src_res`` and
  ``dst_res`` have the same name, or ``src_res`` is a modified version of
  ``dst_res`` (i.e. have the same single letter code), CopyConserved will be
  called, otherwise CopyNonConserved will be called.

  :param src_res: The source residue
  :type src_res: :class:`~mol.ResidueHandle`
  :param dst_res: The destination residue
  :type dst_res: :class:`~mol.ResidueHandle`

  :returns: true if the residue could be copied, false if not.

.. function:: CopyConserved(src_res, dst_res, editor)

  Copies the atoms of ``src_res`` to ``dst_res`` assuming that the parent
  amino acid of ``src_res`` (or ``src_res`` itself) are identical to ``dst_res``.

  If ``src_res`` and ``dst_res`` are identical, all heavy atoms are copied
  to ``dst_res``. If ``src_res`` is a modified version of ``dst_res`` and the
  modification is a pure addition (e.g. the phosphate group of phosphoserine),
  the modification is stripped off and all other heavy atoms are copied to
  ``dst_res``. If the modification is not a pure addition, only the backbone
  heavy atoms are copied to ``dst_res``.

  Additionally, the selenium atom of ``MSE`` is converted to sulphur.

  :param src_res: The source residue
  :type src_res: :class:`~mol.ResidueHandle`
  :param dst_res: The destination residue
  :type dst_res: :class:`~mol.ResidueHandle`

  :returns: a tuple of bools stating whether the residue could be copied and
    whether the Cbeta atom was inserted into the ``dst_res``.

.. function:: CopyNonConserved(src_res, dst_res, editor)

  Copies the heavy backbone atoms and Cbeta (except for ``GLY``) of ``src_res``
  to ``dst_res``.

  :param src_res: The source residue
  :type src_res: :class:`~mol.ResidueHandle`
  :param dst_res: The destination residue
  :type dst_res: :class:`~mol.ResidueHandle`

  :returns: a tuple of bools stating whether the residue could be copied and
    whether the Cbeta atom was inserted into the ``dst_res``.
