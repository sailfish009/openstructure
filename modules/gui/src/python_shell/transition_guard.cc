#include <iostream>
#include "transition_guard.hh"
#include "python_shell_widget.hh"
#include "shell_history.hh"

namespace ost { namespace gui {

TransitionGuard::TransitionGuard():
  QObject()
{
}

bool TransitionGuard::check()
{
  return true;
}

HistoryGuard::HistoryGuard(ShellHistory* history, BlockEditMode mode):
  TransitionGuard(),
  history_(history),
  mode_(mode)
{
}

bool HistoryGuard::check()
{
  return history_->GetCommandMode()==mode_;
}

EditPositionGuard::EditPositionGuard(PythonShellWidget* shell, int flags1, int flags2):
  TransitionGuard(),
  shell_(shell),
  flags1_(flags1),
  flags2_(flags2)
{
}

bool EditPositionGuard::check()
{
  return check_flag(flags1_) &&  check_flag(flags2_);
}

bool EditPositionGuard::check_flag(int flags)
{
  if(flags==0){
    return true;
  }
  bool returnflag=false;
  QTextCursor tc=shell_->textCursor();
  if(flags & SMALLER){
    returnflag|=tc.position()<shell_->GetEditStartBlock().position();
  }
  if(flags & EQUAL){
    returnflag|=tc.position()==shell_->GetEditStartBlock().position();
  }
  if(flags & BIGGER){
    returnflag |= tc.position()> shell_->GetEditStartBlock().position();
  }
  if(flags & ANCHORSMALLER){
    returnflag|=tc.anchor()<shell_->GetEditStartBlock().position();
  }
  if(flags & ANCHOREQUAL){
    returnflag|=tc.anchor()==shell_->GetEditStartBlock().position();
  }
  if(flags & ANCHORBIGGER){
    returnflag |= tc.anchor()> shell_->GetEditStartBlock().position();
  }
  return returnflag;
}

BlockStatusGuard::BlockStatusGuard(PythonShellWidget* shell, int status):
  TransitionGuard(),
  shell_(shell),
  status_(status)
{
}

bool BlockStatusGuard::check()
{
  QString command=shell_->GetCommand();
  return PythonInterpreter::Instance().GetCodeBlockStatus(command) & status_;
}



}} //ns
