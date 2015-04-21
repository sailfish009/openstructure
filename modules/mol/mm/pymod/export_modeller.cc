#include <boost/python.hpp>
#include <ost/mol/mm/block_modifiers.hh>
#include <ost/mol/mm/modeller.hh>

using namespace boost::python;


void export_Modeller()
{

  class_<ost::mol::mm::Modeller>("MMModeller", no_init)
    .def("GenerateDisulfidBonds",&ost::mol::mm::Modeller::GenerateDisulfidBonds,(arg("ent"))).staticmethod("GenerateDisulfidBonds")
    .def("GenerateCYSHEMEBonds",&ost::mol::mm::Modeller::GenerateCYSHEMEBonds,(arg("ent"))).staticmethod("GenerateCYSHEMEBonds")
    .def("GenerateHISHEMEBonds",&ost::mol::mm::Modeller::GenerateHISHEMEBonds,(arg("ent"))).staticmethod("GenerateHISHEMEBonds")
    .def("GenerateMETHEMEBonds",&ost::mol::mm::Modeller::GenerateDisulfidBonds,(arg("ent"))).staticmethod("GenerateMETHEMEBonds")
    .def("AssignPDBNaming",&ost::mol::mm::Modeller::AssignPDBNaming,(arg("ent"))).staticmethod("AssignPDBNaming")
    .def("AssignGromacsNaming",&ost::mol::mm::Modeller::AssignGromacsNaming,(arg("ent"))).staticmethod("AssignGromacsNaming")
  ;
  
}