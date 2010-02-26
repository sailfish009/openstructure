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
#include <iostream>

#include <QDebug>
#include <QFontMetrics>
#include <QClipboard>
#include <QPainter>
#include <QScrollBar>
#include <QDirModel>
#include <QStringList>

#include <ost/gui/python_shell/text_logger.hh>
#include "python_shell_widget.hh"

#include "gutter.hh"

#include "string_literal_positions.hh"

#include "python_shell_text_document_layout.hh"

#include "python_completer.hh"
#include "path_completer.hh"



/*
 Authors: Marco Biasini, Andreas Schenk
 */

namespace ost { namespace gui {


PythonShellWidget::PythonShellWidget(QWidget* parent):
  QPlainTextEdit(parent),
  tab_width_(2),
  highlighter_(document()),
  completer_(new PythonCompleter(viewport())),
  path_completer_(new PathCompleter(viewport())),
  block_edit_mode_(EDITMODE_SINGLELINE),
  gutter_(new Gutter(this)),
  output_visible_(true),
  completion_start_(0),
  completion_end_(0),
  mode_(SHELL_INTERACTION_BASH)
{
  this->setLineWrapMode(QPlainTextEdit::NoWrap);
  document()->setDocumentLayout(new PythonShellTextDocumentLayout(document()));
  setViewportMargins(Gutter::GUTTER_WIDTH, 0, 0, 0);
  setUndoRedoEnabled(false);
  setFont(QFont("Courier",font().pointSize()+2));
  QFontMetrics metrics(font());
  setTabStopWidth(2*metrics.width(" "));
  setMaximumBlockCount(1000000);
  connect(this, SIGNAL(updateRequest(QRect, int)) ,gutter_, 
          SLOT(Update(QRect, int)));
  connect(this, SIGNAL(Execute(QString)),&PythonInterpreter::Instance(), 
          SLOT(RunCommand(QString)),Qt::QueuedConnection);
  connect(&PythonInterpreter::Instance(), SIGNAL(Done(int,const QString&)),this, 
          SLOT(AppendOutput(int,const QString&)),Qt::QueuedConnection);
  textCursor().block().setUserState(BLOCKTYPE_ACTIVE);
  completer_->setWidget(viewport());
  connect(completer_,SIGNAL(activated(const QString&)),this, 
          SLOT(InsertCompletion(const QString&)));
  connect(completer_,SIGNAL(recomplete(const QString&)),this, 
          SLOT(Recomplete(const QString&)));
  connect(this,SIGNAL(RequestCompletion(const QRect&,bool)),completer_, 
          SLOT(complete(const QRect&,bool)));
  connect(this,SIGNAL(SetCompletionPrefix(const QString&)),completer_, 
          SLOT(SetCompletionPrefix(const QString&)));
  path_completer_->setWidget(viewport());
  connect(path_completer_,SIGNAL(activated(const QString&)),this, 
          SLOT(InsertCompletion(const QString&)));
  connect(path_completer_,SIGNAL(recomplete(const QString&)),this, 
          SLOT(Recomplete(const QString&)));
  connect(this,SIGNAL(RequestPathCompletion(const QRect&,bool)),path_completer_, 
          SLOT(complete(const QRect&,bool)));
  connect(this,SIGNAL(SetPathCompletionPrefix(const QString&)),path_completer_, 
          SLOT(setCompletionPrefix(const QString&)));
  if (mode_==SHELL_INTERACTION_BASH) {
    SetBlockEditMode(EDITMODE_SINGLELINE);    
  } else {
    SetBlockEditMode(EDITMODE_MULTILINE_INACTIVE);
  }
  PythonInterpreter::Instance().RedirectOutput();
}

void PythonShellWidget::SetTabWidth(int width) {
  tab_width_=width;
  QFontMetrics metrics(font());
  setTabStopWidth(tab_width_*metrics.width(" "));
}



int PythonShellWidget::GetTabWidth() const {
  return tab_width_;
}



void PythonShellWidget::WrapIntoFunction(const QString& command)
{
  QString tmp_command=command;
  tmp_command.replace(QString(QChar::LineSeparator),
                      QString(QChar::LineSeparator)+QString("\t"));
  QString tmp="def func():"+QString(QChar::LineSeparator)+"\t";
  tmp+=tmp_command;
  SetCommand(tmp);
  QTextCursor tc=textCursor();
  tc.setPosition(document()->lastBlock().position()+QString("def ").length());
  tc.setPosition(document()->lastBlock().position()+
                 QString("def func").length(),QTextCursor::KeepAnchor);
  setTextCursor(tc);
}

void PythonShellWidget::Recomplete(const QString& completion)
{
  InsertCompletion(completion);
  Complete(false);
}

void  PythonShellWidget::InsertCompletion(const QString& completion)
{
  QTextCursor tc = textCursor();
  tc.setPosition(completion_start_);
  tc.setPosition(completion_end_,QTextCursor::KeepAnchor);
  tc.insertText(completion);
  setTextCursor(tc);
}


void PythonShellWidget::AppendOutput(int status,const QString& output)
{
  QStringList output_list=output.split("\n");
  if (output_list.size()>=maximumBlockCount ()){
    QTextCursor cursor=textCursor();
    cursor.movePosition(QTextCursor::Start);
    cursor.movePosition(QTextCursor::End,QTextCursor::KeepAnchor);
    cursor.removeSelectedText();
    output_list.erase(output_list.begin(),output_list.begin()+
                      output_list.size()+1-maximumBlockCount());
  } else if (output_list.size()+blockCount()>maximumBlockCount()){
    QTextCursor cursor=textCursor();
    cursor.movePosition(QTextCursor::Start);
    cursor.movePosition(QTextCursor::NextBlock,QTextCursor::KeepAnchor,
                        output_list.size()+1+blockCount()-maximumBlockCount());
    cursor.removeSelectedText();
  }
  moveCursor(QTextCursor::End);
  QTextCursor cursor=textCursor();
  cursor.beginEditBlock();
  if (output!="" && output_list.size()>0) {
    for(int i=0;i<output_list.size();++i){
      document()->lastBlock().setUserState(status);
      insertPlainText(output_list[i]);
      insertPlainText(QString(QChar::ParagraphSeparator));
    }
  }
  document()->lastBlock().setUserState(BLOCKTYPE_ACTIVE);
  cursor.endEditBlock();
  verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMaximum);
  this->ensureCursorVisible();  
}



void PythonShellWidget::SetCommand(const QString& command)
{
  QTextCursor cursor=textCursor();
  cursor.setPosition(document()->lastBlock().position());
  cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
  cursor.beginEditBlock();
  cursor.removeSelectedText();
  cursor.insertText(command);
  cursor.endEditBlock();
  QAbstractTextDocumentLayout* layout=document()->documentLayout();  
  dynamic_cast<PythonShellTextDocumentLayout*>(layout)->EmitSizeChange();
  verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMaximum);
  ensureCursorVisible();
}


void PythonShellWidget::Complete(bool inline_completion)
{
  QTextCursor cursor=textCursor();
  if(cursor.position()<document()->lastBlock().position()){
    return;
  }
  StringLiteralPositions* positions=dynamic_cast<StringLiteralPositions*>(document()->lastBlock().userData());
  int literal_index=-1;
  if(positions){
    literal_index=positions->GetStringLiteralIndex(textCursor().position());
  }
  if (literal_index>=0) {
    cursor.setPosition(positions->GetStart(literal_index));
    cursor.setPosition(positions->GetEnd(literal_index), 
                       QTextCursor::KeepAnchor);
    completion_start_=positions->GetStart(literal_index);
    completion_end_=positions->GetEnd(literal_index);
    QTextCursor cp=textCursor();
    QString text=cursor.selectedText();        
    int sep=text.lastIndexOf(QDir::separator());    
    cp.setPosition(positions->GetStart(literal_index));
    if (sep!=-1) {
      cp.movePosition(QTextCursor::NextCharacter,
                      QTextCursor::MoveAnchor, sep);      
    }
    if (QDir(text).isAbsolute()) {
      emit SetPathCompletionPrefix(text);
    } else {
      emit SetPathCompletionPrefix(QDir::currentPath()+QDir::separator()+text);
    }
    emit RequestPathCompletion(cursorRect(cp), 
                               inline_completion);
  } else {
    cursor.clearSelection();
    cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
    QString command=cursor.selectedText();
    QRegExp rx("[\\w,\\.]*$");
    int index=rx.indexIn(command);
    completion_start_=cursor.position()+index;
    completion_end_=cursor.position()+index+rx.cap().size();
    emit SetCompletionPrefix(rx.cap());
    // determine position for completion popup. This is basically the position 
    // of the first character after a dot or the beginning of the completion
    // prefix if there is no dot.
    QTextCursor cp=textCursor();    
    int dot=command.lastIndexOf('.');
    int left_shift=rx.cap().size();
    if (dot>=index) {
      left_shift=rx.cap().size()-(dot-index);
    }
    cp.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor,
                    left_shift);
    emit RequestCompletion(cursorRect(cp), inline_completion);
  }
}

bool PythonShellWidget::HandleNav(QKeyEvent* event)
{
  if(event->key() == Qt::Key_Up){
     if (GetBlockEditMode()!=EDITMODE_MULTILINE_ACTIVE) {
       SanitizeCursorPosition();
       if(history_.AtEnd()){
         history_.SetCurrentCommand(textCursor().block().text(),
                                    GetBlockEditMode());
       }
       --history_;
       SetCommand(history_.GetCommand());
       this->SetBlockEditMode(history_.GetCommandMode());
       return true;
     }else{
       QTextCursor temporary_cursor=textCursor();
       int temp_position_before=temporary_cursor.position();
       temporary_cursor.movePosition(QTextCursor::Up);
       if (temporary_cursor.position()<document()->lastBlock().position() && 
           temp_position_before>=document()->lastBlock().position() && 
           !(event->modifiers() & Qt::ShiftModifier)) {
         return true;
       }
     }
   }
   if (event->key() == Qt::Key_Down && 
       GetBlockEditMode()!=EDITMODE_MULTILINE_ACTIVE) {
     SanitizeCursorPosition();
     ++history_;
     SetCommand(history_.GetCommand());
     this->SetBlockEditMode(history_.GetCommandMode());     
     return true;
   }  
  return false;
}

bool PythonShellWidget::HandleCustomCommands(QKeyEvent* event)
{
  // see ticket #38
  #if 0
  if ((event->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier)) && 
      event->key() == Qt::Key_H) {
    SetOutputVisible(!output_visible_);
    return true;
  }
  #endif
  if ((event->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier)) && 
      event->key() == Qt::Key_W) {
    WrapIntoFunction(textCursor().selectedText());
    return true;
  }
  // custom handling of CTRL+A to select only text in edit or output section
  if (event->modifiers() == Qt::ControlModifier && 
      event->key() == Qt::Key_A) {
    QTextCursor cursor=textCursor();
    if(cursor.position()<document()->lastBlock().position()){
      // select all output
      cursor.setPosition(0);
      cursor.setPosition(document()->lastBlock().position(),QTextCursor::KeepAnchor);
      setTextCursor(cursor);
    }else{
      //select whole edit section
      cursor.movePosition(QTextCursor::StartOfBlock);
      cursor.movePosition(QTextCursor::EndOfBlock,QTextCursor::KeepAnchor);
      setTextCursor(cursor);
    }
    return true;
  }
  return false;
}

bool PythonShellWidget::HandleReturnKey(QKeyEvent* event)
{
  if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
    QString command=textCursor().block().text();    
    SanitizeCursorPosition();
    command.replace(QString(QChar::LineSeparator),QString("\n"));
    bool execute_cmd=false;
    bool insert_new_line=false;
    bool move_to_end=false;
    if (mode_==SHELL_INTERACTION_MATHEMATICA) {
      if (event->modifiers() & Qt::ControlModifier ||
          (command.endsWith("\n") && textCursor().atEnd())) {
        execute_cmd=true;
        move_to_end=(event->modifiers() & Qt::ControlModifier)==false;
      } else {
        if (textCursor().block().text().trimmed().length()==0) {
          return true;
        }
        insert_new_line=true;
        move_to_end=true;
      }
    } else {
      if (event->modifiers() & Qt::ControlModifier) {
        insert_new_line=true;
        move_to_end=false;
      } else {
        PythonInterpreter& pi=PythonInterpreter::Instance();
        CodeBlockStatus status=pi.GetCodeBlockStatus(command);
        if (status!=CODE_BLOCK_INCOMPLETE) {
          execute_cmd=true;
          move_to_end=true;
        } else {
          insert_new_line=true;
          move_to_end=true;
        }
      }
    }
    QTextCursor cursor=this->textCursor();      
    if (move_to_end) {
      cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
      this->setTextCursor(cursor);
    }    
    if (insert_new_line) {
      cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
      bool new_indent=cursor.selectedText()==":";
      cursor.movePosition(QTextCursor::StartOfLine);
      cursor.movePosition(QTextCursor::NextWord, QTextCursor::KeepAnchor);
      insertPlainText(QString(QChar::LineSeparator));
      if(cursor.selectedText()[0].isSpace()){
        insertPlainText(QString(cursor.selectedText()));
      }
      if (new_indent){
        insertPlainText(QString("\t"));
      }
      QAbstractTextDocumentLayout* layout=document()->documentLayout();
      dynamic_cast<PythonShellTextDocumentLayout*>(layout)->EmitSizeChange();
      verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMaximum);
      SetBlockEditMode(EDITMODE_MULTILINE_ACTIVE);
      return true;
    }
    if (execute_cmd) {
      QKeyEvent new_event(event->type(),event->key(),
                          Qt::NoModifier,
                          event->text(),event->isAutoRepeat(),
                          event->count());
      QPlainTextEdit::keyPressEvent(&new_event);
      ExecuteCommand();
      return true;                          
    }
  }
  return false;
}


bool PythonShellWidget::HandleCompletion(QKeyEvent* event)
{
  if(event->key() == Qt::Key_Tab){
    QRegExp rx("^\\s*$"); // only white spaces from beginning of line
    QTextCursor cursor= textCursor();
    cursor.clearSelection();
    cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
    QString command=cursor.selectedText();
    if(!command.contains(rx)){
      Complete();
      return true;
    }
  }  
  return false;
}


void PythonShellWidget::keyPressEvent(QKeyEvent* event)
{
  if (this->HandleCustomCommands(event))
    return;
    
  if (this->HandleCompletion(event))
    return;
  if (this->HandleNav(event))
    return;

  if (this->HandleReturnKey(event))
    return;
  if ((event->text()!="" && !(event->modifiers() & Qt::ControlModifier)) || 
      event->matches(QKeySequence::Cut)  ||  event->key()== Qt::Key_Delete){
    SanitizeCursorPosition();
    if(GetBlockEditMode()==EDITMODE_MULTILINE_INACTIVE){
      SetBlockEditMode(EDITMODE_MULTILINE_ACTIVE);
    }
  }
  if (GetBlockEditMode()==EDITMODE_MULTILINE_ACTIVE && event->key()==Qt::Key_Escape){
    SetBlockEditMode(EDITMODE_MULTILINE_INACTIVE);
  }
  
  if (GetBlockEditMode()==EDITMODE_MULTILINE_INACTIVE && (event->key()==Qt::Key_Right || 
      event->key() == Qt::Key_Left)) {
    SetBlockEditMode(EDITMODE_MULTILINE_ACTIVE);
  }
  
  if (event->key()== Qt::Key_Backspace && 
      textCursor().position()<=document()->lastBlock().position()
     && !textCursor().hasSelection()){
    return;
  }
  

  if (event->key()== Qt::Key_Left && 
      textCursor().position()==document()->lastBlock().position() && 
      !(event->modifiers() & Qt::ShiftModifier)) {
    return;
  }
  QPlainTextEdit::keyPressEvent(event);
}

void PythonShellWidget::ExecuteCommand()
{
  QString command=textCursor().block().previous().text();
  textCursor().block().previous().setUserState(BLOCKTYPE_CODE);
  document()->markContentsDirty(textCursor().block().previous().position(),
                                textCursor().block().previous().length());
  QString cmd=command;
  cmd.replace(QString(QChar::LineSeparator),QString("\n"));
  emit Execute(cmd);

  command=command.trimmed();
  if (command.size()>0) {
    history_.AppendCommand(command,GetBlockEditMode());
  }
  if (mode_==SHELL_INTERACTION_MATHEMATICA) {
    SetBlockEditMode(EDITMODE_MULTILINE_INACTIVE);
  } else {
    SetBlockEditMode(EDITMODE_SINGLELINE);
  }  
}

void PythonShellWidget::mouseReleaseEvent(QMouseEvent * event)
{
  QTextCursor mouse_cursor=cursorForPosition(event->pos());
  if (GetBlockEditMode()==EDITMODE_MULTILINE_INACTIVE && 
      event->button() == Qt::LeftButton &&
      mouse_cursor.position()>=document()->lastBlock().position()) {
    // switch to active edit mode upon mouse click in edit section
    SetBlockEditMode(EDITMODE_MULTILINE_ACTIVE);
  }
  if (event->button() == Qt::MidButton && 
      mouse_cursor.position()<document()->lastBlock().position()) {
    // fix cursor position before inserting text
    setTextCursor(mouse_cursor);
    return;
  }
  QPlainTextEdit::mouseReleaseEvent(event);
}

void PythonShellWidget::SanitizeCursorPosition()
{
  if(textCursor().position()<document()->lastBlock().position()){
    if(textCursor().anchor()<document()->lastBlock().position()){
      moveCursor(QTextCursor::End);
    }else{
      QTextCursor cursor=textCursor();
      cursor.setPosition(document()->lastBlock().position(),
                         QTextCursor::KeepAnchor);
      setTextCursor(cursor);
    }
  }else{
    if(textCursor().anchor()<document()->lastBlock().position()){
      int current_position=textCursor().position();
      QTextCursor cursor=textCursor();
      cursor.setPosition(document()->lastBlock().position());
      cursor.setPosition(current_position,QTextCursor::KeepAnchor);
      setTextCursor(cursor);
    }
  }
}

void PythonShellWidget::SetBlockEditMode(BlockEditMode flag)
{
  block_edit_mode_=flag;
  if(flag==EDITMODE_MULTILINE_ACTIVE){
    document()->lastBlock().setUserState(BLOCKTYPE_BLOCKEDIT);
  }else if(flag==EDITMODE_MULTILINE_INACTIVE){
    document()->lastBlock().setUserState(BLOCKTYPE_ACTIVE);
  }else {
    document()->lastBlock().setUserState(BLOCKTYPE_ACTIVE);
  }
  gutter_->update();
  document()->markContentsDirty(document()->lastBlock().position(),
                                document()->lastBlock().length());
}

BlockEditMode PythonShellWidget::GetBlockEditMode()
{
  return block_edit_mode_;
}

QString PythonShellWidget::GetCommand()
{
  return document()->lastBlock().text();
}

void PythonShellWidget::insertFromMimeData(const QMimeData * source)
{
  if(!source->hasText()){
    return;
  }
  QString text=source->text();
  SanitizeCursorPosition();
  text=text.replace("\r\n", QString(1, QChar::LineSeparator))
           .replace('\n', QChar::LineSeparator)
           .replace('\r', QChar::LineSeparator);
  // replace spaces at the beginning of lines by tabs, because spaces are evil!
  QStringList lines=text.split(QChar::LineSeparator);
  for (int i=0; i <lines.size(); ++i) {
    int num_spaces=0;
    for (int j=0; j<lines[i].size()-1; j+=2) {
      if (lines[i][j]==' ' && lines[i][j+1]==' ') {
        num_spaces+=1;
      } else {
        break;
      }
    }
    QString right=lines[i].right(lines[i].size()-2*num_spaces);
    lines[i]=QString(num_spaces, '\t')+right;
  }
  text=lines.join(QString(1, QChar::LineSeparator));
  int line_sep=text.count(QChar::LineSeparator);
  if(line_sep>0){
    QString command=GetCommand();
    int last_block_start=document()->lastBlock().position();
    int last_block_end=last_block_start+document()->lastBlock().length();    
    int shifted_start=textCursor().selectionStart()-last_block_start;
    int shifted_end=last_block_end-textCursor().selectionEnd();
    QString rpl_text=command.left(shifted_start)+text+
                     command.right(shifted_end-1);
    this->SetCommand(rpl_text);
    SetBlockEditMode(EDITMODE_MULTILINE_ACTIVE);
  }else{
    QPlainTextEdit::insertFromMimeData(source);
  }
}

GutterBlockList PythonShellWidget::GetGutterBlocks(const QRect& rect)
{
  GutterBlockList result;
  for (QTextBlock i=firstVisibleBlock(); i!=document()->end(); i=i.next()) {
    QRectF bounds=blockBoundingGeometry(i);
    QRectF geom=bounds.translated(contentOffset());
    if (geom.top()<=rect.bottom() && (geom.top()>=rect.top() || 
        geom.bottom() <= rect.bottom() || (geom.top() <= rect.top() && 
        geom.bottom() >= rect.bottom())) ) {
      BlockType bt=static_cast<BlockType>(i.userState());
      result.push_back(GutterBlock(geom.top(),geom.bottom(),bt));
    } else {
      break;
    }
  }
  return result;
}

void PythonShellWidget::SetOutputVisible(bool flag)
{
  output_visible_=flag;
  for (QTextBlock i=document()->begin(); i!=document()->end(); i=i.next()) {
    if(i.userState()==BLOCKTYPE_ERROR || i.userState()==BLOCKTYPE_OUTPUT){
      i.setVisible(flag);
    }
  }
  gutter_->update();
  viewport()->update();  
  this->update();  
}

void PythonShellWidget::resizeEvent(QResizeEvent* event)
{
  QPlainTextEdit::resizeEvent(event);
  QRect cr=contentsRect();
  gutter_->setGeometry(QRect(cr.left(), cr.top(),Gutter::GUTTER_WIDTH, 
                       cr.height()));
}

void PythonShellWidget::showEvent(QShowEvent* event)
{
  static bool init=false;
  if(!init) {
    init=true;
  }
}

void PythonShellWidget::AquireFocus()
{
  setFocus(Qt::OtherFocusReason);
}

void PythonShellWidget::AddLogger(TextLogger* logger)
{
  loggers_.push_back(logger);
  logger->setParent(this);  
  connect(this,SIGNAL(Execute(const QString&)),
          logger,SLOT(AppendCode(const QString&)));
  connect(&PythonInterpreter::Instance(),SIGNAL(Done(int,const QString&)),
          logger,SLOT(AppendOutput(int,const QString&)));
}

}}
