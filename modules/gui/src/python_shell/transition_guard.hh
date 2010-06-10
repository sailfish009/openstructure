#ifndef TRANSITION_GUARD_HH
#define TRANSITION_GUARD_HH

#include <QObject>
#include "python_shell_fw.hh"
#include "python_interpreter.hh"

namespace ost { namespace gui {


//fw decl
  class ShellHistory;
  class PythonShellWidget;


class TransitionGuard: public QObject
{
Q_OBJECT

public:
  TransitionGuard();
  virtual bool check();
};



class HistoryGuard : public TransitionGuard
{
public:
  HistoryGuard(ShellHistory* history, BlockEditMode mode);
  virtual bool check();
protected:
   ShellHistory* history_;
   BlockEditMode mode_;
};

class EditPositionGuard : public TransitionGuard
{
public:
  enum FLAGS{
    SMALLER=1,
    EQUAL=2,
    BIGGER=4
  };
    EditPositionGuard(PythonShellWidget* shell,int flags);
    virtual bool check();
protected:
    PythonShellWidget* shell_;
    int flags_;
};

class BlockStatusGuard : public TransitionGuard
{
public:
    BlockStatusGuard(PythonShellWidget* shell, int status);
    virtual bool check();
protected:
    PythonShellWidget* shell_;
    int status_;
};

}} //ns


#endif // TRANSITION_GUARD_HH
