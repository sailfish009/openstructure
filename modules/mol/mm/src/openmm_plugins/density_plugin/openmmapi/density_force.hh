#ifndef OPENMM_DENSITYFORCE_H_
#define OPENMM_DENSITYFORCE_H_

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

#include <openmm/Context.h>
#include <openmm/Force.h>
#include <ost/img/image.hh>

namespace ost { namespace mol{ namespace mm{


class DensityForce : public OpenMM::Force {
public:
    /**
     * Create an DensityForce. The density force only makes sense for simulation boxes
     * with orthogonal base vectors.
     */

    //since the density is scaled in Angstrom, the resolution is also in Angstrom
    DensityForce(const ost::img::ImageHandle& d, Real r, Real s): density_(d),
                                                                   resolution_(r),
                                                                   scaling_(s) { }

    void addParticle(Real mass);

    int getNumParticles() const { return masses_.size(); }

    int getNumBodies() const { return bodies_.size(); }

    Real getParticleMass(int idx) const;

    bool isInBody(int idx) const;

    void defineBody(const std::vector<int>& indices);

    const std::vector<int>& getBody(int idx) const;

    void setScaling(Real scaling) { scaling_ = scaling; }

    Real getScaling() const { return scaling_; }

    ost::img::ImageHandle getDensity() const { return density_; }

    Real getResolution() const { return resolution_; }

    void updateParametersInContext(OpenMM::Context& context);

    bool usesPeriodicBoundaryConditions() const { return false; }
      
protected:
    OpenMM::ForceImpl* createImpl() const;
private:

    ost::img::ImageHandle density_;
    std::vector<Real> masses_;
    std::vector<bool> in_body_;
    std::vector<std::vector<int> > bodies_;
    Real resolution_;
    Real scaling_;
};

}}} // ns

#endif /*OPENMM_DENSITYFORCE_H_*/
