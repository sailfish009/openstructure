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

EditPositionGuard::EditPositionGuard(PythonShellWidget* shell, int flags):
  TransitionGuard(),
  shell_(shell),
  flags_(flags)
{
}

bool EditPositionGuard::check()
{
  bool returnflag=false;
  if(flags_ & SMALLER){
    returnflag |= shell_->textCursor().position()< shell_->GetEditStartBlock().position();
  }
  if(flags_ & EQUAL){
    returnflag |= shell_->textCursor().position()== shell_->GetEditStartBlock().position();
  }
  if(flags_ & BIGGER){
    returnflag |= shell_->textCursor().position()> shell_->GetEditStartBlock().position();
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
