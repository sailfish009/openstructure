#ifndef BLOCK_MODIFIERS
#define BLOCK_MODIFIERS

#include <ost/mol/residue_handle.hh>
#include <ost/mol/mm/buildingblock.hh>
#include <boost/shared_ptr.hpp>
#include <ost/mol/xcs_editor.hh>
#include <ost/io/binary_data_source.hh>
#include <ost/io/binary_data_sink.hh>

namespace ost{ namespace mol{ namespace mm{

class HydrogenConstructor;
class TerminiConstructor;
class BlockModifier;

typedef boost::shared_ptr<HydrogenConstructor> HydrogenConstructorPtr;
typedef boost::shared_ptr<TerminiConstructor> TerminiConstructorPtr;
typedef boost::shared_ptr<BlockModifier> BlockModifierPtr;

typedef enum {
  GromacsBlockModifiers
} BlockModifierType;

class HydrogenConstructor{
public:
  HydrogenConstructor() { }
  virtual void ApplyOnBuildingBlock(BuildingBlockPtr p) = 0;
  virtual void ApplyOnResidue(ost::mol::ResidueHandle& res, ost::mol::XCSEditor& ed) = 0;
  virtual void OnSave(ost::io::BinaryDataSink& ds) = 0;
  virtual BlockModifierType GetBlockModifierType() = 0;
};

class TerminiConstructor{
public:
  TerminiConstructor() { }
  virtual void ApplyOnBuildingBlock(BuildingBlockPtr p) = 0;
  virtual void ApplyOnResidue(ost::mol::ResidueHandle& res, ost::mol::XCSEditor& ed) = 0;
  virtual void OnSave(ost::io::BinaryDataSink& ds) = 0;
  virtual BlockModifierType GetBlockModifierType() = 0;
};

class BlockModifier{
public:
  BlockModifier() { }
  virtual void ApplyOnBuildingBlock(BuildingBlockPtr p) = 0;
  virtual void ApplyOnResidue(ost::mol::ResidueHandle& res, ost::mol::XCSEditor& ed) = 0;
  virtual void OnSave(ost::io::BinaryDataSink& ds) = 0;
  virtual BlockModifierType GetBlockModifierType() = 0;
};


}}} //ns

#endif