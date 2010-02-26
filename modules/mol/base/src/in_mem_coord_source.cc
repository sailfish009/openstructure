#include "in_mem_coord_source.hh"

namespace ost { namespace mol {


InMemCoordSource::InMemCoordSource(const AtomHandleList& atoms):
  CoordSource(atoms)
{
  this->SetMutable(true);
}
  
uint InMemCoordSource::GetFrameCount()
{
  return frames_.size();
}

CoordFramePtr InMemCoordSource::GetFrame(uint frame_id)
{
  
  return frame_id>=frames_.size() ? CoordFramePtr() : frames_[frame_id];
}

void InMemCoordSource::AddFrame(const CoordFramePtr& frame)
{
  frames_.push_back(frame);
}

void InMemCoordSource::AddFrame(const std::vector<geom::Vec3>& coords)
{
  frames_.push_back(CoordFramePtr(new CoordFrame(0.0)));
  frames_.back()->insert(frames_.back()->end(), coords.begin(), coords.end());
}

}}
