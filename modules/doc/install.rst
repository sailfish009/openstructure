Installing OpenStructure From Source
================================================================================

.. note::

  This document describes how to install OpenStructure from source. If you are
  mainly insterested in using OpenStructure and are not planning to modify the
  code of OpenStructure itself, please use one of the binaries `available for
  download <http://www.openstructure.org/download/>`_.
  

Brief Overview
--------------------------------------------------------------------------------

Compiling OpenStructure consists of several steps that are described below in more detail. In essence, these steps are:

 * Installing the Dependencies
 * Checking out the source code from GIT
 * Configuring the build with cmake
 * Compiling an Linking
 

Installing the Dependencies
--------------------------------------------------------------------------------

OpenStructure uses a bunch of OpenSource libraries. If you haven't already installed them, please install them now! Where appropriate the minimally required version is given in parantheses.

 * `CMake <http://cmake.org>`_ (2.6.4)
 * `Eigen3 <http://eigen.tuxfamily.org>`_ (3.2.0)
 * `Boost <http://boost.org>`_ (1.53)
 * `libpng <http://www.libpng.org>`_ 
 * `Python <http://python.org>`_ (2.7)
 * `Qt <http://qt-project.org/>`_ (4.5)

When you enable support for image processing, you will need:

 * `FFTW3 <http://fftw.org>`_. By default, OpenStructure is compiled with single precision and thus also requires FFTW to be compiled with single precision. Most platforms offer this as a second package. If you are compiling manually, use the `--enable-single` option.

 * `libtiff <http://www.libtiff.org>`_



If you would like to use the graphical user interface, also install:

 * `SIP <http://www.riverbankcomputing.co.uk/software/sip/download>`_.
 * `PyQt4 <http://www.riverbankcomputing.co.uk/software/pyqt/download>`_.

In case you are compiling under Windows you have to install `Visualstudio
2008 <http://www.microsoft.com/express/Downloads>`_. to compile the dependencies 
and OpenStructure. We recommend to compile the dependecies manually. Enter the 
directories where the dependencies are located in Tools->Options->Projects and 
Solutions->VC++ directories. Choose 'bin' directories to enter program paths to 
cmake, qmake and python, 'lib' directories to point to the location(s) of your 
dependencies.



Getting the Source Code
--------------------------------------------------------------------------------


OpenStructure uses `git` as the revision control system. The main repository can be browsed `here <https://git.scicore.unibas.ch/schwede/openstructure.git>`_. To get the source code, use git clone:


.. code-block:: bash

  git clone https://git.scicore.unibas.ch/schwede/openstructure.git <directory-name>
  
The above command will clone OpenStructure into the directory called `directory-name`. If omitted, the directory will be called ost. 

.. note::

  Some versions of curl have have trouble with the certificate of the 
  OpenStructure git server and fail to clone the repository. To work around 
  this, disable the SSL certificate verification by setting the following
  environment variable:
  
  .. code-block:: bash

    export GIT_SSL_NO_VERIFY=1


Picking the right branch
--------------------------------------------------------------------------------

By default you are checking out the master branch. Master is, by definition a stable branch. It always points to the latest release. However, there are several other branches at your disposal. The main development is happening in the develop branch. It contains the newest features and bug fixes. However, we dont't make any guarantees that the develop branch is bug free and doesn't contain major bugs. After all, it's in constant flux. If you are developing new features, start your feature branch off develop. Besides that, there are several smaller features branches that are used to group together commits for one specific features. To change to a specific branch, use

.. code-block:: bash

  git checkout <branch-name>


Configuring
--------------------------------------------------------------------------------


OpenStructure uses `CMake <http://cmake.org>`_ for compiling and building the project. The next required step is to configure the build environment using cmake. You can do that by invoking `cmake` in the project directory.

.. code-block:: bash

  cmake . <options>

There are two kinds of options: Options that let you control the building behaviour, enabling and disabling the compilation of certain modules and options that let you tell CMake where to find the dependencies. All of them are passed to CMake with via `-D<opt>=<value>`.

On Windows, use Tools -> VisualStudio -> commandline prompt from within VisualStudio

Flag to choose build generator
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

CMake supports different build generators. On UNIX, i.e. MacOS X and Linux, the default build generator is Makefiles, but it is also possible to use other programs. For a list of supported build generators on your platform, start cmake without parameters. 

On Windows you have to explicitly set the build generator to "Visual Studio 9 2008"(or a later version):

.. code-block:: bash

  cmake -G"Visual Studio 9 2008"


.. _cmake-flags:

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
   
 * `COMPOUND_LIB` specifies the location of the compound library and
   activates the rule-based-builder. The compound library is based on 
   the component dictionary released by the PDB, and it specifies atoms
   of a certain residue or connectivities between atoms etc. The 
   :doc:`compound library <conop/compoundlib>` itself is created from the 
   component dictionary by calling the OpenStructure chemdict_tool. 
   By default this is switched off but it is highly recommended to provide a
   compound library to use all features of OpenStructure.

 * `COMPILE_TMTOOLS` will activate bindings for TMAlign and TMScore, which are 
   then available at python level. This option requires a Fortran compiler. 
   By default this option is switched off.

 * `USE_NUMPY` allows OpenStructure to pass back data in NumPy format. By 
   default this is switched off.

Build Options
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

 * `ENABLE_GUI` controls whether to build the graphical user interface module. By
   default it is set to true. 

 * `ENABLE_IMG` controls whether to build the image processing module. This will
   enable support for density maps, and general image processing in 1, 2 an 3
   dimensions. By default it is set to true. 

 * `ENABLE_GFX` controls whether to build the graphics module. By default, this
   is set to true. If set to none, this implies `ENABLE_UI=NO`.
   
 * Shader support is controlled with `USE_SHADER`. By default, no shaders are
   used.
   
 * If `OPTIMIZE` is set to 1, an optimized version of OpenStructure is built.

 * `PREFIX` specifies the location on the file system where to install 
   OpenStructure

 * `USE_DOUBLE_PRECISION` will switch on double precision within OpenStructure. 
   By default this is switched off.

 * `ENABLE_STATIC` allows some parts of OpenStructure to be statically linked 
   and thus can be used more easily across a heterogeneous setup, e.g. older 
   systems and newer systems.


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
  
If you repeatedly use OpenStructure, it is recommended to add /path/to/ost/stage/bin to your path.

Getting the newest changes
--------------------------------------------------------------------------------

To get the newest changes from the central git repository, enter

.. code-block:: bash

  git pull

in your terminal. This will fetch the newest changes.

