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
#include <algorithm>
#include <limits>

#include <boost/bind.hpp>

#include <ost/mol/chain_view.hh>
#include <ost/mol/residue_view.hh>
#include <ost/mol/entity_visitor.hh>
#include <ost/mol/entity_view.hh>
#include <ost/mol/atom_view.hh>
#include <ost/mol/impl/residue_impl.hh>
#include <ost/mol/impl/chain_impl.hh>
#include <ost/mol/entity_handle.hh>

using boost::bind;

namespace ost { namespace mol {

namespace {

// Ideally, the ResNumComp could be written with two call operators, one of
// the form (ResNum, ResidueView) and one (ResidueView, ResNum). Unfortunately,
// the STL on windows wants two argumentso of identical type. That's why we
// work around it by storing a ResNum internally and the ResidueView that is
// valid against it. But because this version is slightly slower, we use the
// fast version where possible.
struct ResNumComp {
  ResNumComp(const ResNum& num): number_(num) {}
  ResNumComp(): number_(1) {};
  bool operator()(const ResNum& a,
                  const ResidueView& b) const {
    return a<b.GetNumber();
  }
  bool operator()(const ResidueView& a,
                  const ResNum& b) const {
    return a.GetNumber()<b;
  }   
  bool operator()(const ResidueView& a, const ResidueView& b)
  {
    assert(!(a.IsValid() && b.IsValid()));
    return a.IsValid() ? a.GetNumber()<number_ : number_<b.GetNumber();
  }
private:
  ResNum number_;
};

}

class DLLEXPORT_OST_MOL ChainViewData {
public:
  ChainViewData(const EntityView& view)
    : entity(view.ViewData()), in_sequence(true) {  
  }
  EntityViewDataWeakPtr entity;
  ResidueViewList residues;
  bool            in_sequence;
};

ChainView::ChainView() {

}

EntityView ChainView::GetEntity() const {
  return EntityView(data_->entity.lock(), Impl()->GetEntity());
}

ChainView::ChainView(const EntityView& entity,
                     const ChainHandle& chain)
 : ChainBase(chain.Impl()), data_(new ChainViewData(entity)) {
}

ChainView::ChainView(ChainViewDataPtr data, 
                     impl::ChainImplPtr impl) 
 : ChainBase(impl), data_(data) {
}

ChainHandle ChainView::GetHandle() const {
  return ChainHandle(Impl());
}

void ChainView::Apply(EntityVisitor& visitor) 
{
  this->CheckValidity();  
  ResidueViewList::iterator i;
  if (visitor.VisitChain(this->GetHandle())) {
    for (i=data_->residues.begin(); i!=data_->residues.end(); ++i) {
      (*i).Apply(visitor);
    }    
  }
}

void ChainView::Apply(EntityViewVisitor& visitor) 
{
  this->CheckValidity();  
  if (visitor.VisitChain(*this)) {
    ResidueViewList::iterator i;
    for (i=data_->residues.begin(); i!=data_->residues.end(); ++i) {
      (*i).Apply(visitor);
    }    
  }
}

int ChainView::GetResidueCount() const {
  this->CheckValidity();  
  return static_cast<int>(data_->residues.size());
}

int ChainView::GetAtomCount() const {
  this->CheckValidity();  
  int count=0;
  ResidueViewList::const_iterator it=data_->residues.begin();
  while(it!=data_->residues.end()) {
    count+=(*it).GetAtomCount();
    ++it;
  }
  return count;
}

ResidueView ChainView::FindResidue(const ResNum& number) const {
  this->CheckValidity();  
  const ResidueViewList& l=data_->residues;
  ResidueViewList::const_iterator i;
  if (data_->in_sequence) {
    std::pair<ResidueViewList::const_iterator, 
              ResidueViewList::const_iterator> p;
#if defined(_MSC_VER)            
    p=std::equal_range(l.begin(), l.end(), ResidueView(), ResNumComp(number));    
#else
    p=std::equal_range(l.begin(), l.end(), number, ResNumComp());    
#endif
    if (p.first!=p.second) {
     return *p.first;
    }
    return ResidueView();
  } else {
    i=std::find_if(l.begin(), l.end(), 
                   bind(&ResidueView::GetNumber, _1)==number);
    return i==data_->residues.end() ? ResidueView() : *i;    
  }
}

ResidueView ChainView::FindResidue(const ResidueHandle& handle) const {
  this->CheckValidity();  
  const ResidueViewList& l=data_->residues;
  ResidueViewList::const_iterator i;
  i=std::find_if(l.begin(), l.end(), bind(&ResidueView::GetHandle, _1)==handle);
  return i==data_->residues.end() ? ResidueView() : *i;
}


bool ChainView::IsResidueIncluded(const ResidueHandle& handle) const {
  this->CheckValidity();  
  return this->FindResidue(handle).IsValid();
}

ResidueView ChainView::AddResidue(const ResidueHandle& residue_handle, 
                                  ViewAddFlags flags) {
  this->CheckValidity();                                    
  ResidueView rv;
  if ((flags & ViewAddFlag::CHECK_DUPLICATES) && 
      (rv=this->FindResidue(residue_handle)))
    return rv;
  rv=ResidueView(*this, residue_handle);
  if (!data_->residues.empty()) {
    if (data_->residues.back().GetNumber()>=rv.GetNumber())
      data_->in_sequence=false;
  }  
  data_->residues.push_back(rv);
  if (flags & ViewAddFlag::INCLUDE_ATOMS) {
    const impl::AtomImplList& l=residue_handle.Impl()->GetAtomList();
    for (impl::AtomImplList::const_iterator i=l.begin(); i!=l.end(); ++i) {
      rv.AddAtom(AtomHandle(*i), flags);
    }
  }  
  return rv;
}

AtomView ChainView::FindAtom(const AtomHandle& atom) const {
  ResidueHandle residue=atom.GetResidue();
  if (atom.GetEntity()!=this->GetEntity().GetHandle())
    return AtomView();

  ResidueView v=this->FindResidue(residue.GetNumber());
  return v.IsValid() ? v.FindAtom(atom) : AtomView();
}

AtomView ChainView::AddAtom(const AtomHandle& atom_handle, 
                            ViewAddFlags flags) {
  ResidueView rv=this->AddResidue(atom_handle.GetResidue(), 
                                  ViewAddFlag::CHECK_DUPLICATES);
  return rv.AddAtom(atom_handle, flags);
}

const ResidueViewList& ChainView::GetResidueList() const {
  this->CheckValidity();
  return data_->residues;
}



void ChainView::RemoveResidue(ResidueView view) {
  this->CheckValidity();
  if (!view.IsValid())
    return;
  view.RemoveAtoms();
  ResidueViewList::iterator i=data_->residues.begin();
  for (; i!=data_->residues.end(); ++i) {
    if (*i==view) {
      data_->residues.erase(i);
      return;
    }
  }
}

ResidueView ChainView::AddResidue(const ResidueView& residue_view, 
                                  ViewAddFlags flags) {
  this->CheckValidity();
  ResidueView rv;
  if ((flags & ViewAddFlag::CHECK_DUPLICATES) && 
      (rv=this->FindResidue(residue_view.GetHandle()))) {
    if (!(flags & ViewAddFlag::INCLUDE_ATOMS)) {
      return rv;
    }
  } else {
    rv=ResidueView(*this, residue_view.GetHandle());    
  }

  if (!data_->residues.empty()) {
    if (data_->residues.back().GetNumber()>=rv.GetNumber())
      data_->in_sequence=false;
  }
  data_->residues.push_back(rv);
  if (flags & ViewAddFlag::INCLUDE_ATOMS) {
    AtomViewList::const_iterator i=residue_view.GetAtomList().begin();
    for (; i!=residue_view.GetAtomList().end(); ++i) {
      rv.AddAtom(*i);
    }
  }
  return rv;  
}
                       
void ChainView::RemoveResidues() {
  this->CheckValidity();
  std::for_each(data_->residues.begin(), data_->residues.end(),
                bind(&ResidueView::RemoveAtoms, _1));
  data_->residues.clear();
}



ResidueView ChainView::GetResidueByIndex(int i) const
{
  this->CheckValidity();
  bool bounded=(i>=0 && i<static_cast<int>(data_->residues.size()));
  return bounded ? data_->residues[i] : ResidueView();
}

int ChainView::GetResidueIndex(const ResNum& number) const
{
  this->CheckValidity();
  ResidueViewList::iterator i;
  if (data_->in_sequence) {
    std::pair<ResidueViewList::iterator, ResidueViewList::iterator> p;
#if defined(_MSC_VER)  
    p=std::equal_range(data_->residues.begin(), data_->residues.end(), 
                       ResidueView(), ResNumComp(number));    
#else
    p=std::equal_range(data_->residues.begin(), data_->residues.end(), 
                       number, ResNumComp());    
#endif
    i=p.first;
  } else {
    i=std::find_if(data_->residues.begin(), data_->residues.end(),
                   bind(&ResidueView::GetNumber, _1)==number);    
  }

  return i==data_->residues.end() ? -1 : i-data_->residues.begin();
}

bool ChainView::operator==(const ChainView& rhs) const
{
  return data_==rhs.data_;
}

bool ChainView::operator!=(const ChainView& rhs) const
{
  return !this->operator==(rhs);
}

Real ChainView::GetMass() const {
  this->CheckValidity();
  Real mass = 0;
  ResidueViewList::const_iterator i;
  for (i=data_->residues.begin(); i!=data_->residues.end(); ++i) {
    ResidueView r=*i;
    for (AtomViewList::const_iterator j=r.GetAtomList().begin(),
         e2=r.GetAtomList().end(); j!=e2; ++j) {
      mass+=j->GetProp().mass;
    }
  }
  return mass;
}

geom::Vec3 ChainView::GetGeometricStart() const
{
  this->CheckValidity();
  geom::Vec3 minimum(std::numeric_limits<Real>::max(),
                     std::numeric_limits<Real>::max(),
                     std::numeric_limits<Real>::max());
  ResidueViewList::const_iterator i;
  for (i=data_->residues.begin(); i!=data_->residues.end(); ++i) {
    ResidueView r=*i;
    for (AtomViewList::const_iterator j=r.GetAtomList().begin(),
         e2=r.GetAtomList().end(); j!=e2; ++j) {
      minimum=geom::Min(minimum, j->GetPos());
    }
  }
  return minimum;
}

geom::Vec3 ChainView::GetGeometricEnd() const
{
  this->CheckValidity();
  geom::Vec3 maximum(-std::numeric_limits<Real>::max(),
                     -std::numeric_limits<Real>::max(),
                     -std::numeric_limits<Real>::max());
  ResidueViewList::const_iterator i;
  for (i=data_->residues.begin(); i!=data_->residues.end(); ++i) {
    ResidueView r=*i;
    for (AtomViewList::const_iterator j=r.GetAtomList().begin(),
         e2=r.GetAtomList().end(); j!=e2; ++j) {
      maximum=geom::Max(maximum, j->GetPos());
    }
  }
  return maximum;
}

geom::Vec3 ChainView::GetGeometricCenter() const
{
  this->CheckValidity();
  return (this->GetGeometricStart() + this->GetGeometricEnd())/2;
}

geom::Vec3 ChainView::GetCenterOfAtoms() const
{
  this->CheckValidity();
  geom::Vec3 center;
  if(this->GetAtomCount() > 0) {
    ResidueViewList::const_iterator i;
    for (i=data_->residues.begin(); i!=data_->residues.end(); ++i) {
      ResidueView r=*i;
      for (AtomViewList::const_iterator j=r.GetAtomList().begin(),
           e2=r.GetAtomList().end(); j!=e2; ++j) {
        center+=j->GetPos();
      }
    }
    center/=this->GetAtomCount();
  }
  return center;
}

geom::Vec3 ChainView::GetCenterOfMass() const
{
  this->CheckValidity();
  geom::Vec3 center;
  Real mass = this->GetMass();
  if(this->GetAtomCount() > 0 && mass > 0) {
    ResidueViewList::const_iterator i;
    for (i=data_->residues.begin(); i!=data_->residues.end(); ++i) {
      ResidueView r=*i;
      for (AtomViewList::const_iterator j=r.GetAtomList().begin(),
          e2=r.GetAtomList().end(); j!=e2; ++j) {
        center+=j->GetPos() * j->GetProp().mass;
      }
    }
    center/=mass;
  }
  return center;
}

AtomView ChainView::FindAtom(const ResNum& num, 
                             const String& name) const
{
  this->CheckValidity();
  ResidueView res=this->FindResidue(num);
  return res.IsValid() ? res.FindAtom(name) : AtomView();
}


bool ChainView::InSequence() const
{
  this->CheckValidity();
  return data_->in_sequence;
}

EntityView ChainView::Select(const Query& q, QueryFlags flags) const
{
  this->CheckValidity();
  if (q.GetQueryString() != "") {
    return this->GetEntity().Select(Query("cname='"+Impl()->GetName()+"' and ("+
                                        q.GetQueryString()+")"), flags);
  }
  else {
  return this->GetEntity().Select(Query("cname='"+Impl()->GetName()+"'"), flags);
}
}

EntityView ChainView::Select(const String& q, QueryFlags flags) const {
  this->CheckValidity();
  if (q != "") {
    return this->GetEntity().Select(Query("cname='"+Impl()->GetName()+"' and ("+
                                        q+")"), flags);
  }
  else return this->GetEntity().Select(Query("cname='"+Impl()->GetName()+"'"), flags);
}
}} // ns

