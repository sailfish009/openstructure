#ifndef OST_CONOP_COMPOUND_LIB_BASE_HH
#define OST_CONOP_COMPOUND_LIB_BASE_HH

#include "compound.hh"

namespace ost { namespace conop {

class CompoundLibBase;
typedef boost::shared_ptr<CompoundLibBase> CompoundLibBasePtr;

class DLLEXPORT_OST_CONOP CompoundLibBase {
public:
  virtual ~CompoundLibBase() {}
  virtual CompoundPtr FindCompound(const String& id, 
                                   Compound::Dialect dialect) const = 0;
};

}}
#endif

