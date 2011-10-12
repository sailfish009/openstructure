import unittest
import ost

# Altough the logging system might appear to be too simple to be worth writing a 
# specific test case for, it actually isn't. The python export is very fragile 
# and seemingly trivial changes can break the code in unexpected ways. So let's 
# check for some invariants
class TestLog(unittest.TestCase):
  def testGetLogSink(self):
    logsink=ost.GetCurrentLogSink()
    self.assertTrue(hasattr(logsink, 'LogMessage'))
    # Check if the return type of logsink is sane
    ost.PushLogSink(ost.GetCurrentLogSink())
  def testPushPopLogSink(self):
    class MyLogSink(ost.LogSink):
       def __init__(self):
         ost.LogSink.__init__(self)
    ls=MyLogSink()
    ost.PushLogSink(ls)
    self.assertEqual(ls, ost.GetCurrentLogSink())
    ost.PopLogSink()
    self.assertNotEqual(ls, ost.GetCurrentLogSink())

  def testLogMessage(self):
    class CapturingLogSink(ost.LogSink):
      def __init__(self):
        ost.LogSink.__init__(self)
      def LogMessage(self, message, severity):
        self.message=message
        self.severity=severity
        ost.PushLogSink(ls)
    ls=CapturingLogSink()
    ost.PushLogSink(ls)
    ost.LogError('error message')
    self.assertEqual(ls.message, 'error message\n')
    self.assertEqual(ls.severity, 0)
    ost.PopLogSink()
if __name__ == "__main__":
  try:
    unittest.main()
  except Exception, e:
    print e