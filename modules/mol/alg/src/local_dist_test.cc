#include <ost/log.hh>
#include <ost/mol/mol.hh>
#include "local_dist_test.hh"

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

std::pair<Real, Real> calc_overlap1(ResidueView ref_res, 
                                    EntityView ref,
                                    ChainView mdl_chain, 
                                    Real tol, Real max_dist, bool only_fixed, 
                                    bool swap,std::vector<std::pair<Real, Real> >& overlap_list )
{
  std::pair<Real, Real> overlap(0.0, 0.0);
  AtomViewList ref_atoms=ref_res.GetAtomList();

  ResidueView mdl_res=mdl_chain.FindResidue(ref_res.GetNumber());
  AtomViewList within;
  if (max_dist<0){
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
      if (aj->GetElement()=="H") { continue; }
      if (only_fixed) {
        if (aj->GetResidue().GetNumber()==ref_res.GetNumber()) {
          continue;
        }
        if (swappable(aj->GetResidue().GetName(), aj->GetName())) {
          continue;
        }
        AtomView av2=mdl_chain.FindAtom(aj->GetResidue().GetNumber(), 
                                        aj->GetName());
        overlap.second+=1.0;
        if (!(av1 && av2)) {
          continue;
        }
        Real mdl_dist=geom::Length(av1.GetPos()-av2.GetPos());
        Real ref_dist=geom::Length(ai->GetPos()-aj->GetPos());
        if (std::abs(mdl_dist-ref_dist)<tol) {
          overlap.first+=1;
        }
        continue;
      }
      if (aj->GetResidue().GetNumber()>ref_res.GetNumber()) {
        AtomView av2=mdl_chain.FindAtom(aj->GetResidue().GetNumber(), 
                                             aj->GetName());
        overlap.second+=1.0;
        overlap_list[ref_res.GetIndex()].second+=1.0;
        overlap_list[aj->GetResidue().GetIndex()].second+=1.0;
        if (!(av1 && av2)) {
          continue;
        }
        Real mdl_dist=geom::Length(av1.GetPos()-av2.GetPos());
        Real ref_dist=geom::Length(ai->GetPos()-aj->GetPos());
        if (std::abs(mdl_dist-ref_dist)<tol) {
          overlap.first+=1;
          overlap_list[ref_res.GetIndex()].first+=1.0;
          overlap_list[aj->GetResidue().GetIndex()].first+=1.0;
        }
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
      if (aj->GetElement()=="H") { continue; }
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

Real LocalDistTest(const EntityView& mdl, const EntityView& ref,
                   Real cutoff, Real max_dist, const String& local_ldt_property_string)
{
    
  if (!mdl.GetResidueCount()) {
    LOG_WARNING("model structures doesn't contain any residues");
    return 0.0;
  }

  if (!ref.GetResidueCount()) {
    LOG_WARNING("reference structures doesn't contain any residues");
    return 0.0;
  }
  ResidueViewList ref_residues=ref.GetResidueList();  
  std::vector<std::pair<Real, Real> > overlap_list(ref_residues.size(), std::pair<Real, Real>(0.0, 0.0));
  ChainView mdl_chain=mdl.GetChainList()[0];  
  // Residues with symmetric side-chains require special treatment as there are 
  // two possible ways to name the atoms. We calculate the overlap with the 
  // fixed atoms and take the solution that gives bigger scores.
  XCSEditor edi=ref.GetHandle().EditXCS(BUFFERED_EDIT);
  for (ResidueViewList::iterator
       i=ref_residues.begin(), e=ref_residues.end(); i!=e; ++i) {
    const String rname=i->GetName();
    ResidueView mdl_res=mdl_chain.FindResidue(i->GetNumber());
    if (!mdl_res) {
     continue;
    }     
    if (!(rname=="GLU" || rname=="ASP" || rname=="VAL" || rname=="TYR" || 
         rname=="PHE" || rname=="LYS" || rname=="ARG")) {
      continue;
    }

    std::pair<Real, Real> ov1=calc_overlap1(*i, ref, mdl_chain, 
                                          cutoff, max_dist, true, 
                                          false, overlap_list);
    std::pair<Real, Real> ov2=calc_overlap1(*i, ref, mdl_chain, 
                                          cutoff, max_dist, true, 
                                          true, overlap_list); 
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
  for (ResidueViewList::iterator
       i=ref_residues.begin(), e=ref_residues.end(); i!=e; ++i) {
     std::pair<Real, Real> ov1=calc_overlap1(*i, ref, mdl_chain, cutoff, 
                                            max_dist, false, false, overlap_list);
     total_ov.first+=ov1.first;
     total_ov.second+=ov1.second;
  }

  // attach local per-residue LDT as property:
  if(local_ldt_property_string!="") {
    for (ResidueViewList::iterator
       i=ref_residues.begin(), e=ref_residues.end(); i!=e; ++i) {
     std::pair<Real, Real> ov1=calc_overlap1(*i, ref, mdl_chain, cutoff, 
                                            max_dist, false, false, overlap_list);
     total_ov.first+=ov1.first;
     total_ov.second+=ov1.second;
     Real local_ldt=overlap_list[(*i).GetIndex()].first/(overlap_list[(*i).GetIndex()].second ? overlap_list[(*i).GetIndex()].second : 1);
     if (mdl_chain.FindResidue((*i).GetNumber()).IsValid()) {
      ResidueView mdl_res=mdl_chain.FindResidue((*i).GetNumber());
      mdl_res.SetFloatProp(local_ldt_property_string, local_ldt);
     }
    }
      mdl_chain.SetFloatProp(local_ldt_property_string, total_ov.first/(total_ov.second ? total_ov.second : 1));
  }
  overlap_list.clear();

  return total_ov.first/(total_ov.second ? total_ov.second : 1);
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

}}}
