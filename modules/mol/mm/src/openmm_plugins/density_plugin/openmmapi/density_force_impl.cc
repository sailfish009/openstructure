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

#include <ost/mol/mm/density_force_impl.hh>
#include <ost/mol/mm/density_kernels.hh>
#include <openmm/OpenMMException.h>
#include <openmm/internal/ContextImpl.h>
#include <cmath>
#include <map>
#include <set>
#include <sstream>

namespace ost{ namespace mol{ namespace mm{

DensityForceImpl::DensityForceImpl(const DensityForce& owner) : owner(owner) {
}

DensityForceImpl::~DensityForceImpl() {
}

void DensityForceImpl::initialize(OpenMM::ContextImpl& context) {
    kernel = context.getPlatform().createKernel(CalcDensityForceKernel::Name(), context);
    kernel.getAs<CalcDensityForceKernel>().initialize(context.getSystem(), owner);
}

double DensityForceImpl::calcForcesAndEnergy(OpenMM::ContextImpl& context, bool includeForces, bool includeEnergy, int groups) {
    if ((groups&(1<<owner.getForceGroup())) != 0)
        return kernel.getAs<CalcDensityForceKernel>().execute(context, includeForces, includeEnergy);
    return 0.0;
}

std::vector<std::string> DensityForceImpl::getKernelNames() {
    std::vector<std::string> names;
    names.push_back(CalcDensityForceKernel::Name());
    return names;
}

void DensityForceImpl::updateParametersInContext(OpenMM::ContextImpl& context) {
    kernel.getAs<CalcDensityForceKernel>().copyParametersToContext(context, owner);
}

}}} //ns

