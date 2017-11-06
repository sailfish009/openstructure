#ifndef REFERENCE_DENSITY_KERNELS_H_
#define REFERENCE_DENSITY_KERNELS_H_

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

#include <ost/mol/mm/density_kernels.hh>
#include <openmm/Platform.h>
#include <vector>
#include <set>
#include <limits> 

namespace ost{ namespace mol{ namespace mm{

/**
 * This kernel is invoked by DensityForce to calculate the forces acting on the system and the energy of the system.
 */
class ReferenceCalcDensityForceKernel : public CalcDensityForceKernel {
public:
    ReferenceCalcDensityForceKernel(std::string name, const OpenMM::Platform& platform) : CalcDensityForceKernel(name, platform), data_(NULL) {
    }

    ~ReferenceCalcDensityForceKernel();

    /**
     * Initialize the kernel.
     * 
     * @param system     the System this kernel will be applied to
     * @param force      the DensityForce this kernel will be used for
     */
    void initialize(const OpenMM::System& system, const DensityForce& force);
    /**
     * Execute the kernel to calculate the forces and/or energy.
     *
     * @param context        the context in which to execute this kernel
     * @param includeForces  true if forces should be calculated
     * @param includeEnergy  true if the energy should be calculated
     * @return the potential energy due to the force
     */
    double execute(OpenMM::ContextImpl& context, bool includeForces, bool includeEnergy);
    /**
     * Copy changed parameters over to a context.
     *
     * @param context    the context to copy parameters to
     * @param force      the DensityForce to copy the parameters from
     */
    void copyParametersToContext(OpenMM::ContextImpl& context, const DensityForce& force);
private:

    inline Real GetValue(int a, int b, int c){
      return data_[a * idx_helper_one_ + b * idx_helper_two_ + c];
    }

    inline Real GetIntpolValue(Real x, Real y, Real z){

      //distances to origin
      Real dx = x - x_origin_;
      Real dy = y - y_origin_;
      Real dz = z - z_origin_;

      int x_bin = std::floor(dx * one_over_x_sampling_);
      int y_bin = std::floor(dy * one_over_y_sampling_);
      int z_bin = std::floor(dz * one_over_z_sampling_);

      if(x_bin < 0 || x_bin >= (x_extent_-1) || 
         y_bin < 0 || y_bin >= (y_extent_-1) ||
         z_bin < 0 || z_bin >= (z_extent_-1)) return 0.0;

      //distances in fraction of bin towards base bin
      dx = (dx - x_bin * x_sampling_) * one_over_x_sampling_;
      dy = (dy - y_bin * y_sampling_) * one_over_y_sampling_;
      dz = (dz - z_bin * z_sampling_) * one_over_z_sampling_;

      intpol_values_[0] = this->GetValue(x_bin,y_bin,z_bin);
      intpol_values_[1] = this->GetValue(x_bin+1,y_bin,z_bin);
      intpol_values_[2] = this->GetValue(x_bin,y_bin+1,z_bin);
      intpol_values_[3] = this->GetValue(x_bin+1,y_bin+1,z_bin);
      intpol_values_[4] = this->GetValue(x_bin,y_bin,z_bin+1);
      intpol_values_[5] = this->GetValue(x_bin+1,y_bin,z_bin+1);
      intpol_values_[6] = this->GetValue(x_bin,y_bin+1,z_bin+1);
      intpol_values_[7] = this->GetValue(x_bin+1,y_bin+1,z_bin+1);

      //do first bilinear interpolation
      Real f11 = (1.0 - dx) * intpol_values_[0] + dx * intpol_values_[1];
      Real f12 = (1.0 - dx) * intpol_values_[2] + dx * intpol_values_[3];
      Real f21 = (1.0 - dx) * intpol_values_[4] + dx * intpol_values_[5];
      Real f22 = (1.0 - dx) * intpol_values_[6] + dx * intpol_values_[7];

      Real f1 = (1.0 - dy) * f11 + dy * f12;
      Real f2 = (1.0 - dy) * f21 + dy * f22;

      return (1.0 - dz) * f1 + dz * f2;
    }

    //this stuff is required to store and extract the internal density values
    Real* data_;
    Real intpol_values_[8];
    int idx_helper_one_;
    int idx_helper_two_;
    Real x_sampling_;
    Real y_sampling_;
    Real z_sampling_;
    Real one_over_x_sampling_;
    Real one_over_y_sampling_;
    Real one_over_z_sampling_;
    Real half_x_sampling_;
    Real half_y_sampling_;
    Real half_z_sampling_;
    Real x_origin_;
    Real y_origin_;
    Real z_origin_;
    int x_extent_;
    int y_extent_;
    int z_extent_;

    //this is stuff used in the actual force calculation
    Real resolution_;
    Real s_;
    Real one_s_;
    Real square_one_s_;
    Real one_sqrt_2_pi_s_;
    Real padding_dist_;
    Real scaling_;
    std::vector<Real> density_values_;
    std::vector<Real> body_values_;

    //this is stuff containing information of the simulated system
    int num_bodies_;
    int num_particles_;
    std::vector<float> particle_masses_;
    std::vector<std::vector<int> > bodies_;
};

}}} // ns

#endif /*REFERENCE_DENSITY_KERNELS_H_*/
