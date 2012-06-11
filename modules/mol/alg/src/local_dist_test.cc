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

std::pair<bool,Real> within_tolerance(Real mdl_dist, std::pair<Real,Real>& values, Real tol)
{
  Real min = values.first;
  Real max = values.second;
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


std::pair<Real, Real> calc_overlap1(const ResidueRDMap& res_distance_list, const ResNum& rnum,
                                    ChainView mdl_chain, 
                                    std::vector<Real>& tol_list, bool only_fixed, 
                                    bool swap,std::vector<std::pair<Real, Real> >& overlap_list, bool log )
{
  std::pair<Real, Real> overlap(0.0, 0.0);
  ResidueView mdl_res=mdl_chain.FindResidue(rnum); 
  for (ResidueRDMap::const_iterator ai=res_distance_list.begin(); ai!=res_distance_list.end(); ++ai) {
    UAtomIdentifiers uais = (*ai).first; 
    std::pair <Real,Real> values = (*ai).second;
    UniqueAtomIdentifier first_atom=uais.first;
    UniqueAtomIdentifier second_atom=uais.second;
    String name=swap ? swapped_name(first_atom.GetAtomName()) : first_atom.GetAtomName();
    AtomView av1=mdl_res ? mdl_res.FindAtom(name) : AtomView();
 
    if (only_fixed) {
      if (swappable(second_atom.GetResidueName(), second_atom.GetAtomName())) {
          continue;
      }
    }    
    AtomView av2=mdl_chain.FindAtom(second_atom.GetResNum(),second_atom.GetAtomName());
    overlap.second+=tol_list.size();
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
    for (std::vector<Real>::const_reverse_iterator tol_list_it=tol_list.rbegin();tol_list_it!=tol_list.rend();++tol_list_it) {
      Real tol = * tol_list_it; 
      if (within_tolerance(mdl_dist,values,tol).first) {
        if (log) {
          LOG_VERBOSE("LDT:" << " " << av1.GetResidue().GetChain() << " " << av1.GetResidue().GetName() << " " << av1.GetResidue().GetNumber() << " " << av1.GetName() 
             << " " << av2.GetResidue().GetChain() << " " << av2.GetResidue().GetName() << " " << av2.GetResidue().GetNumber() << " " << av2.GetName() << " " 
             << mdl_dist << " " << values.first << " " << values.second << " " << tol << " " << "PASS")
        }   
        overlap.first+=1;
        overlap_list[av1.GetResidue().GetIndex()].first+=1;
        overlap_list[av2.GetResidue().GetIndex()].first+=1;
      } else {
        if (log) {
          LOG_VERBOSE("LDT:" << " " << av1.GetResidue().GetChain() << " " << av1.GetResidue().GetName() << " " << av1.GetResidue().GetNumber() << " " << av1.GetName() 
             << " " << av2.GetResidue().GetChain() << " " << av2.GetResidue().GetName() << " " << av2.GetResidue().GetNumber() << " " << av2.GetName() << " " 
             << mdl_dist << " " << values.first << " " << values.second << " " << tol << " " << "FAIL")
        }  
        break;  
      }
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

void check_and_swap(const GlobalRDMap& glob_dist_list, const EntityView& mdl, std::vector<Real> cutoff_list,  std::vector<std::pair<Real, Real> > overlap_list)
{
  ChainView mdl_chain=mdl.GetChainList()[0]; 
  XCSEditor edi=mdl.GetHandle().EditXCS(BUFFERED_EDIT);
  for (GlobalRDMap::const_iterator i=glob_dist_list.begin(); i!=glob_dist_list.end(); ++i) {  
    ResidueRDMap rdl = (*i).second;
    ResNum rnum =  (*i).first;
    if (rdl.size()==0) {
      continue;
    }
    ResidueView mdl_res=mdl_chain.FindResidue(rnum); 
    if (!mdl_res) {   
      continue;
    }
    String rname = mdl_res.GetName();
    if (!(rname=="GLU" || rname=="ASP" || rname=="VAL" || rname=="TYR" || rname=="PHE" || rname=="LYS" || rname=="ARG")) {
      continue;
    }
    std::pair<Real, Real> ov1=calc_overlap1(rdl, rnum,mdl_chain, 
                                          cutoff_list, true, 
                                          false, overlap_list,false);
 
    std::pair<Real, Real> ov2=calc_overlap1(rdl, rnum, mdl_chain, 
                                          cutoff_list, true, 
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
}

void update_existence_map (ExistenceMap& ex_map, const EntityView& ev, int ref_counter)
{
  AtomViewList ev_atom=ev.GetAtomList();
  for (AtomViewList::iterator ev_atom_it=ev_atom.begin(); ev_atom_it!=ev_atom.end();++ev_atom_it) {
    AtomView ev_atom = (*ev_atom_it);
    UniqueAtomIdentifier uai (ev_atom.GetResidue().GetChain().GetName(),ev_atom.GetResidue().GetNumber(),ev_atom.GetResidue().GetName(),ev_atom.GetName());
    ExistenceMap::const_iterator find_uai_ci = ex_map.find(uai);
    int uai_value = 0;
    if (find_uai_ci!=ex_map.end()) {
      uai_value=(*find_uai_ci).second;
    } 
	uai_value |= 1 << (ref_counter);   
    ex_map[uai]=uai_value;     
  }
}

int in_existence_map(const ExistenceMap& ex_map, const UniqueAtomIdentifier& uai) 
{
  ExistenceMap::const_iterator find_uai_ci = ex_map.find(uai);
  int return_value = 0;
  if (find_uai_ci!=ex_map.end()) {
      return_value=(*find_uai_ci).second;
  } 
  return return_value;   
}    

void merge_distance_lists(GlobalRDMap& ref_dist_map, const GlobalRDMap& new_dist_map, ExistenceMap& ex_map, const EntityView& ref,int ref_counter)
{
  // iterate over the residues in the ref_dist_map   
  for (GlobalRDMap::iterator ref_dist_map_it=ref_dist_map.begin();ref_dist_map_it!=ref_dist_map.end();++ref_dist_map_it) {
    ResidueRDMap ref_res_map = (*ref_dist_map_it).second;  
    ResNum ref_resnum = (*ref_dist_map_it).first;
    GlobalRDMap::const_iterator find_new_res_ci = new_dist_map.find(ref_resnum);
    //if the residue is found in new_dist_map,  
    if (find_new_res_ci != new_dist_map.end()) {
      ResidueRDMap found_res_new = (*find_new_res_ci).second;
      //iterate over the the reference distances in the ResidueDistanceMap  
      for (ResidueRDMap::iterator ref_res_map_it = ref_res_map.begin(); ref_res_map_it!=ref_res_map.end();++ref_res_map_it) {
        UAtomIdentifiers ref_rd = (*ref_res_map_it).first;
        std::pair<Real,Real> ref_minmax = (*ref_res_map_it).second;
        ResidueRDMap::const_iterator find_new_rd_ci = found_res_new.find(ref_rd);
        // if you find the distance in the residue new, udate min and max  
        if (find_new_rd_ci != found_res_new.end()) {
          std::pair <Real,Real> new_minmax = (*find_new_rd_ci).second;
	      Real min = ref_minmax.first;
	      Real max = ref_minmax.second;
          if (new_minmax.first < min) min = new_minmax.first;  
          if (new_minmax.second > max) max = new_minmax.second;
          ref_res_map[ref_rd] = std::make_pair<Real,Real>(min,max);
        } else {              
          // if you don't find it in the residue new, check that it is not missing because it is too long  
          UniqueAtomIdentifier first_atom_to_find = ref_rd.first;
          UniqueAtomIdentifier second_atom_to_find = ref_rd.second;  
          // if both atoms are there, remove the distance from the ref_dist_map,    
          if ((ref.FindAtom(first_atom_to_find.GetChainName(),first_atom_to_find.GetResNum(),first_atom_to_find.GetAtomName()).IsValid() && 
               ref.FindAtom(second_atom_to_find.GetChainName(),second_atom_to_find.GetResNum(),second_atom_to_find.GetAtomName()).IsValid()) ) {
            ref_res_map.erase(ref_res_map_it);                   
          }  
        }
      }
      // now iterate over the the new  reference distances in residue new  
      for (ResidueRDMap::const_iterator new_res_map_it = found_res_new.begin(); new_res_map_it!=found_res_new.end();++new_res_map_it) {
        UAtomIdentifiers new_rd = (*new_res_map_it).first;
        std::pair<Real,Real> new_minmax = (*new_res_map_it).second;
        ResidueRDMap::const_iterator find_ref_rd_ci = ref_res_map.find(new_rd);
        // if the distance is found in the residue ref, 
        // it has been taken care of before. If not
        if (find_ref_rd_ci==ref_res_map.end()) {
          UniqueAtomIdentifier first_atom_to_find = new_rd.first;
          UniqueAtomIdentifier second_atom_to_find = new_rd.second;
          // check that there isn't a structure already processed where both atoms are in
          // if there is none, add the distance to the residue ref map 
          if (!(in_existence_map(ex_map,first_atom_to_find) & in_existence_map(ex_map,second_atom_to_find))) {
            ref_res_map[new_rd]= new_minmax; 
          }
        }      
      }      
    }
  // if the residue was not found in the new list, it means that it is 
  // absent in the new structure, no new information
  }

  // now iterate over the residues in the new_list
  for (GlobalRDMap::const_iterator new_dist_map_it=new_dist_map.begin();new_dist_map_it!=new_dist_map.end();++new_dist_map_it) {
    ResidueRDMap new_res_map = (*new_dist_map_it).second;  
    ResNum new_resnum = (*new_dist_map_it).first;
    GlobalRDMap::const_iterator find_ref_res_ci = ref_dist_map.find(new_resnum);
    // if the residue is found in new_dist_map, it has been taken care before,
    // if not, add it to the res_dist_map:
    if (find_ref_res_ci == ref_dist_map.end()) {
      ResidueRDMap found_res_ref = (*find_ref_res_ci).second;       
      ref_dist_map[new_resnum] = found_res_ref;           
    }
  }

  // finally, update the existence map  
  update_existence_map (ex_map,ref,ref_counter);  
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

bool UniqueAtomIdentifier::operator<(const UniqueAtomIdentifier& rhs) const
{
  if (chain_ < rhs.GetChainName()) {
    return true;
  } else if (chain_ > rhs.GetChainName()) {
	return false;
  } else {
    if (residue_ < rhs.GetResNum()) {
      return true;
    } else if (residue_ > rhs.GetResNum()) {
	  return false;
    } else {
      if (residue_name_ < rhs.GetResidueName()) {
        return true;
      } else if (residue_name_ > rhs.GetResidueName()) {
  	    return false;
      } else {
        if (atom_ < rhs.GetAtomName()) {
          return true;
        } else if (atom_ > rhs.GetAtomName()) {
  	      return false;
        } else {
		  return false;	
		}
	  }  
	}
  }
}	

GlobalRDMap CreateDistanceList(const EntityView& ref,Real max_dist)
{
 GlobalRDMap dist_list; 
 ResidueViewList ref_residues=ref.GetChainList()[0].GetResidueList(); 
 for (ResidueViewList::iterator i=ref_residues.begin(), e=ref_residues.end(); i!=e; ++i) {
   ResidueView rview = (*i);
   if (IsStandardResidue(rview.GetName())) {
     ResidueRDMap res_dist_list;
     ResNum rnum = rview.GetNumber();  
     AtomViewList ref_atoms=(*i).GetAtomList();
     AtomViewList within;
     if (max_dist<0){
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
           UAtomIdentifiers atoms = std::make_pair<UniqueAtomIdentifier,UniqueAtomIdentifier>(first_atom,second_atom); 
           std::pair<Real,Real> values = std::make_pair<Real,Real>(dist,dist);  
           res_dist_list[atoms]=values;
         }
       }
     }   
     dist_list[rnum]=res_dist_list;
   }
 } 
 return dist_list;
} 

GlobalRDMap CreateDistanceListFromMultipleReferences(const std::vector<EntityView>& ref_list, std::vector<Real> cutoff_list, Real max_dist)
{	
  int ref_counter=0;  
  ExistenceMap ex_map;  
  GlobalRDMap glob_dist_list = CreateDistanceList(ref_list[0],max_dist);
  update_existence_map (ex_map,ref_list[0],ref_counter);
  ref_counter++;  
  for (std::vector<EntityView>::const_iterator ref_list_it=ref_list.begin()+1;ref_list_it!=ref_list.end();++ref_list_it) {
       EntityView ref = *ref_list_it;
       std::vector<std::pair<Real, Real> > overlap_list(ref.GetResidueCount(), std::pair<Real, Real>(0.0, 0.0));
       check_and_swap(glob_dist_list,ref,cutoff_list,overlap_list);
       GlobalRDMap new_dist_list=CreateDistanceList(ref,max_dist);
       merge_distance_lists(glob_dist_list,new_dist_list,ex_map,ref,ref_counter);   
       ref_counter++;
  }    
  return glob_dist_list;  
}

void PrintResidueRDMap(const ResidueRDMap& res_dist_list)
{
  for (ResidueRDMap::const_iterator res_dist_list_it = res_dist_list.begin();res_dist_list_it!=res_dist_list.end();++res_dist_list_it) {
    UAtomIdentifiers uais = (*res_dist_list_it).first;
    std::pair<Real,Real> minmax = (*res_dist_list_it).second;
    std::cout << uais.first.GetChainName() << " " << uais.first.GetResNum() << " " << uais.first.GetResidueName() << " " << uais.first.GetAtomName() << " " <<
                 uais.second.GetChainName() << " " << uais.second.GetResNum() << " " << uais.second.GetResidueName() << " " << uais.second.GetAtomName() << " " <<
                 minmax.first << " " << minmax.second << std::endl;       
  }
}


void PrintGlobalRDMap(const GlobalRDMap& glob_dist_list){
  for (GlobalRDMap::const_iterator glob_dist_list_it = glob_dist_list.begin();glob_dist_list_it!=glob_dist_list.end();++glob_dist_list_it) {
    if ((*glob_dist_list_it).second.size()!=0) {  
      PrintResidueRDMap((*glob_dist_list_it).second);
    }    
  }    
}



Real LocalDistTest(const EntityView& mdl, const GlobalRDMap& glob_dist_list,
                   std::vector<Real> cutoff_list, const String& local_ldt_property_string)
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
  check_and_swap(glob_dist_list,mdl,cutoff_list,overlap_list);
  ChainView mdl_chain=mdl.GetChainList()[0];  
  overlap_list.clear();
  std::pair<Real, Real> total_ov(0.0, 0.0);
  for (GlobalRDMap::const_iterator i=glob_dist_list.begin(), e=glob_dist_list.end(); i!=e; ++i) {
    ResidueRDMap rdl = (*i).second;
    ResNum rn = (*i).first;
    if (rdl.size()!=0) {
      std::pair<Real, Real> ov1=calc_overlap1(rdl, rn, mdl_chain, cutoff_list, 
                                            false, false, overlap_list,true);
      total_ov.first+=ov1.first;
      total_ov.second+=ov1.second;       
    }
    if(local_ldt_property_string!="") {
      ResidueView mdlr=mdl_chain.FindResidue(rn);  
      if (mdlr.IsValid()) {
        int mdl_res_index =mdlr.GetIndex();
        Real local_ldt=overlap_list[mdl_res_index].first/(overlap_list[mdl_res_index].second ? overlap_list[mdl_res_index].second : 1);
        mdlr.SetFloatProp(local_ldt_property_string, local_ldt);
      }  
    }       
  }
  overlap_list.clear();
  return total_ov.first/(total_ov.second ? total_ov.second : 1);
}

Real LocalDistTest(const EntityView& mdl, const EntityView& target, Real cutoff, Real max_dist, const String& local_ldt_property_string)
{
   std::vector<Real> cutoffs;
   cutoffs.push_back(cutoff);
   GlobalRDMap glob_dist_list = CreateDistanceList(target,max_dist);
   return LocalDistTest(mdl, glob_dist_list, cutoffs, local_ldt_property_string);
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

Real LDTHA(EntityView& v, const GlobalRDMap& global_dist_list)
{
    std::vector<Real> cutoffs;
    cutoffs.push_back(0.5);
    cutoffs.push_back(1.0);
    cutoffs.push_back(2.0);
    cutoffs.push_back(4.0);
    String label="localldt";
    Real ldt=alg::LocalDistTest(v, global_dist_list, cutoffs, label);
    return ldt;
}

Real OldStyleLDTHA(EntityView& v, const GlobalRDMap& global_dist_list)
{
    Real ldt =0;
    std::vector<Real> cutoffs05;
    cutoffs05.push_back(0.5);
    Real ldt05=alg::LocalDistTest(v, global_dist_list, cutoffs05, "localldt0.5");
    std::vector<Real> cutoffs1;
    cutoffs1.push_back(1.0);
    Real ldt1=alg::LocalDistTest(v, global_dist_list, cutoffs1, "localldt1");
    std::vector<Real> cutoffs2;
    cutoffs2.push_back(2.0);
    Real ldt2=alg::LocalDistTest(v, global_dist_list, cutoffs2, "localldt2");
    std::vector<Real> cutoffs4;
    cutoffs4.push_back(4.0);
    Real ldt4=alg::LocalDistTest(v, global_dist_list, cutoffs4, "localldt4");
    ldt = (ldt05+ldt1+ldt2+ldt4)/4.0;
    for (ResidueViewIter rit=v.ResiduesBegin();rit!=v.ResiduesEnd();++rit){
      ResidueView ritv = *rit;
      if (ritv.HasProp("localldt0.5")) {
        Real ldt_local = (ritv.GetFloatProp("localldt0.5")+ritv.GetFloatProp("localldt1")+ritv.GetFloatProp("localldt2")+ritv.GetFloatProp("localldt4"))/4.0;
        ritv.SetFloatProp("localldt",ldt_local);
      }    
    }    
    return ldt;
}

}}}
