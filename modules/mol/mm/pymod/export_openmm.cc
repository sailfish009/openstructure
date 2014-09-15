//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2011 by the OpenStructure authors
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 3.0 of the License, or (at your option)
// any later version.
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//------------------------------------------------------------------------------
#include <OpenMM.h>
#include <boost/python.hpp>
#include <ost/mol/mm/simulation.hh>

using namespace boost::python;
using namespace ost::mol::mm;


void export_OpenMM()
{

  //we have to tell boost, that the Integrator class is around...
  class_<OpenMM::Integrator, boost::noncopyable>("Integrator",no_init)
    .def("GetConstraintTolerance",&OpenMM::Integrator::getConstraintTolerance)
    .def("SetConstraintTolerance",&OpenMM::Integrator::setConstraintTolerance)

  ;
  boost::python::register_ptr_to_python<ost::mol::mm::IntegratorPtr>();


  class_<OpenMM::VerletIntegrator, bases<OpenMM::Integrator> >("VerletIntegrator", init<Real>());
  boost::python::register_ptr_to_python<ost::mol::mm::VerletIntegratorPtr>();


  class_<OpenMM::BrownianIntegrator, bases<OpenMM::Integrator> >("BrownianIntegrator", init<Real,Real,Real>())
    .def("GetTemperature",&OpenMM::BrownianIntegrator::getTemperature)
    .def("SetTemperature",&OpenMM::BrownianIntegrator::setTemperature)
    .def("GetFriction",&OpenMM::BrownianIntegrator::getFriction)
    .def("SetFriction",&OpenMM::BrownianIntegrator::setFriction)
    .def("GetRandomNumberSeed",&OpenMM::BrownianIntegrator::getRandomNumberSeed)
    .def("SetRandomNumberSeed",&OpenMM::BrownianIntegrator::setRandomNumberSeed)
  ;
  boost::python::register_ptr_to_python<ost::mol::mm::BrownianIntegratorPtr>();


  class_<OpenMM::LangevinIntegrator, bases<OpenMM::Integrator> >("LangevinIntegrator", init<Real,Real,Real>())
    .def("GetTemperature",&OpenMM::LangevinIntegrator::getTemperature)
    .def("SetTemperature",&OpenMM::LangevinIntegrator::setTemperature)
    .def("GetFriction",&OpenMM::LangevinIntegrator::getFriction)
    .def("SetFriction",&OpenMM::LangevinIntegrator::setFriction)
    .def("GetRandomNumberSeed",&OpenMM::LangevinIntegrator::getRandomNumberSeed)
    .def("SetRandomNumberSeed",&OpenMM::LangevinIntegrator::setRandomNumberSeed)
  ;
  boost::python::register_ptr_to_python<ost::mol::mm::LangevinIntegratorPtr>();


  class_<OpenMM::VariableVerletIntegrator, bases<OpenMM::Integrator> >("VariableVerletIntegrator",init<Real>())
    .def("GetErrorTolerance", &OpenMM::VariableVerletIntegrator::getErrorTolerance)
    .def("SetErrorTolerance", &OpenMM::VariableVerletIntegrator::setErrorTolerance)
  ;
  boost::python::register_ptr_to_python<ost::mol::mm::VariableVerletIntegratorPtr>();


  class_<OpenMM::VariableLangevinIntegrator, bases<OpenMM::Integrator> >("VaribaleLangevinIntegrator", init<Real,Real,Real>())
    .def("GetTemperature",&OpenMM::VariableLangevinIntegrator::getTemperature)
    .def("SetTemperature",&OpenMM::VariableLangevinIntegrator::setTemperature)
    .def("GetFriction",&OpenMM::VariableLangevinIntegrator::getFriction)
    .def("SetFriction",&OpenMM::VariableLangevinIntegrator::setFriction)
    .def("GetRandomNumberSeed",&OpenMM::VariableLangevinIntegrator::getRandomNumberSeed)
    .def("SetRandomNumberSeed",&OpenMM::VariableLangevinIntegrator::setRandomNumberSeed)
    .def("GetErrorTolerance",&OpenMM::VariableLangevinIntegrator::getErrorTolerance)
    .def("SetErrorTolerance",&OpenMM::VariableLangevinIntegrator::setErrorTolerance)
  ;
  boost::python::register_ptr_to_python<ost::mol::mm::VariableLangevinIntegratorPtr>();

  enum_<OpenMM::NonbondedForce::NonbondedMethod>("NonbondedMethod")
    .value("NoCutoff", OpenMM::NonbondedForce::NoCutoff)
    .value("CutoffNonPeriodic", OpenMM::NonbondedForce::CutoffNonPeriodic)
    .value("CutoffPeriodic", OpenMM::NonbondedForce::CutoffPeriodic)
    .value("Ewald", OpenMM::NonbondedForce::Ewald)
    .value("PME", OpenMM::NonbondedForce::PME)
  ;


}