#include <ost/symmetry.hh>
#include <ost/img/xtal_map.hh>

#include <boost/python.hpp>

using namespace boost::python;
using namespace ost::img;
using namespace ost;

void export_XtalMap()
{
  class_<XtalMap, XtalMapPtr>("XtalMap", init<const ImageHandle&,
                                              const UnitCell&, const Size&>())
    .def("GetReal", &XtalMap::GetReal)
    .def("SetReal", &XtalMap::SetReal)
    .def("IndexToCoord", &XtalMap::IndexToCoord)
    .def("Extract", &XtalMap::Extract)
    .def("GetUnitCell", &XtalMap::GetUnitCell, 
         return_value_policy<copy_const_reference>())
    .add_property("unit_cell", make_function(XtalMap:.GetUnitCell, 
                  return_value_policy<copy_const_reference>()))
  ;

}