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

#include <ost/mol/mm/reference_density_kernels.hh>
#include <ost/mol/mm/density_force.hh>
#include <openmm/OpenMMException.h>
#include <openmm/internal/ContextImpl.h>
#include <openmm/reference/RealVec.h>
#include <openmm/reference/ReferencePlatform.h>

using namespace OpenMM;
using namespace std;

namespace{

static vector<RealVec>& extractPositions(ContextImpl& context) {
  ReferencePlatform::PlatformData* data = 
  reinterpret_cast<ReferencePlatform::PlatformData*>(context.getPlatformData());
  return *((vector<RealVec>*) data->positions);
}

static vector<RealVec>& extractForces(ContextImpl& context) {
    ReferencePlatform::PlatformData* data = 
    reinterpret_cast<ReferencePlatform::PlatformData*>(context.getPlatformData());
    return *((vector<RealVec>*) data->forces);
}

} //anon ns


namespace ost{ namespace mol { namespace mm{

ReferenceCalcDensityForceKernel::~ReferenceCalcDensityForceKernel(){
  if(data_ != NULL) delete [] data_;
}

void ReferenceCalcDensityForceKernel::initialize(const System& system, const DensityForce& force) {

  if(system.getNumParticles() != force.getNumParticles()){
    std::string err = "DensityForce must contain same number of";
    err += " particles as system!";
    throw OpenMM::OpenMMException(err);
  }

  if(data_ != NULL) delete [] data_;

  ost::img::ImageHandle density = force.getDensity();

  x_sampling_ = density.GetSpatialSampling()[0] * 0.1;
  y_sampling_ = density.GetSpatialSampling()[1] * 0.1;
  z_sampling_ = density.GetSpatialSampling()[2] * 0.1;
  one_over_x_sampling_ = Real(1.0) / x_sampling_;
  one_over_y_sampling_ = Real(1.0) / y_sampling_;
  one_over_z_sampling_ = Real(1.0) / z_sampling_;
  half_x_sampling_ = Real(0.5) * x_sampling_;
  half_y_sampling_ = Real(0.5) * y_sampling_;
  half_z_sampling_ = Real(0.5) * z_sampling_;
  x_origin_ = density.GetAbsoluteOrigin()[0] * 0.1;
  y_origin_ = density.GetAbsoluteOrigin()[1] * 0.1;
  z_origin_ = density.GetAbsoluteOrigin()[2] * 0.1;
  x_extent_ = density.GetSize()[0];
  y_extent_ = density.GetSize()[1];
  z_extent_ = density.GetSize()[2];

  data_ = new Real[x_extent_ * y_extent_ * z_extent_];
  int current_idx = 0;
  for(int i = 0; i < x_extent_; ++i){
    for(int j = 0; j < y_extent_; ++j){
      for(int k = 0; k < z_extent_; ++k){
        data_[current_idx] = density.GetReal(ost::img::Point(i,j,k));
        ++current_idx;
      }
    }
  }

  idx_helper_one_ = y_extent_ * z_extent_;
  idx_helper_two_ = z_extent_;

  //sigma for gaussian like representations of the atoms
  resolution_ = force.getResolution() * 0.1;
  s_ = resolution_ * 0.425;
  one_s_ = Real(1.0) / s_;
  square_one_s_ = Real(1.0) / (s_ * s_);
  one_sqrt_2_pi_s_ = Real(1.0) / std::sqrt(Real(2.0) * Real(M_PI) * s_);
  padding_dist_ = s_ * Real(3.0);
  scaling_ = 10000 * force.getScaling();

  //let's get body and particle info...

  num_particles_ = system.getNumParticles();
  num_bodies_ = force.getNumBodies();

  particle_masses_.resize(num_particles_);

  for(int i = 0; i < num_particles_; ++i){
    particle_masses_[i] = force.getParticleMass(i);
  }

  bodies_.clear();
  //let's set every particle not belonging to a body to its own body
  for(int i = 0; i < num_particles_; ++i){
    if(force.isInBody(i)) continue;
      std::vector<int> body(1,i);
      bodies_.push_back(body);
  }

  //let's fill the defined bodies!
  for(int i = 0; i < num_bodies_; ++i){
    const std::vector<int>& current_body = force.getBody(i);
    bodies_.push_back(current_body);
  }

  //let's update the num_bodies_ !!!
  num_bodies_ = bodies_.size();
}

double ReferenceCalcDensityForceKernel::execute(ContextImpl& context, 
                                                bool includeForces, 
                                                bool includeEnergy) {



  vector<RealVec>& pos = extractPositions(context);
  vector<RealVec>& force = extractForces(context);

  //distance of a density position towards a particle
  Real x_d;
  Real y_d;
  Real z_d;

  //derivative of a full body
  Real body_dx;
  Real body_dy;
  Real body_dz;

  //return value for the full energy, will be updated when iterating over
  //all bodies
  Real overall_energy = 0.0;

  //some helper variables
  Real dd;
  Real e_term;
  Real density;
  Real p_density;
  std::vector<Real> p_dx;
  std::vector<Real> p_dy;
  std::vector<Real> p_dz;
  std::vector<Real> p_m;
  Real denominator;
  Real nominator;
  Real a, b;
  Real temp_one, temp_two;
  Real prefac;
  Real min_x_pos, max_x_pos;
  Real min_y_pos, max_y_pos;
  Real min_z_pos, max_z_pos;
  Real body_x_origin, body_y_origin, body_z_origin;
  int body_x_extent, body_y_extent, body_z_extent;
  int num_voxels;

  for(int body_idx = 0; body_idx < num_bodies_; ++body_idx){
        
    int body_size = bodies_[body_idx].size();

    //set derivatives for every particle in each direction to zero
    //and extract the required masses
    p_dx.assign(body_size,0.0);
    p_dy.assign(body_size,0.0);
    p_dz.assign(body_size,0.0);
    p_m.resize(body_size);

    for(int p_idx = 0; p_idx < body_size; ++p_idx){
      p_m[p_idx] = particle_masses_[bodies_[body_idx][p_idx]];
    }

    min_x_pos = std::numeric_limits<Real>::max();
    max_x_pos = -std::numeric_limits<Real>::max();
    min_y_pos = std::numeric_limits<Real>::max();
    max_y_pos = -std::numeric_limits<Real>::max();
    min_z_pos = std::numeric_limits<Real>::max();
    max_z_pos = -std::numeric_limits<Real>::max();

    //let's find out how big this thing is...
    for(int p_idx = 0; p_idx < body_size; ++p_idx){
      RealVec& p = pos[bodies_[body_idx][p_idx]];
      min_x_pos = (p[0] < min_x_pos) ? p[0]: min_x_pos;
      max_x_pos = (p[0] > max_x_pos) ? p[0]: max_x_pos;
      min_y_pos = (p[1] < min_y_pos) ? p[1]: min_y_pos;
      max_y_pos = (p[1] > max_y_pos) ? p[1]: max_y_pos;
      min_z_pos = (p[2] < min_z_pos) ? p[2]: min_z_pos;
      max_z_pos = (p[2] > max_z_pos) ? p[2]: max_z_pos;
    }

    //we add some padding
    min_x_pos -= padding_dist_;
    max_x_pos += padding_dist_;
    min_y_pos -= padding_dist_;
    max_y_pos += padding_dist_;
    min_z_pos -= padding_dist_;
    max_z_pos += padding_dist_;

    body_x_extent = std::max(3, 
      static_cast<int>(std::ceil((max_x_pos - min_x_pos)*one_over_x_sampling_)));
    body_y_extent = std::max(3, 
      static_cast<int>(std::ceil((max_y_pos - min_y_pos)*one_over_y_sampling_)));
    body_z_extent = std::max(3, 
      static_cast<int>(std::ceil((max_z_pos - min_z_pos)*one_over_z_sampling_)));
    num_voxels = body_x_extent * body_y_extent * body_z_extent;

    Real overlap = min_x_pos + body_x_extent * x_sampling_ - max_x_pos;
    body_x_origin = min_x_pos - 0.5 * overlap;
    overlap = min_y_pos + body_y_extent * y_sampling_ - max_y_pos;
    body_y_origin = min_y_pos - 0.5 * overlap;
    overlap = min_z_pos + body_z_extent * z_sampling_ - max_z_pos;
    body_z_origin = min_z_pos - 0.5 * overlap;


    density_values_.resize(num_voxels);
    body_values_.assign(num_voxels,0.0);

    //let's fill the density values
    int current_idx = 0;
    for(int i = 0; i < body_x_extent; ++i){
        for(int j = 0; j < body_y_extent; ++j){
            for(int k = 0; k < body_z_extent; ++k){
                density_values_[current_idx] = 
                this->GetIntpolValue(body_x_origin + i * x_sampling_ + half_x_sampling_,
                                     body_y_origin + j * y_sampling_ + half_y_sampling_,
                                     body_z_origin + k * z_sampling_ + half_z_sampling_);
                ++current_idx;
            }
        }
    }

    for(int p_idx = 0; p_idx < body_size; ++p_idx){

      //the actual position of the particle
      RealVec& p = pos[bodies_[body_idx][p_idx]];
      current_idx = 0;

      for(int i = 0; i < body_x_extent; ++i){

        x_d = p[0] - (body_x_origin + i * x_sampling_ + half_x_sampling_);

        for(int j = 0; j < body_y_extent; ++j){

          y_d = p[1] - (body_y_origin + j * y_sampling_ + half_y_sampling_);

          for(int k = 0; k < body_z_extent; ++k){

            z_d = p[2] - (body_z_origin + k * z_sampling_ + half_z_sampling_);
                        
            dd = x_d * x_d + y_d * y_d + z_d * z_d;

            e_term = std::exp(Real(-0.5) * dd * square_one_s_);
            density = density_values_[current_idx];
            p_dx[p_idx] += (density * (x_d * one_s_) * e_term);
            p_dy[p_idx] += (density * (y_d * one_s_) * e_term);
            p_dz[p_idx] += (density * (z_d * one_s_) * e_term);
            p_density = p_m[p_idx] * one_sqrt_2_pi_s_ * e_term;
            body_values_[current_idx] += p_density;
            ++current_idx;

          }
        }
      }
    }

    nominator = 0.0;
    denominator = 0.0;
    temp_one = 0.0;
    temp_two = 0.0;

    for(int i = 0; i < num_voxels; ++i){
      a = density_values_[i];
      b = body_values_[i];
      nominator += (a * b);
      temp_one += (a * a);
      temp_two += (b * b);
    }        

    if(temp_one == 0.0 || temp_two == 0.0) continue;

    denominator = std::sqrt(temp_one * temp_two);
    prefac = scaling_ / denominator * one_sqrt_2_pi_s_; 

    body_dx = 0.0;
    body_dy = 0.0;
    body_dz = 0.0;

    for(int p_idx = 0; p_idx < body_size; ++p_idx){
      body_dx -= prefac * p_m[p_idx] * p_dx[p_idx];
      body_dy -= prefac * p_m[p_idx] * p_dy[p_idx];
      body_dz -= prefac * p_m[p_idx] * p_dz[p_idx];
    }

    //finally update the forces accordingly
    for(int p_idx = 0; p_idx < body_size; ++p_idx){
      force[bodies_[body_idx][p_idx]][0] += body_dx;
      force[bodies_[body_idx][p_idx]][1] += body_dy;
      force[bodies_[body_idx][p_idx]][2] += body_dz;
    }

    overall_energy -= (scaling_ * nominator/denominator);
  }

  return overall_energy;
}

void ReferenceCalcDensityForceKernel::copyParametersToContext(ContextImpl& context, const DensityForce& force) {
  //ToDo
}

}}} //ns
