#include "message.hh"

namespace ost {
 
Message::Message(const String& mesg,const String& prefix):
  _prefix(prefix), _mesg(mesg) {}

Message::~Message() throw() {}

const char* Message::what() const throw() {
  String msg = _prefix + ": " +_mesg;
  return msg.c_str();
}

Error::Error(const String& m): Message(m,"Error") {}

}