//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2010 by the OpenStructure authors
// Copyright (C) 2003-2010 by the IPLT authors
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
#ifndef OST_IO_MAP_IO_PLUGIN_H
#define OST_IO_MAP_IO_PLUGIN_H

#include <boost/shared_ptr.hpp>
#include <boost/filesystem/operations.hpp>
#include <ost/io/module_config.hh>
#include <ost/iplt/map.hh>
#include <ost/iplt/alg/normalizer.hh>
#include <ost/io/iplt/image_format.hh>

namespace ost { namespace io {

class DLLEXPORT_OST_IO MapIOHandler {
public:
  virtual ~MapIOHandler() {};
  virtual void Import(iplt::MapHandle& surf, const boost::filesystem::path& loc, const ImageFormatBase& formatstruct) = 0;
  virtual void Import(iplt::MapHandle& surf, std::istream& stream,const ImageFormatBase& formatstruct) = 0;
  virtual void Export(const iplt::MapHandle& ent, const boost::filesystem::path& loc,const ImageFormatBase& formatstruct) const = 0;
  virtual void Export(const iplt::MapHandle& ent, std::ostream& stream,const ImageFormatBase& formatstruct) const = 0;
};

typedef boost::shared_ptr<MapIOHandler> MapIOHandlerPtr;

class DLLEXPORT_OST_IO MapIOHandlerFactoryBase {
public:
  virtual ~MapIOHandlerFactoryBase() {}
  virtual bool MatchContent(unsigned char* header) const = 0;
  virtual bool MatchType(const ImageFormatBase& type) const = 0;
  virtual bool MatchSuffix(const String& suffix) const =0 ;
  virtual MapIOHandlerPtr Create() const = 0 ;
};

typedef boost::shared_ptr<MapIOHandlerFactoryBase> MapIOHandlerFactoryBasePtr;

template <class HANDLER>
class MapIOHandlerFactory: public MapIOHandlerFactoryBase
{

  virtual bool MatchContent(unsigned char* header) const {
    return HANDLER::MatchContent(header);
  }

  virtual bool MatchType(const ImageFormatBase& type) const {
    return HANDLER::MatchType(type);
  }

  virtual bool MatchSuffix(const String& suffix) const {
    return HANDLER::MatchSuffix(suffix);
  }

  virtual MapIOHandlerPtr Create() const {
    return MapIOHandlerPtr(new HANDLER);
  }
};


}} // ns

#endif
