:mod:`~ost.io` - Input and Output of Sequences, Structures and Maps
================================================================================

.. module:: ost.io
  :synopsis: Input and output of sequences, structures and density maps

The io module deals with input and output of :class:`entities 
<ost.mol.EntityHandle>`, :class:`alignments <ost.seq.AlignmentHandle>`, and
:class:`images <ost.img.ImageHandle>`. Importers for common file formats such 
as PDB, SDF, FASTA, CLUSTAL W, DX and CHARMM trajectory files are available. 

Molecular Structures
--------------------------------------------------------------------------------

Loading Molecular Structures
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The :mod:`~ost.io` modules offers several ways to load molecular structures 
depending on your requirements. The most general way is offered by 
:func:`~ost.io.LoadEntity`, which will automatically detect the file format based 
on the file extension.

.. function:: LoadEntity(filename, format='auto')

  Load entity from disk. If format is set to 'auto', the function guesses the 
  filetype based on the extension of the file. Files ending in '.pdb', '.ent', 
  '.ent.gz', '.pdb.gz' will automatically be loaded as PDB files, for example. 
  For files without or exotic extensions, the format can be set explicitly as 
  the second parameter. 
  
  .. code-block:: python

    # recognizes SDF file by file extension
    ent=io.LoadEntity('file.sdf')

    # In this case, there is no file extensions, so you have to say it's a 
    # SDF file explicitly
    ent=io.LoadEntity('file', 'sdf')

  For a list of file formats supported by :func:`LoadEntity`, see :doc:`formats`.
  
  :raises: :exc:`~ost.io.IOUnknownFormatException` if the format string supplied 
      is not recognized or the file format can not be detected based on the 
      file extension
      
      :exc:`~ost.io.IOException` if the import fails due to an erroneous or 
      inexistent file

Some of the formats have a dedicated function that allows you to tweak many 
parameters that affect the import. PDB files can be loaded with 
:func:`~ost.io.LoadPDB`. It offers a tighter control over the exact loading 
behaviour.

.. autofunction:: ost.io.LoadPDB


Saving Molecular Structures
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Saving a complete entity or a view is a matter of calling 
:func:`~ost.io.SaveEntity`.

.. code-block:: python
  
  ent=io.LoadEntity('protein.pdb')
  # save full entity
  io.SaveEntity(ent, 'full.pdb')
  # only save C-alpha atoms
  io.SaveEntity(ent.Select('aname=CA and peptide=true'), 'calpha.pdb')
  
:func:`~ost.io.SavePDB` provides a simple way to save several entities into one 
file:


.. code-block:: python
  
  ent=io.LoadEntity('protein.pdb')
  # Save complete entity
  io.SavePDB(ent, 'full.pdb')
  # Save chain A and chain B separately
  io.SavePDB([ent.Select('cname=A'), ent.Select('cname=B')], 'split.pdb')


.. function:: SaveEntity(ent, filename, format='auto')
  
  Save entity to disk. If format is set to 'auto', the function guesses the 
  filetype based on the file extension, otherwise the supplied format is checked 
  against the available export plugins.
  
  :param ent: The entity to be saved
  :type  ent: :class:`~ost.mol.EntityHandle` or :class:`~ost.mol.EntityView`
  :param filename: The filename
  :type  filename: string
  :param format: Name of the format
  :type  format: string
  
  :raises: :exc:`~ost.io.IOUnknownFormatException` if the format string supplied 
      is not recognized or the file format can not be detected based on the 
      file extension
      
.. autofunction:: ost.io.SavePDB

  