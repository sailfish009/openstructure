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
#include "residue_prop.hh"
#include <boost/format.hpp>

namespace ost { namespace mol {

ResNum::ResNum(int n):
  num_(n), alt_('\0')
{}

ResNum::ResNum(int n, char a):
  num_(n), alt_(a)
{}

bool ResNum::operator==(const ResNum& r) const
{
  return num_==r.num_ && alt_==r.alt_;
}

bool ResNum::operator<(const ResNum& r) const
{
  return num_==r.num_ ? alt_<r.alt_ : num_<r.num_;
}

int ResNum::operator+=(int i)
{
  num_+=i;
  return num_;
}

int ResNum::operator-=(int i)
{
  num_-=i;
  return num_;
}

int ResNum::operator+=(const ResNum& r)
{
  num_+=r.num_;
  return num_;
}

int ResNum::operator-=(const ResNum& r)
{
  num_-=r.num_;
  return num_;
}

ResNum& ResNum::operator++()
{
  ++num_;
  return *this;
}

ResNum& ResNum::operator--()
{
  --num_;
  return *this;
}

ResNum ResNum::NextInsertionCode() const
{
  char alt= alt_=='\0' ? 'a' : alt_+1;
  ResNum nrvo(num_,alt);
  return nrvo;
}

String ResNum::AsString() const {
  return str(boost::format("%d%s") % num_ % (alt_==0 ? "" : String(1, alt_)));
}

std::ostream& operator<<(std::ostream& os, const ResNum& n)
{
  return os << n.AsString();
}


}} // ns
