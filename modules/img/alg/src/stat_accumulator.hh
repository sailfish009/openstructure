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

#ifndef OST_STAT_ACCUMULATOR_HH
#define OST_STAT_ACCUMULATOR_HH

#include <ost/base.hh>
#include <ost/img/alg/module_config.hh>

namespace ost { namespace img { namespace alg {

class DLLEXPORT_IMG_ALG StatAccumulator
{
public:
  StatAccumulator();
  void operator()(Real val);
  StatAccumulator operator+(const StatAccumulator& acc) const;
  StatAccumulator& operator+=(const StatAccumulator& acc);

  Real GetMean() const ;
  Real GetSum() const ;
  Real GetVariance() const;
  Real GetStandardDeviation() const ;
  Real GetRootMeanSquare() const;
  Real GetSkewness() const ;
  Real GetKurtosis() const;

private:
  Real mean_;
  Real sum_;
  Real sum2_;
  Real m2_;
  Real m3_;
  Real m4_;
  Real n_;
};

}}} //ns
#endif // OST_STAT_ACCUMULATOR_HH
