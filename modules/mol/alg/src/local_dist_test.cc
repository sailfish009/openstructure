#include <ost/log.hh>
#include <ost/mol/mol.hh>
#include "local_dist_test.hh"
#include <boost/concept_check.hpp>

namespace ost { namespace mol { namespace alg {

namespace {

String swapped_name(const String& name)
{
  if (name=="OE1") return "OE2";
  if (name=="OE2") return "OE1";

  if (name=="OD1") return "OD2";
  if (name=="OD2") return "OD1";

  if (name=="CG1") return "CG2";
  if (name=="CG2") return "CG1";

  if (name=="CE1") return "CE2";
  if (name=="CE2") return "CE1";  

  if (name=="CD1") return "CD2";
  if (name=="CD2") return "CD1";

  if (name=="NH1") return "NH2";
  if (name=="NH2") return "NH1";

  return name;
}

bool swappable(const String& rname, const String& aname)
{
  if (rname=="GLU") {
    return (aname=="OE1" || aname=="OE2");
  }
  if (rname=="ASP") {
    return (aname=="OD1" || aname=="OD2");
  }
  if (rname=="VAL") {     

    return (aname=="CG1" || aname=="CG2");
  }
  if (rname=="TYR" || rname=="PHE") {
    return (aname=="CD1" || aname=="CD2" || aname=="CE1" || aname=="CE2");
  }
  if (rname=="ARG") {
    return (aname=="NH1" || aname=="NH2");
  }  
  return false;
}

std::pair<bool,Real> within_tolerance(Real mdl_dist, const ReferenceDistance& ref_dist, Real tol)
{
  Real min = ref_dist.GetMaxDistance();
  Real max = ref_dist.GetMaxDistance();
  bool within_tol = false;
  Real difference = 0;
  if (mdl_dist>=min && mdl_dist <=max) {
    within_tol=true;
  } else if (mdl_dist < min && std::abs(min-mdl_dist) < tol) {
    within_tol = true;
  } else if (mdl_dist > max && std::abs(mdl_dist-max) < tol) {
    within_tol = true;
  }
  if (within_tol == false) {
    if (mdl_dist > max) {
      difference = mdl_dist-(max+tol);
    } else {
      difference = mdl_dist-(min-tol);
    }
  }  
  return std::make_pair<bool,Real>(within_tol,difference);  
}      


std::pair<Real, Real> calc_overlap1(const ResidueDistanceList& res_distance_list, 
                                    ChainView mdl_chain, 
                                    Real tol, bool only_fixed, 
                                    bool swap,std::vector<std::pair<Real, Real> >& overlap_list, bool log )
{
  std::pair<Real, Real> overlap(0.0, 0.0);

  ResidueView mdl_res=mdl_chain.FindResidue(res_distance_list[0].GetFirstAtom().GetResNum()); 
  for (ResidueDistanceList::const_iterator ai=res_distance_list.begin(),
       ae=res_distance_list.end(); ai!=ae; ++ai) {
    UniqueAtomIdentifier first_atom=ai->GetFirstAtom();
    UniqueAtomIdentifier second_atom=ai->GetSecondAtom();
    String name=swap ? swapped_name(first_atom.GetAtomName()) : first_atom.GetAtomName();
    AtomView av1=mdl_res ? mdl_res.FindAtom(name) : AtomView();
 
    if (only_fixed) {
      if (swappable(second_atom.GetResidueName(), second_atom.GetAtomName())) {
          continue;
      }
      AtomView av2=mdl_chain.FindAtom(second_atom.GetResNum(),second_atom.GetAtomName());
      overlap.second+=1.0;
      if (!(av1 && av2)) {
        continue;
      }
      Real mdl_dist=geom::Length(av1.GetPos()-av2.GetPos());
      ReferenceDistance ref_dist=*ai;
      if (within_tolerance(mdl_dist,ref_dist,tol).first) {
        if (log) {
          LOG_VERBOSE("LDT:" << " " << av1.GetResidue().GetChain() << " " << av1.GetResidue().GetName() << " " << av1.GetResidue().GetNumber() << " " << av1.GetName() 
             << " " << av2.GetResidue().GetChain() << " " << av2.GetResidue().GetName() << " " << av2.GetResidue().GetNumber() << " " << av2.GetName() << " " 
             << mdl_dist << " " << ref_dist.GetMinDistance() << " " << ref_dist.GetMaxDistance() << " " << tol << " " << "PASS")
        }   
        overlap.first+=1;
      } else {
        if (log) {
          LOG_VERBOSE("LDT:" << " " << av1.GetResidue().GetChain() << " " << av1.GetResidue().GetName() << " " << av1.GetResidue().GetNumber() << " " << av1.GetName() 
             << " " << av2.GetResidue().GetChain() << " " << av2.GetResidue().GetName() << " " << av2.GetResidue().GetNumber() << " " << av2.GetName() << " " 
             << mdl_dist << " " << ref_dist.GetMinDistance() << " " << mdl_dist-ref_dist.GetMinDistance() << " " << tol << " " << "FAIL")
        }  
      }
      continue;
    }
    AtomView av2=mdl_chain.FindAtom(second_atom.GetResNum(), second_atom.GetAtomName());
    overlap.second+=1.0;
    if (av1) {
      overlap_list[av1.GetResidue().GetIndex()].second+=1.0;
    }
    if (av2) {
      overlap_list[av2.GetResidue().GetIndex()].second+=1.0;
     }  
    if (!(av1 && av2)) {
      continue;
    }
    
    Real mdl_dist=geom::Length(av1.GetPos()-av2.GetPos());
    ReferenceDistance ref_dist=*ai;
    if (within_tolerance(mdl_dist,ref_dist,tol).first) {
      LOG_INFO("LDT:" << " " << av1.GetResidue().GetChain() << " " << av1.GetResidue().GetName() << " " << av1.GetResidue().GetNumber() << " " << av1.GetName() 
           << " " << av2.GetResidue().GetChain() << " " << av2.GetResidue().GetName() << " " << av2.GetResidue().GetNumber() << " " << av2.GetName() << " " 
           << mdl_dist << " " << ref_dist.GetMinDistance() << " " << mdl_dist-ref_dist.GetMinDistance() << " " << tol << " " << "PASS")
      overlap.first+=1;
      overlap_list[av1.GetResidue().GetIndex()].first+=1.0;
      overlap_list[av2.GetResidue().GetIndex()].first+=1.0;
    } else {
      LOG_VERBOSE("LDT:" << " " << av1.GetResidue().GetChain() << " " << av1.GetResidue().GetName() << " " << av1.GetResidue().GetNumber() << " " << av1.GetName() 
             << " " << av2.GetResidue().GetChain() << " " << av2.GetResidue().GetName() << " " << av2.GetResidue().GetNumber() << " " << av2.GetName() << " " 
             << mdl_dist << " " << ref_dist.GetMinDistance() << " " << mdl_dist-ref_dist.GetMinDistance() << " " << tol << " " << "FAIL")
    } 
  }    
  return overlap;
}


std::pair<Real, Real> calc_overlap2(const seq::ConstSequenceHandle& ref_seq,
                                    const seq::ConstSequenceHandle& mdl_seq,
                                    int pos, Real tol, Real max_dist, 
                                    bool only_fixed, bool swap)
{
  std::pair<Real, Real> overlap(0.0, 0.0);
  EntityView ref=ref_seq.GetAttachedView();
  ResidueView ref_res=ref_seq.GetResidue(pos);
  if (!ref_res.IsValid()) {
    return std::pair<Real,Real>(0.0, 0.0);
  }
  AtomViewList ref_atoms=ref_res.GetAtomList();
  ResidueView mdl_res=mdl_seq.GetResidue(pos);
  AtomViewList within;
  if (max_dist<0) {
    within=ref.GetAtomList();
  }  
  for (AtomViewList::iterator ai=ref_atoms.begin(),
       ae=ref_atoms.end(); ai!=ae; ++ai) {
    if (ai->GetElement()=="H") { continue; }
    String name=swap ? swapped_name(ai->GetName()) : ai->GetName();
    AtomView av1=mdl_res ? mdl_res.FindAtom(name) : AtomView();
    if (max_dist>=0){ 
      within=ref.FindWithin(ai->GetPos(), max_dist);
    }
    for (AtomViewList::iterator aj=within.begin(),
         ae2=within.end(); aj!=ae2; ++aj) {
      if (aj->GetElement()=="H" || 
          aj->GetResidue().GetChain()!=ai->GetResidue().GetChain()) { 
          continue; 
      }
      if (only_fixed) {
        if (aj->GetResidue().GetNumber()==ref_res.GetNumber()) {
          continue;
        }
        if (swappable(aj->GetResidue().GetName(), aj->GetName())) {
          continue;
        }
        overlap.second+=1.0;
        // map from residue index to position in alignment
        try {
          int aln_pos=ref_seq.GetPos(aj->GetResidue().GetIndex());
          ResidueView r2=mdl_seq.GetResidue(aln_pos);
          if (!r2.IsValid()) {
            continue;
          }
          AtomView av2=r2.FindAtom(aj->GetName());
          if (!(av1 && av2)) {
            continue;
          }
          Real mdl_dist=geom::Length(av1.GetPos()-av2.GetPos());
          Real ref_dist=geom::Length(ai->GetPos()-aj->GetPos());
          if (std::abs(mdl_dist-ref_dist)<tol) {
            overlap.first+=1;
          }
        } catch(...) { }
        continue;
      } else {
        if (aj->GetResidue().GetNumber()>ref_res.GetNumber()) {
          overlap.second+=1.0;    

          try {
           int aln_pos=ref_seq.GetPos(aj->GetResidue().GetIndex());
            ResidueView r2=mdl_seq.GetResidue(aln_pos);
            if (!r2.IsValid()) {
              continue;
            }
            AtomView av2=r2.FindAtom(aj->GetName());
            if (!(av1 && av2)) {
              continue;
            }
            Real mdl_dist=geom::Length(av1.GetPos()-av2.GetPos());
            Real ref_dist=geom::Length(ai->GetPos()-aj->GetPos());
            if (std::abs(mdl_dist-ref_dist)<tol) {
              overlap.first+=1;
            }
          } catch (...) { }
        }  
      }
    }      
  }
  return overlap;
}

}

bool IsStandardResidue(String rn)
{
  String upper_rn=rn;
  std::transform(rn.begin(),rn.end(),rn.begin(),toupper);
  if (upper_rn == "ALA" ||    
  upper_rn == "ARG" ||
  upper_rn == "ASN" ||
  upper_rn == "ASP" ||
  upper_rn == "GLN" ||
  upper_rn == "GLU" ||
  upper_rn == "LYS" ||
  upper_rn == "SER" ||
  upper_rn == "CYS" ||      
  upper_rn == "TYR" ||  
  upper_rn == "TRP" || 
  upper_rn == "THR" ||
  upper_rn == "VAL" ||
  upper_rn == "ILE" ||
  upper_rn == "MET" ||
  upper_rn == "LEU" ||  
  upper_rn == "GLY" ||  
  upper_rn == "PRO" ||  
  upper_rn == "HIS" ||  
  upper_rn == "PHE") {
    return true;
  }  
  return false;
}  

bool UniqueAtomIdentifier::operator==(const UniqueAtomIdentifier& rhs) const
{
  if (chain_ == rhs.GetChainName() && 
      residue_ == rhs.GetResNum() &&
      residue_name_ == rhs.GetResidueName() &&
      atom_ == rhs.GetAtomName() ) {
    return true;
  }
  return false;   
}

bool ReferenceDistance::IsValid() const 
{
  if (mind_ == -1.0 && maxd_ == -1.0) {
    return false;
  } 
  return true;
}

void ReferenceDistance::Print() const 
{ 
  if (this->IsValid() == true) {
    std::cout << first_atom_.GetChainName() << " " << first_atom_.GetResNum() << " " << first_atom_.GetResidueName() << " " << first_atom_.GetAtomName() << " " <<
                 second_atom_.GetChainName() << " " << second_atom_.GetResNum() << " " << second_atom_.GetResidueName() << " " << second_atom_.GetAtomName() << " " <<
                 mind_ << " " << maxd_ << std::endl; 
  } else {
    std::cout << first_atom_.GetChainName() << " " << first_atom_.GetResNum() << " " << first_atom_.GetResidueName() << " Placeholder" << std::endl;
  }
}

bool ReferenceDistance::operator==(const ReferenceDistance& rhs) const
{
  if (first_atom_ == rhs.GetFirstAtom() && 
      second_atom_ == rhs.GetSecondAtom() &&
      mind_ == rhs.GetMinDistance() &&
      maxd_ == rhs.GetMaxDistance() ) {
    return true;
  }
  return false;
}




GlobalDistanceList CreateDistanceList(const EntityView& ref,Real max_dist)
{
 GlobalDistanceList dist_list; 
 ResidueViewList ref_residues=ref.GetChainList()[0].GetResidueList();
 for (ResidueViewList::iterator i=ref_residues.begin(), e=ref_residues.end(); i!=e; ++i) {
   ResidueView rview = (*i);
   if (IsStandardResidue(rview.GetName())) {
     ResidueDistanceList res_dist_list;
     AtomViewList ref_atoms=(*i).GetAtomList();
     AtomViewList within;
     if (max_dist<0){
     dist_list.push_back(res_dist_list);
       within=ref.GetAtomList();
     }  
     for (AtomViewList::iterator ai=ref_atoms.begin(), ae=ref_atoms.end(); ai!=ae; ++ai) {
       UniqueAtomIdentifier first_atom(ai->GetResidue().GetChain().GetName(),ai->GetResidue().GetNumber(),ai->GetResidue().GetName(),ai->GetName());
       if (ai->GetElement()=="H") { continue; }
       if (max_dist>=0){ 
         within=ref.FindWithin(ai->GetPos(), max_dist);
       }
       for (AtomViewList::iterator aj=within.begin(), ae2=within.end(); aj!=ae2; ++aj) {      
         UniqueAtomIdentifier second_atom(aj->GetResidue().GetChain().GetName(),aj->GetResidue().GetNumber(),aj->GetResidue().GetName(),aj->GetName());
         if (aj->GetElement()=="H" ||
             aj->GetResidue().GetChain()!=ai->GetResidue().GetChain()) {
             continue;
         }
         if (aj->GetResidue().GetNumber()>i->GetNumber()) {
           Real dist=geom::Length(ai->GetPos()-aj->GetPos());
           ReferenceDistance ref_dist(first_atom,second_atom,dist,dist);
           res_dist_list.push_back(ref_dist);
         }
       }
     }
     if (res_dist_list.size()==0) {
       UniqueAtomIdentifier current_residue_fake_atom(rview.GetChain().GetName(),rview.GetNumber(),rview.GetName(),"CA");
       ReferenceDistance fake_ref_distance(current_residue_fake_atom,current_residue_fake_atom,-1.0,-1.0);
       res_dist_list.push_back(fake_ref_distance);
     }  
     dist_list.push_back(res_dist_list);
   }
 }  
 return dist_list;
} 


Real LocalDistTest(const EntityView& mdl, const GlobalDistanceList& glob_dist_list,
                   Real cutoff, const String& local_ldt_property_string)
{
  if (!mdl.GetResidueCount()) {
    LOG_WARNING("model structures doesn't contain any residues");
    return 0.0;
  }
  if (glob_dist_list.size()==0) {
    LOG_WARNING("global reference list is empty");
    return 0.0;
  }
  std::vector<std::pair<Real, Real> > overlap_list(mdl.GetResidueCount(), std::pair<Real, Real>(0.0, 0.0));
  ChainView mdl_chain=mdl.GetChainList()[0];  
  // Residues with symmetric side-chains require special treatment as there are 
  // two possible ways to name the atoms. We calculate the overlap with the 
  // fixed atoms and take the solution that gives bigger scores.
  XCSEditor edi=mdl.GetHandle().EditXCS(BUFFERED_EDIT);
  for (GlobalDistanceList::const_iterator i=glob_dist_list.begin(), e=glob_dist_list.end(); i!=e; ++i) {
    ResidueDistanceList rdl = *i;
    if (!rdl[0].IsValid()) {
      continue;
    }
    
    ResNum rnum =  rdl[0].GetFirstAtom().GetResNum() ;
    String rname=rdl[0].GetFirstAtom().GetResidueName();
    ResidueView mdl_res=mdl_chain.FindResidue(rnum);  
    if (!mdl_res) {   
      continue;
    }
    if (!(rname=="GLU" || rname=="ASP" || rname=="VAL" || rname=="TYR" || rname=="PHE" || rname=="LYS" || rname=="ARG")) {
      continue;
    }
    std::pair<Real, Real> ov1=calc_overlap1(*i, mdl_chain, 
                                          cutoff, true, 
                                          false, overlap_list,false);
 
    std::pair<Real, Real> ov2=calc_overlap1(*i, mdl_chain, 
                                          cutoff, true, 
                                          true, overlap_list,false); 
    if (ov1.first/ov1.second<ov2.first/ov2.second) {
      AtomViewList atoms=mdl_res.GetAtomList();
      for (AtomViewList::iterator j=atoms.begin(), e2=atoms.end(); j!=e2; ++j) {
        if (swappable(rname, j->GetName())) {
          edi.RenameAtom(j->GetHandle(), swapped_name(j->GetName()));
        }
      }
    }
  }
  overlap_list.clear();
  std::pair<Real, Real> total_ov(0.0, 0.0);
  for (GlobalDistanceList::const_iterator i=glob_dist_list.begin(), e=glob_dist_list.end(); i!=e; ++i) {
    ResidueDistanceList rdl = *i;
    if (rdl[0].IsValid()) {
      std::pair<Real, Real> ov1=calc_overlap1(*i, mdl_chain, cutoff, 
                                            false, false, overlap_list,true);
      total_ov.first+=ov1.first;
      total_ov.second+=ov1.second;       
    }
    if(local_ldt_property_string!="") {
      ResNum rn = rdl[0].GetFirstAtom().GetResNum();
      ResidueView mdlr=mdl_chain.FindResidue(rn);  
      if (mdlr.IsValid()) {
        int mdl_res_index =mdlr.GetIndex();
        Real local_ldt=overlap_list[mdl_res_index].first/(overlap_list[mdl_res_index].second ? overlap_list[mdl_res_index].second : 1);
        ResidueView res_to_wr = mdl_chain.FindResidue((*i)[0].GetFirstAtom().GetResNum()); 
        res_to_wr.SetFloatProp(local_ldt_property_string, local_ldt);
      }  
    }       
  }
  overlap_list.clear();
  return total_ov.first/(total_ov.second ? total_ov.second : 1);
}

Real LocalDistTest(const EntityView& mdl, const EntityView& target, Real cutoff, Real max_dist, const String& local_ldt_property_string)
{
   GlobalDistanceList glob_dist_list = CreateDistanceList(target,max_dist);
   return LocalDistTest(mdl, glob_dist_list, cutoff, local_ldt_property_string);
}



Real LocalDistTest(const ost::seq::AlignmentHandle& aln,
                   Real cutoff, Real max_dist, int ref_index, int mdl_index)
{
  seq::ConstSequenceHandle ref_seq=aln.GetSequence(ref_index);
  seq::ConstSequenceHandle mdl_seq=aln.GetSequence(mdl_index);  
  if (!ref_seq.HasAttachedView()) {
    LOG_ERROR("reference sequence doesn't have a view attached.");
    return 0.0;
  }
  if (!mdl_seq.HasAttachedView()) {
    LOG_ERROR("model sequence doesn't have a view attached");
    return 0.0;
  }
  XCSEditor edi=ref_seq.GetAttachedView().GetHandle().EditXCS(BUFFERED_EDIT);
  for (int i=0; i<aln.GetLength(); ++i) {
    ResidueView mdl_res=mdl_seq.GetResidue(i);
    if (!mdl_res) {
      continue;
    }
    String rname=mdl_res.GetName();
    if (!(rname=="GLU" || rname=="ASP" || rname=="VAL" || rname=="TYR" || 
         rname=="PHE" || rname=="LYS" || rname=="ARG")) {
      continue;
    }
    std::pair<Real, Real> ov1=calc_overlap2(ref_seq, mdl_seq, i,
                                            cutoff, max_dist, true, 
                                            false);
    std::pair<Real, Real> ov2=calc_overlap2(ref_seq, mdl_seq, i,
                                            cutoff, max_dist, true, 
                                            true);
    if (ov1.first/ov1.second<ov2.first/ov2.second) {
     AtomViewList atoms=mdl_res.GetAtomList();
     for (AtomViewList::iterator j=atoms.begin(), 
          e2=atoms.end(); j!=e2; ++j) {
       if (swappable(rname, j->GetName())) {
         edi.RenameAtom(j->GetHandle(), swapped_name(j->GetName()));
       }
     }
    }
  }
  std::pair<Real, Real> total_ov(0.0, 0.0);
  for (int i=0; i<aln.GetLength(); ++i) {
     std::pair<Real, Real> ov1=calc_overlap2(ref_seq, mdl_seq, i, cutoff, 
                                             max_dist, false, false);
     total_ov.first+=ov1.first;
     total_ov.second+=ov1.second;
  }
  return total_ov.first/(total_ov.second ? total_ov.second : 1);
  return 0.0;
}

Real LDTHA(EntityView& v, const GlobalDistanceList& global_dist_list)
{
    
    Real cutoffs[]={0.5,1,2,4};
    String labels[]={"localldt0.5","localldt1","localldt2","ldtlocal4"};
    Real ldt=0.0;   
    for (int n=0; n<4; ++n) { 
      ldt+=alg::LocalDistTest(v, global_dist_list, cutoffs[n], labels[n]);
    }      
    ldt/=4.0;    
    return ldt;
}

}}}
