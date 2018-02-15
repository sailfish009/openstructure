OST Docker
==========

Build Docker image
------------------

In order to build OST image:

.. code-block::
    cd <OST ROOT>/docker
    (sudo) docker build --tag <TAG> .

One can chose any tag eg. ost.

Run script with OST
-------------------

In order to run OST script do:

.. code-block::
    (sudo) docker run --rm -v <PATH TO SCRIPT DIR>:/home <IMAGE NAME> /home/<SCRIPT NAME> [OPTIONS]

Run GUI
-------

In order to run GUI do (tested on linux machine):

.. code-block::
    xhost +local:docker
    (sudo) docker run -ti --rm -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix --entrypoint dng <IMAGE NAME>
