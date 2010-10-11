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
#ifndef OST_GUI_LOADER_MANAGER_HH
#define OST_GUI_LOADER_MANAGER_HH
#include <vector>

#include <boost/shared_ptr.hpp>

#include <QString>
#include <QMap>

#include <ost/gui/remote_site_loader.hh>

namespace ost { namespace gui {

class DLLEXPORT_OST_GUI LoaderManager {

  QMap<QString,RemoteSiteLoader*> site_loaders_;
  QActionGroup* site_actions_;
  QMenu* site_menu_;
  QString default_site_;

public:
  LoaderManager();
  virtual ~LoaderManager();
  std::vector<String> GetSiteLoaderIdents();
  void AddRemoteSiteLoader(const QString& ident, RemoteSiteLoader* site_loader);
  void RemoveRemoteSiteLoader(const QString& ident);
  RemoteSiteLoader* GetRemoteSiteLoader(const QString& ident);
  RemoteSiteLoader* GetCurrentSiteLoader();
  RemoteSiteLoader* GetDefaultRemoteSiteLoader();
  QString GetDefaultRemoteSiteIdent();
  void SetDefaultRemoteSiteIdent(const QString& ident);
  QMenu* GetSiteMenu();
};

typedef boost::shared_ptr<LoaderManager> LoaderManagerPtr;

} }

#endif /* OST_GUI_LOADER_MANAGER_HH */
