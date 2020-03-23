def RunTests():
  '''
  This function behaves as a custom TestLoader for python unittests.

  With no system arguments, the default unittest TestRunner is used.

  If the first system argument (sys.argv[1]) is set to 'xml', a XMLTestRunner
  is used, which produces a JUnit compatible XML output file. Within the current
  module, each function is identified which is a subclass of unittest.TestCase
  and for each TestCase, a test suite is executed, producing an individual
  output file for each TestCase. The output file has the name,
  'PYTEST-<TestCaseName>.xml'.

  Example of a Python testcase:

  .. code-block:: python

    import unittest

    class TestRenumber(unittest.TestCase):

      def setUp(self):
        # prepare stuff"
        pass

      def testSomeFunction(self):
        # do some asserts
        pass

    if __name__ == "__main__":
      from ost import testutils
      testutils.RunTests()

  '''
  import unittest
  import sys
  try:
    if len(sys.argv)>1 and sys.argv[1]=='xml':
      import inspect
      import types
      import __main__
      from ost import xmlrunner
      for name, obj in inspect.getmembers(__main__):
        if (isinstance(obj, type) and
                            issubclass(obj, unittest.TestCase)):
          suite = unittest.TestLoader().loadTestsFromTestCase(obj)
          stream = open('PYTEST-%s.xml'%name, 'w')
          xmlrunner.XMLTestRunner(stream).run(suite)
          stream.close()

    else:
      unittest.main()
  except Exception as e:
    print(e)


def SetDefaultCompoundLib():
  '''
  This function tries to ensure that a default compound library is set.
  When calling scripts with ``ost`` this is not needed, but since unit tests are
  called with ``python`` we need to ensure that it is set. The function is only
  expected to succeed (and return True) if ``COMPOUND_LIB`` was set when
  :ref:`configuring the compilation <cmake-flags>`.

  It tries the following:

  - get it with :func:`ost.conop.GetDefaultLib`
  - look for ``compounds.chemlib`` in ``$OST_ROOT/share/openstructure``
  - if ``OST_ROOT`` not set in the above, try to guess it based on the path of
    the ``conop`` module

  To use this check modify the :func:`RunTests` call to read:

  .. code-block:: python

    if __name__ == "__main__":
      from ost import testutils
      if testutils.SetDefaultCompoundLib():
        testutils.RunTests()
      else:
        print 'No compound library available. Ignoring test_XXX.py tests.'

  :return: True, if a compound library was found and set to be accessed with
           :func:`ost.conop.GetDefaultLib`. False otherwise.
  '''
  import os
  from ost import conop, GetSharedDataPath, SetPrefixPath
  # check if already there
  if conop.GetDefaultLib():
    return True
  else:
    # try to get the shared data path?
    try:
      shared_data_path = GetSharedDataPath()
    except Exception as e:
      SetPrefixPath(os.path.abspath(os.path.join(conop.__path__[0], os.pardir,
                                                 os.pardir, os.pardir,
                                                 os.pardir, os.pardir)))
      shared_data_path = GetSharedDataPath()
    # look for compounds.chemlib in there
    compound_lib_path = os.path.join(shared_data_path, 'compounds.chemlib')
    if os.path.exists(compound_lib_path):
      compound_lib = conop.CompoundLib.Load(compound_lib_path)
      conop.SetDefaultLib(compound_lib)
      return True
    else:
      return False
