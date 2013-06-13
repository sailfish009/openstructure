#include "io_profile.hh"

namespace ost { namespace io {

IOProfileRegistry& IOProfileRegistry::Instance()
{
  static IOProfileRegistry reg;
  return reg;
}

IOProfileRegistry::IOProfileRegistry():
  profiles_()
{
  profiles_["DEFAULT"]=IOProfile();
}

}}
