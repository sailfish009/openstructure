#ifndef OST_EXPORT_HELPER_VECTOR_HH
#define OST_EXPORT_HELPER_VECTOR_HH

# include <boost/python/def_visitor.hpp>

/*
  Author: Marco Biasini
 */

namespace ost {


template <typename Container>
class VectorAdditions : 
  public boost::python::def_visitor<VectorAdditions<Container> > {
public:
 template <class Class>
 void visit(Class& cl) const
 {
   cl
     .def("__str__", &to_string)
   ;
 }
private:
  static std::string to_string(Container& cl)
  {
    std::stringstream ss;
    ss << "[";
    bool first=true;
    for (typename Container::const_iterator
         i=cl.begin(), e=cl.end(); i!=e; ++i) {
      if (first) {
        first=false;
      } else {
        ss << ", ";
      }
      ss << (*i);
    }
    ss << "]";
    return ss.str();
  }
};

}

#endif
