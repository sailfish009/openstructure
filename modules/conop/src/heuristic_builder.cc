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
#include <boost/algorithm/string/trim.hpp>

#include <ost/log.hh>
#include <ost/geom/geom.hh>
#include "conop.hh"
#include "heuristic_builder.hh"
#include "heuristic_connect_table.hh"
#include <ost/mol/residue_handle.hh>


namespace ost { namespace conop {

namespace detail {

// internal helper class
ConnResEntry::ConnResEntry(const String& rname, char single,
                           const mol::ChemClass& chem_class):
  rkey_(rname),
  abbrev_(single),
  chem_class_(chem_class),
  pset_(),
  prev_(""),
  next_(""),
  torsion_entry_list_()
{}
  
int ConnResEntry::Check(const String& name1, const String& name2) const
{
  NamePair np1(name1,name2);
  NamePair np2(name2,name1);
  // does this pair exist?
  if(pset_.find(np1)!=pset_.end()) {
    return 1;
  } else if(pset_.find(np2)!=pset_.end()) {
    return 2;
  }

  return 0;
}

bool ConnResEntry::HasAtom(const String& name)
{
  for(PairSet::const_iterator it=pset_.begin();it!=pset_.end();++it) {
    if(it->first==name || it->second==name) return true;
  }
  return false;
}

// add a connection entry
void ConnResEntry::AddConn(const String& n1, const String& n2)
{
  pset_.insert(std::make_pair(n1,n2));
}

void ConnResEntry::AddTors(const String& a1, const String& a2, const String& a3,
                           const String& a4, const String& name)
{
  TorsionEntry te={{a1,a2,a3,a4},name};
  torsion_entry_list_.push_back(te);
}

// atom that connects to previous residue in chain
void ConnResEntry::SetPrev(const String& n)
{
  prev_=n;
}

const String& ConnResEntry::GetPrev() const
{
  return prev_;
}

// atom that connects to next residue in chain
void ConnResEntry::SetNext(const String& n)
{
  next_=n;
}

const String& ConnResEntry::GetNext() const
{
  return next_;
}

ConnResEntry::TorsionEntryList ConnResEntry::GetTorsionList() const
{
  return torsion_entry_list_;
}

} // ns

// actual builder

HeuristicBuilder::HeuristicBuilder():
  emap_(),
  default_peptide_()
{
  int def_entry_count = sizeof(heuristic_connect::def_entry_table)/sizeof(heuristic_connect::CONN_DEF_ENTRY);

  LOG_DEBUG("importing internal connectivity tables");
  for(int ec=0;ec<def_entry_count;++ec) {
    heuristic_connect::CONN_DEF_ENTRY& def_entry = heuristic_connect::def_entry_table[ec];
    detail::ConnResEntry entry(def_entry.abbrev, def_entry.single,
                               def_entry.chem_class);
    LOG_TRACE("creating table entry for " << def_entry.abbrev);
    LOG_TRACE("working on bond entries");
    for (int xx=0;xx<def_entry.name_count;++xx) {
      String name=def_entry.name_list[xx];
      if (name!="OXT")
        entry.AddAtom(name);
    }
    // first the connectivity entries
    for(int cc=0;cc<def_entry.conn_count;++cc) {
      int conn_id[] = {def_entry.conn_list[cc][0],def_entry.conn_list[cc][1]};
      String conn_nam[] = {"",""};
      bool special=false;
      for(int cid=0;cid<2;++cid) {
        if(conn_id[cid]>0) {
          conn_nam[cid]=def_entry.name_list[conn_id[cid]-1];
        } else if (conn_id[cid]==-2) {
          // PREV key
          conn_nam[cid]="-";
          special=true;
        } else if (conn_id[cid]==-3) {
          // NEXT key
          conn_nam[cid]="+";
          special=true;
        } else {
          // ignore
          continue;
        }
      }
      if(special) {
        if(conn_nam[0]==String("-")) { entry.SetPrev(conn_nam[1]);}
        else if(conn_nam[1]==String("-")) { entry.SetPrev(conn_nam[0]);}
        else if(conn_nam[0]==String("+")) { entry.SetNext(conn_nam[1]);}
        else if(conn_nam[1]==String("+")) { entry.SetNext(conn_nam[0]);}
        LOG_TRACE(" " << conn_nam[0] << " " << conn_nam[1]);
            } else {
        LOG_TRACE(" " << conn_nam[0] << " " << conn_nam[1]);
        entry.AddConn(conn_nam[0],conn_nam[1]);
      }
    }
    // then the torsion entries
    LOG_DEBUG("working on torsion entries");
    for(int cc=0;cc<def_entry.tor_count;++cc) {
      int tor_id[] = {def_entry.tor_list[cc].n1,
                      def_entry.tor_list[cc].n2,
                      def_entry.tor_list[cc].n3,
                      def_entry.tor_list[cc].n4};
      String tor_nam[] = {"","","",""};
      String tor_nam2(def_entry.tor_list[cc].name);
      for(int cid=0;cid<4;++cid) {
        if(tor_id[cid]>0) {
          tor_nam[cid]=def_entry.name_list[tor_id[cid]-1];
        } else if (tor_id[cid]==-2) {
          tor_nam[cid]="-";
        } else if (tor_id[cid]==-3) {
          tor_nam[cid]="+";
        } else if (tor_id[cid]==-4) {
          tor_nam[cid]=":";
        }
      }

      entry.AddTors(tor_nam[0],tor_nam[1],tor_nam[2],tor_nam[3],tor_nam2);
    }
    if(ec==0) { 
      default_peptide_=entry;
    } else {
      emap_[def_entry.abbrev]=entry;
    }
  }
  LOG_DEBUG("done importing internal tables");
}

HeuristicBuilder::~HeuristicBuilder()
{}


void HeuristicBuilder::ConnectivityFromAtomNames(const mol::ResidueHandle& res,
                                                 detail::ConnResEntry& centry,
                                                 mol::AtomHandleList& unknown_atoms) {
 unknown_atoms.clear();
 mol::AtomHandleList atomlist=res.GetAtomList();
 mol::XCSEditor editor=res.GetEntity().EditXCS(mol::BUFFERED_EDIT);
 for (mol::AtomHandleList::iterator it1=atomlist.begin();it1!=atomlist.end();++it1) {
   if (centry.HasAtom(it1->GetName())) {
     mol::AtomHandleList::iterator it2=it1;
     ++it2;
     for (;it2!=atomlist.end();++it2) {
       LOG_TRACE("checking for atom pair (" << it1->GetName() << ","
                << it2->GetName() << ") in connectivity table of "
                << res.GetKey() << "... ");
       int conn=centry.Check(it1->GetName(),it2->GetName());
       if (conn==1) {
	 if (this->GetBondFeasibilityCheck()==false) {
           LOG_TRACE( "found");
           editor.Connect(*it1,*it2);
	 } else {  
	   if (this->IsBondFeasible(*it1, *it2)) {
	     LOG_TRACE( "found");
             editor.Connect(*it1,*it2);
	   } else {
             LOG_TRACE( "not found");
	   }  
	 }	 
       } else if (conn==2) {
	 if (this->GetBondFeasibilityCheck()==false) {
             LOG_TRACE( "found (reversed)");
             editor.Connect(*it2,*it1);
	 } else {
           if(this->IsBondFeasible(*it2, *it1)) {
             LOG_TRACE( "found (reversed)");             
             editor.Connect(*it2,*it1);
           }
         }  
       } else {
          LOG_TRACE( "not found");
       }
     }
   } else {
     unknown_atoms.push_back(*it1);
     LOG_TRACE( "atom not found, pushing it to unknown atoms");
   }	 
 }
}

void HeuristicBuilder::ConnectAtomsOfResidue(mol::ResidueHandle res)
{
  LOG_TRACE("HeuristicBuilder: ConnectAtomsOfResidue on " << res.GetKey() << " " << res.GetNumber());

  mol::AtomHandleList atomlist = res.GetAtomList();
  mol::AtomHandleList unk_atomlist;
#if !defined(NDEBUG)
  std::stringstream ss;
  ss << "using atom list:";
  for(mol::AtomHandleList::iterator it=atomlist.begin();it!=atomlist.end();++it) {
    ss << " " << it->GetName() << " @" << it->GetPos();
  }
  LOG_TRACE(ss.str());
#endif
  std::pair<detail::ConnResEntry,bool> ret = LookupResEntry(res.GetKey());

  if(ret.second) {
    // residue entry found
    this->ConnectivityFromAtomNames(res, ret.first, unk_atomlist);
    // only run the distance based connectivity after the rest is done
    for(mol::AtomHandleList::iterator it1=unk_atomlist.begin();
        it1!=unk_atomlist.end();
        ++it1) {
      LOG_TRACE( "atom " << it1->GetName() << " not found, using distance based connect");
      Builder::DistanceBasedConnect(*it1);
    }
  } else {
    LOG_TRACE("no residue entry found, using distance based connect");
    for(mol::AtomHandleList::iterator it1=atomlist.begin();
        it1!=atomlist.end();
        ++it1) {
      Builder::DistanceBasedConnect(*it1);
    }
  }
}

namespace {

template<bool flag> // false:=prev, true:=next
void ConnectPrevNext(HeuristicBuilder* builder,mol::ResidueHandle res0, 
                     mol::ResidueHandle res1)
{
  static String fname=flag ? "HeuristicBuilder: ConnectNextXCS" : "HeuristicBuilder: ConnectPrevXCS";
  if(!res0) return; // return if invalid
  mol::XCSEditor editor=res0.GetEntity().EditXCS(mol::BUFFERED_EDIT);
  LOG_TRACE(fname << " on " << res0.GetKey() << " " << res0.GetNumber());

  if(!res1) {
    // auto-detect prev or next residue in chain
    // and perform sequence check
    if(flag) {
      LOG_TRACE(fname << " autodecting next residue");
      res1 = res0.GetChain().GetNext(res0);
    } else {
      LOG_TRACE(fname << " autodecting next residue");
      res1 = res0.GetChain().GetPrev(res0);
    }
  } else {
    if(res0.GetEntity() != res1.GetEntity()) return; // different entity handles
  }
  if (flag) {
    if(!Builder::AreResiduesConsecutive(res0,res1)) return;
  } else {
    if(!Builder::AreResiduesConsecutive(res1, res0)) return;
  }

  if(!res1) return; // ignore if prev/next residue is invalid
  LOG_TRACE(fname << " found second residue " << res1.GetKey() 
            << " " << res1.GetNumber());

  std::pair<detail::ConnResEntry,bool> res0_ret = builder->LookupResEntry(res0.GetKey());
  std::pair<detail::ConnResEntry,bool> res1_ret = builder->LookupResEntry(res1.GetKey());

  if(!res0_ret.second) {
    if(res0.FindAtom("N") && res0.FindAtom("CA") && res0.FindAtom("C")) {
      LOG_TRACE("using default peptide for " << res0.GetKey());
      res0_ret.first=builder->DefaultPeptide();
      res0_ret.second=true;
    }
  }

  if(!res1_ret.second) {
    if(res1.FindAtom("N") && res1.FindAtom("CA") && res1.FindAtom("C")) {
      LOG_TRACE("using default peptide for " << res1.GetKey());
      res1_ret.first=builder->DefaultPeptide();
      res1_ret.second=true;
    }
  }

  if(res0_ret.second && res1_ret.second) {
    detail::ConnResEntry& res0_centry=res0_ret.first;
    detail::ConnResEntry& res1_centry=res1_ret.first;
    String res0_atom_name = res0_centry.GetPrev();
    String res1_atom_name = res1_centry.GetNext();

    if(res0_atom_name.empty() || res1_atom_name.empty()) return;
    LOG_TRACE(fname << ": looking up atom names " << res0_atom_name << " " << res1_atom_name);

    // lookup both atoms in their respective residues
    mol::AtomHandle res0_atom = res0.FindAtom(res0_atom_name);
    mol::AtomHandle res1_atom = res1.FindAtom(res1_atom_name);
    if(res0_atom && res1_atom) {
      LOG_TRACE(fname << ": found atoms, connecting");
      if(flag) {
        if (builder->DoesPeptideBondExist(res0_atom, res1_atom)) {
          editor.Connect(res0_atom,res1_atom);
          res0.SetIsProtein(true);
          res1.SetIsProtein(true);
        }
      } else {
        if (builder->DoesPeptideBondExist(res1_atom, res0_atom)) {
          editor.Connect(res1_atom, res0_atom);
          res0.SetIsProtein(true);
          res1.SetIsProtein(true);
        }

      }
    }
  } else {
    // could implement some sort of distance based heuristics here
  }
}

}

void HeuristicBuilder::AssignTorsionsToResidue(mol::ResidueHandle res)
{

  mol::XCSEditor editor=res.GetEntity().EditXCS(mol::BUFFERED_EDIT);
  mol::ChainHandle chain=res.GetChain();
  std::pair<detail::ConnResEntry,bool> centry2 = LookupResEntry(res.GetKey());
  if (centry2.second) {
    // residue entry found
    detail::ConnResEntry& centry=centry2.first;

    detail::ConnResEntry::TorsionEntryList tel=centry.GetTorsionList();
    // for each entry in torsion list
    for (unsigned int ti=0;ti<tel.size();++ti) {
      // assign the four atom handles
      mol::AtomHandle ah[4];
      for (unsigned int ahi=0;ahi<4;++ahi) {
        mol::ResidueHandle search_in=res;
        String cur_name=tel[ti].a[ahi];
        bool flag=false;
        if (cur_name=="+") {
          mol::ResidueHandle next=res.GetNext();
          // NEXT
          if (Builder::AreResiduesConsecutive(res, next)) {
            centry2=LookupResEntry(next.GetKey());
            if (centry2.second) {
              cur_name=centry2.first.GetPrev();
              search_in=next;
              flag=true;
            }
          }
        } else if (cur_name=="-") {
          mol::ResidueHandle prev=res.GetPrev();
          if (Builder::AreResiduesConsecutive(prev, res)) {
            centry2=LookupResEntry(prev.GetKey());
            if (centry2.second) {
              cur_name=centry2.first.GetNext();
              search_in=prev;
              flag=true;
            }
          }
        } else {
          flag=true;
        }
        if (flag) {
          // lookup atom based on cur_residue and cur_name
          mol::AtomHandle a=search_in.FindAtom(cur_name);
          if (!a.IsValid())
            continue;
          ah[ahi]=a;
        }
      } // ahi
      if (ah[0] && ah[1] && ah[2] && ah[3]) {
        mol::TorsionHandle th = editor.AddTorsion(tel[ti].name, ah[0], ah[1],
                                             ah[2], ah[3]);
        if(th) {
          LOG_TRACE("added torsion entry for " << tel[ti].a[0] << " "
                    << tel[ti].a[1] << " " << tel[ti].a[2] << " "
                    << tel[ti].a[3]);
        } else {
          LOG_TRACE("no torsion entry for " << tel[ti].a[0] << " "
                    << tel[ti].a[1] << " " << tel[ti].a[2]
                    << " " << tel[ti].a[3]);
        }
      }
    }
  }
}

void HeuristicBuilder::AssignTorsions(mol::ChainHandle chain)
{
  if (chain.GetResidueCount()==0)
    return;
  std::vector<mol::ResidueHandle> rlist = chain.GetResidueList();
  for(unsigned int ri=0;ri<rlist.size();++ri) {
    mol::ResidueHandle res=rlist[ri];
    this->AssignTorsionsToResidue(res);
    if (!res.IsPeptideLinking()) {
      return;
    }
    Builder::AssignBackBoneTorsionsToResidue(res);
  }
}

void HeuristicBuilder::ConnectResidueToPrev(mol::ResidueHandle rh,
                                            mol::ResidueHandle prev) {
  ConnectPrevNext<false>(this, rh, prev);
}

void HeuristicBuilder::ConnectResidueToNext(mol::ResidueHandle rh,
                                            mol::ResidueHandle next) {
  ConnectPrevNext<true>(this, rh, next);
}

void HeuristicBuilder::FillResidueProps(mol::ResidueHandle residue) {
  std::pair<detail::ConnResEntry,bool> ret = LookupResEntry(residue.GetKey());
  if (ret.second) {
    residue.SetChemClass(mol::ChemClass(ret.first.GetChemClass()));
    residue.SetOneLetterCode(ret.first.GetOneLetterCode());    
  } else {
    if (residue.FindAtom("N") && residue.FindAtom("CA") && 
        residue.FindAtom("C") && residue.FindAtom("O")) {
      residue.SetChemClass(mol::ChemClass(mol::ChemClass::L_PEPTIDE_LINKING));
    } else {
      residue.SetChemClass(mol::ChemClass(mol::ChemClass::UNKNOWN));
    }

    residue.SetOneLetterCode('?');
  }

}

std::pair<detail::ConnResEntry,bool> HeuristicBuilder::LookupResEntry(const mol::ResidueKey& key)
{
  static detail::ConnResEntry dummy;



  detail::ConnResEntryMap::iterator pos = emap_.find(key);
  if(pos!=emap_.end()) {
    LOG_TRACE("reskey '" << key << "' found in connectivity map");
    return std::make_pair(pos->second,true);
  }
  LOG_TRACE("reskey '" << key << "' not found connectivity map");
  return std::make_pair(dummy,false);
}

bool HeuristicBuilder::IsResidueComplete(const mol::ResidueHandle& rh)
{
  std::pair<detail::ConnResEntry,bool> ret=LookupResEntry(rh.GetKey());  
  if (!ret.second) {
    // if the residue is unknown we return false...
    return false;
  }
  for (std::vector<String>::const_iterator i=ret.first.GetReqAtoms().begin(),
       e=ret.first.GetReqAtoms().end(); i!=e; ++i) {
    if (!rh.FindAtom(*i)) {
      return false;
    }
  }
  return true;
}

void HeuristicBuilder::FillAtomProps(mol::AtomHandle atom)
{
  if (atom.GetElement()=="") {
    atom.SetElement(Builder::GuessAtomElement(atom.GetName(), atom.IsHetAtom()));
  }
}

}} // ns
