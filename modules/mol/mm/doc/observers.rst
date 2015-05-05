Observers
================================================================================

.. currentmodule:: ost.mol

Observers can be registered to a :class:`Simulation` and get called at a 
defined interval.


Trajectory Observers
--------------------------------------------------------------------------------

.. class:: TrajObserver(rythm) 

  Stores and updates the MD trajectory produced by a :class:`Simulation`
  in memory.

  :param rythm:         :class:`int` rythm, at which the observer gets
                        notified by the simulation

  .. method:: GetTraj()

    Get current Trajectory

    :returns:           :class:`ost.mol.CoordGroupHandle`

  .. method:: Rythm()

    :returns:           :class:`int` calling rythm


.. class:: TrajWriter(rythm, pdb_filename, dcd_filename)

  Writes the MD trajectory produced by a :class:`Simulation` directly to
  disk in dcd format. 

  :param rythm:         :class:`int` rythm, at which the observer gets
                        notified by the simulation

  :param pdb_filename:  :class:`str` with name of produced pdb file

  :param dcd_filename:  :class:`str` with name of produced dcd file

  .. method:: Rythm()

    :returns:           :class:`int` calling rythm

  .. method:: Finalize()

    Obligatory function to be called after the last simulation step to
    write missing information into the dcd header.

