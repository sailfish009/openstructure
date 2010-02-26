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
#include <ost/info/info.hh>
#include "subst_weight_matrix.hh"

namespace ost { namespace seq { namespace alg {

SubstWeightMatrixPtr SubstWeightMatrixFromInfo(const info::InfoGroup& group)
{
  SubstWeightMatrixPtr m(new SubstWeightMatrix);
  info::InfoItemList weights=group.GetItems("Weight");
  for (info::InfoItemList::iterator i=weights.begin(),
       e=weights.end(); i!=e; ++i) {
    info::InfoItem item=*i;
    String from=item.GetAttribute("From");
    String to=item.GetAttribute("To");
    m->SetWeight(from[0], to[0], item.AsInt());
  }
  return m;
}

void SubstWeightMatrixToInfo(const SubstWeightMatrixPtr& subst_mat, 
                             info::InfoGroup& group)
{
  for (int i=0; i<SubstWeightMatrix::ALPHABET_SIZE; ++i) {
    for (int j=0; j<SubstWeightMatrix::ALPHABET_SIZE; ++j) {
      char from=char(i+'A');
      char to=char(j+'A');
      info::InfoItem item=group.CreateItem("Weight", 
                                           subst_mat->GetWeight(from, to));
      item.SetAttribute("From", String(1, from)); 
      item.SetAttribute("To", String(1, to));
    }
  }
}

}}}
