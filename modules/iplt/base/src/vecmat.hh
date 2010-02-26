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
  vectors and matrices using geom library

  Author: Ansgar Philippsen
*/

#ifndef IPLT_VECMAT_H
#define IPLT_VECMAT_H

#include <cmath>
#include <vector>

#include <ost/geom/geom.hh>

#include <ost/iplt/module_config.hh>

namespace ost { namespace iplt {

// this pulls the geom namespace into the iplt one
using namespace ::geom;


}} // ns iplt

#endif
