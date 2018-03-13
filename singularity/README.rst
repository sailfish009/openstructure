OST Singularity
===============

.. note::

  In order to run some singularity commands there might be a need to run them as root (sudo).

Build Singularity image
------------------

In order to build OST Singularity image:

.. code-block:: bash

  cd <OST ROOT>/singularity
  singularity build ost.img Singularity.1.7.1

One can chose any name for an image. For the purose of this file we will assume
that the image name is `ost.img`.

Run script using OST
--------------------

In order to run OST script do:

.. code-block:: bash
  singularity run --app OST ost.img <SCRIPT NAME> [SCRIPT OPTIONS]

.. warning::

  If script requires some external files eg. PDBs, they have to be located in the
  path accessible via mounted volume. By default Singularity mounts $HOME and
  goes to CWD. Thus this sould work as expected.

One can find a exemplary script (`test_docker.py`) in the <OST ROOT>/docker
directory. To run it do:

.. code-block::

  cd <OST ROOT>/singularity
  singularity run --app OST ost.img ../docker/test_docker.py

Available apps
--------------

TODO
