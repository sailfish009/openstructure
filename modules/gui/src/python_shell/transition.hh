#ifndef PYTHON_SHELL_TRANSITION_HH
#define PYTHON_SHELL_TRANSITION_HH

#include <utility>
#include <ost/gui/python_shell/python_interpreter.hh>

#include <QObject>
#include <QEvent>
#include "transition_guard.hh"
//fw decl
class QKeyEvent;
class QMouseEvent;

namespace ost { namespace gui {

//fw decl
class State;
class PythonShellWidget;
class ShellHistory;

class TransitionBase: public QObject{
Q_OBJECT
public:
  TransitionBase(State* target, TransitionGuard* guard=new TransitionGuard());
signals:
  void triggered();
protected:
  void trigger_();
  bool is_active_();
  State* target_;
  TransitionGuard* guard_;
};

class AutomaticTransition: public TransitionBase{
Q_OBJECT
public:
  AutomaticTransition(State* target, TransitionGuard* guard=new TransitionGuard());
  bool checkTransition();
};

class SignalTransition: public TransitionBase{
Q_OBJECT
public:
  SignalTransition(QObject * sender,const char *  signal, State* target, TransitionGuard* guard=new TransitionGuard());
protected slots:
  void onSignal();
};

class KeyEventTransition: public TransitionBase{
Q_OBJECT
public:
  KeyEventTransition(QEvent::Type type,int key,Qt::KeyboardModifiers modifiers, State* target, bool swallow_event=true, TransitionGuard* guard=new TransitionGuard());
  virtual std::pair<bool,bool> checkEvent(QKeyEvent* event);
protected:
  QEvent::Type type_;
  int key_;
  Qt::KeyboardModifiers modifiers_;
  bool swallow_;
};

class MouseEventTransition: public TransitionBase{
Q_OBJECT
public:
  MouseEventTransition(QEvent::Type type,Qt::MouseButton button,Qt::KeyboardModifiers modifiers, State* target, bool swallow_event=true, TransitionGuard* guard=new TransitionGuard());
  virtual std::pair<bool,bool> checkEvent(QMouseEvent* event);
protected:
  QEvent::Type type_;
  Qt::MouseButton button_;
  Qt::KeyboardModifiers modifiers_;
  bool swallow_;
};




}}//ns
#endif // PYTHON_SHELL_TRANSITION_HH
