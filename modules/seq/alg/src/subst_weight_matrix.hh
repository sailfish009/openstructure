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
#ifndef OST_SEQ_SUBST_WEIGHT_MATRIX_HH
#define OST_SEQ_SUBST_WEIGHT_MATRIX_HH

#include <ctype.h>
#include <string.h>
#include <ost/info/info_fw.hh>
#include <ost/seq/alg/module_config.hh>

/*
  Author: Marco Biasini
 */
namespace ost { namespace seq { namespace alg {

class SubstWeightMatrix;

typedef boost::shared_ptr<SubstWeightMatrix> SubstWeightMatrixPtr;
  
/// \brief position-independet substitution weight matrix  
class DLLEXPORT_OST_SEQ_ALG SubstWeightMatrix {
  
public:
  typedef short WeightType;
  const static int ALPHABET_SIZE='Z'-'A'+1;
  /// \brief Initialize substitution matrix with zero.
  /// 
  /// In order to get a useful  substitution weight matrix, use SetWeight(). 
  /// Alternatively you may want to load the substitution from an  info group.
  SubstWeightMatrix() {
    ::memset(weights_, 0, sizeof(WeightType)*ALPHABET_SIZE*ALPHABET_SIZE);
  }

  WeightType GetWeight(char aa_one, char aa_two) const 
  {
    int i=Index(aa_one, aa_two); 
    assert(i>=0 && i<ALPHABET_SIZE*ALPHABET_SIZE);    
    return weights_[i];
  }

  void SetWeight(char aa_one, char aa_two, WeightType weight) 
  {
    int i=Index(aa_one, aa_two);
    assert(i>=0 && i<ALPHABET_SIZE*ALPHABET_SIZE);
    weights_[i]=weight;
  }
private:
  int Index(char aa_one, char aa_two) const {
    return (toupper(aa_one)-'A')*ALPHABET_SIZE+(toupper(aa_two)-'A');
  }
   WeightType weights_[ALPHABET_SIZE*ALPHABET_SIZE];
};

SubstWeightMatrixPtr DLLEXPORT_OST_SEQ_ALG
SubstWeightMatrixFromInfo(const info::InfoGroup& group);

void DLLEXPORT_OST_SEQ_ALG 
SubstWeightMatrixToInfo(const SubstWeightMatrixPtr& subst_mat, 
                        info::InfoGroup& group);

}}}

#endif
