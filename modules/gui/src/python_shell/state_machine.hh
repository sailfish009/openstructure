#ifndef PYTHON_SHELL_STATE_MACHINE_HH
#define PYTHON_SHELL_STATE_MACHINE_HH

#include <QObject>

//fw decl
class QEvent;

namespace ost { namespace gui {

//fw decl
class State;

class StateMachine: public QObject{
Q_OBJECT
public:
  StateMachine(QObject* parent);
  void addState(State* state);
  void setInitialState(State* state);
  void start();
  bool isActive(State* state);
  void setActive(State* state);
  bool eventFilter (QObject * watched, QEvent * event);
protected:
  State* active_state_;
};

}}//ns

#endif // PYTHON_SHELL_STATE_MACHINE_HH
