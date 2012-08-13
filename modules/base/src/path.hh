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

/*
  Author: Valerio Mariani
*/


#ifndef OST_PATH_REF_HH
#define OST_PATH_REF_HH

#include <ost/base.hh>
#include <ost/module_config.hh>

#ifdef WIN32
#define OST_DIRECTORY_SEPARATOR '\\'
#else
#define OST_DIRECTORY_SEPARATOR '/'
#endif

namespace ost {

/// \brief convenient datatype for file paths
class DLLEXPORT_OST_BASE Path {

public:

    Path (const String& path): path_(path) {};
    String GetFullPath() const { return path_; }
    operator std::string() const { return path_;}
    Path operator/(const Path& path) const;
    Path operator/(const String& path) const;
    Path& operator/=(const Path& path);
    Path& operator/=(const String& path);
    String GetDirName() const;
    String GetFileName() const;
    String GetExtension() const;
    String GetAbsolutePath() const;
    bool Exists() const;
    bool IsWritable() const;

private:

    String path_;
};


Path RootPath() {
#ifdef WIN32
  return Path("c:\\");
#else
  return Path("/");
#endif
}

#ifdef WIN32

Path DiscRootPath(char disc) {
  return Path("\");    
}

#endif

} // ost

#endif // OST_PATH_REF

