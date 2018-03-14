OST Singularity
===============

.. note::

  In order to run some singularity commands there might be a need to run them as root (sudo).

Build Singularity image
-----------------------

In order to build OST Singularity image:

.. code-block:: bash

  cd <OST ROOT>/singularity
  singularity build ost.img Singularity.1.7.1

One can chose any name for an image. For the purose of this file we will assume
that the image name is `ost.img`.

Available apps
--------------

OST
===

The OST app exposes OpenStructure binary and can be used to run interactive shell
and scripts.

To invoke OST shell run:

.. code-block:: bash
  singularity run --app OST ost.img

In order to run OST script do:

.. code-block:: bash
  singularity run --app OST ost.img <SCRIPT NAME> [SCRIPT OPTIONS]

.. warning::

  If script requires some external files eg. PDBs, they have to be located in the
  path accessible via mounted volumes. By default Singularity mounts $HOME and
  goes to CWD. Thus this sould work as expected out of the box.

One can find a exemplary script (`test_docker.py`) in the <OST ROOT>/docker
directory. To run it do:

.. code-block::

  cd <OST ROOT>/singularity
  singularity run --app OST ost.img ../docker/test_docker.py

lDDT
====

This app runs the Local Distance Test. To see more details run:

.. code-block::

  cd <OST ROOT>/singularity
  singularity run --app lDDT ost.img

Molck
====

This app runs the Local Distance Test. To see more details run:

.. code-block::

  cd <OST ROOT>/singularity
  singularity run --app Molck ost.img

Notebook
========

Probably the best way to play with OpenStructure is to run it wihtin the Jupyter
notebook. In order to so run:

.. code-block::

  cd <OST ROOT>/singularity
  singularity run --app Notebook ost.img

Copy the URL to the browser and launch the notebook with OST kernel. This will
load all necessary OST components just like in the OST shell. We also enabled
the nglview widget to interactively view molecular structures and trajectories.
For more details on how to use nglview see `the documentation 
<http://nglviewer.org/nglview/latest/>`_.


.. warning::

  As the Singularity mounts $HOME by default Jupyter and Ipython config files
  are moved to separate directories. Proper environmental variables are also set.
  In addition, Jupyter is run in a separate virtualenv to not interact with possibly
  installed host version.
