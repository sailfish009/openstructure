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

#include <iostream>
#include <fstream>
#include <ost/string_ref.hh>
#include "compare_files.hh"

namespace ost {
bool compare_files(const String& test, const String& gold_standard, bool rstrip)
{
  std::ifstream test_stream(test.c_str());
  std::ifstream gold_stream(gold_standard.c_str());
  String test_line, gold_line;
  while (true) {
    bool test_end=std::getline(test_stream, test_line) != 0;
    bool gold_end=std::getline(gold_stream, gold_line) != 0;
    if (!(test_end || gold_end)) {
      return true;
    }
    if (!test_end) {
      std::cerr << gold_standard << " contains additional line(s):"
                << std::endl << gold_line;
      return false;
    }
    if (!gold_end) {
      std::cerr << test << " contains additional line(s):"
                << std::endl << test_line;
      return false;
    }
    StringRef gold_ref(gold_line.data(), gold_line.size());
    StringRef test_ref(test_line.data(), test_line.size());
    if (rstrip) {
      gold_ref=gold_ref.rtrim();
      test_ref=test_ref.rtrim();
    }
    if (test_ref!=gold_ref) {
      std::cerr << "line mismatch:" << std::endl << "test: " << test_line
                << std::endl << "gold: " << gold_line;
      return false;
    }
  }
  return true;
}

}
