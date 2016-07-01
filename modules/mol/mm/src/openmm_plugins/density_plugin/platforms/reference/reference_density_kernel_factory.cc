/* -------------------------------------------------------------------------- *
 *                              OpenMMExample                                   *
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

#include <ost/mol/mm/reference_density_kernel_factory.hh>
#include <ost/mol/mm/reference_density_kernels.hh>
#include <openmm/reference/ReferencePlatform.h>
#include <openmm/internal/ContextImpl.h>
#include <openmm/OpenMMException.h>

using namespace ost::mol::mm;

extern "C" void registerPlatforms() {
}

extern "C" void registerKernelFactories() {
    for (int i = 0; i < OpenMM::Platform::getNumPlatforms(); i++) {
        OpenMM::Platform& platform = OpenMM::Platform::getPlatform(i);
        if (dynamic_cast<OpenMM::ReferencePlatform*>(&platform) != NULL) {
            ReferenceDensityKernelFactory* factory = new ReferenceDensityKernelFactory();
            platform.registerKernelFactory(CalcDensityForceKernel::Name(), factory);
        }
    }
}

extern "C" void registerDensityReferenceKernelFactories() {
    registerKernelFactories();
}

OpenMM::KernelImpl* ReferenceDensityKernelFactory::createKernelImpl(std::string name, const OpenMM::Platform& platform, OpenMM::ContextImpl& context) const {
    OpenMM::ReferencePlatform::PlatformData& data = *static_cast<OpenMM::ReferencePlatform::PlatformData*>(context.getPlatformData());
    if (name == CalcDensityForceKernel::Name())
        return new ReferenceCalcDensityForceKernel(name, platform);
    throw OpenMM::OpenMMException((std::string("Tried to create kernel with illegal kernel name '")+name+"'").c_str());
}
