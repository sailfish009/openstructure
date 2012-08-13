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

#include <ost/path.hh>
#include <ost/string_ref.hh>

#ifdef WIN32
// Insert Windows code here
#else
#include <sys/param.h>
#include <unistd.h>
#endif

namespace ost {



Path& Path::operator/=(const Path& path)
{
    if (path_.size()==0 || (path_[path_.size()-1] != OST_DIRECTORY_SEPARATOR)) {
      this->path_ += OST_DIRECTORY_SEPARATOR;
    }
    this->path_ += path.GetFullPath();
    return *this;
}

Path& Path::operator/=(const String& str)
{
    if (path_.size()==0 || (path_[path_.size()-1] != OST_DIRECTORY_SEPARATOR)) {
      this->path_ += OST_DIRECTORY_SEPARATOR;
    }
    this->path_ += str;
    return *this;
}

Path Path::operator/(const Path& path) const
{
    Path retpath(*this);
    retpath /= path;
    return retpath;
}

Path Path::operator/(const String& str) const
{
    Path retpath(*this);
    retpath /= str;
    return retpath;
}

String Path::GetFileName() const
{
    StringRef path_sr(path_.data(),path_.size());
    std::vector<StringRef> path_sr_split = path_sr.split(OST_DIRECTORY_SEPARATOR,true);
    return (path_sr_split.rbegin())->str();
}

String Path::GetDirName() const
{
    StringRef path_sr(path_.data(),path_.size());
    std::vector<StringRef> path_sr_split = path_sr.split(OST_DIRECTORY_SEPARATOR,true);
    path_sr_split.pop_back();
    String return_string("");
    for (std::vector<StringRef>::const_iterator path_sr_split_it=path_sr_split.begin();
                                                path_sr_split_it!=path_sr_split.end();
                                                ++path_sr_split_it ) {
        return_string+=path_sr_split_it->str();
        return_string+=OST_DIRECTORY_SEPARATOR;
    }
    return return_string;
}

String Path::GetExtension() const
{
    StringRef path_sr(path_.data(),path_.size());
    std::vector<StringRef> path_sr_split = path_sr.split(OST_DIRECTORY_SEPARATOR,true);
    StringRef filename_sr = *path_sr_split.rbegin();
    std::vector<StringRef> filename_sr_split = filename_sr.split('.',true);
    return (filename_sr_split.rbegin())->str();
}

#ifdef WIN32
// Insert Windows Code Here
#else

String Path::GetAbsolutePath() const
{
    if (path_.size() !=0 && path_[0]==OST_DIRECTORY_SEPARATOR) {
        return path_;
    }
    char path[MAXPATHLEN];  // This is a buffer for the text
    getcwd(path,MAXPATHLEN);
    String abspath = std::string(path);
    abspath += OST_DIRECTORY_SEPARATOR;
    abspath += path_;
    return abspath;
}

bool Path::Exists() const
{
    if (access(path_.c_str(), F_OK)==0) {
      return true;
    }
    return false;
}

bool Path::IsWritable() const
{
    return (access(path_.c_str(), W_OK)==0);
}

#endif

} // ost
