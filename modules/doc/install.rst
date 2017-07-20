Installing OpenStructure From Source
================================================================================

.. note::

  This document describes how to install OpenStructure from source. If you are
  mainly insterested in using OpenStructure and are not planning to modify the
  code of OpenStructure itself, please use one of the binaries `available for
  download <http://www.openstructure.org/download/>`_.
  

Brief Overview
--------------------------------------------------------------------------------

Compiling OpenStructure consists of several steps that are described below in
more detail. In essence, these steps are:

* Installing the Dependencies
* Checking out the source code from GIT
* Configuring the build with cmake
* Compiling an Linking
 

Installing the Dependencies
--------------------------------------------------------------------------------

OpenStructure uses a bunch of OpenSource libraries. If you haven't already
installed them, please install them now! Where appropriate the minimally
required version is given in parentheses.

* `CMake <http://cmake.org>`_ (2.6.4)
* `Python2 <http://python.org>`_ (2.7)
* `Boost <http://boost.org>`_ (1.53)
* `zlib <https://zlib.net/>`_ (usually comes with Boost or system)
* `Eigen3 <http://eigen.tuxfamily.org>`_ (3.2.0)

When you enable support for image processing, you will need:

* `FFTW3 <http://fftw.org>`_. By default, OpenStructure is compiled with single
  precision and thus also requires FFTW to be compiled with single precision.
  Most platforms offer this as a second package. If you are compiling manually,
  use the `--enable-single` option.
* `libtiff <http://www.libtiff.org>`_
* `libpng <http://www.libpng.org>`_ (also needed for GUI)

If you would like to use the info module, also install:

* `Qt4 <http://qt-project.org/>`_ (4.5, Qt5 is not supported yet)

If you would like to use the graphical user interface (GUI), also install:

* `SIP <http://www.riverbankcomputing.co.uk/software/sip/download>`_
* `PyQt4 <http://www.riverbankcomputing.co.uk/software/pyqt/download>`_

If you would like to use the :mod:`molecular mechanics <ost.mol.mm>` module:

* `OpenMM <https://simtk.org/home/openmm>`_ (6.1)

In case you are compiling under Windows you have to install `Visualstudio
2008 <http://www.microsoft.com/express/Downloads>`_. to compile the dependencies 
and OpenStructure. We recommend to compile the dependencies manually. Enter the 
directories where the dependencies are located in Tools->Options->Projects and 
Solutions->VC++ directories. Choose 'bin' directories to enter program paths to 
cmake, qmake and python, 'lib' directories to point to the location(s) of your 
dependencies.



Getting the Source Code
--------------------------------------------------------------------------------

OpenStructure uses `git` as the revision control system. The main repository can
be browsed `here <https://git.scicore.unibas.ch/schwede/openstructure.git>`_. To
get the source code, use git clone:

.. code-block:: bash

  git clone https://git.scicore.unibas.ch/schwede/openstructure.git <directory-name>
  
The above command will clone OpenStructure into the directory specified by
`<directory-name>`. If omitted, the directory will be called ost. 

.. note::

  Some versions of curl have have trouble with the certificate of the 
  OpenStructure git server and fail to clone the repository. To work around 
  this, disable the SSL certificate verification by setting the following
  environment variable:
  
  .. code-block:: bash

    export GIT_SSL_NO_VERIFY=1


Picking the right branch
--------------------------------------------------------------------------------

By default you are checking out the master branch. Master is, by definition a
stable branch. It always points to the latest release. However, there are
several other branches at your disposal. The main development is happening in
the develop branch. It contains the newest features and bug fixes. However, we
don't make any guarantees that the develop branch is bug free and doesn't
contain major bugs. After all, it's in constant flux. If you are developing new
features, start your feature branch off develop. Besides that, there are several
smaller features branches that are used to group together commits for one
specific features. To change to a specific branch, use

.. code-block:: bash

  git checkout <branch-name>


Configuring
--------------------------------------------------------------------------------


OpenStructure uses `CMake <http://cmake.org>`_ for compiling and building the
project. The next required step is to configure the build environment using
cmake. You can do that by invoking `cmake` in the project directory.

.. code-block:: bash

  cmake . <options>

There are two kinds of options: Options that let you control the building
behaviour, enabling and disabling the compilation of certain modules and options
that let you tell CMake where to find the dependencies. All of them are passed
to CMake with via `-D<opt>=<value>`.

On Windows, use Tools -> VisualStudio -> commandline prompt from within
VisualStudio.

Flag to choose build generator
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

CMake supports different build generators. On UNIX, i.e. MacOS X and Linux, the
default build generator is Makefiles, but it is also possible to use other
programs. For a list of supported build generators on your platform, start cmake
without parameters.

On Windows you have to explicitly set the build generator to
"Visual Studio 9 2008" (or a later version):

.. code-block:: bash

  cmake -G"Visual Studio 9 2008"


.. _cmake-flags:

Flags to Control the Dependencies
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

By default, `CMake <http://cmake.org>`_ searches the standard directories for
dependencies. However, on some systems, this might not be enough. Here is a
short description of how CMake figures out what dependencies to take and how you
can influence it.

* Boost is mainly controlled via the `BOOST_ROOT` option. If boost wasn't
  found, it should be set to the prefix of the boost installation. If for some
  reason, it is desirable to use the non-multithreaded boost libraries, you can
  switch `Boost_USE_MULTITHREADED` on (it is off by default).

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

* `ENABLE_GUI` controls whether to build the graphical user interface module.
  By default, this is switched on.

* `ENABLE_GFX` controls whether to build the graphics module. By default, this
  is switched on. If it is switched off, it also switches `ENABLE_GUI` off.

* `ENABLE_INFO` controls whether to build the info module. By default, this is
  switched on. If it is switched off, it also switches `ENABLE_GFX` off and
  removes all dependencies to Qt.

* `QT_QMAKE_EXECUTABLE` defines the exact Qt installation to take. It should 
  be set to the full path to `qmake`. This is only needed if `ENABLE_INFO` is
  switched on.

* `COMPILE_TMTOOLS` will activate bindings for TMAlign and TMScore, which are 
  then available at python level. This option requires a Fortran compiler. 
  By default, this option is switched off.

* `USE_NUMPY` allows OpenStructure to pass back data in NumPy format. By 
  default, this is switched off.

* `ENABLE_IMG` controls whether to build the image processing module. This will
  enable support for density maps, and general image processing in 1, 2 an 3
  dimensions. By default, this is switched on.

* `ENABLE_MM` controls whether the molecular mechanics module is enabled. By
  default, this is switched off. If it is turned on, you should also set the
  paths to your local OpenMM installation:

  * `OPEN_MM_INCLUDE_DIR`: the include path
  * `OPEN_MM_LIBRARY`: the libOpenMM library
  * `OPEN_MM_PLUGIN_DIR`: the path for OpenMM plugins
  * see example below for commonly used paths

* Several paths to other libraries can be set if they are not in the expected
  locations:

  * `PYTHON_LIBRARIES` defines the location of the Python library (file name
    starting with `libpython`). This must be set if it is not in
    `$PYTHON_ROOT/lib`.
  * `EIGEN3_INCLUDE_DIR` defines the include folder of Eigen3 (contains `Eigen`
    folder with include files).
  * `FFTW_LIBRARY` defines the location of the FFTW3 library (file name starting
    with `libfftw3f` (or `libfftw3` if `USE_DOUBLE_PRECISION` is switched on))
  * `FFTW_INCLUDE_DIR` defines the include folder of FFTW3 (contains include
    files directly)
  * `PNG_LIBRARY` defines the location of the libpng library (file name starting
    with `libpng`)
  * `PNG_INCLUDE_DIR` defines the include folder of libpng (contains include
    files directly)
  * `ZLIB_LIBRARY` defines the location of the zlib library (file name starting
    with `libz`)
  * `ZLIB_INCLUDE_DIR` defines the include folder of zlib (contains include
    files directly)
  * `TIFF_LIBRARY` defines the location of the libtiff library (file name
    starting with `libtiff`)
  * `TIFF_INCLUDE_DIR` defines the include folder of libtiff (contains include
    files directly)
  * Usually, you will receive errors for those variables when executing `cmake`
    and set them accordingly as needed.

  
Build Options
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* `OPTIMIZE` can be switched on to build an optimized (-O3 -DNDEBUG) version of
  OpenStructure. By default, this is switched off.

* `USE_DOUBLE_PRECISION` will switch on double precision within OpenStructure. 
  By default, this is switched off.

* `ENABLE_STATIC` allows some parts of OpenStructure to be statically linked 
  and thus can be used more easily across a heterogeneous setup, e.g. older 
  systems and newer systems. By default, this is switched off.

* For deployment of OpenStructure with `make install` there are two relevant
  settings to consider:

  * `PREFIX` or `CMAKE_INSTALL_PREFIX` are used to define the path where the
    OpenStructure `stage` folder will be installed to.
  * `USE_RPATH` can be switched on to embed rpath upon make install. By default,
    this option is switched off.

* Experimental settings (only change if you know what you are doing):

  * `USE_MESA` to use software rendered Mesa instead of hardware GL. By default,
    this is turned off.
  * `USE_SHADER` controls whether to compile with shader support. By default,
    this is turned off.
  * `ENABLE_SPNAV` controls whether 3DConnexion devices should be supported. By
    default, this is turned off.
  * `PROFILE` can be switched on to enable a (very verbose) code profiler. By
    default, this is turned off.
  * `UBUNTU_LAYOUT` can be turned on to switch the directory layout of the
    `stage` folder to be more ubuntu-like. By default, this is switched off.
  * `HIDDEN_VISIBILITY` can be turned on to add "-fvisibility=hidden" to gcc's
    compile flags (only if GNU compiler used). By default, this is switched off.


Example Configurations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Generic linux without GUI**

The simplest way to compile OpenStructure is to disable the GUI and any
dependency to Qt4. You can build an optimized OpenStructure without GUI as
follows:

.. code-block:: bash

  cmake . -DOPTIMIZE=ON -DENABLE_INFO=OFF

The molecular mechanics module can be enabled by installing OpenMM and adding
the appropriate flags as follows (replace `<OPENMM>` with the actual path to
OpenMM):

.. code-block:: bash

  cmake . -DOPTIMIZE=ON -DENABLE_INFO=OFF -DENABLE_MM=ON \
          -DOPEN_MM_LIBRARY=<OPENMM>/lib/libOpenMM.so \
          -DOPEN_MM_INCLUDE_DIR=<OPENMM>/include/ \
          -DOPEN_MM_PLUGIN_DIR=<OPENMM>/lib/plugins

Note that the OpenMM binaries available online may be incompatible with files
compiled using your gcc compiler (known as "Dual ABI" issue). This has been
observed for OpenMM versions 6.1 until 7.1.1 when compiling with gcc versions >=
5.1. In those cases, you cannot use the binaries and will have to install OpenMM
from source.

On some Linux distributions, there are issues with Qt4 and hence it may not be
possible to build OpenStructure with GUI support at all. This is for instance
known to be an issue with boost versions >= 1.62.

An additional problem arises for gcc versions >= 6. There an extra flag is
required to use the C++98 standard:

.. code-block:: bash

  cmake . -DOPTIMIZE=ON -DENABLE_INFO=OFF -DCMAKE_CXX_FLAGS='-std=c++98'

We hope to support Qt5 and C++11 in the next OpenStructure release.


**Ubuntu 16.04 with GUI**

All the dependencies can be installed from the package manager as follows:

.. code-block:: bash

  sudo apt-get install cmake sip-dev libtiff-dev libfftw3-dev libeigen3-dev \
               libpng-dev python-all python2.7 python-qt4 libboost-all-dev \
               qt4-qtconfig qt4-qmake libqt4-dev libpng-dev

Now, all dependencies are located in standard locations and cmake will
automatically find them without the need to pass any additional parameters. The
only exception is the Python library which is put in a different path than
expected. Also, we add -DOPTIMIZE, which will tell cmake to build an optimized
version of OpenStructure.

.. code-block:: bash

  cmake . -DPYTHON_LIBRARIES=/usr/lib/x86_64-linux-gnu/libpython2.7.so \
          -DOPTIMIZE=ON


**Fedora 26 without GUI**

All the dependencies can be installed from the package manager as follows:

.. code-block:: bash

  sudo dnf install cmake eigen3-devel boost-devel libpng-devel python2-devel \
                   fftw-devel libtiff-devel

Fedora 26 has gcc 7 and boost 1.63 by default. Hence, we will need to disable
Qt4, the GUI and add the extra flag described above:

.. code-block:: bash

  cmake . -DOPTIMIZE=ON -DENABLE_INFO=OFF -DCMAKE_CXX_FLAGS='-std=c++98'


**macOS with Homebrew without GUI**

`Homebrew <https://brew.sh/>`_ can be used to conveniently install all packages
on macOS. Unfortunately, Qt4 is not (officially) supported on macOS Sierra (and
newer). Hence, it is not possible to build OpenStructure with GUI support there.
Homebrew installs all the software under /usr/local. Thus we have to tell cmake
where to find Boost and Python. Also the Python headers and libraries are not
located as they are on linux and hence they must be specified too:

.. code-block:: bash
  
  cmake . -DPYTHON_INCLUDE_PATH=/usr/local/opt/python/Frameworks/Python.framework/Headers \
          -DPYTHON_LIBRARIES=/usr/local/opt/python/Frameworks/Python.framework/Python \
          -DBOOST_ROOT=/usr/local -DPYTHON_ROOT=/usr/local \
          -DSYS_ROOT=/usr/local -DENABLE_INFO=OFF -DOPTIMIZE=ON


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

