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
#include <map>
#include <set>

#include <ost/entity.hh>
#include <ost/log.hh>

#include "heuristic_connect.hh"
#include "heuristic_connect_table.hh"

namespace ost { namespace conop {

namespace {
  
class ConnResEntry {
public:
  typedef std::pair<String,String> NamePair;
  typedef std::multiset<NamePair> PairSet;
  struct TorsionEntry {
    String a[4];
    String name;
  };
  typedef std::vector<TorsionEntry> TorsionEntryList;

public:
  ConnResEntry(const String& rname=""): 
    rkey_(rname),
    pset_(),
    prev_(""),
    next_(""),
    torsion_entry_list_()
  {}
  
  /*
    checks for connection between two atoms. if returned flag is zero,
    then the two bools that were passed in indicate wether the names
    actually exist in this entry or not. The return value is one for
    the correct order and 2 of the order is reversed
  */
  int Check(const String& name1, const String& name2, bool& ex1, bool& ex2) const 
  {
    NamePair np1(name1,name2);
    NamePair np2(name2,name1);
    // does this pair exist?
    if(pset_.find(np1)!=pset_.end()) {
      ex1=true;
      ex2=true;
      return 1;
    } else if(pset_.find(np2)!=pset_.end()) {
      ex1=true;
      ex2=true;
      return 2;
    }

    // check for occurence of either names in the name pairs
    ex1=false;
    ex2=false;
    for(PairSet::const_iterator it=pset_.begin();it!=pset_.end();++it) {
      if(it->first==name1 || it->second==name1) ex1=true;
      if(it->first==name2 || it->second==name2) ex2=true;
      if(ex1 && ex2) break;
    }

    return false;
  }

  // add a connection entry
  void AddConn(const String& n1, const String& n2) 
  {
    pset_.insert(std::make_pair(n1,n2));
  }

  void AddTors(const String& a1, const String& a2, const String& a3, const String& a4, const String& name)
  {
    TorsionEntry te={{a1,a2,a3,a4},name};
    torsion_entry_list_.push_back(te);
  }
  
  // atom that connects to previous residue in chain
  void SetPrev(const String& n)
  {
    prev_=n;
  }
  
  const String& GetPrev() const
  {
    return prev_;
  }

  // atom that connects to next residue in chain
  void SetNext(const String& n)
  {
    next_=n;
  }

  const String& GetNext() const
  {
    return next_;
  }
  
  TorsionEntryList GetTorsionList() const {return torsion_entry_list_;}

private:
  ResidueKey rkey_;
  PairSet pset_;
  String prev_;
  String next_;
  TorsionEntryList torsion_entry_list_;
};

typedef std::map<ResidueKey,ConnResEntry> ConnResEntryMap;
void fill_map(ConnResEntryMap& em)
{
  int def_entry_count = sizeof(heuristic_connect::def_entry_table)/sizeof(heuristic_connect::CONN_DEF_ENTRY);

  for(int ec=0;ec<def_entry_count;++ec) {
    heuristic_connect::CONN_DEF_ENTRY& def_entry = heuristic_connect::def_entry_table[ec];
    ConnResEntry entry(def_entry.abbrev);
    LOG_DUMP("creating default entry for " << def_entry.abbrev << std::endl);
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
        LOG_DUMP(" " << conn_nam[0] << " " << conn_nam[1] << std::endl);
      } else {
        LOG_DUMP(" " << conn_nam[0] << " " << conn_nam[1] << std::endl);
        entry.AddConn(conn_nam[0],conn_nam[1]);
      }
    }
    // then the torsion entries
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
        }
      }
      entry.AddTors(tor_nam[0],tor_nam[1],tor_nam[2],tor_nam[3],tor_nam2);
    }
    
    em[def_entry.abbrev]=entry;

  }
}

std::pair<ConnResEntry,bool> LookupResEntry(const ResidueKey& key)
{
  static ConnResEntry dummy;
  static ConnResEntryMap emap;
  static bool initialized=false;

  if(!initialized) {
    LOG_DEBUG("initializing Connectivity Map" << std::endl);
    fill_map(emap);
    initialized=true;
  }

  LOG_DUMP("Looking for reskey '" << key << "' in connectivity map... ");

  ConnResEntryMap::iterator pos = emap.find(key);
  if(pos!=emap.end()) {
    LOG_DUMP("found" << std::endl);
    return std::make_pair(pos->second,true);
  } else {
    LOG_DUMP("not found" << std::endl);
  }
  return std::make_pair(dummy,false);
}

class Connector: public EntityVisitor
{
public:
  Connector() {Reset();}

  // visitor interface
  virtual bool VisitChain(const ChainHandle& chain) {
    LOG_DUMP("connect: setting current chain to " << chain.GetName() << std::endl);
    curr_chain_=chain;
    curr_residue_=ResidueHandle();
    prev_residue_=ResidueHandle();
    prev_atom_=AtomHandle();
    next_atom_=AtomHandle();
    return true;
  }

  virtual bool VisitResidue(const ResidueHandle& res) {
    LOG_DUMP("connect: setting current residue to " << res.GetKey() << " " << res.GetNumber() << std::endl);
    // residue has changed
    curr_residue_=res;
    AtomHandleList atomlist = res.GetAtomList();
    LOG_DUMP( "using atom list:" << std::endl);
    for(AtomHandleList::iterator it=atomlist.begin();it!=atomlist.end();++it) {
      LOG_DUMP( " " << it->GetName() << " @" << it->GetPos() << std::endl);
    }
    
    std::pair<ConnResEntry,bool> ret = LookupResEntry(res.GetKey());

    if(ret.second) {
      // residue entry found
      ConnResEntry& centry=ret.first;
      for(AtomHandleList::iterator it1=atomlist.begin();it1!=atomlist.end();++it1) {
        AtomHandleList::iterator it2=it1;
        ++it2;
        for(;it2!=atomlist.end();++it2) {
          bool ex1,ex2;
          LOG_DUMP( "checking for atom pair (" << it1->GetName() << "," << it2->GetName() << ") in connectivity table of " << res.GetKey() << "... ");
          int conn=centry.Check(it1->GetName(),it2->GetName(),ex1,ex2);
          if (conn==1) {
            LOG_DUMP( "found" << std::endl);
            res.GetEntity().Connect(*it1,*it2);
          } else if(conn==2) {
            LOG_DUMP( "found (reversed)" << std::endl);
            res.GetEntity().Connect(*it2,*it1);
          } else {
            LOG_DUMP( "not found" << std::endl);
            // check ex1 and/or ex2
          }
        }

        // check this atom for interresidue connectivity
        /*
          make interresidue bond if
          - previous residue had a "next" entry
          - previous residue number is consecutive to this one
        */
        if(centry.GetPrev()==it1->GetName()) { // 'PREV' entry
          LOG_DUMP( "found 'prev' atom entry: " << it1->GetName() << std::endl);
          if(prev_residue_) { // previous residue exists
            if(!next_atom_name_.empty()) { // previous residue 'NEXT' atom set
              if(InSequence(prev_residue_,res)) {
                AtomHandleList prev_atomlist = prev_residue_.GetAtomList();
                for(AtomHandleList::iterator pit=prev_atomlist.begin();
                    pit!=prev_atomlist.end();++pit) {
                  if(pit->GetName()==next_atom_name_) {
                    LOG_DUMP( "connecting previous atom " << pit->GetName() << " to "  << it1->GetName() << std::endl);
                    res.GetEntity().Connect(*pit,*it1);
                  }
                }
              }
            }
          }
        }

        if(centry.GetNext()==it1->GetName()) {
          LOG_DUMP( "found 'next' atom entry: " << it1->GetName() << std::endl);
          // remember this atom for the next residue
          next_atom_name_=it1->GetName();
        }

      } // connectivity
    } else {
      /* 
         here a distance based lookup over all atoms of this residue compared
         to all other atoms should be implemented
      */
      
    }

    prev_residue_=res; // remember for next round
    
    return true;
  }

  virtual bool VisitAtom(const AtomHandle& a) {
    return true;
  }

  void Reset() {
    curr_chain_=ChainHandle();
    curr_residue_=ResidueHandle();
    prev_atom_=AtomHandle();
    next_atom_=AtomHandle();
  }

private:
  ChainHandle curr_chain_;
  ResidueHandle curr_residue_;
  ResidueHandle prev_residue_;
  String next_atom_name_;
  AtomHandle prev_atom_;
  AtomHandle next_atom_;
};

class TorsionMaker: public EntityVisitor
{
public:
  TorsionMaker() {}

  // visitor interface
  virtual bool VisitChain(const ChainHandle& chain) {
    std::vector<ResidueHandle> rlist = chain.GetResidueList();

    for(unsigned int ri=0;ri<rlist.size();++ri) {
      ResidueHandle res=rlist[ri];
      std::pair<ConnResEntry,bool> centry2 = LookupResEntry(res.GetKey());

      if(centry2.second) {
        // residue entry found
        ConnResEntry& centry=centry2.first;
        AtomHandleList atom_list = res.GetAtomList();
        ConnResEntry::TorsionEntryList tel=centry.GetTorsionList();
        // for each entry in torsion list
        for(unsigned int ti=0;ti<tel.size();++ti) {
          // assign the four atom handles
          AtomHandle ah[4];
          for(unsigned int ahi=0;ahi<4;++ahi) {
            ResidueHandle cur_res=res;
            String cur_name=tel[ti].a[ahi];
            bool flag=false;
            if(cur_name=="+") {
              // NEXT
              if(ri+1<rlist.size()) {
                centry2=LookupResEntry(rlist[ri+1].GetKey());
                if(centry2.second) {
                  cur_name=centry2.first.GetPrev();
                  cur_res=rlist[ri+1];
                  flag=true;
                }
              }
            } else if (tel[ti].a[ahi]=="-") {
              // PREV
              if(ri>0) {
                centry2=LookupResEntry(rlist[ri-1].GetKey());
                if(centry2.second) {
                  cur_name=centry2.first.GetNext();
                  cur_res=rlist[ri-1];
                  flag=true;
                }
              }
            } else {
              flag=true;
            }
            if(flag) {
              // lookup atom based on cur_residue and cur_name
              ah[ahi]=cur_res.FindAtom(cur_name);
            }
          } // ahi
          if(ah[0] && ah[1] && ah[2] && ah[3]) {
            TorsionHandle th = chain.GetEntity().AddTorsion(tel[ti].name,ah[0],ah[1],ah[2],ah[3]);
            if(th) {
              LOG_DUMP("added torsion entry for " << tel[ti].a[0] << " " << tel[ti].a[1] << " " << tel[ti].a[2] << " " << tel[ti].a[3] << std::endl);
            } else {
              LOG_DUMP("no torsion entry for " << tel[ti].a[0] << " " << tel[ti].a[1] << " " << tel[ti].a[2] << " " << tel[ti].a[3] << std::endl);
            }
          }
        } // ti
      }
    }

    return false;
  }
private:
};
  
} // ns

/*

*/
void HeuristicConnect(EntityHandle& eh)
{
  static Connector conn;
  static TorsionMaker tor;
  conn.Reset();
  eh.Apply(conn);
  eh.Apply(tor);
}

}} // ns
