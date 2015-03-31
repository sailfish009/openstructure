#ifndef OST_STEEP_H_
#define OST_STEEP_H_

#include <ost/base.hh>
#include <cmath>
#include <sstream>
#include <vector>
#include <iostream>
#include <cstdio>
#include <ctime>

#include <ost/message.hh>

namespace OpenMM{
  class Context; //hacky way of telling the Context is around.
                 //will be included in source file to avoid
                 //dependencies on external libraries
}

namespace ost { namespace mol { namespace mm{


class Steep {
public:
  static bool Minimize(OpenMM::Context& context, double tolerance = 100, uint maxIterations = 1000);
};

}}}

#endif 
