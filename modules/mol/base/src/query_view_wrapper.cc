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
#include "query_view_wrapper.hh"

#include <ost/mol/view_type_fw.hh>

namespace ost { namespace mol {

QueryViewWrapper::QueryViewWrapper(const EntityHandle& entity_handle):
    view_set_(false),
    entity_handle_(entity_handle),
    entity_view_(),
    query_(){}

QueryViewWrapper::QueryViewWrapper(const EntityView& entity_view):
    view_set_(true),
    entity_handle_(),
    entity_view_(entity_view),
    query_(){}

QueryViewWrapper::QueryViewWrapper(const Query& query, 
                                   const EntityHandle& entity_handle):
    view_set_(false),
    entity_handle_(entity_handle),
    entity_view_(),
    query_(query)
{ }

QueryViewWrapper::QueryViewWrapper(const Query& query, 
                                   const EntityView& view):
    view_set_(true),
    entity_handle_(),
    entity_view_(view),
    query_(query)
{ }

EntityView QueryViewWrapper::GetEntityView() const
{
  if(view_set_) {
    return query_.MatchAll() ? entity_view_ : entity_view_.Select(query_);
  } else {
    return entity_handle_.Select(query_);
  }
}

bool QueryViewWrapper::DependsOnQuery() const
{
  return query_.MatchAll();
}

} } // ns
