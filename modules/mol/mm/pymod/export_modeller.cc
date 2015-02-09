#include <boost/python.hpp>
#include <ost/mol/mm/block_modifiers.hh>
#include <ost/mol/mm/mm_modeller.hh>

using namespace boost::python;


void export_Modeller()
{

  class_<ost::mol::mm::MMModeller>("MMModeller", no_init)
    .def("GenerateDisulfidBonds",&ost::mol::mm::MMModeller::GenerateDisulfidBonds,(arg("ent"))).staticmethod("GenerateDisulfidBonds")
    .def("GenerateCYSHEMEBonds",&ost::mol::mm::MMModeller::GenerateCYSHEMEBonds,(arg("ent"))).staticmethod("GenerateCYSHEMEBonds")
    .def("GenerateHISHEMEBonds",&ost::mol::mm::MMModeller::GenerateHISHEMEBonds,(arg("ent"))).staticmethod("GenerateHISHEMEBonds")
    .def("GenerateMETHEMEBonds",&ost::mol::mm::MMModeller::GenerateDisulfidBonds,(arg("ent"))).staticmethod("GenerateMETHEMEBonds")
    .def("AssignPDBNaming",&ost::mol::mm::MMModeller::AssignPDBNaming,(arg("ent"))).staticmethod("AssignPDBNaming")
    .def("AssignGromacsNaming",&ost::mol::mm::MMModeller::AssignGromacsNaming,(arg("ent"))).staticmethod("AssignGromacsNaming")

  ;
  
}