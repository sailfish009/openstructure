//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2015 by the OpenStructure authors
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
 Author: Marco Biasini
 */
#include <ost/mol/query_state.hh>
#include <ost/mol/mol.hh>
#include <ost/mol/impl/entity_impl.hh>
#include <ost/mol/impl/chain_impl.hh>
#include <ost/mol/impl/residue_impl.hh>
#include <ost/mol/impl/atom_impl.hh>
#include <ost/mol/impl/query_impl.hh>
#include <ost/mol/impl/query_ast.hh>

namespace ost { namespace mol {

using namespace impl;

struct LazilyBoundRef {
  LazilyBoundRef& operator=(const LazilyBoundRef& rhs);
  //EntityView for now, will be generalized to a point cloud later on.
  EntityView  view;
};
  
struct LazilyBoundData {
  std::vector<LazilyBoundRef> refs;
};


    
bool cmp_string(CompOP op,const String& lhs, const StringOrRegexParam& rhs) {
  switch (op) {
    case COP_EQ:
      return rhs.Match(lhs);
    case COP_NEQ:
      return !rhs.Match(lhs);
    default:
      assert(0 && "should be checked during ast generation");
      return false;
  }
}

bool QueryState::do_within(const geom::Vec3& pos, const WithinParam& p, 
                           CompOP op) 
{
  if (!p.HasValidRef()) {
    geom::Vec3 d=pos-p.GetCenter();
    if (op==COP_LE)
      return geom::Dot(d, d) <= p.GetRadiusSquare();
    else
      return geom::Dot(d, d) > p.GetRadiusSquare();
  } else {
    const LazilyBoundRef& r=this->GetBoundObject(p.GetRef());
    for (AtomViewIter i=r.view.AtomsBegin(), e=r.view.AtomsEnd(); i!=e; ++i) {
      geom::Vec3 d=pos-(*i).GetPos();
      if (op==COP_LE) {
        if (geom::Dot(d, d) <= p.GetRadiusSquare()) {
          return true;
        }
      } else if (geom::Dot(d, d) < p.GetRadiusSquare()) {
        return false;
      }
    }
    return op!=COP_LE;
  }
}


template <typename T>
bool cmp_num(CompOP op, T lhs, T rhs) 
{
  switch (op) {
    case COP_EQ:
      return lhs == rhs;
    case COP_NEQ:
      return lhs != rhs;
    case COP_GE:
      return lhs >= rhs;
    case COP_LE:
      return lhs <= rhs;
    case COP_LT:
      return lhs < rhs;
    case COP_GT:
      return lhs > rhs;
  }
  return true;
}

QueryState::~QueryState()
{
}

QueryState::QueryState(const QueryImpl& query, const EntityHandle& ref)
  : q_(query) {
  s_.resize(query.sel_values_.size(),boost::logic::indeterminate);
  if (! query.bracketed_expr_.empty()) {
    r_.reset(new LazilyBoundData);
    r_->refs.resize(query.bracketed_expr_.size());
    for (size_t i=0;i<query.bracketed_expr_.size(); ++i) {
      r_->refs[i].view=ref.Select(Query(query.bracketed_expr_[i]));
    }    
  }

}

LazilyBoundRef& LazilyBoundRef::operator=(const LazilyBoundRef& rhs) {
  view=rhs.view;
  return *this;
}

QueryState::QueryState(const QueryImpl& query, const EntityView& ref)
  : q_(query) {
  s_.resize(query.sel_values_.size(),boost::logic::indeterminate);
  if (! query.bracketed_expr_.empty()) {
    r_.reset(new LazilyBoundData);
    r_->refs.resize(query.bracketed_expr_.size());
    for (size_t i=0;i<query.bracketed_expr_.size(); ++i) {
      r_->refs[i].view=ref.Select(Query(query.bracketed_expr_[i]));
    }    
  }
}

const LazilyBoundRef& QueryState::GetBoundObject(int i) const {
  return r_->refs[i];
}




boost::logic::tribool QueryState::EvalChain(const ChainImplPtr& c) {
  if (q_.empty_optimize_)
    return true;
  const std::set<size_t>& indices = q_.indices_[(int)Prop::CHAIN];
  std::set<size_t>::const_iterator i = indices.begin();
  for (; i != indices.end(); ++i) {
    const SelStmt& ss = q_.sel_values_[*i];
    String value;
    float float_value;
    switch (ss.sel_id) {
      case Prop::CNAME:
        value = c->GetName();
        s_[*i] = cmp_string(ss.comp_op,
                            value,boost::get<StringOrRegexParam>(ss.param));
        continue;
      default:
        if (ss.sel_id>=Prop::CUSTOM) {
          int index=ss.sel_id-Prop::CUSTOM;
          GenProp gen_prop=q_.gen_prop_list_[index];
          if (gen_prop.has_default) {
            float_value=gen_prop.mapper.Get(c, gen_prop.default_val);
          } else {
            float_value=gen_prop.mapper.Get(c);
          }
          s_[*i]=cmp_num<Real>(ss.comp_op, float_value,
                               boost::get<float>(ss.param));
        } else {
          assert(0 && "not implemented" );
        }
    }
  }  
  return this->EvalStack(Prop::CHAIN);
}

boost::logic::tribool QueryState::EvalResidue(const ResidueImplPtr& r) {
  if (q_.empty_optimize_)
    return true;
  const std::set<size_t>& indices = q_.indices_[(int)Prop::RESIDUE];
  std::set<size_t>::const_iterator i = indices.begin();
  String str_value;
  int int_value;
  String p;  
  for (; i != indices.end(); ++i) {
    const SelStmt& ss = q_.sel_values_[*i];
    Real float_value;
    switch (ss.sel_id) {
      case Prop::RNAME:
        str_value = r->GetKey();
        s_[*i] = cmp_string(ss.comp_op,str_value,
                            boost::get<StringOrRegexParam>(ss.param));
        continue;
      case Prop::RNUM:
        int_value=r->GetNumber().GetNum();
        s_[*i]=cmp_num<int>(ss.comp_op,int_value,boost::get<int>(ss.param));        
        break;
      case Prop::PEPTIDE:
        int_value=r->GetChemClass().IsPeptideLinking();
        s_[*i] = cmp_num<int>(ss.comp_op,int_value,boost::get<int>(ss.param));        
        break;        
      case Prop::RBFAC:
        // This is ugly! Outcome is the same for a prefiltered view as it is for
        // as for the full entity, even though the residue view might not have
        // all the atoms included.
        float_value=r->GetAverageBFactor();
        s_[*i] = cmp_num<Real>(ss.comp_op, float_value,
                                 boost::get<float>(ss.param));        
        break;
      case Prop::PROTEIN:
        int_value=r->IsProtein();
        s_[*i]=cmp_num<int>(ss.comp_op,int_value,boost::get<int>(ss.param));
        break;
      case Prop::WATER:
        int_value=r->GetChemClass().IsWater();
        s_[*i]=cmp_num<int>(ss.comp_op,int_value,boost::get<int>(ss.param));
        break;
      case Prop::LIGAND:
        int_value=r->IsLigand();
        s_[*i]=cmp_num<int>(ss.comp_op,int_value,boost::get<int>(ss.param));
        break;        
      case Prop::RTYPE:
        p=boost::get<StringOrRegexParam>(ss.param).str();
        if (p.length()>1) {
          bool b=false;
          if (p=="helix") {
            b=r->GetSecStructure().IsHelical();
          } else if (p=="ext" || p=="strand") {
            b=r->GetSecStructure().IsExtended();
          } else if (p=="coil") {
            b=r->GetSecStructure().IsCoil();
          } else if (p=="*") {
            b=true;
          }
          s_[*i]=ss.comp_op==COP_EQ ? b : !b;          
        } else {
          str_value= String(1, (char)r->GetSecStructure());
          s_[*i]=cmp_string(ss.comp_op,str_value,
                            boost::get<StringOrRegexParam>(ss.param));          
        }
        break;
      case Prop::RINDEX:
        int_value=r->GetIndex();
        s_[*i]=cmp_num<int>(ss.comp_op,int_value,boost::get<int>(ss.param));
        break;
      default:
        if (ss.sel_id>=Prop::CUSTOM) {
          int index=ss.sel_id-Prop::CUSTOM;
          GenProp gen_prop=q_.gen_prop_list_[index];
          if (gen_prop.has_default) {
            float_value=gen_prop.mapper.Get(r, gen_prop.default_val);
          } else {
            float_value=gen_prop.mapper.Get(r);
          }
          s_[*i]=cmp_num<Real>(ss.comp_op, float_value,
                               boost::get<float>(ss.param));
        } else {
          assert(0 && "not implemented" );
        }
    }
  }
  return this->EvalStack(Prop::RESIDUE);  
}

boost::logic::tribool QueryState::EvalStack(Prop::Level level) {
  const SelStack& stack = q_.sel_stacks_[(int)level];
  SelStack::const_reverse_iterator i = stack.rbegin();
  std::vector<boost::logic::tribool> value_stack;

  while (i!=stack.rend()) {
    const SelItem& si = *i;
    if (si.type==VALUE) {
      value_stack.push_back(s_[si.value]);
      ++i;
      continue;
    } else {
      boost::logic::tribool lhs = value_stack.back();
      value_stack.pop_back();
      boost::logic::tribool rhs = value_stack.back();
      value_stack.pop_back();      
      LogicOP lop = (LogicOP)si.value;
      boost::logic::tribool result;
      switch(lop) {
        case LOP_AND:
          result = lhs && rhs;
          break;
        case LOP_OR:
          result = lhs || rhs;
          break;              
      }      
      value_stack.push_back(result);
      ++i;
    }
  }
  assert(value_stack.size()==1);
  return value_stack.back();
}

namespace {
// silences a warning for VS90
QueryImpl dummy_query_impl;
}
QueryState::QueryState()
  : s_(), q_(dummy_query_impl) {
}

boost::logic::tribool QueryState::EvalAtom(const AtomImplPtr& a) {
  if (q_.empty_optimize_)
    return true;  
  const std::set<size_t>& indices = q_.indices_[(int)Prop::ATOM];
  std::set<size_t>::const_iterator i = indices.begin();
  for (; i != indices.end(); ++i) {
    const SelStmt& ss = q_.sel_values_[*i];
    String str_value;
    Real float_value;
    int int_value;
    switch (ss.sel_id) {
      case Prop::ANAME:
        str_value = a->Name();
        s_[*i] = cmp_string(ss.comp_op,str_value,
                            boost::get<StringOrRegexParam>(ss.param));
        break;
      case Prop::AINDEX:
        int_value=(a->GetIndex());
        s_[*i]=cmp_num<int>(ss.comp_op, int_value,boost::get<int>(ss.param));
        break;
      case Prop::AX:
        float_value=(a->TransformedPos())[0];
        s_[*i]=cmp_num<Real>(ss.comp_op, float_value, 
        boost::get<float>(ss.param));
        break;
      case Prop::AY:
        float_value=(a->TransformedPos())[1];
        s_[*i]=cmp_num<Real>(ss.comp_op, float_value, 
                             boost::get<float>(ss.param));
        break;
      case Prop::AZ:
        float_value=(a->TransformedPos())[2];
        s_[*i]=cmp_num<Real>(ss.comp_op, float_value, 
                             boost::get<float>(ss.param));
        break;                
      case Prop::OCC:
        float_value=a->GetOccupancy();
        s_[*i]=cmp_num<Real>(ss.comp_op, float_value, 
                             boost::get<float>(ss.param));
        break;                        
      case Prop::ELE:
        str_value = a->GetElement();
        s_[*i] = cmp_string(ss.comp_op,str_value,
                            boost::get<StringOrRegexParam>(ss.param));                          
        break;
      case Prop::ABFAC:
        float_value=a->GetBFactor();
        s_[*i]=cmp_num<Real>(ss.comp_op, float_value, 
                              boost::get<float>(ss.param));
        break;
      case Prop::WITHIN:
        s_[*i]= this->do_within(a->TransformedPos(), 
                                boost::get<WithinParam>(ss.param),
                                ss.comp_op);
        break;
      case Prop::ISHETATM:
        int_value = a->IsHetAtom();
        s_[*i] = cmp_num<int>(ss.comp_op,int_value,boost::get<int>(ss.param));
        break;
      case Prop::ACHARGE:
        float_value=a->GetCharge();
        s_[*i]=cmp_num<Real>(ss.comp_op, float_value, 
                              boost::get<float>(ss.param));
        break;
      default:
        if (ss.sel_id>=Prop::CUSTOM) {
          int index=ss.sel_id-Prop::CUSTOM;
          GenProp gen_prop=q_.gen_prop_list_[index];
          if (gen_prop.has_default) {
            float_value=gen_prop.mapper.Get(a, gen_prop.default_val);
          } else {
            float_value=gen_prop.mapper.Get(a);
          }
          s_[*i]=cmp_num<Real>(ss.comp_op, float_value,
                               boost::get<float>(ss.param));
        } else {
          assert(0 && "not implemented" );
        }
    }    
  }
  return this->EvalStack(Prop::ATOM);
}

void QueryState::Reset(Prop::Level level) {
  if (q_.empty_optimize_)
    return;
  const std::set<size_t>& indices = q_.indices_[(int)level];
  std::set<size_t>::const_iterator i = indices.begin();
  for (; i != indices.end(); ++i) {
    s_[*i] = boost::logic::indeterminate;
  }
}

bool QueryState::IsAtomSelected(const AtomHandle& h)
{
  boost::logic::tribool ret = this->EvalChain(h.GetResidue().GetChain().Impl());
  if(ret==false) return false;
  ret = this->EvalResidue(h.GetResidue().Impl());
  if(ret==false) return false;
  return this->EvalAtom(h.Impl());
}

bool QueryState::IsResidueSelected(const ResidueHandle& r)
{
  boost::logic::tribool ret = this->EvalChain(r.GetChain().Impl());
  if(ret==false) return false;
  ret = this->EvalResidue(r.Impl());
  if(ret==false) return false;
  AtomHandleList atoms=r.GetAtomList();
  for (AtomHandleList::iterator i=atoms.begin(), e=atoms.end(); i!=e; ++i) {
    if (this->EvalAtom((*i).Impl())==true)
      return true;
  }
  return false;
}

bool QueryState::IsChainSelected(const ChainHandle& chain)
{
  boost::logic::tribool ret = this->EvalChain(chain.Impl());
  if(ret==false) return false;
  ResidueHandleList res=chain.GetResidueList();
  for (ResidueHandleList::iterator j=res.begin(), k=res.end(); j!=k; ++j) {
    ResidueHandle r=*j;
    ret = this->EvalResidue(r.Impl());
    if(ret==false)
      continue;
    AtomHandleList atoms=r.GetAtomList();
    for (AtomHandleList::iterator i=atoms.begin(), e=atoms.end(); i!=e; ++i) {
      if (this->EvalAtom((*i).Impl())==true)
        return true;
    }    
  }
  return false;
}


}} // ns
