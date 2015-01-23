#ifndef OST_TOPOLOGY_CREATOR_HH
#define OST_TOPOLOGY_CREATOR_HH

#include <OpenMM.h>

#include <vector>
#include <map>
#include <set>
#include <math.h>

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


namespace ost { namespace mol{ namespace mm{

class TopologyCreator;
typedef boost::shared_ptr<TopologyCreator> TopologyCreatorPtr;

class TopologyCreator {
public:
  static TopologyPtr Create(ost::mol::EntityHandle& ent, 
                            const MMSettingsPtr settings);

};


}}}//ns

#endif