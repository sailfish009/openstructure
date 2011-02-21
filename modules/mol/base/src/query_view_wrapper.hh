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
#ifndef OST_QUERY_VIEW_WRAPPER_HH
#define OST_QUERY_VIEW_WRAPPER_HH

#include <ost/mol/view_type_fw.hh>
#include <ost/mol/module_config.hh>
#include <ost/mol/entity_view.hh>
#include <ost/mol/entity_handle.hh>
#include <ost/mol/query.hh>

namespace ost { namespace mol {

class DLLEXPORT_OST_MOL QueryViewWrapper {
public:
  explicit QueryViewWrapper(const EntityHandle& entity_handle);
  explicit QueryViewWrapper(const EntityView& entity_view);
  QueryViewWrapper();
  QueryViewWrapper(const Query& query, const EntityHandle& handle);
  QueryViewWrapper(const Query& query, const EntityView& view = mol::EntityView());
  QueryViewWrapper(const Query& query, QueryFlags flags, const EntityHandle& handle);
  QueryViewWrapper(const Query& query, QueryFlags flags, const EntityView& view = mol::EntityView());
  EntityView GetEntityView() const;
  bool DependsOnQuery() const;
  bool IsDataValid() const;

  void SetQuery(const Query& query);
  const Query& GetQuery() const;

  void SetFlags(QueryFlags flags);
  QueryFlags GetFlags() const;

private:
  bool view_set_;
  EntityHandle entity_handle_;
  EntityView entity_view_;
  Query query_;
  QueryFlags flags_;
};

} } // ns

#endif
