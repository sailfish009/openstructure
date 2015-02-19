#ifndef OST_SYSTEM_CREATOR_HH
#define OST_SYSTEM_CREATOR_HH

#include <vector>
#include <map>
#include <set>

#include <ost/message.hh>
#include <ost/mol/entity_handle.hh>
#include <ost/mol/residue_handle.hh>
#include <ost/mol/atom_handle.hh>
#include <ost/mol/mm/forcefield.hh>
#include <ost/mol/mm/buildingblock.hh>
#include <ost/mol/mm/block_modifiers.hh>
#include <ost/mol/mm/mm_interaction.hh>
#include <ost/mol/mm/mm_settings.hh>
#include <ost/mol/mm/mm_modeller.hh>
#include <ost/mol/mm/index.hh>
#include <ost/mol/mm/topology.hh>


#include <time.h>


namespace OpenMM{
  class System;  //hacky way of telling the System is around.
                 //will be included in source file to avoid
                 //dependencies on external libraries
}

namespace ost { namespace mol{ namespace mm{

typedef boost::shared_ptr<OpenMM::System> SystemPtr;

class SystemCreator {
public:
  static SystemPtr Create(const TopologyPtr top, 
                          const MMSettingsPtr settings,
                          std::map<FuncType,uint>& mapper);

};


}}}//ns

#endif