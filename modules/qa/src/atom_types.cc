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
#include <boost/version.hpp>
#if BOOST_VERSION>=103800
#define BOOST_SPIRIT_USE_OLD_NAMESPACE
#include <boost/spirit/include/classic_symbols.hpp>
#else
#include <boost/spirit/symbols.hpp>
#endif

#include <map>
#include "atom_types.hh"

namespace ost { namespace qa {

using namespace boost::spirit;


// let's have a little bit of syntactic sugar to define the atom name to
// atom type mapping.
class ResidueMap : public std::map<AminoAcid, atom::ChemType> {
public:
  ResidueMap(AminoAcid amino_acid, atom::ChemType type) {
    this->operator()(amino_acid, type);
  }
  ResidueMap& operator()(AminoAcid amino_acid, atom::ChemType type) {
    this->insert(std::make_pair(amino_acid, type));
    return *this;
  }
  atom::ChemType Get(AminoAcid amino_acid) {
    assert(amino_acid!=Xxx);
    std::map<AminoAcid, atom::ChemType>::const_iterator i;
    i=this->find(amino_acid);
    if (i!=this->end())
      return i->second;
    return atom::UNKNOWN;
  }
};
typedef ResidueMap R;


struct AtomNames : public symbols<R> {
  AtomNames() {
    add
    ("C",   R(Ala, atom::C_ALA)
             (Arg, atom::C_ARG)
             (Asn, atom::C_ASN)
             (Asp, atom::C_ASP)
             (Gln, atom::C_GLN)
             (Glu, atom::C_GLU)
             (Lys, atom::C_LYS)
             (Ser, atom::C_SER)
             (Cys, atom::C_CYS)
             (Met, atom::C_MET)
             (Trp, atom::C_TRP)
             (Tyr, atom::C_TYR)
             (Thr, atom::C_THR)
             (Val, atom::C_VAL)
             (Ile, atom::C_ILE)
             (Leu, atom::C_LEU)
             (Gly, atom::C_GLY)
             (Pro, atom::C_PRO)
             (His, atom::C_HIS)
             (Phe, atom::C_PHE))
    ("N",   R(Ala, atom::N_ALA)
             (Arg, atom::N_ARG)
             (Asn, atom::N_ASN)
             (Asp, atom::N_ASP)
             (Gln, atom::N_GLN)
             (Glu, atom::N_GLU)
             (Lys, atom::N_LYS)
             (Ser, atom::N_SER)
             (Cys, atom::N_CYS)
             (Met, atom::N_MET)
             (Trp, atom::N_TRP)
             (Tyr, atom::N_TYR)
             (Thr, atom::N_THR)
             (Val, atom::N_VAL)
             (Ile, atom::N_ILE)
             (Leu, atom::N_LEU)
             (Gly, atom::N_GLY)
             (Pro, atom::N_PRO)
             (His, atom::N_HIS)
             (Phe, atom::N_PHE))
    ("O",   R(Ala, atom::O_ALA)
             (Arg, atom::O_ARG)
             (Asn, atom::O_ASN)
             (Asp, atom::O_ASP)
             (Gln, atom::O_GLN)
             (Glu, atom::O_GLU)
             (Lys, atom::O_LYS)
             (Ser, atom::O_SER)
             (Cys, atom::O_CYS)
             (Met, atom::O_MET)
             (Trp, atom::O_TRP)
             (Tyr, atom::O_TYR)
             (Thr, atom::O_THR)
             (Val, atom::O_VAL)
             (Ile, atom::O_ILE)
             (Leu, atom::O_LEU)
             (Gly, atom::O_GLY)
             (Pro, atom::O_PRO)
             (His, atom::O_HIS)
             (Phe, atom::O_PHE))
    ("CA",  R(Ala, atom::C_ALA_A)
             (Arg, atom::C_ARG_A)
             (Asn, atom::C_ASN_A)
             (Asp, atom::C_ASP_A)
             (Gln, atom::C_GLN_A)
             (Glu, atom::C_GLU_A)
             (Lys, atom::C_LYS_A)
             (Ser, atom::C_SER_A)
             (Cys, atom::C_CYS_A)
             (Met, atom::C_MET_A)
             (Trp, atom::C_TRP_A)
             (Tyr, atom::C_TYR_A)
             (Thr, atom::C_THR_A)
             (Val, atom::C_VAL_A)
             (Gly, atom::C_GLY_A)
             (Ile, atom::C_ILE_A)
             (Leu, atom::C_LEU_A)
             (Pro, atom::C_PRO_A)
             (His, atom::C_HIS_A)
             (Phe, atom::C_PHE_A))
    ("CB",  R(Ala, atom::C_ALA_B)
             (Arg, atom::C_ARG_B)
             (Asn, atom::C_ASN_B)
             (Asp, atom::C_ASP_B)
             (Gln, atom::C_GLN_B)
             (Glu, atom::C_GLU_B)
             (Lys, atom::C_LYS_B)
             (Ser, atom::C_SER_B)
             (Cys, atom::C_CYS_B)
             (Met, atom::C_MET_B)
             (Trp, atom::C_TRP_B)
             (Tyr, atom::C_TYR_B)
             (Thr, atom::C_THR_B)
             (Val, atom::C_VAL_B)
             (Ile, atom::C_ILE_B)
             (Leu, atom::C_LEU_B)
             (Pro, atom::C_PRO_B)
             (His, atom::C_HIS_B)
             (Phe, atom::C_PHE_B))
    ("CG",  R(Arg, atom::C_ARG_G)
             (Asn, atom::C_ASN_G)
             (Asp, atom::C_ASP_G)
             (Gln, atom::C_GLN_G)
             (Glu, atom::C_GLU_G)
             (Lys, atom::C_LYS_G)
             (Met, atom::C_MET_G)
             (Trp, atom::C_TRP_G)
             (Leu, atom::C_LEU_G)
             (Pro, atom::C_PRO_G)
             (Phe, atom::C_PHE_G)
             (Tyr, atom::C_TYR_G)
             (His, atom::C_HIS_G))
    ("CG1", R(Ile, atom::C_ILE_G1)
             (Val, atom::C_VAL_G))
    ("CG2", R(Ile, atom::C_ILE_G2)
             (Thr, atom::C_THR_G2)
             (Val, atom::C_VAL_G))
    ("CD",  R(Glu, atom::C_GLU_D)
             (Ile, atom::C_GLN_D)
             (Lys, atom::C_LYS_D)
             (Pro, atom::C_PRO_D)
             (Gln, atom::C_GLN_D)
             (Glu, atom::C_GLU_D)
             (Arg, atom::C_ARG_D))
    ("CD1", R(Trp, atom::C_TRP_D1)
             (Tyr, atom::C_TYR_D)
             (Phe, atom::C_PHE_D)
             (Leu, atom::C_LEU_D)
             (Ile, atom::C_ILE_D1))
    ("CD2", R(Trp, atom::C_TRP_D2)
             (Tyr, atom::C_TYR_D)
             (Phe, atom::C_PHE_D)
             (Leu, atom::C_LEU_D)
             (His, atom::C_HIS_D2))
    ("CE",  R(Lys, atom::C_LYS_E))
    ("CE1", R(Phe, atom::C_PHE_E)
             (Tyr, atom::C_TYR_E)
             (His, atom::C_HIS_E1))
    ("CE2", R(Phe, atom::C_PHE_E)
             (Tyr, atom::C_TYR_E))
    ("CE3", R(Trp, atom::C_TRP_E3))
    ("CZ",  R(Arg, atom::C_ARG_Z)
             (Phe, atom::C_PHE_Z)
             (Tyr, atom::C_TYR_Z))
    ("CZ3", R(Trp, atom::C_TRP_Z3))
    ("OG",  R(Ser, atom::O_SER_G))
    ("OG1", R(Thr, atom::O_THR_G1))
    ("SG",  R(Cys, atom::S_CYS_G))
    ("SD",  R(Met, atom::S_MET_D))
    ("OD1", R(Asp, atom::O_ASP_D)
             (Asn, atom::O_ASN_D))
    ("OD2", R(Asp, atom::O_ASP_D))
    ("OE1", R(Glu, atom::O_GLU_E)
             (Gln, atom::O_GLN_E))
    ("OE2", R(Glu, atom::O_GLU_E))
    ("ND1", R(His, atom::N_HIS_D1))
    ("ND2", R(Asn, atom::N_ASN_D))
    ("NE",  R(Arg, atom::N_ARG_E))
    ("NE1", R(Arg, atom::N_ARG_E))
    ("NE2", R(Gln, atom::N_GLN_E)
             (His, atom::N_HIS_E2))
    ("NH1", R(Arg, atom::N_ARG_H))
    ("NH2", R(Arg, atom::N_ARG_H))
    ("NZ",  R(Lys, atom::N_LYS_Z))
    ("OH",  R(Tyr, atom::O_TYR_H))
     ;
  }
};

atom::ChemType GetAtomTypeByName(AminoAcid amino_acid, const String& aname) {

  static AtomNames aa_name_symbols;
  R* m=find(aa_name_symbols, aname.c_str());
  if (m) {
    atom::ChemType t=m->Get(amino_acid);
    return t;
  }
  return atom::UNKNOWN;

}
}}
