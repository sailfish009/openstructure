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
#include "amino_acids.hh"

#include <boost/version.hpp>
#if BOOST_VERSION>=103800
#define BOOST_SPIRIT_USE_OLD_NAMESPACE
#include <boost/spirit/include/classic_symbols.hpp>
#else
#include <boost/spirit/symbols.hpp>
#endif


#include <ost/mol/chain_handle.hh>
#include <ost/mol/entity_handle.hh>
#include <ost/log.hh>
/*
 Author: Marco Biasini
 */
namespace ost { namespace qa {

using namespace boost::spirit;

namespace {

struct AminoAcidKeys : public symbols<AminoAcid> {
  AminoAcidKeys() {
    add("ALA", Ala)
       ("ARG", Arg)
       ("ASN", Asn)
       ("ASP", Asp)
       ("GLN", Gln) 
       ("GLU", Glu)
       ("LYS", Lys)
       ("SER", Ser)
       ("CYS", Cys)
       ("TYR", Tyr)
       ("THR", Thr)
       ("VAL", Val)
       ("ILE", Ile)
       ("LEU", Leu)
       ("PRO", Pro)
       ("GLY", Gly)
       ("MET", Met)
       ("HIS", His)
       ("TRP", Trp)
       ("PHE", Phe);
  }
};
} // anon namespace
   
AminoAcid ResidueToAminoAcid(const mol::ResidueHandle& r) {
  const mol::ResidueKey& key=r.GetKey();
  // use boost::spirits symbol table implementation that is particularly well 
  // suited for this job as it only has to traverse the keys once.
  static AminoAcidKeys aa_keys = AminoAcidKeys();
  AminoAcid* aa=find(aa_keys, key.c_str());
  if (aa)
    return *aa;
    
  return Xxx;
}

String AminoAcidToResidueName(AminoAcid aa)
{
  switch(aa) {
    case Ala:
      return "ALA";
    case Arg:
      return "ARG";
    case Asn:
      return "ASN";
    case Asp:
      return "ASP";
    case Gln:
      return "GLN";
    case Glu:
      return "GLU";
    case Lys:
      return "LYS";
    case Ser:
      return "SER";
    case Cys:
      return "CYS";
    case Tyr:  
      return "TYR";
    case Trp:
      return "TRP";
    case Thr:
      return "THR";
    case Val:
      return "VAL";
    case Ile:
      return "ILE";
    case Met:
      return "MET";
    case Leu:  
      return "LEU";
    case Gly:  
      return "GLY";
    case Pro:  
      return "PRO";
    case His:  
      return "HIS";
    case Phe:
      return "PHE";
    default:
      return "UNK";
  }
}

String OneLetterCodeToResidueName(char olc)
{
  char upper_olc=toupper(olc);
  switch(upper_olc) {
    case 'A':
      return "ALA";
    case 'R':
      return "ARG";
    case 'N':
      return "ASN";
    case 'D':
      return "ASP";
    case 'Q':
      return "GLN";
    case 'E':
      return "GLU";
    case 'K':
      return "LYS";
    case 'S':
      return "SER";
    case 'C':
      return "CYS";
    case 'Y':  
      return "TYR";
    case 'W':
      return "TRP";
    case 'T':
      return "THR";
    case 'V':
      return "VAL";
    case 'I':
      return "ILE";
    case 'M':
      return "MET";
    case 'L':  
      return "LEU";
    case 'G':  
      return "GLY";
    case 'P':  
      return "PRO";
    case 'H':  
      return "HIS";
    case 'F':
      return "PHE";
    default:
      return "UNK";
  }
}

char ResidueNameToOneLetterCode(String rn)
{
  String upper_rn=rn;
  std::transform(rn.begin(),rn.end(),rn.begin(),toupper);
  if (upper_rn == "ALA") {    
    return 'A';
  }  
  if (upper_rn == "ARG") {
    return 'R';
  }
  if (upper_rn == "ASN") {
    return 'N';
  }  
  if (upper_rn == "ASP") {
    return 'D';
  }  
  if (upper_rn == "GLN") {
    return 'Q';
  }  
  if (upper_rn == "GLU") {
    return 'E';
  }  
  if (upper_rn == "LYS") {
    return 'K';
  }  
  if (upper_rn == "SER") {
    return 'S';
  }  
  if (upper_rn == "CYS") {      
    return 'C';
  }  
  if (upper_rn == "TYR") {  
    return 'Y';
  }  
  if (upper_rn == "TRP") {
    return 'W';
  }  
  if (upper_rn == "THR") {
    return 'T';
  }  
  if (upper_rn == "VAL") {
    return 'V';
  }  
  if (upper_rn == "ILE") {
    return 'I';
  }  
  if (upper_rn == "MET") {
    return 'M';
  }  
  if (upper_rn == "LEU") {  
    return 'L';
  }  
  if (upper_rn == "GLY") {  
    return 'G';
  }  
  if (upper_rn == "PRO") {  
    return 'P';
  }  
  if (upper_rn == "HIS") {  
    return 'H';
  }  
  if (upper_rn == "PHE") {
    return 'F';
  }  
  return 'X';
}

AminoAcid OneLetterCodeToAminoAcid(char olc)
{
  char upper_olc=toupper(olc);
  switch(upper_olc) {
    case 'A':
      return Ala;
    case 'R':   
      return Arg;
    case 'N':   
      return Asn;
    case 'D':   
      return Asp;
    case 'Q':   
      return Gln;
    case 'E':   
      return Glu;
    case 'K':   
      return Lys;
    case 'S':   
      return Ser;
    case 'C':   
      return Cys;
    case 'Y':   
      return Tyr;
    case 'W':   
      return Trp;
    case 'T':   
      return Thr;
    case 'V':   
      return Val;
    case 'I':   
      return Ile;
    case 'M':   
      return Met;
    case 'L':   
      return Leu;
    case 'G':   
      return Gly;
    case 'P':   
      return Pro;
    case 'H':   
      return His;
    case 'F':   
      return Phe;
    default:
      return Xxx;
  }
}

AminoAcidSet AminoAcidSet::CreatePolarSet()
{
  AminoAcidSet polar;
  polar.Add(Arg);
  polar.Add(Asn);
  polar.Add(Asp);
  polar.Add(Gln);
  polar.Add(Glu);
  polar.Add(Ser);
  polar.Add(His);
  polar.Add(Lys);
  polar.Add(Thr);
  return polar;  
}

AminoAcidSet AminoAcidSet::CreateAromaticSet()
{
  AminoAcidSet aromatic;
  aromatic.Add(Trp);
  aromatic.Add(Tyr);
  aromatic.Add(Phe);
  return aromatic;  
}

AminoAcidSet AminoAcidSet::CreateApolarSet()
{
  AminoAcidSet apolar;
  apolar.Add(Ala);
  apolar.Add(Val);
  apolar.Add(Leu);
  apolar.Add(Ile);
  apolar.Add(Gly);
  apolar.Add(Pro);
  apolar.Add(Cys);
  apolar.Add(Met);
  return apolar;
}

AminoAcidSet AminoAcidSet::CreateSet(AminoAcid aa)
{
  AminoAcidSet one_aa_set;
  one_aa_set.Add(aa);
  return one_aa_set;
}

std::vector<AminoAcidSet> AminoAcidSet::CreateCompleteSet() {
  std::vector<AminoAcidSet> alphabet;

  alphabet.push_back(AminoAcidSet::CreateSet(Ala));
  alphabet.push_back(AminoAcidSet::CreateSet(Arg));
  alphabet.push_back(AminoAcidSet::CreateSet(Asn));
  alphabet.push_back(AminoAcidSet::CreateSet(Asp));
  alphabet.push_back(AminoAcidSet::CreateSet(Gln));
  alphabet.push_back(AminoAcidSet::CreateSet(Glu));
  alphabet.push_back(AminoAcidSet::CreateSet(Lys));
  alphabet.push_back(AminoAcidSet::CreateSet(Ser));
  alphabet.push_back(AminoAcidSet::CreateSet(Cys));
  alphabet.push_back(AminoAcidSet::CreateSet(Met));
  alphabet.push_back(AminoAcidSet::CreateSet(Trp));
  alphabet.push_back(AminoAcidSet::CreateSet(Tyr));
  alphabet.push_back(AminoAcidSet::CreateSet(Thr));
  alphabet.push_back(AminoAcidSet::CreateSet(Val));
  alphabet.push_back(AminoAcidSet::CreateSet(Ile));
  alphabet.push_back(AminoAcidSet::CreateSet(Leu));
  alphabet.push_back(AminoAcidSet::CreateSet(Gly));
  alphabet.push_back(AminoAcidSet::CreateSet(Pro));
  alphabet.push_back(AminoAcidSet::CreateSet(His));
  alphabet.push_back(AminoAcidSet::CreateSet(Phe));
  return alphabet;
}


std::vector<AminoAcidSet> AminoAcidSet::CreateThreeStateSet() {
  std::vector<AminoAcidSet> alphabet;

  alphabet.push_back(CreatePolarSet());
  alphabet.push_back(CreateApolarSet());
  alphabet.push_back(CreateAromaticSet());
  return alphabet;
}

std::vector<AminoAcidSet> AminoAcidSet::CreatePseudoSet() {
  std::vector<AminoAcidSet> alphabet;
  AminoAcidSet full_alphabet;

  full_alphabet.Add(Ala);
  full_alphabet.Add(Val);
  full_alphabet.Add(Leu);
  full_alphabet.Add(Ile);
  full_alphabet.Add(Gly);
  full_alphabet.Add(Pro);
  full_alphabet.Add(Cys);
  full_alphabet.Add(Met);
  full_alphabet.Add(Trp);
  full_alphabet.Add(Tyr);
  full_alphabet.Add(Phe);
  full_alphabet.Add(Arg);
  full_alphabet.Add(Asn);
  full_alphabet.Add(Asp);
  full_alphabet.Add(Gln);
  full_alphabet.Add(Glu);
  full_alphabet.Add(Ser);
  full_alphabet.Add(His);
  full_alphabet.Add(Lys);
  full_alphabet.Add(Thr);

  alphabet.push_back(full_alphabet);
  return alphabet;
}


AminoAcidSet::AminoAcidSet():
  bits_(0)
{
  
}

void AminoAcidSet::Add(AminoAcid amino_acid)
{
  bits_|=(1 << (amino_acid));
}

void AminoAcidSet::Remove(AminoAcid amino_acid)
{
  bits_&=~(1 << (amino_acid));
}

bool AminoAcidSet::Contains(AminoAcid amino_acid) const
{
  return (bits_ & (1 << (amino_acid)))!=0;
}

bool AminoAcidSet::Empty() const
{
  return bits_==0;
}

AminoAcidSet::Iterator AminoAcidSet::Begin() const
{
  // find first bit that is set
  int start=0;
  while (start<=Xxx+1 && !(bits_ & (1 << start))) {
    ++start;
  }
  return AminoAcidSetIterator(bits_, start);
}

AminoAcidSet::Iterator AminoAcidSet::End() const
{
  return AminoAcidSetIterator(bits_, Xxx+1);
}

bool AminoAcidSet::operator==(const AminoAcidSet& rhs) const
{
  return bits_==rhs.bits_;
}

bool AminoAcidSet::operator!=(const AminoAcidSet& rhs) const
{
  return !this->operator==(rhs);
}

std::ostream& operator<<(std::ostream& os, 
                         const AminoAcidSet& aa_set)
{
  os << "[";
  for (AminoAcidSet::Iterator i=aa_set.Begin(), e=aa_set.End(); i!=e; ++i) {
    os << " " << AminoAcidToResidueName(*i);    
  }
  os << " ]";
  return os;
}

}}
