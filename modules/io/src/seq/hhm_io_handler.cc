//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2015 by the OpenStructure authors
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
  Author: Gerardo Tauriello, Gabriel Studer
 */
  
#include "hhm_io_handler.hh"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_stream.hpp>

#include <ost/string_ref.hh>
#include <ost/io/io_exception.hh>
#include <ost/io/io_utils.hh>

namespace ost { namespace io {

namespace {
// reset pc with freqs in chunks[i+offset] (i in [0,20[)
// checks for consistency of chunks and values read
seq::ProfileColumn GetColumn(const std::vector<ost::StringRef>& chunks,
                             const uint offset, const char* olcs,
                             const String& exception) {
  // check chunks size
  if (chunks.size() < offset+20) {
    throw IOException(exception);
  }
  // reset pc
  seq::ProfileColumn pc; // all 0
  for (uint i = 0; i < 20; ++i) {
    if (chunks[i+offset] != ost::StringRef("*", 1)) {
      std::pair<bool, int> pbi = chunks[i+offset].to_int();
      if (!pbi.first) {
        throw IOException(exception);
      }
      pc.SetFreq(olcs[i], pow(2.0, -0.001 * pbi.second));
    }
  }
  return pc;
}
} // anon ns

void HhmIOHandler::Import(seq::ProfileHandle& prof,
                          const boost::filesystem::path& loc) {
  // open it up
  boost::iostreams::filtering_stream<boost::iostreams::input> in;
  boost::filesystem::ifstream stream(loc);
  if (!stream) {
    throw IOException("Could not open " + loc.string());
  }
  // add unzip if necessary
  if (boost::iequals(".gz", boost::filesystem::extension(loc))) {
    in.push(boost::iostreams::gzip_decompressor());
  }
  in.push(stream);

  // reset profile
  prof.clear();
  
  // tmp. storage
  std::string line;
  ost::StringRef sline;
  std::string null_line;
  char olcs[20];
  std::vector<ost::StringRef> chunks;

  // read line-by-line looking for NULL
  bool null_found = false;
  while (std::getline(in, line)) {
    sline = ost::StringRef(line.c_str(), line.length());
    if (sline.length()>4 && 
        sline.substr(0, 5) == ost::StringRef("NULL ", 5)) {
      null_line = line;
      null_found = true;
      break;
    }
  }
  if (!null_found) {
    throw IOException("No NULL found in file " + loc.string());
  }

  // read until we hit HMM, extract olcs, then skip 2 more lines
  bool hmm_found = false;
  while (std::getline(in, line)) {
    sline = ost::StringRef(line.c_str(), line.length());
    chunks = sline.split();
    if (chunks[0] == ost::StringRef("HMM", 3)) {
      // extract olcs
      if (chunks.size() != 21) {
        throw IOException("Badly formatted HMM line in " + loc.string());
      }
      for (uint i = 1; i < 21; ++i) {
        if (chunks[i].length() != 1) {
          throw IOException("Badly formatted HMM line in " + loc.string());
        }
        olcs[i-1] = chunks[i][0];
      }
      // skip 2 lines and get out
      std::getline(in, line);
      std::getline(in, line);
      hmm_found = true;
      break;
    }
  }
  if (!hmm_found) {
    throw IOException("No HMM found in file " + loc.string());
  }

  // set null model
  chunks = ost::StringRef(null_line.c_str(), null_line.length()).split();
  prof.SetNullModel(GetColumn(chunks, 1, olcs, "Badly formatted NULL line\n"
                              + null_line + "\n in " + loc.string()));

  // set columns
  String seqres;
  while (std::getline(in, line)) {
    sline = ost::StringRef(line.c_str(), line.length());
    if (sline.trim().empty()) continue;
    if (sline.trim() == ost::StringRef("//", 2)) break;
    chunks = sline.split();
    // one letter code
    seqres += chunks[0][0];
    // frequencies
    prof.push_back(GetColumn(chunks, 2, olcs, "Badly formatted line\n"
                             + line + "\n in " + loc.string()));
    // skip line (trans. matrix)
    std::getline(in, line);
  }
  prof.SetSequence(seqres);
}

void HhmIOHandler::Export(const seq::ProfileHandle& prof,
                          const boost::filesystem::path& loc) const {
  throw IOException("Cannot write hhm files.");
}

bool HhmIOHandler::ProvidesImport(const boost::filesystem::path& loc, 
                                  const String& format) {
  if (format=="auto") {
    String match_suf_string = loc.string();
    std::transform(match_suf_string.begin(), match_suf_string.end(),
                   match_suf_string.begin(), tolower);
    if (   detail::FilenameEndsWith(match_suf_string,".hhm")
        || detail::FilenameEndsWith(match_suf_string,".hhm.gz")) {
      return true;
    }
  } else if (format == "hhm") {
    return true;
  }
  return false;
}

bool HhmIOHandler::ProvidesExport(const boost::filesystem::path& loc, 
                                  const String& format) {
  // no writers here
  return false;
}

}}
