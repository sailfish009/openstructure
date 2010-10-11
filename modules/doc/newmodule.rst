Creating a New Module
================================================================================

OpenStructure can be extended by writing additional modules. A module will 
usually consist of a set of C++ classes and methods, most of which will also be 
exported to Python. It is also possible to write modules completely in Python.

The build system of OpenStructure is quite simple. The main difference to other 
projects is the use of a so-called stage  directory. The stage directory 
replicates the normal layout of a standard Linux directory structure, with an 
'include' directory for the headers, a 'lib' directory containing the shared 
library files, a `bin` directory  for the executables and a 'share' directory 
for the platform-independent data like icons, images and examples.

OpenStructure uses `CMake <http://www.cmake.org>`_ to build the project. The 
rules for the build-system are defined in `CMakeLists.txt` files. When running 
`cmake`, the files are  compiled and copied into stage. The 
real installation, if necessary, happens at a later stage. This is referred to 
as staging of the files.

If a new module is written following the guidelines in this page, it will be
seamlessly included in the build system and will then be available form both
the DNG python console and the OpenStructure command line as any other native 
module.

As a first step, a new directory structure must be created to accommodate the 
new module.

Directory Structure
--------------------------------------------------------------------------------

For the purpose of this example, let's assume we are creating a new module 
called 'mod' (for 'modeling'). Let's create a directory named `mod` under the 
'modules' directory in the OpenStructure development tree, and  populate it with 
the three subdirectories `src`, `pymod`, and `tests`.  Then we add a 
`CMakeLists.txt` file in the 'mod' directory, consisting of three lines:

.. code-block:: bash

  add_subdirectory(src)
  add_subdirectory(pymod)
  add_subdirectory(tests)
  
The Module Code
--------------------------------------------------------------------------------

In the `src` subdirectory we put the code that implements the functionality of 
the new module, plus a `config.hh` header file.

Here is a skeleton of one of the files in  the directory , `modeling_new_class.hh`:

.. code-block:: cpp 

  #ifndef OST_MOD_NEW_CLASS_H
  #define OST_MOD_NEW_CLASS_H
  
  #include <ost/mod/module_config.hh>
  
  // All other necessary includes go here
  
  namespace ost { namespace mod {
  
  class DLLEXPORT_OST_MOD NewClass {
   public:
     void NewMethod();
           
    // All declarations of NewClass go here 
  
  };
  
  }} // namespaces
  
  #endif
  
And here is the skeleton of the corresponding `modeling_new_class.cc` file:

.. code-block:: cpp

  #include "modeling_new_class.hh"
  
  using namespace ost::mol;
  using namespace ost::mod;
  
  // All other necessary includes and namespace directives
  // go here
  
  void NewClass::NewMethod():
  {
    // Implementation     
  }
  
  // Implementation code for NewClass goes here
  
Obviously, the `src` directory can contain many files, each implementing classes
and functions that will end up in the module. In order to build and stage
the module shared library, a `CMakeLists.txt` file needs to be written for the
`src` directory:

.. code-block:: bash

  set(OST_MOD_SOURCES 
  modeling_new_class.cc
  // All other source files 
  )
  
  set(OST_MOD_HEADERS
  modeling_new_class.hh
  // All other header files
  )
  
   module(NAME mod SOURCES "${OST_MOD_SOURCES}"
          HEADERS ${OST_MOD_HEADERS}
          DEPENDS_ON mol mol_alg)
  

The line containing the `DEPENDS_ON` directive lists all the modules on which
the new module depends (in this case :mod:`mol` and :mod:`ost.mol.alg`).  The 
`module` macro will take care of staging the headers, in this case into 
`ost/mod` and compiling, linking and staging of a library with the  name 
`libost_mod.so` (`libost_gmod.dylib` on MacOS X).

.. note:: 

  Due to a limitation in the built-int install command of CMake, for modules
  that have their headers in several directories, it is required to group the
  headers by directory, leading to a call of module like:

.. code-block:: bash

  module(NAME mol SOURCES atom_handle.cc impl/atom_impl.cc
         HEADERS atom_impl.hh IN_DIR impl
         atom_handle.hh)   

The `module_config.hh` header is required for each module to setup the 
environment on Windows: Each public class, method and function needs to marked 
with `DLLEXPORT` or `DLLIMPORT` to teach the linker where to look for the 
symbol. The correct use of either `DLLIMPORT` and `DLLEXPORT` is depending on 
the context: While compiling a header file that is part of the same shared
library, `DLLEXPORT` must be used. When compiling a header that is part of
an external shared library, `DLLIMPORT` must be used. A typical module_config
header looks like this:

.. code-block:: cpp   

  #ifndef OST_MOD_MODULE_CONFIG_HH
  #define OST_MOD_MODULE_CONFIG_HH
  
  #include <ost/base.hh>
  
  #if defined(OST_MODULE_OST_MOD)
  #  define DLLEXPORT_OST_MOD DLLEXPORT
  #else
  #  define DLLEXPORT_OST_MOD DLLIMPORT
  #endif
  #endif
      
The Testing Framework
--------------------------------------------------------------------------------

The `tests` directory contains code for unit tests. The code is compiled and 
executed when one invokes compilation using the 'make check' command.  Tests are 
run by means of the `Boost Unitests Library 
<http://www.boost.org/doc/libs/1_37_0/libs/test/doc/html/index.html>`_, which is 
used throughout OpenStructure. Before coding the test routines, the required 
skeleton needs to be put in place.

The main code is put into 'tests.cc', which will become the test executable:
    
.. code-block:: cpp

  #include <boost/test/unit_test.hpp>
  using boost::unit_test_framework::test_suite;
  
  #include "test_modeling.hh"
  
  test_suite*
  unit_unit_test_suite( int argc, char * argv[] ) {
    std::auto_ptr<test_suite> test(BOOST_TEST_SUITE( "Module Mod Test" ));
  
    test->add(CreateModelingTest()); 
  
    return test.release(); 
  }
  
        
The most relevant line adds the test suite for the new module to the global test 
list. The test suite is created by the factory function CreateModelingTest(), 
which is declared in the `test_modeling.hh` header file. 

.. code-block:: cpp

  #ifndef OST_MOD_TEST_MODELING_H
  #define OST_MOD_TEST_MODELING_H
  
  #include <boost/test/unit_test.hpp>
  using boost::unit_test_framework::test_suite;
  
  test_suite* CreateModelingTest();
  
  #endif
      
The definition of the factory function is found in the actual test code,
which we put in `test_modeling.cc`. Here is a skeleton version of that file:

.. code-block:: cpp

  #include "test_modeling.hh"
  
  // additional include statements will go here
  
  namespace test_modeling {
  
    void test() 
    {
      // test code will go here
    }
  
  }
  
  test_suite* CreateModelingTest()
  {
    using namespace test_modeling;
    test_suite* ts=BOOST_TEST_SUITE("Modeling Test");
     ts->add(BOOST_TEST_CASE(&test));
  
     return ts;
  }
  
In this file, all the normal Boost Test Library macros and functions can be used. (For example `BOOST_CHECK`, `BOOST_FAIL`, etc.)

Here is finally the build script skeleton that needs to be put into 
`mod/tests/`:

.. code-block:: bash

  set(OST_MOD_UNIT_TESTS
  tests.cc
  test_modeling.cc
  )
  
  ost_unittest(mod "${OST_MOD_UNIT_TESTS}")
  target_link_libraries(ost_mol ost_mol_alg ost_mod)
  
In the last line the call to the 'target\_link\_libraries' function contains the 
names of the modules on which the 'mod' unit test code depends (in this case, 
the :mod:`mol` and :mod:`ost.mol.alg` modules), in addition to the `mod` module 
itself.

The Python Wrapper
--------------------------------------------------------------------------------

Finally, the module API is exported to Python using the `Boost Python 
Library <http://www.boost.org/doc/libs/1_37_0/libs/python/doc/index.html>`_.
In `mod/pymod`, the wrapper code for the classes in the new module is put into a 
file named `wrap\_mod.cc`:

.. code-block:: cpp

  #include <boost/python.hpp>
  using namespace boost::python;
  
  #include <ost/mod/modeling_new_class.hh>
  
  using namespace ost::mol;
  using namespace ost::mod;
  
  // All other necessary includes and namespace directives
  // go here
  
  BOOST_PYTHON_MODULE(_mod)
  {
    class_<NewClass>("NewClass", init<>() )
      .def("NewMethod", &NewClass::NewMethod)
    ;
  
    // All other Boost Python code goes here    
  }

The `mod/pymod` directory must obviously contain a `CMakeLists.txt` file:

.. code-block:: bash

  set(OST_MOD_PYMOD_SOURCES
  wrap_mod.cc
  )
  
  pymod(NAME mod OUTPUT_DIR ost/mod 
        CPP ${OST_MOD_PYMOD_SOURCES} PY __init__.py)
  
The directory should also contain an `__init.py__` file with the
following content:

.. code-block:: python
  
  from _mod import *
    
In case one wants to implement Python-only functionality for the new module, any
number of function definitions can be added to the `__init.py__` file.

That's it!. The next time the OpenStructure project is compiled, the new module 
will be built and made available at both the C++ and the Python level.
