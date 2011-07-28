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
#ifndef OST_CHAIN_TYPE_HH
#define OST_CHAIN_TYPE_HH

/// \enum different kinds of chains
typedef enum {
  CHAINTYPE_POLY,         ///< chain name by author as in PDB
  CHAINTYPE_AA,           ///< amino acid sequence
  CHAINTYPE_NT,           ///< nucleotides
  CHAINTYPE_UNKNOWN,      ///< guess what
  CHAINTYPE_N_CHAINTYPES  ///< no. of chain types
} ChainType;

#endif
