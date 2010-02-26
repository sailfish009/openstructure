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
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
using namespace boost::python;
#include <vector>
#include <iostream>
#include <QString>

#include <ost/gui/file_loader.hh>
#include <ost/gui/loader_manager.hh>

using namespace ost;
using namespace ost::gui;

namespace {

void load_object(const QString& file_name)
{
  FileLoader::LoadObject(file_name);
}

void load_from_site(const QString& id, const QString& site)
{
  FileLoader::LoadFrom(id,site);
}

void add_remote_site_loader(LoaderManager* loader_manager, const QString& site, RemoteSiteLoader* site_loader){
  loader_manager->AddRemoteSiteLoader(site,site_loader);
}

}

void export_FileLoader()
{
  class_<LoaderManager, LoaderManagerPtr, boost::noncopyable>("LoaderManager", no_init)
    .def("GetSiteLoaderIdents", &LoaderManager::GetSiteLoaderIdents)
    .def("AddRemoteSiteLoader", &add_remote_site_loader)
    .def("RemoveRemoteSiteLoader", &LoaderManager::RemoveRemoteSiteLoader)
  ;

  class_<FileLoader, boost::noncopyable>("FileLoader", no_init)
    .def("LoadObject", &FileLoader::LoadObject)
    .def("LoadObject", &load_object)
    .staticmethod("LoadObject")
    .def("LoadFrom", &FileLoader::LoadFrom)
    .def("LoadFrom", &load_from_site)
    .staticmethod("LoadFrom")
    .def("GetSiteLoaderIdents", &FileLoader::GetSiteLoaderIdents)
    .staticmethod("GetSiteLoaderIdents")
    .def("GetLoaderManager", &FileLoader::GetLoaderManager)
    .staticmethod("GetLoaderManager")
  ;
}
