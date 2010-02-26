//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2010 by the OpenStructure authors
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 3.0 of the License, or (at your option)
// any later version.
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//------------------------------------------------------------------------------
#ifndef OST_GUI_GOSTY_APP_HH
#define OST_GUI_GOSTY_APP_HH

/*
  Author: Marco Biasini, Andreas Schenk
 */

#include <QObject>
#include <QString>
#include <QMap>

#include <ost/config.hh>
#include <ost/gui/module_config.hh>
#include <ost/gui/scene_win/scene_win.hh>
#include <ost/gui/sequence_viewer/sequence_viewer.hh>
#include <ost/gui/main.hh>
#include <ost/gui/widget_geom_handler.hh>
#if OST_IPLT_ENABLED
  #include <ost/gui/data_viewer/data_viewer.hh>
#endif




class QMainWindow;
class QMdiArea;
class QWidget;


namespace ost { namespace gui {

class PythonShell;
class GLWin;
class TextLogger;
class ToolOptionsWin;
class Perspective;

/// The gosty app serves as a GUI Manager. It distinguishes between two types of
/// windows: Windows that can only exist once (singletons) and windows with 
/// several independent instances of the same class. The first type includes 
/// windows  such as the 3D Scene and the Python Shell. They can be accessed 
/// through one of the getter methods #GetPyShell(), #GetGLWin(), 
/// #GetToolOptionsWin(). In Python, these classes are additionally exported as 
/// read-only properties \ref #GetPyShell() "py_shell", \ref #GetGLWin() "gl_win".
/// The other class of windows include data viewers, plot viewers and sequence 
/// viewers. They can be created for example the \ref #CreateDataViewer() method.
/// 
/// The layout of the widgets inside the main window is managed through a 
/// \ref Perspective "perspective".
class DLLEXPORT_OST_GUI GostyApp : public QObject {
  Q_OBJECT
public:
  /// \brief get python shell
  /// 
  /// The Python Shell is initialized when this method is first called. 
  /// There may at most be one python shell at any given time. 
  /// Call PythonShell::Show() to display the shell.
  PythonShell* GetPyShell();
  
  /// \brief get 3d graphics window
  /// 
  /// The GL window is initialized when this method is first called. All 
  /// subsequent calls will return the same GLWin instance.
  GLWin* GetGLWin();
  
  /// \brief get scene menu
  /// 
  /// The scene menu window is initialized when this method is first called. All 
  /// subsequent calls will return the same SceneWin instance.  
  SceneWin* GetSceneWin();

  /// \brief get scene menu
  ///
  /// The sequence viewer is initialized when this method is first called. All
  /// subsequent calls will return the same SequenceViewer instance.
  SequenceViewer* GetSequenceViewer();
  
  /// \brief get tool options window
  /// 
  /// The tool options window is initialized when this method is first called. 
  /// All subsequent calls will return the same SceneWin instance.  
  ToolOptionsWin* GetToolOptionsWin();
  
#if OST_IPLT_ENABLED
  /// \brief create new DataViewer
  /// 
  ost::iplt::gui::DataViewer* CreateDataViewer(const ost::iplt::Data& d, const QString& name="");
#endif

  /// \brief give the application time to run the mainloop and process events
  void ProcessEvents();
  
  /// \brief get perspective
  ///
  /// The perspective is initialized when this method is first called. All
  /// subsequent calls will return the same Perspective instance.
  Perspective* GetPerspective();
  
  /// \brief add a custom QWidget to the gosty_app
  ///
  /// This method adds a custom widget to OpenStructure.
  /// It can be used for example in a python script, so that the widget will not be destroyed, when the script reaches the end.
  /// The widget position will be stored, when OpenStructure is being closed. It will be restored the next time, a widget is added with the same identifier.
  /// \param ident the ident is used to identify a custom widget. It must be unique. Otherwise there might occur an unexpected behaviour.
  /// \param widget the widget which will be added to the GostyApp
  void AddWidgetToApp(const QString& ident, QWidget* widget);

  /// \brief remove a custom QWidget from the gosty_app
  ///
  /// This method removes a custom widget from OpenStructure. If the given ident is not known, nothing happens. Read more about custom widgets at \ref #AddWidgetToApp() .
  /// \param ident the ident of the widget which should be removed. Before the widget is removed, the position will be stored.
  void RemoveWidgetFromApp(const QString& ident);

  /// \brief get the GostyApp instance
  /// GostyApp is implemented as Singleton. This returns the Instance of GostyApp.
  /// \return the GostyApp instance
  static GostyApp* Instance();

  /// \brief This methods sets the Title of this Application
  /// \param app_title Title that will be displayed in the title bar
  void SetAppTitle(const QString& app_title);

public slots:
  /// \brief This slot must be called when the application is going to be terminated.
  void OnQuit();
private:  
  void SetupPyShellLogging();
  void ReadLoggerSettings(const QString& group_name, TextLogger* logger);
  GostyApp();  
  PythonShell*      py_shell_;
  QWidget*          w_py_shell_;
                    
  GLWin*            gl_win_;
  QWidget*          w_gl_win_;
                    
  SceneWin*         scene_win_;
  QWidget*          w_scene_win_;

  SequenceViewer*   seq_viewer_;

  ToolOptionsWin*   tool_options_win_;
  QWidget*          w_tool_options_;  
  GostyMainWindow*  main_;

  Perspective*      perspective_;
  
  QMap<QString,WidgetGeomHandler *> external_widgets_;

  static GostyApp*  app_;
};

}}

#endif
