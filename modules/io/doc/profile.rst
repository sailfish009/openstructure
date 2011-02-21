IO Profiles for entity importer
================================================================================

.. currentmodule:: ost.io

As of version 1.1, OpenStructure introduces IO profiles to fine-tune the
behaviour of the molecule importers. A profile aggregates flags and methods
that affect the import of molecular structures and influence both the behaviour
of :mod:`~ost.conop` and :mod:`~ost.io`.

Basic usage of IO profiles
--------------------------------------------------------------------------------

You are most certainly reading this document because you were having trouble
loading PDB files. In that case, as a first step you will want to set the profile parameter of  :func:`LoadPDB`. The profile
parameter can either be the name of a profile or an instance of
:class:`IOProfile`. Both of the following two examples are equivalent:

.. code-block:: python

  ent=io.LoadPDB('weird.pdb', profile=io.profiles['SLOPPY'])
  ent=io.LoadPDB('weird.pdb', profile='SLOPPY')

Profiles is a dictionary-like object containing all the profiles known to OpenStructure. You can add new ones by inserting them into the dictionary. If you are loading a lot of structures, you may want to set the default profile to avoid having to pass the profile every time you load a structure. This is done by assigning a different profile to ``DEFAULT``:

.. code-block:: python

  io.profiles['DEFAULT']='SLOPPY'
  ent=io.LoadPDB('weird.pdb')

Again, you can either assign the name of the profile, or the profile itself. If none of the profiles available by default suits your needs, feel free to create one to your liking.

Available default profiles
--------------------------------------------------------------------------------

The following profiles are available by default. For a detailed description of what the different parameters mean, consult the documentation of the :class:`IOProfile` class.

STRICT

  This profile is the default and is known to work very well with PDB files
  coming from the official PDB website. It is equivalent to the following
  profile:

  .. code-block:: python

    IOProfile(dialect='PDB', strict_hydrogens=False, quack_mode=False,
              fault_tolerant=False, join_spread_atom_records=False,
              no_hetatms=False)

SLOPPY:

  This profile loads essentially everything

  .. code-block:: python

    IOProfile(dialect='PDB', strict_hydrogens=False, quack_mode=True,
              fault_tolerant=True, join_spread_atom_records=False,
              no_hetatms=False)

CHARMM:

  This format is the default when importing CHARMM trajectories and turns on the
  CHARMM specific compound dictionary.

  .. code-block:: python

    IOProfile(dialect='CHARMM', strict_hydrogens=False, quack_mode=True,
              fault_tolerant=True, join_spread_atom_records=True,
              no_hetatms=False)


The IOProfile Class
--------------------------------------------------------------------------------

.. class:: IOProfile(quack_mode=False, dialect='PDB', strict_hydrogens=False, fault_tolerant=False)

  Aggregates flags that control the import of molecular structures.

  .. attribute:: quack_mode

    :type: bool

    Read/write property. When quack_mode is enabled, the chemical class for
    unknown residues is guessed based on its atoms and connectivity. Turn this
    on, if you are working with non-standard conforming PDB files and are
    experiencing problems with the rendering of the backbone trace and/or see
    peptidic residues with unknown chemical class.

  .. attribute:: dialect

    :type: str

    The dialect to be used for PDB files. At the moment, this is either CHARMM
    or PDB. More will most likely come in the future. By setting the dialect to
    CHARMM, the loading is optimized for CHARMM PDB files. This turns on
    support for chain names with length up to 4 characters (column 72-76) and
    increase the size of the residue name to 4 residues.

  .. attribute:: strict_hydrogens

    :type: bool

    Whether hydrogen names should be strictly checked.  It is very common for
    PDB files to not follow the correct naming conventions for hydrogen atoms.
    That's why by default, the names of the hydrogens are not required to be
    correct. Rather, the connectivity is inferred with distance-based checks. By
    turning this flag on, the names of the hydrogen atoms are checked against
    the names in the database like all other atom types.
    
  .. attribute:: no_hetatms
  
    If set to true, HETATM records are ignored during import.
    
  .. attribute:: fault_tolerant

    :type: bool

    If true, the import will succeed, even if the PDB contains faulty records.
    The faulty records will be ignored and import continues as if the records
    haven't been present.
    
  .. attribute::   join_spread_atom_records
  
    If set to true, atom records belonging to the same residue are joined, even 
    if they do not appear sequentially in the PDB file.
