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
//
#include <boost/python.hpp>
#include <ost/conop/processor.hh>
using namespace ost;

using namespace ost::conop;
using namespace boost::python;

struct PyProcessor : public Processor {};
struct WrappedProcessor : public PyProcessor, public wrapper<WrappedProcessor> {
  WrappedProcessor(PyObject* self): self_(self)
  { }
  virtual void DoProcess(DiagnosticsPtr diag, mol::EntityHandle ent) const
  {
    call_method<void>(self_, "DoProcess", diag, ent);
  }
  DiagnosticsPtr DoProcessDefault(DiagnosticsPtr diag, 
                                  mol::EntityHandle ent) const {
    return DiagnosticsPtr();
  }
  ProcessorPtr Copy() const { 
    return call_method<ProcessorPtr>(self_, "Copy"); 
  }
  virtual String ToString() const {
    return call_method<String>(self_, "ToString");
  }
  String ToStringDefault() const { return ""; }
  ProcessorPtr CopyDefault() const { return ProcessorPtr(); }
  
  PyObject* self_;
};


void export_processor() {
  
  enum_<Dialect>("Dialect")
    .value("PDB_DIALECT", PDB_DIALECT)
    .value("CHARMM_DIALECT", CHARMM_DIALECT)
    .export_values()
  ;
  enum_<ConopAction>("ConopAction")
    .value("CONOP_WARN", CONOP_WARN)
    .value("CONOP_SILENT", CONOP_SILENT)
    .value("CONOP_REMOVE", CONOP_REMOVE)
    .value("CONOP_REMOVE_ATOM", CONOP_REMOVE_ATOM)
    .value("CONOP_REMOVE_RESIDUE", CONOP_REMOVE_RESIDUE)
    .value("CONOP_FATAL", CONOP_FATAL)
    .export_values()
  ;
  class_<Processor, ProcessorPtr, boost::noncopyable>("_Processor", no_init)
    .def("Copy", &Processor::Copy)
    .add_property("check_bond_feasibility", 
                  &Processor::GetCheckBondFeasibility,
                  &Processor::SetCheckBondFeasibility)
    .add_property("connect", &Processor::GetConnect,
                 &Processor::SetConnect)
    .add_property("assign_torsions", &Processor::GetAssignTorsions,
                 &Processor::SetAssignTorsions)
    .def("Process", &Processor::Process, 
         (arg("ent"), arg("log_diags")=true))
  ;
  class_<PyProcessor, boost::noncopyable, 
         boost::shared_ptr<WrappedProcessor>, 
         bases<Processor> >("Processor")
    .def("Copy", &WrappedProcessor::CopyDefault)
    .def("DoProcess", &WrappedProcessor::DoProcessDefault)
    .def("ToString", &WrappedProcessor::ToStringDefault)
  ;
  def("IsBondFeasible", &IsBondFeasible);
}

