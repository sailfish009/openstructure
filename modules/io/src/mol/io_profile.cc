#include "io_profile.hh"

namespace ost { namespace io {

IOProfileRegistry& IOProfileRegistry::Instance()
{
  static IOProfileRegistry reg;
  return reg;
}

IOProfileRegistry::IOProfileRegistry()
{
  profiles_["DEFAULT"]=IOProfile();
}

}}
