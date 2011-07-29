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
  CHAINTYPE_POLY,           ///< polymer
  CHAINTYPE_NON_POLY,       ///< non-polymer
  CHAINTYPE_WATER,          ///< water
  CHAINTYPE_POLY_PEPTIDE_D, ///< (D) amino acid sequence
  CHAINTYPE_POLY_PEPTIDE_L, ///< (L) amino acid sequence
  CHAINTYPE_POLY_DN,        ///< polydeoxyribonucleotide
  CHAINTYPE_POLY_RN,        ///< polyribonucleotide
  CHAINTYPE_POLY_SAC_D,     ///< polysaccharide(D)
  CHAINTYPE_POLY_SAC_L,     ///< polysaccharide(L)
  CHAINTYPE_POLY_DN_RN,     ///<polydeoxyribonucleotide/ -ribonucleotide hybrid
  CHAINTYPE_UNKNOWN,        ///< guess what
  CHAINTYPE_N_CHAINTYPES    ///< no. of chain types
} ChainType;


#endif
