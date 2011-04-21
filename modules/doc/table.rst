:mod:`~ost.table` - Working with tabular data
================================================================================

.. module:: ost.table
  :synopsis: Working with tabular data
  
This module defines the table class that provides convenient functionality to work with tabular data. It features functions to calculate statistical moments, e.g. mean, standard deviations as well as functionality to plot the data using matplotlib.

Basic Usage
--------------------------------------------------------------------------------

.. code-block:: python

  from ost.table import *
  # create table with two columns, x and y both of float type
  tab=Table(['x', 'y'], 'ff')
  for x in range(1000):
    tab.AddRow([x, x**2])
  # create a plot
  plt=tab.Plot('x', 'y')
  # save resulting plot to png file
  plt.savefig('x-vs-y.png')


The Table class
--------------------------------------------------------------------------------


.. autoclass:: ost.table.Table
  :members:
  :undoc-members: SUPPORTED_TYPES

.. autofunction:: ost.table.Merge