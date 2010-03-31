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
#include <boost/python/slice.hpp>
#include <boost/python/register_ptr_to_python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
using namespace boost::python;

#include <ost/info/info.hh>
#include <ost/mol/mol.hh>
#include <ost/seq/sequence_handle.hh>
#include <ost/seq/alignment_handle.hh>
#include <ost/seq/sequence_op.hh>
#include <ost/seq/sequence_list.hh>
#include <ost/seq/aligned_region.hh>
#include "const_seq_list_export_def.hh"

using namespace ost;
using namespace ost::seq;

namespace {

void (SequenceHandle::*attach_one)(const mol::EntityView&)=&SequenceHandle::AttachView;
void (SequenceHandle::*attach_two)(const mol::EntityView&,
                             const String&)=&SequenceHandle::AttachView;

void (AlignmentHandle::*attach_view_a)(int, const mol::EntityView&)=&AlignmentHandle::AttachView;
void (AlignmentHandle::*attach_view_b)(int, const mol::EntityView&,
                             const String&)=&AlignmentHandle::AttachView;
SequenceHandle (*seq_from_chain_a)(const String&,const mol::ChainView&)=&SequenceFromChain;
SequenceHandle (*seq_from_chain_b)(const String&,const mol::ChainHandle&)=&SequenceFromChain;

template <typename T>
T do_slice(const T& t, slice& sl) {
  int start=0, end=t.GetCount();
  try {
    start=extract<int>(sl.start());
  } catch(error_already_set) {
    PyErr_Clear();
  }
  try {
    end=extract<int>(sl.stop());
  } catch(error_already_set) {
    PyErr_Clear();    
  }  
  return t.Slice(start, end-start);
}

ConstSequenceList do_slice_a(ConstSequenceList& t, slice sl)
{
  return do_slice<ConstSequenceList>(t, sl);
}

SequenceList do_slice_b(SequenceList& t, slice sl)
{
  return do_slice<SequenceList>(t, sl);
}


AlignedRegion slice_aln(const AlignmentHandle& aln, slice sl) {
  int start=0, end=aln.GetLength();
  try {
    start=extract<int>(sl.start());
  } catch(error_already_set) {
    PyErr_Clear();
  }
  try {
    end=extract<int>(sl.stop());
  } catch(error_already_set) {
    PyErr_Clear();    
  }  
  return aln.MakeRegion(start, end-start);
}

}

void export_sequence()
{
  class_<ConstSequenceHandle>("ConstSequenceHandle", init<>())
    .def("GetResidueIndex", &ConstSequenceHandle::GetResidueIndex)
    .def("GetPos", &ConstSequenceHandle::GetPos)
    .def("GetLength", &ConstSequenceHandle::GetLength)
    .def("GetResidue", &ConstSequenceHandle::GetResidue)
    .def("GetOneLetterCode", &ConstSequenceHandle::GetOneLetterCode)  
    .def("GetSequenceOffset", &ConstSequenceHandle::GetSequenceOffset)    
    .def("Copy", &ConstSequenceHandle::Copy)
    .def("GetFirstNonGap", &ConstSequenceHandle::GetFirstNonGap)
    .def("GetLastNonGap", &ConstSequenceHandle::GetLastNonGap)    
    .add_property("first_non_gap", &SequenceHandle::GetFirstNonGap)
    .add_property("last_non_gap", &SequenceHandle::GetLastNonGap)    
    .def("GetAttachedView", &ConstSequenceHandle::GetAttachedView)
    .def("GetGaplessString", &ConstSequenceHandle::GetGaplessString)
    .def("GetString", &ConstSequenceHandle::GetString,
         return_value_policy<copy_const_reference>())
         .def("GetName", &ConstSequenceHandle::GetName,
              return_value_policy<copy_const_reference>())
    .def("HasAttachedView", &ConstSequenceHandle::HasAttachedView)
    .def("__len__", &SequenceHandle::GetLength)
    .add_property("length", &SequenceHandle::GetLength)    
    .add_property("attached_view", &ConstSequenceHandle::GetAttachedView)
    .add_property("name",
                  make_function(&ConstSequenceHandle::GetName,
                                return_value_policy<copy_const_reference>()))
    .add_property("sequence_offset", &ConstSequenceHandle::GetSequenceOffset)
    .add_property("gapless_string", &ConstSequenceHandle::GetGaplessString)
    .add_property("string",
                  make_function(&ConstSequenceHandle::GetString,
                                return_value_policy<copy_const_reference>()))
    .def("__str__", &ConstSequenceHandle::GetString, 
         return_value_policy<copy_const_reference>())
  ;    
  
  class_<SequenceHandle, bases<ConstSequenceHandle> >("SequenceHandle", init<>())
    .def("SetSequenceOffset", &SequenceHandle::SetSequenceOffset)
    .def("AttachView", attach_one)
    .def("AttachView", attach_two)
    .def("SetString", &SequenceHandle::SetString)
    .add_property("string",
                  make_function(&SequenceHandle::GetString,
                                return_value_policy<copy_const_reference>()),
                  &SequenceHandle::SetString)
    .def("SetName", &SequenceHandle::SetName)
    .add_property("name",
                  make_function(&SequenceHandle::GetName,
                                return_value_policy<copy_const_reference>()),
                  &SequenceHandle::SetName)
    .add_property("sequence_offset", &SequenceHandle::GetSequenceOffset,
                  &SequenceHandle::SetSequenceOffset)    
  ;
  def("CreateSequence", &CreateSequence);
  /*class_<SequenceHandleList>("SequenceHandleList", init<>())
    .def(vector_indexing_suite<SequenceHandleList>())
  ;*/
  class_<AlignmentHandle>("AlignmentHandle", init<>())
    .def("GetCount", &AlignmentHandle::GetCount)
    .add_property("sequence_count", &AlignmentHandle::GetCount)
    .def("GetSequence", &AlignmentHandle::GetSequence)
    .def("GetPos", &AlignmentHandle::GetPos)
    .def("GetResidueIndex", &AlignmentHandle::GetResidueIndex)
    .def("AddSequence", &AlignmentHandle::AddSequence)
    .def("FindSequence", &AlignmentHandle::FindSequence)
    .def("Copy", &AlignmentHandle::Copy)
    .def("ToString", &AlignmentHandle::ToString)
    .def("GetLength", &AlignmentHandle::GetLength)
    .def("GetSequences", &AlignmentHandle::GetSequences)
    .def("AttachView", attach_view_a)
    .def("AttachView", attach_view_b)
    .def("Cut", &AlignmentHandle::Cut)
    .def("Replace",&AlignmentHandle::Replace)
    .def("__getitem__", &slice_aln)
    .def("__getitem__", &AlignmentHandle::operator[])
    .def("__iter__", iterator<AlignmentHandle>())
    .add_property("sequences", &AlignmentHandle::GetSequences)
  ;
  class_<AlignedColumn>("AlignedColumn", no_init)
    .def("GetIndex", &AlignedColumn::GetIndex)
    .def("__getitem__", &AlignedColumn::operator[])
    .def("GetRowCount", &AlignedColumn::GetRowCount)
    .def(self_ns::str(self))
  ;
  class_<AlignedRegion>("AlignedRegion", no_init)
    .def("Delete", &AlignedRegion::Delete)
    .def("Replace", &AlignedRegion::Replace)
    .def("ShiftLeft", &AlignedRegion::ShiftLeft)
    .def("ShiftRight", &AlignedRegion::ShiftRight)
    .def("GetStart", &AlignedRegion::GetStart)
    .def("GetEnd", &AlignedRegion::GetEnd)
    .def("GetLength", &AlignedRegion::GetLength)
    .def("SetMaster", &AlignedRegion::SetMaster)
    .def("GetMaster", &AlignedRegion::GetMaster)
    .def("GetAlignmentHandle",&AlignedRegion::GetAlignmentHandle)
    .add_property("master", &AlignedRegion::GetMaster, 
                  &AlignedRegion::SetMaster)
    .def("__getitem__", &AlignedRegion::operator[])                  
    .def("__len__", &AlignedRegion::GetLength)
    .def("__iter__", iterator<AlignedRegion>())
    .add_property("start", &AlignedRegion::GetStart)
    .add_property("end", &AlignedRegion::GetEnd)    
  ;
  class_<ConstSequenceList>("ConstSequenceList", init<>())
    CONST_SEQ_LIST_DEF(ConstSequenceList)
    .def("__getitem__", &do_slice_a)
  ;
  class_<SequenceList>("SequenceList", init<>())
    CONST_SEQ_LIST_DEF(SequenceList)
    .def("__getitem__", &do_slice_b)    
  ;  
  class_<AlignmentList>("AlignmentList", init<>())
    .def(vector_indexing_suite<AlignmentList>())
    .def("__getitem__", &do_slice_b)    
  ;  
  implicitly_convertible<SequenceList, ConstSequenceList>();
  def("CreateSequenceList", &CreateSequenceList);
  def("SequenceFromChain", seq_from_chain_a);
  def("SequenceFromChain", seq_from_chain_b);    
  def("SequenceToInfo", &SequenceToInfo);
  def("SequenceListToInfo", &SequenceListToInfo);
  def("SequenceFromInfo", &SequenceFromInfo);
  def("CreateAlignment", &CreateAlignment);
  def("AlignmentFromSequenceList", &AlignmentFromSequenceList);
  def("SequenceListFromInfo", &SequenceListFromInfo);  
}
