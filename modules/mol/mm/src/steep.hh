#ifndef OST_STEEP_H_
#define OST_STEEP_H_

#include <OpenMM.h>
#include <ost/base.hh>
#include <cmath>
#include <sstream>
#include <vector>
#include <iostream>
#include <cstdio>
#include <ctime>

#include <ost/message.hh>


namespace ost { namespace mol { namespace mm{


class Steep {
public:
  static void Minimize(OpenMM::Context& context, double tolerance = 100, uint maxIterations = 1000);
};

}}}

#endif 
