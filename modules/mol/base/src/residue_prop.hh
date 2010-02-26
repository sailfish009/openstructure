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
#ifndef OST_RESIDUE_PROP_HH
#define OST_RESIDUE_PROP_HH

#include <boost/operators.hpp>

#include <ost/mol/module_config.hh>

namespace ost { namespace mol {

/*
  
*/
class DLLEXPORT_OST_MOL ResNum: private
    boost::additive<ResNum, int,
    boost::totally_ordered<ResNum, 
    boost::totally_ordered<ResNum, int,
    boost::unit_steppable<ResNum> > > >
{
public:
  ResNum(int num); // no explicit on purpose
  ResNum(int num, char alt);

  bool operator==(const ResNum&) const;
  bool operator<(const ResNum&) const;
  int operator+=(int);
  int operator-=(int);
  int operator+=(const ResNum&);
  int operator-=(const ResNum&);
  ResNum& operator++();
  ResNum& operator--();

  ResNum NextInsertionCode() const;

  int GetNum() const {return num_;}
  char GetInsCode() const {return alt_;}
  
  /// \brief get residue number as String
  ///
  /// The returned String consists of both the numeric residue sequence number
  /// and the insertion code. If the insertion code is not defined, i.e. is 
  /// equal to the null character, only the residue sequence number is returned.
  String AsString() const;
private:
  int num_;
  char alt_;
};

DLLEXPORT_OST_MOL std::ostream& operator<<(std::ostream& os, const ResNum& n);

typedef String ResidueKey;

/// \brief Residue properties
/// 
/// Small data structure to aggregate residue properties.
struct DLLEXPORT_OST_MOL ResidueProp {
  ResidueProp(const String& k, char a)
    : key(k), abbrev(a) {    
  }
  ResidueProp()
    :key(""), abbrev('\0') {   
  }
  ResidueKey   key;
  char         abbrev;
};

}} // ns


#endif
