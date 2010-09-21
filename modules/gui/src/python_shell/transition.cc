#include <cassert>
#include "python_shell_widget.hh"
#include "transition.hh"
#include "state.hh"
#include <QKeyEvent>
#include <QMouseEvent>

namespace ost { namespace gui {

TransitionBase::TransitionBase(State* target,  TransitionGuard* guard):
  QObject(),
  target_(target),
  guard_(guard)
{
  guard_->setParent(this);
}
void TransitionBase::trigger_()
{
  emit triggered();
  target_->setActive();
}
bool TransitionBase::is_active_()
{
  return parent() && dynamic_cast<State*>(parent())->isActive();
}

AutomaticTransition::AutomaticTransition(State* target,  TransitionGuard*  guard):
  TransitionBase(target,guard)
{
}

bool AutomaticTransition::checkTransition()
{
  if(guard_->check()){
    trigger_();
    return true;
  }
  return false;
}

SignalTransition::SignalTransition(QObject * sender,const char *  signal, State* target,  TransitionGuard*  guard):
  TransitionBase(target,guard)
{
  connect(sender,signal,this,SLOT(onSignal()));
}
void SignalTransition::onSignal()
{
  if(is_active_() && guard_->check()){
    trigger_();
  }
}

KeyEventTransition::KeyEventTransition(QEvent::Type type,int key,Qt::KeyboardModifiers modifiers, State* target, bool swallow_event,  TransitionGuard*  guard):
  TransitionBase(target,guard),
  type_(type),
  key_(key),
  modifiers_(modifiers),
  swallow_(swallow_event)
{
}
std::pair<bool,bool> KeyEventTransition::checkEvent(QKeyEvent* event)
{
  assert(is_active_());
  if(event->type()==type_ && (event->key()==key_ || 
     key_==Qt::Key_Any) && event->modifiers() == modifiers_ && guard_->check()){
    trigger_();
    return std::pair<bool,bool>(true,swallow_);
  }
  return std::pair<bool,bool>(false,false);
}

MouseEventTransition::MouseEventTransition(QEvent::Type type,Qt::MouseButton button,Qt::KeyboardModifiers modifiers, State* target, bool swallow_event,  TransitionGuard*  guard):
  TransitionBase(target,guard),
  type_(type),
  button_(button),
  modifiers_(modifiers),
  swallow_(swallow_event)
{
}
std::pair<bool,bool> MouseEventTransition::checkEvent(QMouseEvent* event)
{
// only gets called if active
  assert(is_active_());
  if(event->type()==type_ && event->button()==button_ && event->modifiers() == modifiers_ && guard_->check()){
    trigger_();
    return std::pair<bool,bool>(true,swallow_);
  }
  return std::pair<bool,bool>(false,false);
}


}}//ns
