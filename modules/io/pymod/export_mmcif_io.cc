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
#include <boost/shared_ptr.hpp>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
using namespace boost::python;

#include <ost/io/mol/io_profile.hh>
#include <ost/io/mol/mmcif_reader.hh>
#include <ost/io/mol/mmcif_info.hh>
using namespace ost;
using namespace ost::io;
using namespace ost::mol;

void export_mmcif_io()
{
  class_<MMCifParser, boost::noncopyable>("MMCifParser", init<const String&, EntityHandle&, const IOProfile&>())
    .def("Parse", &MMCifParser::Parse)
    .def("SetRestrictChains", &MMCifParser::SetRestrictChains)
    .def("SetReadCanonicalSeqRes", &MMCifParser::SetReadCanonicalSeqRes)
    .def("GetSeqRes", &MMCifParser::GetSeqRes)
    .def("GetInfo", make_function(&MMCifParser::GetInfo,
                                  return_value_policy<copy_const_reference>()))
    .add_property("restrict_chains",
                  make_function(&MMCifParser::GetRestrictChains,
                                return_value_policy<copy_const_reference>()),
                  &MMCifParser::SetRestrictChains)
    .add_property("seqres", &MMCifParser::GetSeqRes)
    .add_property("read_seqres", &MMCifParser::GetReadSeqRes, 
                  &MMCifParser::SetReadSeqRes)
    .add_property("info", make_function(&MMCifParser::GetInfo,
                                   return_value_policy<copy_const_reference>()))
    ;

  class_<MMCifInfoCitation>("MMCifInfoCitation", init<>())
    .def("SetID", &MMCifInfoCitation::SetID)
    .def("GetID", &MMCifInfoCitation::GetID)
    .def("SetCAS", &MMCifInfoCitation::SetCAS)
    .def("GetCAS", &MMCifInfoCitation::GetCAS)
    .def("SetISBN", &MMCifInfoCitation::SetISBN)
    .def("GetISBN", &MMCifInfoCitation::GetISBN)
    .def("SetPublishedIn", &MMCifInfoCitation::SetPublishedIn)
    .def("GetPublishedIn", &MMCifInfoCitation::GetPublishedIn)
    .def("SetVolume", &MMCifInfoCitation::SetVolume)
    .def("GetVolume", &MMCifInfoCitation::GetVolume)
    .def("SetPageFirst", &MMCifInfoCitation::SetPageFirst)
    .def("GetPageFirst", &MMCifInfoCitation::GetPageFirst)
    .def("SetPageLast", &MMCifInfoCitation::SetPageLast)
    .def("GetPageLast", &MMCifInfoCitation::GetPageLast)
    .def("SetDOI", &MMCifInfoCitation::SetDOI)
    .def("GetDOI", &MMCifInfoCitation::GetDOI)
    .def("SetPubMed", &MMCifInfoCitation::SetPubMed)
    .def("GetPubMed", &MMCifInfoCitation::GetPubMed)
    .def("SetYear", &MMCifInfoCitation::SetYear)
    .def("GetYear", &MMCifInfoCitation::GetYear)
    .def("SetTitle", &MMCifInfoCitation::SetTitle)
    .def("GetTitle", &MMCifInfoCitation::GetTitle)
    .def("SetAuthorList", &MMCifInfoCitation::SetAuthorList)
    .def("GetAuthorList", make_function(&MMCifInfoCitation::GetAuthorList,
                                   return_value_policy<copy_const_reference>()))
    .add_property("id", &MMCifInfoCitation::GetID, &MMCifInfoCitation::SetID)
    .add_property("cas", &MMCifInfoCitation::GetCAS, &MMCifInfoCitation::SetCAS)
    .add_property("isbn", &MMCifInfoCitation::GetISBN,
                  &MMCifInfoCitation::SetISBN)
    .add_property("published_in", &MMCifInfoCitation::GetPublishedIn,
                  &MMCifInfoCitation::SetPublishedIn)
    .add_property("volume", &MMCifInfoCitation::GetVolume,
                  &MMCifInfoCitation::SetVolume)
    .add_property("page_first", &MMCifInfoCitation::GetPageFirst,
                  &MMCifInfoCitation::SetPageFirst)
    .add_property("page_last", &MMCifInfoCitation::GetPageLast,
                  &MMCifInfoCitation::SetPageLast)
    .add_property("doi", &MMCifInfoCitation::GetDOI, &MMCifInfoCitation::SetDOI)
    .add_property("pubmed", &MMCifInfoCitation::GetPubMed,
                  &MMCifInfoCitation::SetPubMed)
    .add_property("year", &MMCifInfoCitation::GetYear,
                  &MMCifInfoCitation::SetYear)
    .add_property("title", &MMCifInfoCitation::GetTitle,
                  &MMCifInfoCitation::SetTitle)
    .add_property("authors", make_function(&MMCifInfoCitation::GetAuthorList,
                                   return_value_policy<copy_const_reference>()),
                  &MMCifInfoCitation::SetAuthorList)
  ;

  typedef std::vector<MMCifInfoCitation> MMCifInfoCitationList;
  class_<std::vector<MMCifInfoCitation> >("MMCifInfoCitationList", init<>())
    .def(vector_indexing_suite<std::vector<MMCifInfoCitation> >())
  ;


  class_<MMCifInfoTransOp, MMCifInfoTransOpPtr>("MMCifInfoTransOp", init<>())
    .def("SetID", &MMCifInfoTransOp::SetID)
    .def("GetID", &MMCifInfoTransOp::GetID)
    .def("SetType", &MMCifInfoTransOp::SetType)
    .def("GetType", &MMCifInfoTransOp::GetType)
    .def("SetVector", &MMCifInfoTransOp::SetVector)
    .def("GetVector", &MMCifInfoTransOp::GetVector)
    .def("SetMatrix", &MMCifInfoTransOp::SetMatrix)
    .def("GetMatrix", &MMCifInfoTransOp::GetMatrix)
    .add_property("id", &MMCifInfoTransOp::GetID,
                  &MMCifInfoTransOp::SetID)
    .add_property("type", &MMCifInfoTransOp::GetType,
                  &MMCifInfoTransOp::SetType)
    .add_property("translation", &MMCifInfoTransOp::GetVector,
                  &MMCifInfoTransOp::SetVector)
    .add_property("rotation", &MMCifInfoTransOp::GetMatrix,
                  &MMCifInfoTransOp::SetMatrix)
  ;

  typedef std::vector<MMCifInfoTransOp> MMCifInfoTransOpList;
  class_<std::vector<MMCifInfoTransOp> >("MMCifInfoTransOpList", init<>())
    .def(vector_indexing_suite<std::vector<MMCifInfoTransOp> >())
  ;

  typedef std::vector<MMCifInfoTransOpPtr> MMCifInfoTransOpPtrList;
  class_<std::vector<MMCifInfoTransOpPtr> >("MMCifInfoTransOpPtrList", init<>())
    .def(vector_indexing_suite<std::vector<MMCifInfoTransOpPtr>, true >())
  ;

  typedef std::vector<MMCifInfoTransOpPtrList > MMCifInfoTransOpPtrListList;
  class_<std::vector<MMCifInfoTransOpPtrList > >("MMCifInfoTransOpPtrListList",
                                                init<>())
    .def(vector_indexing_suite<std::vector<MMCifInfoTransOpPtrList >, true >())
  ;

  class_<MMCifInfoBioUnit>("MMCifInfoBioUnit", init<>())
    .def("SetDetails", &MMCifInfoBioUnit::SetDetails)
    .def("GetDetails", &MMCifInfoBioUnit::GetDetails)
    .def("AddChain", &MMCifInfoBioUnit::AddChain)
    .def("GetChainList", make_function(&MMCifInfoBioUnit::GetChainList,
                                   return_value_policy<copy_const_reference>()))
    .def("AddOperations", &MMCifInfoBioUnit::AddOperations)
    .def("GetOperations", make_function(&MMCifInfoBioUnit::GetOperations,
                                   return_value_policy<copy_const_reference>()))
    .add_property("details", &MMCifInfoBioUnit::GetDetails,
                  &MMCifInfoBioUnit::SetDetails)
    .add_property("chains", make_function(&MMCifInfoBioUnit::GetChainList,
                                   return_value_policy<copy_const_reference>()))
    .add_property("operations", make_function(&MMCifInfoBioUnit::GetOperations,
                                   return_value_policy<copy_const_reference>()))
  ;

  typedef std::vector<MMCifInfoBioUnit> MMCifInfoBioUnitList;
  class_<std::vector<MMCifInfoBioUnit> >("MMCifInfoBioUnitList", init<>())
    .def(vector_indexing_suite<std::vector<MMCifInfoBioUnit> >())
  ;

  class_<MMCifInfo>("MMCifInfo", init<>())
    .def("AddCitation", &MMCifInfo::AddCitation)
    .def("GetCitations", make_function(&MMCifInfo::GetCitations,
                                   return_value_policy<copy_const_reference>()))
    .def("AddBioUnit", &MMCifInfo::AddBioUnit)
    .def("GetBioUnits", make_function(&MMCifInfo::GetBioUnits,
                                   return_value_policy<copy_const_reference>()))
    .def("SetMethod", &MMCifInfo::SetMethod)
    .def("GetMethod", &MMCifInfo::GetMethod)
    .def("SetResolution", &MMCifInfo::SetResolution)
    .def("GetResolution", &MMCifInfo::GetResolution)
    .def("AddAuthorsToCitation", &MMCifInfo::AddAuthorsToCitation)
    .def("AddOperation", &MMCifInfo::AddOperation)
    .def("GetOperations", make_function(&MMCifInfo::GetOperations,
                                   return_value_policy<copy_const_reference>()))
    .add_property("citations", make_function(&MMCifInfo::GetCitations,
                                   return_value_policy<copy_const_reference>()))
    .add_property("biounits", make_function(&MMCifInfo::GetBioUnits,
                                   return_value_policy<copy_const_reference>()))
    .add_property("method", &MMCifInfo::GetMethod, &MMCifInfo::SetMethod)
    .add_property("resolution", &MMCifInfo::GetResolution,
                  &MMCifInfo::SetResolution)
    .add_property("operations", make_function(&MMCifInfo::GetOperations,
                                   return_value_policy<copy_const_reference>()))
  ;
}
