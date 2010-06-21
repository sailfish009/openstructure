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

void load_object_a(const QString& file_name)
{
  FileLoader::LoadObject(file_name);
}

void load_object_b(const QString& file_name, const QString& selection)
{
  FileLoader::LoadObject(file_name, selection);
}

void load_from_site_a(const QString& id)
{
  FileLoader::LoadFrom(id);
}

void load_from_site_b(const QString& id, const QString& site)
{
  FileLoader::LoadFrom(id,site);
}

void load_from_site_c(const QString& id, const QString& site, const QString& selection)
{
  FileLoader::LoadFrom(id,site,selection);
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
    .def("SetDefaultRemoteSiteIdent", &LoaderManager::SetDefaultRemoteSiteIdent)
    .def("GetDefaultRemoteSiteIdent", &LoaderManager::GetDefaultRemoteSiteIdent)
  ;

  class_<FileLoader, boost::noncopyable>("FileLoader", no_init)
    .def("LoadObject", &load_object_a)
    .def("LoadObject", &load_object_b)
    .staticmethod("LoadObject")
    .def("LoadFrom", &load_from_site_a)
    .def("LoadFrom", &load_from_site_b)
    .def("LoadFrom", &load_from_site_c)
    .staticmethod("LoadFrom")
    .def("GetSiteLoaderIdents", &FileLoader::GetSiteLoaderIdents)
    .staticmethod("GetSiteLoaderIdents")
    .def("GetLoaderManager", &FileLoader::GetLoaderManager)
    .staticmethod("GetLoaderManager")
  ;
}
