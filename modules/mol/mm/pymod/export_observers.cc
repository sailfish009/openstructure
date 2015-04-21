#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>
#include <ost/mol/mm/observer.hh>

using namespace boost::python;


void export_Observers()
{

  class_<ost::mol::mm::Observer, boost::noncopyable>("Observer", no_init);

  class_<ost::mol::mm::TrajObserver, bases<ost::mol::mm::Observer> >("TrajObserver", init<int>())
    .def("GetTraj", &ost::mol::mm::TrajObserver::GetTraj)
  ;

  //noncopyable, because TrajWriter contains a unique ofstream
  class_<ost::mol::mm::TrajWriter, boost::noncopyable, bases<ost::mol::mm::Observer> >("TrajWriter", init<int, String, String>())
    .def("Finalize", &ost::mol::mm::TrajWriter::Finalize)
  ;

  boost::python::register_ptr_to_python<ost::mol::mm::ObserverPtr>();
  boost::python::register_ptr_to_python<ost::mol::mm::TrajObserverPtr>();
  boost::python::register_ptr_to_python<ost::mol::mm::TrajWriterPtr>();
}