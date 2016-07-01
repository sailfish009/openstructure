/* -------------------------------------------------------------------------- *
 *                                   OpenMM                                   *
 * -------------------------------------------------------------------------- *
 * This is part of the OpenMM molecular simulation toolkit originating from   *
 * Simbios, the NIH National Center for Physics-Based Simulation of           *
 * Biological Structures at Stanford, funded under the NIH Roadmap for        *
 * Medical Research, grant U54 GM072970. See https://simtk.org.               *
 *                                                                            *
 * Portions copyright (c) 2014 Stanford University and the Authors.           *
 * Authors: Peter Eastman                                                     *
 * Contributors:                                                              *
 *                                                                            *
 * Permission is hereby granted, free of charge, to any person obtaining a    *
 * copy of this software and associated documentation files (the "Software"), *
 * to deal in the Software without restriction, including without limitation  *
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,   *
 * and/or sell copies of the Software, and to permit persons to whom the      *
 * Software is furnished to do so, subject to the following conditions:       *
 *                                                                            *
 * The above copyright notice and this permission notice shall be included in *
 * all copies or substantial portions of the Software.                        *
 *                                                                            *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    *
 * THE AUTHORS, CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,    *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR      *
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE  *
 * USE OR OTHER DEALINGS IN THE SOFTWARE.                                     *
 * -------------------------------------------------------------------------- */

#include <ost/mol/mm/density_force.hh>
#include <ost/mol/mm/density_force_impl.hh>
#include <openmm/OpenMMException.h>
#include <openmm/internal/AssertionUtilities.h>
#include <string.h>

namespace ost{ namespace mol{ namespace mm{

OpenMM::ForceImpl* DensityForce::createImpl() const {
    return new DensityForceImpl(*this);
}

void DensityForce::updateParametersInContext(OpenMM::Context& context) {
    dynamic_cast<DensityForceImpl&>(getImplInContext(context)).updateParametersInContext(getContextImpl(context));
}

void DensityForce::addParticle(Real mass){
  masses_.push_back(mass);
  in_body_.push_back(false);
}

Real DensityForce::getParticleMass(int idx) const{
  if(idx >= this->getNumParticles() || idx < 0){
    throw OpenMM::OpenMMException("Invalid index provided!");
  }
  return masses_[idx];
}

bool DensityForce::isInBody(int idx) const{
  if(idx >= this->getNumParticles() || idx < 0){
    throw OpenMM::OpenMMException("Invalid index provided!");
  }
  return in_body_[idx];
}

void DensityForce::defineBody(const std::vector<int>& indices){
  //first check, whether any of the indices is invalid or already part of a body
  int num_particles = this->getNumParticles();
  for(std::vector<int>::const_iterator i = indices.begin();
      i != indices.end(); ++i){
    if(*i >= num_particles || *i < 0){
      throw OpenMM::OpenMMException("Invalid index provided!");
    }
    if(in_body_[*i]){
      throw OpenMM::OpenMMException("Particle can be part of only one body!");
    }
  }

  //let's change the state of the according particles
  for(std::vector<int>::const_iterator i = indices.begin();
      i != indices.end(); ++i){
    in_body_[*i] = true;
  }

  //and finally define the body
  bodies_.push_back(indices);
}

const std::vector<int>& DensityForce::getBody(int idx) const{
  if(idx >= this->getNumBodies() || idx < 0){
    throw OpenMM::OpenMMException("Invalid index provided!");
  }
  return bodies_[idx];
}

}}} //ns
