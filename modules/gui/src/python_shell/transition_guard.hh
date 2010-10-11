#ifndef TRANSITION_GUARD_HH
#define TRANSITION_GUARD_HH

#include "python_shell_fw.hh"
#include "python_interpreter.hh"
#include <QObject>

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
    ALWAYSTRUE=0,
    SMALLER=1,
    EQUAL=2,
    BIGGER=4,
    ANCHORSMALLER=8,
    ANCHOREQUAL=16,
    ANCHORBIGGER=32
  };
    EditPositionGuard(PythonShellWidget* shell,int flags1, int flags2=ALWAYSTRUE);
    virtual bool check();
protected:
    virtual bool check_flag(int flag);
    PythonShellWidget* shell_;
    int flags1_;
    int flags2_;
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
