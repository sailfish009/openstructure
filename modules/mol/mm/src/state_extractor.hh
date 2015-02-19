#ifndef OST_STATE_EXTRACTOR_HH
#define OST_STATE_EXTRACTOR_HH

#include <vector>

#include <boost/shared_ptr.hpp>
#include <ost/geom/vec3.hh>

namespace OpenMM{
  class Context; //hacky way of telling the Context is around.
                 //will be included in source file to avoid
                 //dependencies on external libraries

}

namespace ost { namespace mol{ namespace mm{

class StateExtractor{

public:

  static void ExtractPotentialEnergy(boost::shared_ptr<OpenMM::Context> context,
  	                                 Real& energy);

  static void ExtractKineticEnergy(boost::shared_ptr<OpenMM::Context> context,
  	                               Real& energy);

  static void ExtractEnergy(boost::shared_ptr<OpenMM::Context> context,
  	                        Real& energy);

  static void ExtractPositions(boost::shared_ptr<OpenMM::Context> context,
  	                           geom::Vec3List& positions,
                               bool enforce_periodic_box = false,
                               bool in_angstrom = true);

  static void ExtractVelocities(boost::shared_ptr<OpenMM::Context> context,
  	                            geom::Vec3List& velocities);

  static void ExtractForces(boost::shared_ptr<OpenMM::Context> context,
  	                        geom::Vec3List& forces);

};


}}} //ns

#endif
