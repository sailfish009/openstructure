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
#ifndef OST_GUI_FILE_LOADER_HH
#define OST_GUI_FILE_LOADER_HH
#include <vector>

#include <QString>
#include <QList>
#include <QMap>

#include <ost/gfx/gfx_object.hh>
#include <ost/gui/module_config.hh>
#include <ost/gui/remote_site_loader.hh>
#include <ost/gui/loader_manager.hh>
#include <ost/io/io_exception.hh>

namespace ost { namespace gui {

class DLLEXPORT_OST_GUI FileLoader {
private:
  FileLoader();
  static void HandleError(Message m, gfx::GfxObjP obj);
  static gfx::GfxObjP TryLoadEntity(const QString& filename);
  static gfx::GfxObjP TryLoadMap(const QString& filename) throw(io::IOException);
  static gfx::GfxObjP TryLoadSurface(const QString& filename);
  static void RunScript(const QString& filename);
  static void LoadPDB(const QString& filename);
  virtual ~FileLoader();

  static LoaderManagerPtr loader_manager_;

#if OST_IMG_ENABLED
  static QList<img::ImageHandle> loaded_images_;
#endif

public:
  static void LoadObject(const QString& file_name);
  static void LoadFrom(const QString& id, const QString& site=QString());
  static std::vector<String> GetSiteLoaderIdents();
  static LoaderManagerPtr GetLoaderManager();
};

} }

#endif /* OST_GUI_FILE_LOADER_HH */
