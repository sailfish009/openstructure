#ifndef PYTHON_SHELL_STATE_HH
#define PYTHON_SHELL_STATE_HH

#include <QObject>
#include <QList>

//fw decl
class QKeyEvent;
class QMouseEvent;

namespace ost { namespace gui {

//fw decl
class SignalTransition;
class MouseEventTransition;
class KeyEventTransition;
class AutomaticTransition;

class State: public QObject{
Q_OBJECT
public:
  State();
  void addTransition(SignalTransition * transition);
  void addTransition(MouseEventTransition * transition);
  void addTransition(KeyEventTransition* transition);
  void addTransition(AutomaticTransition* transition);
  bool isActive();
  void setActive();
  bool checkEvent(QKeyEvent* event);
  bool checkEvent(QMouseEvent* event);
  bool checkAutomaticTransitions();
  virtual void onEntry();
  virtual void onExit();
signals:
  void entered();
  void exited();
protected:
  QList<MouseEventTransition*> mouse_event_transitions_;
  QList<KeyEventTransition*> key_event_transitions_;
  QList<AutomaticTransition*> automatic_transitions_;
};


}}//ns
#endif // PYTHON_SHELL_STATE_HH
