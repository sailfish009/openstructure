Installing OpenStructure From Source
================================================================================

.. note::

  This document describes how to install OpenStructure from source. If you are
  mainly insterested in using OpenStructure and are not planning to modify the
  code of OpenStructure itself, please use one of the binaries `available for
  download <http://openstructure.org/download/>`_.

Brief Overview
--------------------------------------------------------------------------------

Compiling OpenStructure consists of several steps that are described below in more detail. In essence, these steps are:

 * Installing the Dependencies
 * Checking out the source code from SVN
 * Configuring the build with cmake
 * Compiling an Linking
 

.. installdeps_

Installing the Dependencies
--------------------------------------------------------------------------------

OpenStructure uses a bunch of OpenSource libraries. If you haven't already installed them, please install them now! Where appropriate the minimally required version is given in parantheses.

 * `CMake <http://cmake.org>`_ (2.6.4)
 * `Eigen2 <http://eigen.tuxfamily.org>`_ (2.0.6)
 * `Boost <http://boost.org>`_ (1.37)
 * `libpng <http://www.libpng.org>`_ 
 * `Python <http://python.org>`_ (2.4)
 * `Qt <http://qt.nokia.com>`_ (4.5)

When you enable support for image processing, you will need:

 * `FFTW3 <http://fftw.org>`_. By default, OpenStructure is compiled with single precision and thus also requires FFTW to be compiled with single precision. Most platforms offer this as a second package. If you are compiling manually, use the `--enable-single` option.

 * `libtiff <http://www.libtiff.org>`_



If you would like to use the graphical user interface, also install:

 * `SIP <http://www.riverbankcomputing.co.uk/software/sip/download>`_.
 * `PyQt4 <http://www.riverbankcomputing.co.uk/software/pyqt/download>`_.

In case you are compiling under Windows you have to install `Visualstudio
2008 <http://www.microsoft.com/express/Downloads>`_. to compile the dependecies 
and OpenStructure. We recommend to compile the dependecies manually. Enter the 
directories where the dependencies are located in Tools->Options->Projects and 
Solutions->VC++ directories. Choose 'bin' directories to enter program paths to 
cmake, qmake and python, 'lib' directories to point to the location(s) of your 
dependencies.

Checking out the Source
--------------------------------------------------------------------------------

You can checkout the source from SVN. The repository is located at

   https://dng.biozentrum.unibas.ch/svn/openstructure/trunk

If you are using the commandline client, type in your shell 

.. code-block:: bash

   svn co https://dng.biozentrum.unibas.ch/svn/openstructure/trunk

On Windows, we recommend to install a graphical frontend for svn, for example `tortoisesvn <http://tortoisesvn.tigris.org>`_. 


Configuring
--------------------------------------------------------------------------------


OpenStructure uses `CMake <http://cmake.org>`_ for compiling and building the project. The next required step is to configure the build environment using cmake. You can do that by invoking `cmake` in the project directory.

.. code-block:: bash

  cmake . <options>

There are two kinds of options: Options that let you control the building behaviour, enabling and disabling the compilation of certain modules and options that let you tell CMake where to find the dependencies. All of them are passed to CMake with via `-D<opt>=<value>`.

On Windows, use Tools -> VisualStudio commandline prompt from within VisualStudio)

Flag to choose build generator
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

CMake supports different build generators. On UNIX, that is MacOS X and Linux, the default build generator is Makefiles, but it is also possible to use other programs. For a list of supported build generators on your platform, start cmake without parameters. 

On Windows you have to explicitly set the buil generator to "Visual Studio 9 2008":

.. code-block:: bash

  cmake -G"Visual Studio 9 2008"


Flags to Control the Dependencies
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

By default, `CMake <http://cmake.org>`_ searches the standard directories for dependencies. However, on some systems, this might not be enough. Here is a short description of how CMake figures out what dependencies to take and how you can influence it.

 * Boost is mainly controlled via the `BOOST_ROOT` option. If boost wasn't
   found, it should be set to the prefix of the boost installation.

 * `QT_QMAKE_EXECUTABLE` defines the exact Qt installation to take. It should 
   be set to the full path to `qmake`.
 
 * `PYTHON_ROOT` is the Python equivalent of BOOST_ROOT. It should be set to 
   the prefix path containing the python binary, headers and libraries.

 * `SYS_ROOT` controls the general prefix for searching libraries and headers.
   By default, it is set to `/`.

Build Options
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

 * `ENABLE_UI` controls whether to build the graphical user interface module. By
   default it is set to true. 
 * `ENABLE_IMG` controls whether to build the image processing module. This will
   enable support for density maps, and general image processing in 1, 2 an 3
   dimensions. By default it is set to true. 

 * `ENABLE_GFX` controls whether to build the graphics module. By default, this
   is set to true. If set to none, this implies `ENABLE_UI=NO`.
   
 * Shader support is controlled with `USE_SHADER`. By default, no shaders are
   used.
   
 * If `OPTIMIZE` is set to 1, an optimized version of OpenStructure is built.

Example Configurations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Ubuntu 10.04 Lucid/Lynx**

All the dependencies can be installed from the package manager and are thus located in standard locations. cmake will automatically find them without the need to pass any additional parameters. The only exception is -DOPTIMIZE, which will tell cmake to build an optimized (-O3 -DNDEBUG) version of OpenStructure.

.. code-block:: bash

  cmake . -DOPTIMIZE=1

**MacOS X with MacPorts and optimization turned on**

MacPorts installs all the software under /opt/local. Thus we have to tell cmake where to find Boost, Python and Qt.

.. code-block:: bash
  
  cmake . -DBOOST_ROOT=/opt/local -DPYTHON_ROOT=/opt/local \
        -DSYS_ROOT=/opt/local -DQT_QMAKE_EXECUTABLE=/opt/local/bin/qmake \
        -DOPTIMIZE=1


Building the Project
--------------------------------------------------------------------------------

Type `make`. If you are using a multi-core machine, you can use the `-j` flag to 
run multiple jobs at once.

On Windows run 'Build OpenStructure' from the build menu.



What's next?
--------------------------------------------------------------------------------

On Linux and MacOS X, you can start dng from the command-line. The binaries are all located in stage/bin:

.. code-block:: bash

  stage/bin/dng
  
or, to start the command-line interpreter:

.. code-block:: bash

  stage/bin/ost
  
If you repeatedly use OpenStructure, it is recommended to add /path/to/dng/stage/bin to your path.
