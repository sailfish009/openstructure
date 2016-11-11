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

#ifndef OST_SEQ_PROFILE_HANDLE_HH
#define OST_SEQ_PROFILE_HANDLE_HH

#include <ost/base.hh>
#include <ost/stdint.hh>
#include <ost/message.hh>
#include <ost/seq/module_config.hh>

#include <string.h> // for memcpy, etc
#include <vector>
#include <map>
#include <fstream>
#include <boost/shared_ptr.hpp>

namespace ost { namespace seq { 

class ProfileHandle;
class ProfileColumn;
class ProfileDB;
typedef boost::shared_ptr<ProfileHandle> ProfileHandlePtr;
typedef std::vector<ProfileHandle> ProfileHandleList;
typedef boost::shared_ptr<ProfileDB> ProfileDBPtr;
typedef std::vector<ProfileColumn> ProfileColumnList;

/// \brief Defines profile of 20 frequencies for one residue.
///
/// Frequencies are identified by the one-letter-code for that amino acid.
/// (possible codes: ACDEFGHIKLMNPQRSTVWY)
class DLLEXPORT_OST_SEQ ProfileColumn {
public:

  /// \brief Construct a profile with all frequencies set to 0.
  ProfileColumn() {
    memset(freq_, 0, sizeof(freq_));
  }

  ProfileColumn(const ProfileColumn& rhs) {
    memcpy(freq_, rhs.freq_, sizeof(freq_));
  }
  ProfileColumn& operator= (const ProfileColumn& rhs) {
    memcpy(freq_, rhs.freq_, sizeof(freq_));
    return *this;
  }

  static ProfileColumn BLOSUMNullModel();

  /// \brief Translate one-letter-code to index (0-indexing).
  static int GetIndex(char ch);

  Real GetFreq(char ch) const;

  void SetFreq(char ch, Real freq);

  bool operator==(const ProfileColumn& rhs) const {
    return !memcmp(freq_, rhs.freq_, sizeof(freq_));
  }
  bool operator!=(const ProfileColumn& rhs) const { return !(rhs == (*this)); }

  Real* freqs_begin() { return freq_; }
  Real* freqs_end() { return freq_ + 20; }
  const Real* freqs_begin() const { return freq_; }
  const Real* freqs_end() const { return freq_ + 20; }

  /// \brief Get entropy for this column.
  Real GetEntropy() const;

  /// \brief Get column score as in Soeding-2005
  Real GetScore(const ProfileColumn& other,
                const ProfileColumn& null_model) const;

  // functions to feed streams with limited accuracy of internal data
  // not intended for python export

  friend std::ofstream& operator<<(std::ofstream& os, ProfileColumn& col) {
    int16_t data[20];
    //transform aa_freq
    for (uint i = 0; i < 20; ++i) {
      data[i] = static_cast<int16_t>(col.freq_[i]*10000);
    }
    os.write(reinterpret_cast<char*>(data), sizeof(data));
    return os;
  }

  friend std::ifstream& operator>>(std::ifstream& is, ProfileColumn& col) {
    int16_t data[20];
    is.read(reinterpret_cast<char*>(data), sizeof(data));
    //transform aa_freq
    for (uint i = 0; i < 20; ++i) {
      col.freq_[i] = data[i] * 0.0001;
    }
    return is;
  }

private:
  Real freq_[20];
};

/// \brief Provides a profile for a sequence.
///
/// Properties:
/// - consists of N ProfileColumn (N = #residues in sequence)
/// - contains sequence (String) of length N
/// - user must enforce consistency between sequence length and #columns
/// - contains a null_model to use for this sequence
class DLLEXPORT_OST_SEQ ProfileHandle { 
public:
  /// \brief Constructs an empty profile handle (sequence = '', 0 columns).
  ProfileHandle() {}

  // uses compiler-generated copy- and assignment operators (work here!)

  const std::vector<ProfileColumn>& GetColumns() const { return columns_; }

  const ProfileColumn& GetNullModel() const { return null_model_; } 

  void SetNullModel(const ProfileColumn& null_model) { null_model_ = null_model; }

  String GetSequence() const { return seq_; }

  void SetSequence(const String& seq) { seq_ = seq; }

  /// \brief Extract subset of profile for columns from until to-1 (0-indexing).
  /// Null model is copied from this profile.
  /// \throw Error if to <= from or to > size().
  ProfileHandlePtr Extract(uint from, uint to);

  /// \brief Compute average entropy over all columns.
  Real GetAverageEntropy() const;

  /// \brief Compute score comparing columns other[i] and this->at(i+offset)
  /// Column score as in Soeding-2005, null model of this object used, 
  /// result normalized by other.size()
  Real GetAverageScore(const ProfileHandle& other, uint offset = 0) const;

  // some functions to make it behave like a vector

  void clear() { seq_ = ""; columns_.clear(); }

  size_t size() const { return columns_.size(); }

  bool empty() const { return columns_.empty(); }

  void push_back(const ProfileColumn& c) { columns_.push_back(c); }  

  ProfileColumn& operator[](size_t index) { return columns_[index]; }

  const ProfileColumn& operator[](size_t index) const { return columns_[index]; }

  ProfileColumn& at(size_t index) { return columns_.at(index); }

  const ProfileColumn& at(size_t index) const { return columns_.at(index); }

  bool operator==(const ProfileHandle& other) const {
    return seq_ == other.seq_ &&
           columns_ == other.columns_ &&
           null_model_ == other.null_model_;
  }
  
  bool operator!=(const ProfileHandle& other) const { return !(other == (*this)); }

  ProfileColumnList::const_iterator columns_end() const { return columns_.end(); }
  ProfileColumnList::iterator columns_end() { return columns_.end(); }
  ProfileColumnList::const_iterator columns_begin() const { return columns_.begin(); }
  ProfileColumnList::iterator columns_begin() { return columns_.begin(); }

  // functions to feed streams with limited accuracy of internal data
  // not intended for python export

  friend std::ofstream& operator<<(std::ofstream& os, ProfileHandle& prof) {
    // null model
    os << prof.null_model_;
    // num. columns/residues
    uint32_t size = prof.size();
    os.write(reinterpret_cast<char*>(&size), sizeof(uint32_t));
    // sequence
    if (prof.seq_.length() != size) {
      throw Error("ProfileHandle - Inconsistency between number of columns and "
                  " seq. length.");
    }
    os.write(prof.seq_.c_str(), size);
    // columns
    for(uint i = 0; i < size; ++i){
      os << prof.columns_[i];
    }

    return os;
  }

  friend std::ifstream& operator>>(std::ifstream& is, ProfileHandle& prof) {
    // null model
    is >> prof.null_model_;
    // num. columns/residues
    uint32_t size;
    is.read(reinterpret_cast<char*>(&size), sizeof(uint32_t));
    // sequence
    std::vector<char> tmp_buf(size);
    is.read(&tmp_buf[0], size);
    prof.seq_.assign(&tmp_buf[0], size);
    // columns
    prof.columns_.resize(size);
    for(uint i = 0; i < size; ++i){
      is >> prof.columns_[i];
    }

    return is;
  }

private:
  String             seq_;
  ProfileColumn      null_model_; 
  ProfileColumnList  columns_;
};

/// \brief Contains a DB of profiles (identified by a unique name (String)).
class DLLEXPORT_OST_SEQ ProfileDB {
public:
  /// \brief Saves all profiles in DB with limited accuracy of internal data.
  /// Binary format with fixed-width integers (should be portable).
  void Save(const String& filename) const;

  static ProfileDBPtr Load(const String& filename);

  void AddProfile(const String& name, ProfileHandlePtr prof);

  ProfileHandlePtr GetProfile(const String& name) const;

  size_t size() const { return data_.size(); }

  std::vector<String> GetNames() const;

private:
  std::map<String, ProfileHandlePtr> data_;
};

}}

#endif
