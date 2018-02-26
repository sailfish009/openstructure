OST Docker
==========

.. note::

  In order to run docker commands there might be a need to run them as root (sudo).

Build Docker image
------------------

In order to build OST image:

.. code-block:: bash

  cd <OST ROOT>/docker
  docker build --tag <TAG> .

One can chose any tag eg. ost.

Run script with OST
-------------------

In order to run OST script do:

.. code-block:: bash

  docker run [DOCKER OPTIONS] --rm -v <PATH TO SCRIPT DIR>:/home <IMAGE NAME> /home/<SCRIPT NAME> [SCRIPT OPTIONS]

.. warning::

  If script requires some external files eg. PDBs, they have to be located in the
  path accessible via mounted volume and should be accessed via docker (NOT LOCAL)
  path. Eg. assuming that we have a struc.pdb file in /home/user/pdbs directory and
  a script.py in /home/user we could mount the /home/user to /home in docker as
  above by specifying -v /home/user:/home. To run the script we thus need to
  provide the (relative) path to the script and (relative) path to the file eg:

  .. code-block:: bash

    docker run --rm -v /home/user:/home <IMAGE NAME> script.py pdbs/struct.pdb

  or with absolute paths:

  .. code-block:: bash

    docker run --rm -v /home/user:/home <IMAGE NAME> home/script.py /home/pdbs/struct.pdb

One can find a exemplary script (`test_docker.py`) in the <OST ROOT>/docker
directory. To run it do:

.. code-block::

  cd <OST ROOT>/docker
  docker run --rm -v <OST ROOT>/docker:/home <IMAGE NAME> test_docker.py

Run script with utility command
###############################

One can also use provided utility bash script `run_docker_ost` to run basic
scripts:

.. code-block:: bash

  <OST ROOT>/docker/run_docker_ost <IMAGE_NAME> [<SCRIPT_PATH>] [SCRIPT OPTIONS]

One just needs to provide image name and optionally a script and its options. It
is useful to link the command to the binary directory eg. in linux:

.. code-block:: bash

  ln -s <OST ROOT>/docker/run_docker_ost /usr/bin/run_docker_ost

In order to run an exemplary script (`test_docker.py`) do:

.. code-block::

  cd <OST ROOT>/docker
  ./run_docker_ost <IMAGE NAME> test_docker.py

Run GUI
-------

In order to run GUI do (tested on linux machine):

.. code-block:: bash

  xhost +local:docker
  docker run -ti --rm -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix --entrypoint dng <IMAGE NAME>

Running other commands
----------------------

The default entrypoint of the Docker image is "ost" thus in order to run other
available commands (or other commands in general) one need to override
the entrypoint:

.. code-block::

  sudo docker run --rm -ti --entrypoint <COMMAND> <IMAGE NAME> [COMMAND OPTIONS]

Eg. to run molck type:

.. code-block::

  sudo docker run --rm -ti --entrypoint molck <IMAGE NAME> --help

.. note::

  Note how the options to the command are specified after the image name.
