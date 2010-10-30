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
#include <ost/log.hh>
#include <ost/mol/entity_visitor.hh>

#include "chain_impl.hh"
#include "residue_impl.hh"
#include "atom_impl.hh"
#include "entity_impl.hh"
#include <boost/bind.hpp>
#include <algorithm>
#include "torsion_impl.hh"
#include <ost/mol/impl/connector_impl.hh>

using boost::bind;

namespace ost { namespace mol { namespace impl {

ResidueImpl::ResidueImpl(const EntityImplPtr& ent,
                         const ChainImplPtr& ch,
                         const ResNum& num,
                         const ResidueKey& key):
  ent_(ent),
  chain_(ch),
  num_(num),
  key_(key),
  atom_list_(),
  sec_structure_(SecStructure::COIL),
  olc_('?')
{}


AtomImplPtr ResidueImpl::InsertAtom(const String& name,
                                    const geom::Vec3& pos,
                                    const String& ele)
{
  AtomImplPtr ap=ent_.lock()->CreateAtom(shared_from_this(),
                                         name, pos, ele);
  atom_list_.push_back(ap);
  return ap;
}

AtomImplPtr ResidueImpl::InsertAtom(const AtomImplPtr& atom)
{
  AtomImplPtr dst_atom=this->InsertAtom(atom->GetName(), 
                                        atom->GetPos(),
                                        atom->GetElement());

  dst_atom->Assign(*atom.get());
  dst_atom->SetState(atom->GetState());
  dst_atom->SetBFactor(atom->GetBFactor());
  dst_atom->SetOccupancy(atom->GetOccupancy());
  dst_atom->SetHetAtom(atom->IsHetAtom());  

  if (!atom->HasDefaultProps()) {
    dst_atom->SetRadius(atom->GetRadius());
    dst_atom->SetCharge(atom->GetCharge());
    dst_atom->SetMass(atom->GetMass());
    dst_atom->SetAnisou(atom->GetAnisou());
  }
  return dst_atom;
}

Real ResidueImpl::GetAverageBFactor() const
{
  Real sum=0;
  for (AtomImplList::const_iterator i=atom_list_.begin(), 
       e=atom_list_.end(); i!=e; ++i) {
    sum+=(*i)->GetBFactor();
  }
  return atom_list_.size()>0 ? sum/atom_list_.size() : 0.0;
}

void ResidueImpl::AddAltAtom(const String& group, const AtomImplPtr& atom,
                             const geom::Vec3& position) {

  if (group.length()==0)
   throw Error("alt atom group name can't be empty String");
  AtomEntryGroups::iterator i=alt_groups_.find(group);
  if (i==alt_groups_.end()) {
    std::pair<AtomEntryGroups::iterator, bool> p;
    AtomGroup ag;
    ag.name=group;
    p=alt_groups_.insert(std::make_pair(group, ag));
    i=p.first;
    if (alt_groups_.size()==1) {
      curr_group_=group;
    }
  }
  i->second.atoms.push_back(AtomGroupEntry(atom, position));
}

geom::Vec3 ResidueImpl::GetAltAtomPos(const AtomImplPtr& atom, 
                                      const String& group) const
{
  AtomEntryGroups::const_iterator i=alt_groups_.find(group);
  if (i==alt_groups_.end()) {
    throw Error("No alt atom group '"+group+"'");
  }
  const AtomGroup& g=i->second;
  for (AtomGroupEntryList::const_iterator j=g.atoms.begin(), 
       e=g.atoms.end(); j!=e; ++j) {
    if (atom==j->atom.lock()) {
      return j->pos;
    }
  }
  throw Error(atom->GetQualifiedName()+
              " does not have alternative atom position '"+group+"'");
}

AtomImplPtr ResidueImpl::InsertAltAtom(const String& name,
                                       const String& alt_group,
                                       const geom::Vec3& pos,
                                       const String& ele) {
  AtomImplPtr atom=this->InsertAtom(name, pos, ele);
  this->AddAltAtom(alt_group, atom, pos);
  return atom;
}

void ResidueImpl::Apply(EntityVisitor& v)
{
  LOG_TRACE("visitor @" << &v << " visiting residue ; @" << this);
  if(v.VisitResidue(ResidueHandle(shared_from_this()))) {
    for (AtomImplList::iterator it=atom_list_.begin();
         it!=atom_list_.end();++it) {
      (*it)->Apply(v);
    }
  }
}

SecStructure ResidueImpl::GetSecStructure() const 
{
  return sec_structure_;
}

void ResidueImpl::SetSecStructure(SecStructure ss) 
{
  sec_structure_=ss;
}

EntityImplPtr ResidueImpl::GetEntity() const
{
  return ent_.lock();
}

AtomImplPtr ResidueImpl::GetCentralAtom() const
{
  if (chem_class_.IsNucleotideLinking()) {
    for (AtomImplList::const_iterator it=atom_list_.begin();
         it!=atom_list_.end();++it) {
      if((*it)->GetName()=="P") return *it;
    }    
  } else if (chem_class_.IsPeptideLinking()) {
    for (AtomImplList::const_iterator it=atom_list_.begin();
         it!=atom_list_.end();++it) {
      if((*it)->GetName()=="CA") return *it;
    }    
  }

  return AtomImplPtr();
}

char ResidueImpl::GetOneLetterCode() const
{
  return olc_;
}

void ResidueImpl::SetOneLetterCode(const char olc)  
{
  olc_=olc;
}

int ResidueImpl::GetAtomCount() const 
{
  return static_cast<int>(atom_list_.size());
}

int ResidueImpl::GetBondCount() const 
{
  int count=0;
  AtomImplList::const_iterator it=atom_list_.begin();
  while(it!=atom_list_.end()) {
    count+=(AtomHandle(*it)).GetBondCount();
    ++it;
  }
  return count/2;
}


ChainImplPtr ResidueImpl::GetChain() const
{
  return chain_.lock();
}

geom::Vec3 ResidueImpl::GetCentralNormal() const
{
  if (chem_class_.IsPeptideLinking()) {
    AtomImplPtr a1 = FindAtom("C");
    AtomImplPtr a2 = FindAtom("O"); 
    geom::Vec3 nrvo;
    if(a1 && a2) {
      nrvo = geom::Normalize(a1->GetPos()-a2->GetPos());
    } else {
      geom::Vec3 v0=GetCentralAtom()->GetPos();
      nrvo=geom::Cross(geom::Normalize(v0),
                       geom::Normalize(geom::Vec3(-v0[2],v0[0],v0[1])));
      LOG_VERBOSE("warning: could not find atoms for proper central normal calculation");
    }
    return nrvo;    
  } else if (chem_class_.IsNucleotideLinking()) {
    AtomImplPtr a1 = FindAtom("P");
    AtomImplPtr a2 = FindAtom("OP1");
    AtomImplPtr a3 = FindAtom("OP2");
    geom::Vec3 nrvo;
    if(a1 && a2 && a3) {
      nrvo = geom::Normalize(a1->GetPos()-(a2->GetPos()+a3->GetPos())*.5);
    } else {
      geom::Vec3 v0=GetCentralAtom()->GetPos();
      nrvo=geom::Cross(geom::Normalize(v0),
                       geom::Normalize(geom::Vec3(-v0[2],v0[0],v0[1])));
      LOG_VERBOSE("warning: could not find atoms for proper central normal calculation");
    }
    return nrvo;
  }
  return geom::Vec3();
}


AtomImplPtr ResidueImpl::FindAtom(const String& aname) const
{
  for (AtomImplList::const_iterator it=atom_list_.begin();
       it!=atom_list_.end();++it) {
    if ((*it)->GetName()==aname) {
      return *it;
    }
  }
  return AtomImplPtr();
}

const AtomImplList& ResidueImpl::GetAtomList() const
{
  return atom_list_;
}

TorsionImplP ResidueImpl::GetPsiTorsion() const {
  ChainImplPtr chain=this->GetChain();
  ResidueImplPtr self=const_cast<ResidueImpl*>(this)->shared_from_this();
  ResidueImplPtr next=chain->GetNext(self);
  if (!next || !InSequence(self, next))
    return TorsionImplP();
  AtomImplPtr calpha=this->FindAtom("CA");
  AtomImplPtr n=this->FindAtom("N");
  AtomImplPtr c=this->FindAtom("C");
  AtomImplPtr npo=next->FindAtom("N");
  TorsionImplList::const_iterator i=torsion_list_.begin();
  for (;i!=torsion_list_.end(); ++i) {
    TorsionImplP t=*i;
    if (t->Matches(n, calpha, c, npo))
      return t;
  }

  LOG_DEBUG("Can't find torsion PSI for " <<
           this->GetKey() << this->GetNumber());
  return TorsionImplP();

}

void  ResidueImpl::AddTorsion(const TorsionImplP& torsion) {
  torsion_list_.push_back(torsion);
}

TorsionImplP ResidueImpl::FindTorsion(const String& torsion_name) const {
  TorsionImplList::const_iterator i=torsion_list_.begin();
  for (; i!=torsion_list_.end(); ++i) {
    if ((*i)->GetName()==torsion_name) {
      return *i;
    }
  }
  return TorsionImplP();
}


TorsionImplP ResidueImpl::GetOmegaTorsion() const {
  ChainImplPtr chain=this->GetChain();
  ResidueImplPtr self=const_cast<ResidueImpl*>(this)->shared_from_this();
  ResidueImplPtr prev=chain->GetPrev(self);
  if (!prev || !InSequence(prev, self))
    return TorsionImplP();

  AtomImplPtr calpha_prev=prev->FindAtom("CA");
  AtomImplPtr c_prev=prev->FindAtom("C");
  AtomImplPtr n=this->FindAtom("N");
  AtomImplPtr calpha=this->FindAtom("CA");
  TorsionImplList::const_iterator i=torsion_list_.begin();

  for (;i!=torsion_list_.end(); ++i) {
    TorsionImplP t=*i;
    if (t->Matches(calpha_prev, c_prev, n,  calpha))
      return t;
  }

  LOG_DEBUG("Can't find torsion Omega for " <<
           this->GetKey() << this->GetNumber());
  return TorsionImplP();
}


TorsionImplP ResidueImpl::GetPhiTorsion() const {
  ChainImplPtr chain=this->GetChain();
  ResidueImplPtr self=const_cast<ResidueImpl*>(this)->shared_from_this();
  ResidueImplPtr prev=chain->GetPrev(self);
  if (!prev || !InSequence(prev, self))
    return TorsionImplP();

  AtomImplPtr cmo=prev->FindAtom("C");
  AtomImplPtr n=this->FindAtom("N");
  AtomImplPtr calpha=this->FindAtom("CA");
  AtomImplPtr c=this->FindAtom("C");
  TorsionImplList::const_iterator i=torsion_list_.begin();
  for (;i!=torsion_list_.end(); ++i) {
    TorsionImplP t=*i;
    if (t->Matches(cmo, n, calpha, c))
      return t;
  }

  LOG_DEBUG("Can't find torsion PHI for " <<
           this->GetKey() << this->GetNumber());
  return TorsionImplP();
}



void ResidueImpl::DeleteAtom(const AtomImplPtr& atom) {
  AtomImplList::iterator i=atom_list_.begin();
  for (; i!=atom_list_.end(); ++i) {
    if ((*i)==atom) {
      (*i)->DeleteAllTorsions();
      (*i)->DeleteAllConnectors();
      this->RemoveAltPositionsForAtom(atom);
      this->GetEntity()->DeleteAtom(*i);
      atom_list_.erase(i);
      break;
    }

  }
}

void ResidueImpl::DeleteAtoms(const String& atom_name) {
  AtomImplList::iterator i=atom_list_.begin();
  for (; i!=atom_list_.end(); ++i) {
    if ((*i)->GetName()==atom_name) {
      (*i)->DeleteAllTorsions();
      (*i)->DeleteAllConnectors();
      this->GetEntity()->DeleteAtom(*i);
    }
  }
  AtomImplList::iterator new_end;
  new_end=std::remove_if(atom_list_.begin(), atom_list_.end(),
                         bind(&AtomImpl::GetName, _1)==atom_name);
  atom_list_.erase(new_end, atom_list_.end());
}

void ResidueImpl::DeleteAllAtoms() {
  AtomImplList::iterator i=atom_list_.begin();
  for (; i!=atom_list_.end(); ++i) {
    (*i)->DeleteAllTorsions();
    (*i)->DeleteAllConnectors();
      this->GetEntity()->DeleteAtom(*i);
  }
  atom_list_.clear();
}

String ResidueImpl::GetQualifiedName() const {
  String chain_name=this->GetChain()->GetName();
  return (chain_name==" " ? "" :  chain_name+".")+
         this->GetKey()+
         this->GetNumber().AsString();
}

void ResidueImpl::RemoveAltPositionsForAtom(const AtomImplPtr& atom) {
  AtomEntryGroups::iterator i=alt_groups_.begin();
  bool set_name=false, inc=false;
  // we do not increment the iterator in the for loop directly, but do it
  // inside the loop to get our hands on the next iterator in the map
  // before we delete the element
  for (; i!=alt_groups_.end(); ) {
    AtomGroupEntryList::iterator k=i->second.atoms.begin();
    inc=true;
    for (; k!=i->second.atoms.end(); ++k) {
      if (k->atom.lock().get()==atom.get()) {
        i->second.atoms.erase(k);
        if (i->second.atoms.empty()) {
          AtomEntryGroups::iterator to_be_deleted=i;
          if (i->first==curr_group_) {
            set_name=true;
          }      
          ++i;
          inc=false;
          alt_groups_.erase(to_be_deleted);
        }
        break;
      }
    }
    if (inc) {
      ++i;
    }
  }
  if (set_name) {
    if (alt_groups_.size()==0)
      curr_group_="";
    else
      curr_group_=alt_groups_.begin()->first;
  }
}

Real ResidueImpl::GetMass() const
{
  Real mass = 0;
  for (AtomImplList::const_iterator i=atom_list_.begin(); 
       i!=atom_list_.end(); ++i) {
    mass+=(*i)->GetMass();
  }
  return mass;
}

geom::AlignedCuboid ResidueImpl::GetBounds() const 
{
  
  geom::Vec3 mmin( std::numeric_limits<Real>::infinity());
  geom::Vec3 mmax(-std::numeric_limits<Real>::infinity());  

  if (atom_list_.size()>0) {
    AtomImplList::const_iterator i=atom_list_.begin();
    mmin=mmax=(*i)->GetPos();
    for (++i; i!=atom_list_.end(); ++i) {
      mmax=geom::Max(mmax,(*i)->GetPos());
      mmin=geom::Min(mmin,(*i)->GetPos());      
    }    
    return geom::AlignedCuboid(mmin, mmax);
  } else {
    return geom::AlignedCuboid(geom::Vec3(), geom::Vec3());
  }
}

geom::Vec3 ResidueImpl::GetCenterOfAtoms() const
{
  geom::Vec3 sum;
  if (!atom_list_.empty()) {
    for (AtomImplList::const_iterator i=atom_list_.begin(); 
        i!=atom_list_.end(); ++i) {
      sum+=(*i)->GetPos();
    }
    sum/=atom_list_.size();
  }
  return sum;
}

geom::Vec3 ResidueImpl::GetCenterOfMass() const
{
  geom::Vec3 center;
  Real mass = this->GetMass();
  if (this->GetAtomCount() > 0 && mass > 0) {
    for (AtomImplList::const_iterator i=atom_list_.begin(); 
        i!=atom_list_.end(); ++i) {
      center+=(*i)->GetPos()*(*i)->GetMass();
    }
  }
  return center/mass;
}

void ResidueImpl::AddAltAtomPos(const String& group,
                                     const AtomImplPtr& atom,
                                     const geom::Vec3& position) {
  // Make sure atom is already registered for having an alternative position.
  // Bail out, if this is not the case.
  AtomEntryGroups::iterator i=alt_groups_.begin();
  bool found=false;
  for (; i!=alt_groups_.end(); ++i) {
    AtomGroupEntryList::iterator k=i->second.atoms.begin();
    for (; k!=i->second.atoms.end(); ++k) {
      AtomGroupEntry& entry=*k;
      if (entry.atom.lock().get()==atom.get()) {
        found=true;
        break;
      }
    }
  }
  if (found)
    this->AddAltAtom(group, atom, position);
  else {
    String m="Definition of alternative position without prior call to "
             "InsertAltAtom is not allowed";
    throw Error(m);
  }
}

int ResidueImpl::GetIndex() const {
  return this->GetChain()->GetIndex(this->GetNumber());
}

bool ResidueImpl::HasAltAtomGroup(const String& group) const {
  return alt_groups_.find(group)!=alt_groups_.end();
}

std::vector<String> ResidueImpl::GetAltAtomGroupNames() const {
  std::vector<String> names;
  AtomEntryGroups::const_iterator i=alt_groups_.begin();
  for (; i!=alt_groups_.end(); ++i) {
    names.push_back(i->first);
  }
  return names;
}


std::vector<String> ResidueImpl::GetAltAtomGroupNames(const AtomImplPtr& atom) const
{
  std::vector<String> names;
  AtomEntryGroups::const_iterator i=alt_groups_.begin();
  for (; i!=alt_groups_.end(); ++i) {
    const AtomGroup& g=i->second;
    for (AtomGroupEntryList::const_iterator j=g.atoms.begin(), 
         e=g.atoms.end(); j!=e; ++j) {
      if (atom==j->atom.lock()) {
        names.push_back(i->first);
      }
    }
  }
  return names;
}


bool ResidueImpl::HasAltAtoms() const {
  return !alt_groups_.empty();
}

bool ResidueImpl::SwitchAtomPos(const String& group) {
  AtomEntryGroups::iterator i=alt_groups_.find(group);
  if (i==alt_groups_.end())
    return false;
  AtomEntryGroups::iterator old=alt_groups_.find(curr_group_);
  if (old!=alt_groups_.end()) {
    AtomGroup& gr=old->second;
    AtomGroupEntryList::iterator k=gr.atoms.begin();
    for (; k!=gr.atoms.end(); ++k) {
      AtomGroupEntry& entry=*k;
      assert(!entry.atom.expired());
      entry.pos=entry.atom.lock()->GetOriginalPos();
    }
  }
  AtomGroup& agr=i->second;
  AtomGroupEntryList::iterator j=agr.atoms.begin();
  for (; j!=agr.atoms.end(); ++j) {

    AtomGroupEntry& entry=*j;
    assert(!entry.atom.expired());
    entry.atom.lock()->SetOriginalPos(entry.pos);
    EntityHandle ent = entry.atom.lock()->GetEntity();
    geom::Mat4 transf_matrix = ent.GetTransformationMatrix();
    geom::Vec3 transf_pos = geom::Vec3(transf_matrix*geom::Vec4(entry.pos));
    entry.atom.lock()->SetTransformedPos(transf_pos);
  }
  curr_group_=group;
  return true;
}

int ResidueImpl::GetIntProperty(Prop::ID prop_id) const
{
  switch (prop_id) {
    case Prop::RNUM:
      return num_.GetNum();
    default:
      throw PropertyError(prop_id);
  }
}

Real ResidueImpl::GetFloatProperty(Prop::ID prop_id) const
{
  switch (prop_id) {
    case Prop::RBFAC:
      return this->GetAverageBFactor();
    default:
      throw PropertyError(prop_id);
  }
}

String ResidueImpl::GetStringProperty(Prop::ID prop_id) const
{
  switch (prop_id) {
    case Prop::RNAME:
      return this->GetName();
    default:
      throw PropertyError(prop_id);
  }
}

}}} // ns
