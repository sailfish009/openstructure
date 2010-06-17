:mod:`~ost.gui` -- Graphical User Interface
================================================================================

.. module:: ost.gui

The GUI of OpenStructure is designed to be intuitive, flexible and extensible. 
Most of the widgets are organized in a big main window which is divided into
four parts: the main area and three panels containing one or more 
smaller widgets:

.. image:: images/100208_OpenStructure_UI_Colored.png

The panels primarily contains widgets interacting with the 3D window.
All parts of the GUI are exported to Python and can be expanded with 
C++ Qt widgets as well as with PyQt widgets. Learn more about :doc:`python_cpp`.

.. toctree::

  python_cpp
  scene_win
  inspector_gadget
  tools
  drag_and_drop
  python_shell
  menubar
  panels
  sequence_viewer

Tools
--------------------------------------------------------------------------------
OpenStructure has several :class:`Tool` which are all listed in the toolbar
close to the 3D window. A tool is activated upon selecting it in the toolbar.
Input events are sent to the tool as long as the `Ctrl` key (`Cmd` on Mac) is
pressed. For example the rigid body manipulator will rotate and shift the
currently selected nodes in the scene win. Upon releasing `Ctrl`, the events are
sent to the camera, making it possible to rapidly switch between adjusting the
viewing angle of the camera and manipulating the position and orientation of
scene objects.
 
.. image:: images/100208_Tools.png  
 
The example `Write a Tool in Python` demonstrates how to add a simple tool
(written in python) to OpenStructure.

Drag and Drop support
--------------------------------------------------------------------------------
The user interface of OpenStructure supports drag and drop events. Every file
format that is supported by Openstructure can be opened by dragging and dropping
it on the main window. When a python script (ending with .py) is being dropped
on the UI, the script will be executed. For any other file type (for example PDB
files, images and density maps), OpenStructure will try to load the file and
display it in the 3D window, in the data viewer for images or in the sequence
viewer.

Python Shell
--------------------------------------------------------------------------------
OpenStructure has a integrated python shell which allows you interactively enter
Python commands. Learn more about it in :doc:`python_shell`-Tutorial.  

The Side Panels
--------------------------------------------------------------------------------
View Modes
^^^^^^^^^^
Each :class:`PanelBar` can have different :class:`PanelWidgetContainer` also
called view modes. View modes can display the widgets which are held by the Side
Panel in a different style. Every panel has the splitter and the tabbed view
mode.
The view mode can be changed in the Window menu of the Menubar.

Drag and Drop
^^^^^^^^^^^^^
The widgets which are held by a Side Panel can be moved to an other position in
the panel or even to another side panel. The widget can be moved by simply
clicking on the border of the widget and drag and drop it to the desired
position. The drag and drop feature is currently only supported by the splitter
view mode.

Adding a custom Widget
^^^^^^^^^^^^^^^^^^^^^^
The Left-, Bottom- and Right-Panel are organized by the :class:`Panels`-class. 
It is only possible to display a widget which is in the :class:`WidgetPool` of 
the panels class. Once a widget is in the pool all the methods of the Panels
class can be used to display / hide the widget in any position of the panels.
The Example `Add a custom Widget` describes, how to add a fancy PyQt Widget to
the widget pool and finally display it in the right side bar.
OpenStructure remembers the size and location of a Widget. So, OpenStructure 
should look the same after restarting it.

Main Area
--------------------------------------------------------------------------------
Adding an own Widget
^^^^^^^^^^^^^^^^^^^^
The :class:`MainArea` is a mdi (multi document interface). Therefore it's
possible to display multiple widgets in it. The example `Add a Widget to MDI` 
demonstrates how to add a  widget to the MDI area.

Menubar
--------------------------------------------------------------------------------
Adding an new Menupoint
^^^^^^^^^^^^^^^^^^^^^^^
It is really straightforward to add a custom menupoint. Since the menubar is 
exported to Python it is even easier to create such e menupoint. See the example 
`Adding a Menu` which describes how this is done within python and PyQt.

Nice to know
---------------------------------------------------------------------------------
As you might have noticed nearly every gui example needs the :class:`GostyApp` 
and the :class:`Perspective` within the code. Gosty app is a singleton class 
which has getter methods to all important gui classes. The perspective class has 
all the information about the layout of the whole application, it is accessible 
over the gosty app instance.  
