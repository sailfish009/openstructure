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

/*
  abstract base class for generic data

  Author: Ansgar Philippsen
*/

#ifndef IPLT_DATA_TYPES_H
#define IPLT_DATA_TYPES_H

#include <ost/base.hh>



namespace ost { namespace iplt {


#ifdef Complex
  // undef Complex from xorg X.h to avoid clash with our definition
  #undef Complex
#endif
typedef std::complex<Real> Complex;
typedef unsigned short Word;

enum DataType {REAL=0,
               COMPLEX,
               WORD,
               DEFAULTTYPE
}; //! underlying data type
enum DataDomain {SPATIAL=0,
                 FREQUENCY,
                 HALF_FREQUENCY,
                 DEFAULTDOMAIN
}; //! data domain

#ifdef IGNORE
#undef IGNORE
#endif
enum DivZeroMethod {SET_ZERO=0,
                    DIV_EPS=1,
                    INTERPOLATE
};

enum DomainToColorMode {MODULUS,
                        PHASECOLOR
};

}} // namespace iplt

#endif
