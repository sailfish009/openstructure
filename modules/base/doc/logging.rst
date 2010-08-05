Logging
================================================================================

.. currentmodule:: ost

How to log messages
--------------------------------------------------------------------------------

OpenStructure has an easy to use Logging API. Depended on how severe the message 
is, one of the following four methods can be used to log the message:

  .. method:: LogError(message)
  
    Use this Method to log whenever an error occured that may cause the 
    application to fail
    
  .. method:: LogMessage(message)
  
    Use this Method to warn the user that something went wrong but generally does
    not pose a threat to the stability of the application
      
  .. method:: LogVerbose(message)
  
    Use this Method to inform the user about processed data
    
  .. method:: LogDebug(message)
  
    Use this Method for informational messages that can help debug the program   


Log sinks
--------------------------------------------------------------------------------

It is possible to log into different log backends namely log sinks. Log sink classes
define, how to handle log messages:

.. class:: LogSink

  .. method:: LogMessage(message, severity)

     This method is called whenever something has been logged. This i a pure virtual
     function therefore it must be overloaded in base classes.
     
     :param message: The logged message
    
     :type  message: :class:`str`
     
     :param severity: Marks how severe the logged message is, starts from 0 which 
      is really important. Value is always 0 or larger 
    
     :type  severity: :data:`int`

There are four LogSink implementations available in OpenStructure:

.. class:: FileLogSink

     The FileLogSink logs all messages into the given file.

.. class:: NullLogSink

     The NullLogSink suppresses all LogMessages

.. class:: ObservedLogSink

     An observed LogSink allows to log into multiple sinks.
     
  .. method:: AttachObserver(sink)
  
    Attach an Observer which will be called whenever something has been logged 
    
  .. method:: RemoveObserver(sink)
  
    Removes the given sink it will no longer be called when something has been logged

The following snippet explains how to create a custom log sink which logs to the python shell:

.. code-block:: python
  
  class PyLogger(ost.LogSink):
    def __init__(self):
      ost.LogSink.__init__(self)
  
    def LogMessage(self, message, severity):
      print severity, message


To change the logger you can use the following methods:

  .. method:: PushLogSink(sink)
  
     Change the log sink to the given sink

  .. method:: PopLogSink()
  
     Change the log sink back to the previous sink
     

Verbosity Level
--------------------------------------------------------------------------------

You can set the verbosity level of the Logger with the following methods:

  .. method:: PushVerbosityLevel(verbosity):
   
     Change the Verbosity Level to the given integer value. All LoggedMethods which
     have a severity above verbosity will not be passed to the current log sink.
   
    :type  verbosity: :data:`int`
    
  .. method:: PopVerbosityLevel():
    
     Change the log sink back to the previous verbosity level
          