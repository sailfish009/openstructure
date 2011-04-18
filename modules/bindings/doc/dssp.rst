:mod:`~ost.bindings.dssp` - Secondary structure assignment
================================================================================

.. module:: ost.bindings.dssp
  :synopsis: Interface to the DSSP commandline utility

Introduction
--------------------------------------------------------------------------------

DSSP is a program developed by Wolfgang Kabsch and Chris Sander to assign secondary structure states to protein structures. The assignment is based on hydrogen bonding patterns and geometric features.

The program can be downloaded from `<http://swift.cmbi.ru.nl/gv/dssp/>`_.

Example
--------------------------------------------------------------------------------

The following example assigns secondary structure states to an entity by using the DSSP program.


.. code-block:: python

  from ost.bindings import dssp
  ent=io.LoadPDB('1ake.pdb')
  dssp.AssignDSSP(ent)

DSSP bindings Usage
--------------------------------------------------------------------------------

.. autofunction:: ost.bindings.dssp.AssignDSSP

.. autofunction:: ost.bindings.dssp.LoadDSSP
