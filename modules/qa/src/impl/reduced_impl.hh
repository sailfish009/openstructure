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

#ifndef OST_QA_IMPL_REDUCED_HH
#define OST_QA_IMPL_REDUCED_HH


#include <ost/mol/entity_visitor.hh>
#include <ost/mol/entity_handle.hh>
#include <ost/mol/entity_view.hh>
#include <ost/qa/module_config.hh>
#include <ost/qa/amino_acids.hh>
#include <ost/qa/reduced_statistics.hh>

namespace ost { namespace qa { namespace impl {

class DLLEXPORT_OST_QA ReducedPotentialImpl : public mol::EntityVisitor {
public:

  ReducedPotentialImpl(const ReducedStatOptions& opts, mol::EntityHandle ent): 
    opts_(opts), ent_(ent)
  { }
  
  ReducedPotentialImpl(const ReducedStatOptions& opts, mol::EntityView ent): 
    opts_(opts), view_(ent)
  { }

  virtual bool VisitResidue(const mol::ResidueHandle& res);

  static bool GetCAlphaCBetaPos(const mol::ResidueHandle& res, 
                                geom::Vec3& ca_pos, 
                                geom::Vec3& cb_pos);
                         
  virtual void OnInteraction(AminoAcid aa_one, AminoAcid aa_two, 
                             Real dist, Real angle)=0;
private:  
  void HandleResidue(AminoAcid aa_one, const geom::Vec3& ca_pos_one, 
                     const geom::Vec3& cb_pos_one, 
                     uint index_one, const mol::AtomHandle& ca_two);
  ReducedStatOptions opts_;
  mol::EntityHandle  ent_;
  mol::EntityView    view_;
};

}}}

#endif
