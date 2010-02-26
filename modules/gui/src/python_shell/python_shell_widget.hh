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
#ifndef OST_GUI_PYTHON_SHELL_WIDGET_HH
#define OST_GUI_PYTHON_SHELL_WIDGET_HH

/*
  Authors: Marco Biasini, Andreas Schenk
 */

#include <ost/gui/module_config.hh>


#include "python_interpreter.hh"
#include "shell_history.hh"
#include  "python_shell_fw.hh"

#include "python_syntax_highlighter.hh"

#include <QPlainTextEdit>

namespace ost { namespace gui {

// fw decl
class Gutter;
class PythonCompleter;
class PathCompleter;
class TextLogger;

typedef enum {
  // to execute a command, the command must end with an empty line, or
  // Cmd+Return must be pressed
  SHELL_INTERACTION_MATHEMATICA=0,
  // to execute a command, a single return key press is sufficient. Cmd+Return
  // starts newline. This mode is the default
  SHELL_INTERACTION_BASH=1
} ShellInteractionMode;

class DLLEXPORT_OST_GUI PythonShellWidget : public QPlainTextEdit {
  Q_OBJECT
public:
  PythonShellWidget(QWidget* parent=NULL);
  GutterBlockList GetGutterBlocks(const QRect& rect);
  void SetOutputVisible(bool flag=true);
  
  void AddLogger(TextLogger* logger);
  
  void SetInteractionMode(ShellInteractionMode mode);

  void SetTabWidth(int width);
  int GetTabWidth() const;
  int tab_width_;
signals:
  void Execute(const QString& command);
  void RequestCompletion(const QRect& rect,bool inline_completion);
  void SetCompletionPrefix(const QString& prefix);
  void RequestPathCompletion(const QRect& rect,bool inline_completion);
  void SetPathCompletionPrefix(const QString& prefix);

public slots:
  void InsertCompletion(const QString& completion);
  void AppendOutput(int status,const QString& output);
  void AquireFocus();
  void Complete(bool inline_completion=true);
  void Recomplete(const QString& completion);

protected:
  virtual void mouseReleaseEvent (QMouseEvent* event );
  virtual void keyPressEvent (QKeyEvent* event );
  virtual void resizeEvent(QResizeEvent* event);
  virtual void showEvent(QShowEvent* event);

  void WrapIntoFunction(const QString& command);
  void SetCommand(const QString& command);
  QString GetCommand();
  void insertFromMimeData( const QMimeData * source );
  void SanitizeCursorPosition();
  void SetBlockEditMode(BlockEditMode flag);
  
  bool HandleNav(QKeyEvent* event);
  bool HandleCompletion(QKeyEvent* event);
  bool HandleCustomCommands(QKeyEvent* event);
  bool HandleReturnKey(QKeyEvent* event);
  BlockEditMode GetBlockEditMode();
  void ExecuteCommand();

  PythonSyntaxHighlighter highlighter_;
  PythonCompleter* completer_;
  PathCompleter* path_completer_;
  BlockEditMode block_edit_mode_;
  ShellHistory history_;
  Gutter* gutter_;
  bool output_visible_;
  int completion_start_;
  int completion_end_;
  std::vector<TextLogger*>  loggers_;  
  ShellInteractionMode mode_;  
};

}}//ns

#endif
