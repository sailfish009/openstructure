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
#ifndef OST_GFX_IMPL_BACKBONE_TRACE_HH
#define OST_GFX_IMPL_BACKBONE_TRACE_HH

/*
  Authors: Ansgar Philippsen, Marco Biasini
 */

#include <ost/mol/mol.hh>

#include <ost/gfx/module_config.hh>
#include <ost/gfx/impl/entity_detail.hh>

namespace ost { namespace gfx { namespace impl {

class BackboneTraceBuilder;

/// \internal
/// \brief a smoothed backbone trace
/// 
/// This class can be used to calculate a smoothed backbone trace for an 
/// entity view.
class DLLEXPORT_OST_GFX BackboneTrace {
public:

  BackboneTrace(const mol::EntityView& ent);
  BackboneTrace();
  
  int GetListCount() const;
  
  const NodeEntryList& GetList(int index) const;
  NodeEntryList& GetList(int index);
  
  void SetView(const mol::EntityView& ent);
  
  void AddNodeList(const NodeEntryList& entries);  

  static void PrepList(NodeEntryList& nelist);
  
  void Rebuild();
  
private:  
  mol::EntityView      view_;
  NodeEntryListList    node_list_list_;
};

/// \internal
/// \brief a subset of a node list
class DLLEXPORT_OST_GFX NodeListSubset {
public:
  friend class TraceSubset;
  
  NodeListSubset(BackboneTrace& trace, int index);
  
  int GetSize() const 
  {
     return indices_.size();
  }
  int AtStart() const 
  { 
    return at_start_;
  }
  
  int AtEnd() const 
  { 
    return at_end_;
  }  
  const NodeEntry& operator [](int index) const 
  {
    assert(index>=0 && index<static_cast<int>(indices_.size()));    
    return trace_.GetList(list_index_)[indices_[index]];
  }
  NodeEntry& operator [](int index)
  {
    assert(index>=0 && index<static_cast<int>(indices_.size()));    
    return trace_.GetList(list_index_)[indices_[index]];
  }  
  NodeListSubset& operator=(const NodeListSubset& rhs);    
private:
  BackboneTrace&    trace_;
  int               list_index_;
protected:
  std::vector<int>  indices_;
  int               at_start_;
  int               at_end_;
};

/// \brief a subset of the trace
class DLLEXPORT_OST_GFX TraceSubset {
public:
  TraceSubset(BackboneTrace& trace, const mol::EntityView& view, int n);
  TraceSubset(BackboneTrace& trace, int n);
  
  const NodeListSubset& operator[](int index) const
  {
    return lists_[index];
  }
  
  NodeListSubset& operator[](int index)
  {
    return lists_[index];
  }
  
  int GetSize() const
  {
    return lists_.size();
  }
  void SetOvershoot(int n) { overshoot_=n; }
  int GetOvershoot() const { return overshoot_; }
  TraceSubset& operator=(const TraceSubset& rhs);  
  void Update(const mol::EntityView& view);
private:
  void NodeListStart(NodeListSubset& nl, int c);
  void NodeListEnd(NodeListSubset& nl, int c, int s);
  BackboneTrace& trace_;
  std::vector<NodeListSubset> lists_;
  int overshoot_;
};

}}}

#endif
