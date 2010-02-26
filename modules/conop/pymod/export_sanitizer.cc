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
#include <boost/python/register_ptr_to_python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
using namespace boost::python;
#include <ost/conop/sanitizer.hh>
using namespace ost;
using namespace ost::conop;

namespace {
struct WrappedSanitizer : Sanitizer
{
    WrappedSanitizer(PyObject *p)
        : Sanitizer(CompoundLibPtr()), self(p) {
    }

    WrappedSanitizer(PyObject *p, const Sanitizer& s)
        : Sanitizer(s), self(p) {}

    virtual void OnUnknownAtom(const mol::AtomHandle& atom) {
      call_method<void, mol::AtomHandle>(self, "OnUnknownAtom", atom);
    }
    void OnUnknownAtomDefault(const mol::AtomHandle& atom) {
    }
    virtual void OnMissingAtom(const mol::ResidueHandle& residue,
                               const String& atom) {
      call_method<void, mol::ResidueHandle, String>(self, "OnMissingAtom",
                                               residue, atom);
    }
    void OnMissingAtomDefault(const mol::ResidueHandle& residue,
                              const String& atom) {
    }
 private:
    PyObject* self;
};

}

void export_Sanitizer() {
  class_<Sanitizer, WrappedSanitizer, bases<mol::EntityVisitor>,boost::noncopyable >("Sanitizer", init<CompoundLibPtr>())
    .def("OnUnknownAtom", &WrappedSanitizer::OnUnknownAtomDefault)
    .def("OnMissingAtom", &WrappedSanitizer::OnMissingAtomDefault)
  ;
}
