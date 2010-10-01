#ifndef OST_MOL_ALG_LOCAL_DIST_TEST_HH
#define OST_MOL_ALG_LOCAL_DIST_TEST_HH

#include <ost/mol/entity_view.hh>
#include <ost/mol/alg/module_config.hh>

namespace ost { namespace mol { namespace alg {
  
Real DLLEXPORT_OST_MOL_ALG LocalDistTest(const EntityView& mdl,
                                         const EntityView& ref,
                                         Real cutoff, Real max_dist);
}}}

#endif

