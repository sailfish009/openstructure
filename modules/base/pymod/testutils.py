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
        if (isinstance(obj, (type, types.ClassType)) and
                            issubclass(obj, unittest.TestCase)):
          suite = unittest.TestLoader().loadTestsFromTestCase(obj)
          stream = open('PYTEST-%s.xml'%name, 'w')
          xmlrunner.XMLTestRunner(stream).run(suite)
          stream.close()

    else:
      unittest.main()
  except Exception, e:
    print e