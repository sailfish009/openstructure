//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2016 by the OpenStructure authors
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
  Author: Gerardo Tauriello, Juergen Haas
 */

#include <fstream>
#include <iostream>
#include <sstream>

#include <ost/mol/mol.hh>

#include <ost/seq/alg/distance_map.hh>
#include <ost/seq/alg/variance_map.hh>

namespace ost { namespace seq { namespace alg {

///////////////////////////////////////////////////////////////////////////////
// HELPERS
namespace {
// dump stuff using () operator
template <typename T>
void DumpCsv(const String& file_name, const T& data, uint num_rows,
             uint num_cols) {
  // open file
  std::ofstream out_file(file_name.c_str());
  if (!out_file) throw Error("The file '" + file_name + "' cannot be opened.");
  // dump each row
  for (uint row = 0; row < num_rows; ++row) {
    for (uint col = 0; col < num_cols; ++col) {
      out_file << data(row, col);
      if (col < num_cols-1) out_file << ";";
    }
    out_file << std::endl;
  }
  out_file.close();
}

template <typename T>
String GetJson(const T& data, uint num_rows, uint num_cols) {
  std::ostringstream out_stream;
  out_stream << "[";
  for (uint row = 0; row < num_rows; ++row) {
    out_stream << "[";
    for (uint col = 0; col < num_cols; ++col) {
      out_stream << data(row, col);
      if (col < num_cols-1) out_stream << ",";
    }
    out_stream << "]";
    if (row < num_rows-1) out_stream << ",";
  }
  out_stream << "]";
  return out_stream.str();
}
} // anon ns

///////////////////////////////////////////////////////////////////////////////
// IO
void VarianceMap::ExportDat(const String& file_name) {
  uint len = this->GetSize();
  if (len == 0) throw IntegrityError("Matrix empty");
  // dump it
  std::ofstream out_file(file_name.c_str());
  if (!out_file) throw Error("The file '" + file_name + "' cannot be opened.");
  for (uint rows = 0; rows < len; ++rows) {
    for (uint cols = 0; cols < len; ++cols) {
      out_file << (rows+1) << " " << (cols+1) << " " << this->Get(rows, cols)
               << std::endl;
    }
  }
  out_file.close();
}

void VarianceMap::ExportCsv(const String& file_name) {
  uint len = this->GetSize();
  if (len == 0) throw IntegrityError("Matrix empty");
  DumpCsv(file_name, *this, len, len);
}

void VarianceMap::ExportJson(const String& file_name) {
  if (this->GetSize() == 0) throw IntegrityError("Matrix empty");
  std::ofstream out_file(file_name.c_str());
  if (!out_file) throw Error("The file '" + file_name + "' cannot be opened.");
  out_file << this->GetJsonString() << std::endl;
  out_file.close();
}

String VarianceMap::GetJsonString() {
  uint len = this->GetSize();
  if (len == 0) throw IntegrityError("Matrix empty");
  return GetJson(*this, len, len);
}

void Dist2Mean::ExportDat(const String& file_name) {
  if (values_.size() == 0) throw IntegrityError("Matrix empty");
  // dump it
  std::ofstream out_file(file_name.c_str());
  if (!out_file) throw Error("The file '" + file_name + "' cannot be opened.");
  for (uint i_res = 0; i_res < num_residues_; ++i_res) {
    out_file << (i_res+1);
    for (uint i_str = 0; i_str < num_structures_; ++i_str) {
      out_file << " " << this->Get(i_res, i_str);
    }
    out_file << std::endl;
  }
  out_file.close();
}

void Dist2Mean::ExportCsv(const String& file_name) {
  if (values_.size() == 0) throw IntegrityError("Matrix empty");
  DumpCsv(file_name, *this, num_residues_, num_structures_);
}

void Dist2Mean::ExportJson(const String& file_name) {
  if (values_.size() == 0) throw IntegrityError("Matrix empty");
  std::ofstream out_file(file_name.c_str());
  if (!out_file) throw Error("The file '" + file_name + "' cannot be opened.");
  out_file << this->GetJsonString() << std::endl;
  out_file.close();
}

String Dist2Mean::GetJsonString() {
  if (values_.size() == 0) throw IntegrityError("Matrix empty");
  return GetJson(*this, num_residues_, num_structures_);
}

///////////////////////////////////////////////////////////////////////////////
// Create maps
VarianceMapPtr CreateVarianceMap(const DistanceMapPtr dmap, Real sigma) {
  // setup
  uint len = dmap->GetSize();
  if (len == 0) {
    throw IntegrityError("empty distance map provided");
  }
  // get weighted std for each pair (symmetric storage!)
  VarianceMapPtr vmap(new VarianceMap(len));
  for (uint i_res = 0; i_res < len; ++i_res) {
    for (uint i_res2 = 0; i_res2 <= i_res; ++i_res2) {
      const Distances& di = dmap->GetDistances(i_res, i_res2);
      if (di.GetDataSize() > 0) {
        vmap->Set(i_res, i_res2, di.GetWeightedStdDev(sigma));
      } else {
        vmap->Set(i_res, i_res2, 0);
      }
    }
  }
  return vmap;
}

Dist2MeanPtr CreateDist2Mean(const DistanceMapPtr dmap) {
  // setup/check
  uint nstruc = dmap->GetNumStructures();
  uint len = dmap->GetSize();
  if (len == 0 || nstruc == 0) {
    throw IntegrityError("empty distance map provided");
  }
  // sum up all distances to mean for each residue (symmetric dmap!)
  Dist2MeanPtr dist2mean(new Dist2Mean(len, nstruc));
  for (uint i_res = 0; i_res < len; ++i_res) {
    for (uint i_res2 = 0; i_res2 <= i_res; ++i_res2) {
      const Distances& di = dmap->GetDistances(i_res, i_res2);
      const uint size = di.GetDataSize();
      if (size >= 2) {
        const Real avg = di.GetAverage();
        for (uint h = 0; h < size; ++h) {
          const std::pair<Real, int> ret = di.GetDataElement(h);
          const Real val = std::abs(ret.first - avg);
          dist2mean->Add(i_res, ret.second-1, val);
          if (i_res != i_res2) dist2mean->Add(i_res2, ret.second-1, val);
        }
      }
    }
  }
  // normalize
  dist2mean->DivideBy(len);
  return dist2mean;
}
 
}}}
