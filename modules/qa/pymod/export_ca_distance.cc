//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2010 by the OpenStructure authors
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
#include <boost/python.hpp>


#include <ost/qa/ca_distance.hh>

using namespace ost::qa;  
using namespace ost;

///\todo put this into a header? seems like other might need this as well.
namespace boost{

    template<class T>
    inline T* get_pointer( boost::shared_ptr<const T> const& p ){
        return const_cast< T* >( p.get() );
    }

}

namespace boost{ namespace python{

    template<class T>
    struct pointee< boost::shared_ptr<T const> >{
        typedef T type;
    };

} } //boost::python

namespace utils{

    template< class T >
    void register_shared_ptrs_to_python(){
        namespace bpl = boost::python;
        bpl::register_ptr_to_python< boost::shared_ptr< T > >();
        bpl::register_ptr_to_python< boost::shared_ptr< const T > >();
        bpl::implicitly_convertible< boost::shared_ptr< T >, 
             boost::shared_ptr< const T > >();
    }

}
using namespace boost::python;

void export_CaDistance()
{
  def("CalculateCalphaDistance", &CalculateCalphaDistance);
  
  class_<CalphaDistance, boost::noncopyable>("CalphaDistance", no_init)
    .def("IsDistanceDefined", &CalphaDistance::IsDistanceDefined, 
         args("number_a", "number_b"))
    .def("GetDistance", &CalphaDistance::GetDistance, 
         args("number_a", "number_b"))
  ;
  utils::register_shared_ptrs_to_python<CalphaDistance>();
}
