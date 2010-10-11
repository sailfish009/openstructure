#include <cassert>
#include "transition.hh"
#include "state.hh"
#include "state_machine.hh"

namespace ost { namespace gui {

State::State():
  QObject(),
  mouse_event_transitions_(),
  key_event_transitions_(),
  automatic_transitions_()
{
}
void State::addTransition(SignalTransition* transition)
{
  transition->setParent(this);
}
void State::addTransition(MouseEventTransition* transition)
{
  transition->setParent(this);
  mouse_event_transitions_.append(transition);
}
void State::addTransition(KeyEventTransition* transition)
{
  transition->setParent(this);
  key_event_transitions_.append(transition);
}
void State::addTransition(AutomaticTransition* transition)
{
  transition->setParent(this);
  automatic_transitions_.append(transition);
}
bool State::isActive()
{
  return parent() && dynamic_cast<StateMachine*>(parent())->isActive(this);
}
void State::setActive()
{
  assert(parent());
  dynamic_cast<StateMachine*>(parent())->setActive(this);
}

bool State::checkEvent(QKeyEvent* event)
{
  for(QList<KeyEventTransition*>::iterator it=key_event_transitions_.begin();it!=key_event_transitions_.end();++it){
    std::pair<bool,bool> pair=(*it)->checkEvent(event);
    if(pair.first){
      return pair.second;
    }
  }
  return false;
}

bool State::checkAutomaticTransitions()
{
  for(QList<AutomaticTransition*>::iterator it=automatic_transitions_.begin();it!=automatic_transitions_.end();++it){
    if((*it)->checkTransition()){
      return true;
    }
  }
  return false;
}

bool State::checkEvent(QMouseEvent* event)
{
  for(QList<MouseEventTransition*>::iterator it=mouse_event_transitions_.begin();it!=mouse_event_transitions_.end();++it){
    std::pair<bool,bool> pair=(*it)->checkEvent(event);
    if(pair.first){
      return pair.second;
    }
  }
  return false;
}
void State::onEntry()
{
  emit entered();
}
void State::onExit()
{
  emit exited();
}

}}//ns
