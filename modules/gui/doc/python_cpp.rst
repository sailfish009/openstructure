Mixing PyQt and C++ Widgets
================================================================================
.. currentmodule:: ost.gui

PyQt4 is a set of python bindings for Qt4. The exposure from C++ to Python is 
done with SIP, which has a different mechanism than boost::python.
 
To access a exported boost::python Qt-Object from python we provide a method 
which wraps the Object into a Python SIP Object.

  .. code-block:: python
   
    seq_viewer = gui.SequenceViewer() # Create SequenceViewer Object
    qobj = seq_viewer.qobject #Get Python SIP Object
    print qobj.size()   # Call function on QWidget

The other way around, each boost::python Qt Object accepts python objects as 
input for Qt Objects. It handles the cast to a C++ Qt Object internally.

  .. code-block:: python
   
    persp=gui.GostyApp.Instance().perspective
    test=persp.GetMenu("Test") #Get boost::python qobject
    test_action = QtGui.QAction('&Test me', test) #Create Python SIP Object 
    test.addAction(test_action) #Add Action to boost::python object
