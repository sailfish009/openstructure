OST Singularity
===============

Building Singularity image
--------------------------

In order to build OST Singularity image:

.. code-block:: bash

  cd <OST ROOT>/singularity
  sudo singularity build ost.img Singularity

.. note::

  Running singularity build command requires root permissions (sudo).

One can chose any name for an image. For the purose of this file we will assume
that the image name is ``ost.img``.

Here we only keep the recipe for the most recent version of OpenStructure. To
build an image for a different version, you can either adapt the
``OPENSTRUCTURE_VERSION`` variable in the recipe or look in the git history for
an older recipe.

Available apps
--------------

This container includes the following apps:
 * **OST** - OpenStructure binary
 * **IPython** - OST-powered iPython shell
 * **Notebook** - A Jupyter notebook playground with OST and nglview
 * **lDDT** - The Local Distance Difference Test
 * **Molck** - Molecular checker
 * **ChemdictTool** - Creating or update a compound library

To see the help for each individual app run:

.. code-block:: bash

    singularity run-help --app <APP NAME> <PATH TO OST IMAGE>

Eg.:

.. code-block:: bash

    singularity run-help --app OST ost.img


Facilitating the usage
----------------------

For each of these apps it is useful to create an alias if they will be
frequently used. Eg. to create an alias for IPython app one can run:

.. code-block::

  alias ost_ipython="singularity run --app IPython <PATH TO OST IMAGE>"

Then (in the same terminal window) to invoke IPython app one can just type:

.. code-block::

  ost_ipython

To make the alias permanent put it into your ``.bashrc`` file or whatever file
you use to store the aliases.
