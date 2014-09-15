#include <boost/python.hpp>
#include <ost/mol/mm/index.hh>

using namespace boost::python;

namespace{

template<typename T>
int Get(T index, uint i){
  return index[i];
}

template<typename T>
void Set(T& index, uint i, uint v){
  index[i] = v;
}
}

void export_Index()
{
  class_<ost::mol::mm::Index<1> >("Index1", init<uint>())
    .def("__getitem__",Get<ost::mol::mm::Index<1> >)
    .def("__setitem__",Set<ost::mol::mm::Index<1> >)
  ;

  class_<ost::mol::mm::Index<2> >("Index2", init<uint, uint>())
    .def("__getitem__",Get<ost::mol::mm::Index<2> >)
    .def("__setitem__",Set<ost::mol::mm::Index<2> >)
  ;

  class_<ost::mol::mm::Index<3> >("Index3", init<uint, uint, uint>())
    .def("__getitem__",Get<ost::mol::mm::Index<3> >)
    .def("__setitem__",Set<ost::mol::mm::Index<3> >)
  ;

  class_<ost::mol::mm::Index<4> >("Index4", init<uint, uint, uint,uint>())
    .def("__getitem__",Get<ost::mol::mm::Index<4> >)
    .def("__setitem__",Set<ost::mol::mm::Index<4> >)
  ;

  class_<ost::mol::mm::Index<5> >("Index5", init<uint, uint, uint, uint, uint>())
    .def("__getitem__",Get<ost::mol::mm::Index<5> >)
    .def("__setitem__",Set<ost::mol::mm::Index<5> >)
  ;

  class_<ost::mol::mm::Index<6> >("Index6", init<uint, uint, uint, uint, uint, uint>())
    .def("__getitem__",Get<ost::mol::mm::Index<6> >)
    .def("__setitem__",Set<ost::mol::mm::Index<6> >)
  ;

  class_<ost::mol::mm::Index<7> >("Index7", init<uint, uint, uint, uint, uint, uint, uint>())
    .def("__getitem__",Get<ost::mol::mm::Index<7> >)
    .def("__setitem__",Set<ost::mol::mm::Index<7> >)
  ;
}
