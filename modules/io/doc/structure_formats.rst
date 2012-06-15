Supported Structure File Formats
================================================================================

The following file formats are supported by :func:`~ost.io.LoadEntity`. 



CRD - CARD format file used by CHARMM
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
This trajectory file format is used by the CHARMM program suite (Molecular Modelling).

*Recognized File Extensions*
  crd

PDB - Brookhaven PDB File
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Fine grained control over PDB file import is available via the 
:func:`~ost.io.LoadPDB` function. The PDB importer supports loading gzipped PDB 
files, which are auto-detected by the .gz file extension.

*Recognized File Extensions*
  ent, pdb, ent.gz, pdb.gz

*Format Name*
  pdb

PQR
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

*Recognized File Extensions*
  pqr

*Format Name*
  pqr
  
SDF - Structured Data File
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

*Recognized File Extensions*
  sdf
  
