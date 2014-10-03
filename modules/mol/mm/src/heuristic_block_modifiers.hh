#ifndef HEURISTIC_BLOCK_MODIFIERS
#define HEURISTIC_BLOCK_MODIFIERS


#include <boost/shared_ptr.hpp>

#include <ost/message.hh>
#include <ost/mol/bond_handle.hh>
#include <ost/mol/residue_handle.hh>
#include <ost/mol/entity_handle.hh>
#include <ost/mol/atom_handle.hh>
#include <ost/mol/xcs_editor.hh>
#include <ost/geom/vec3.hh>
#include <ost/mol/mm/block_modifiers.hh>
#include <ost/mol/mm/buildingblock.hh>


namespace ost{ namespace mol{ namespace mm{

class HeuristicHydrogenConstructor;
class HeuristicTerminiConstructor;
class HeuristicBlockModifier;

typedef boost::shared_ptr<HeuristicHydrogenConstructor> HeuristicHydrogenConstructorPtr;
typedef boost::shared_ptr<HeuristicTerminiConstructor> HeuristicTerminiConstructorPtr;
typedef boost::shared_ptr<HeuristicBlockModifier> HeuristicBlockModifierPtr; 


class HeuristicHydrogenConstructor : public HydrogenConstructor{

public:

  HeuristicHydrogenConstructor( BuildingBlockPtr block): building_block_(block) { }

  virtual void ApplyOnBuildingBlock(BuildingBlockPtr);

  virtual void ApplyOnResidue(ost::mol::ResidueHandle& res, ost::mol::XCSEditor& ed);

  virtual void OnSave(ost::io::BinaryDataSink& ds) { ds << *this; }

  virtual BlockModifierType GetBlockModifierType() { return HeuristicBlockModifiers; }
  
  template <typename DS>
  
  void Serialize(DS& ds){

  }

private:
  BuildingBlockPtr building_block_;
  std::vector<std::vector<String> > hydrogen_names_;
  std::vector<std::vector<String> > anchor_atom_names_;
  
};


class HeuristicBlockModifier : public BlockModifier{

public:

  HeuristicBlockModifier() { }

  virtual void ApplyOnBuildingBlock(BuildingBlockPtr p);

  virtual void ApplyOnResidue(ost::mol::ResidueHandle& res, ost::mol::XCSEditor& ed);

  virtual BlockModifierType GetBlockModifierType() { return HeuristicBlockModifiers; }
  
  virtual void OnSave(ost::io::BinaryDataSink& ds) { ds << *this; }

  template <typename DS>
  
  void Serialize(DS& ds){
    
  }

};



}}}

#endif
