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
#include <ost/io/entity_io_handler.hh>
#include <ost/io/sequence_io_handler.hh>
#include <ost/io/surface_io_handler.hh>
#if OST_IMG_ENABLED
#include <ost/io/map_io_handler.hh>
#endif

namespace ost { namespace gui {

class DLLEXPORT_OST_GUI FileLoader {
private:

  enum ErrorType { DEFAULT = 0,
    IO_LOADING,
    GFX_ADD,
    GFX_MULTIPLE_ADD,
    INFO
  };

  FileLoader();
  static gfx::GfxObjP TryLoadEntity(const QString& filename, io::EntityIOHandlerP handler=io::EntityIOHandlerP(), const QString& selection=QString());
  static gfx::GfxObjP TryLoadSurface(const QString& filename, io::SurfaceIOHandlerPtr handler=io::SurfaceIOHandlerPtr());
  static gfx::GfxObjP TryLoadAlignment(const QString& filename, io::SequenceIOHandlerPtr handler=io::SequenceIOHandlerPtr());
#if OST_IMG_ENABLED
  static gfx::GfxObjP TryLoadMap(const QString& filename, io::MapIOHandlerPtr handler=io::MapIOHandlerPtr());
#endif
  static void RunScript(const QString& filename);
  static void LoadPDB(const QString& filename, const QString& selection=QString());
  static void AddToScene(const QString& filename, gfx::GfxObjP obj);
  static void HandleError(Message m, ErrorType type, const QString& filename, gfx::GfxObjP obj=gfx::GfxObjP());
  static gfx::GfxObjP NoHandlerFound(const QString& filename);
  virtual ~FileLoader();

  static LoaderManagerPtr loader_manager_;

#if OST_IMG_ENABLED
  static QList<img::ImageHandle> loaded_images_;
#endif

public:
  static void LoadObject(const QString& filename, const QString& selection=QString());
  static void LoadFrom(const QString& id, const QString& site=QString(), const QString& selection=QString());
  static std::vector<String> GetSiteLoaderIdents();
  static LoaderManagerPtr GetLoaderManager();
};

} }

#endif /* OST_GUI_FILE_LOADER_HH */
