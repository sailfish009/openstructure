#include <ost/log.hh>
#include <ost/mol/mol.hh>
#include <ost/mol/alg/distance_rmsd_test.hh>
#include <ost/mol/alg/distance_test_common.hh>
#include <boost/concept_check.hpp>

namespace ost { namespace mol { namespace alg {

namespace {

std::pair<Real, long int> calc_rmsd(const ResidueRDMap& res_distance_list, const ResNum& rnum,
                                    ChainView mdl_chain, int sequence_separation, Real cap_distance,
                                    bool only_fixed, bool swap, std::vector<std::pair<Real, long int> >& local_rmsd_data_list, bool log )
{
  std::pair<Real, long int> rmsd_data(0, 0);
  ResidueView mdl_res=mdl_chain.FindResidue(rnum);
  for (ResidueRDMap::const_iterator ai=res_distance_list.begin(); ai!=res_distance_list.end(); ++ai) {
    const UAtomIdentifiers& uais = ai->first;
    const std::pair <Real,Real>& values = ai->second;
    const UniqueAtomIdentifier& first_atom=uais.first;
    const UniqueAtomIdentifier& second_atom=uais.second;
    String name=swap ? SwappedName(first_atom.GetAtomName()) : first_atom.GetAtomName();
    AtomView av1=mdl_res ? mdl_res.FindAtom(name) : AtomView();

    if (only_fixed) {
       if (std::abs(first_atom.GetResNum().GetNum()-second_atom.GetResNum().GetNum())<=sequence_separation) {
          continue;
        }
      if (Swappable(second_atom.GetResidueName(), second_atom.GetAtomName())) {
          continue;
      }
    }
    if (!only_fixed) {
      if (first_atom.GetResNum().GetNum()<=(second_atom.GetResNum().GetNum()+sequence_separation)) {
        continue;
      }
    }
    ResidueView rv2=mdl_chain.FindResidue(second_atom.GetResNum());
    rmsd_data.second+=1;
    int rindex2=0;
    int rindex1=mdl_res ? mdl_res.GetIndex() : -1;
    if (!only_fixed && rindex1!=-1)
      local_rmsd_data_list[rindex1].second+=1;
    if (!rv2) {
      rmsd_data.first+=(cap_distance*cap_distance);
      local_rmsd_data_list[rindex1].first+=(cap_distance*cap_distance);
      continue;
    }
    rindex2=rv2.GetIndex();
    if (!only_fixed)
      local_rmsd_data_list[rindex2].second+=1;
    AtomView av2=rv2.FindAtom(second_atom.GetAtomName());
    if (!(av1 && av2)) {
      rmsd_data.first+=(cap_distance*cap_distance);
      local_rmsd_data_list[rindex1].first+=(cap_distance*cap_distance);
      local_rmsd_data_list[rindex2].first+=(cap_distance*cap_distance);
      continue;
    }
    Real mdl_dist=geom::Length(av1.GetPos()-av2.GetPos());
    Real diff = mdl_dist-values.first;
    if (log) {
//      LOG_VERBOSE("drmsd:" << " " << av1.GetResidue().GetChain() << " " << av1.GetResidue().GetName() << " " << av1.GetResidue().GetNumber() << " " << av1.GetName()
//                  << " " << av2.GetResidue().GetChain() << " " << av2.GetResidue().GetName() << " " << av2.GetResidue().GetNumber() << " " << av2.GetName() << " "
//                  << mdl_dist << " " << values.first << " " <<  diff)
      LOG_VERBOSE("drmsd:" << " " << av1.GetResidue().GetChain() << " " << av1.GetResidue().GetName() << " " << av1.GetResidue().GetNumber() << " " << av1.GetName()
                  << " " << av1.GetPos() << " " << av2.GetResidue().GetChain() << " " << av2.GetResidue().GetName() << " " << av2.GetResidue().GetNumber() << " " << av2.GetName() << " "
                  << av2.GetPos() << " " << mdl_dist << " " << values.first << " " <<  diff)
    }
    rmsd_data.first+=(diff*diff);
    if (!only_fixed) {
      local_rmsd_data_list[rindex1].first+=(diff*diff);
      local_rmsd_data_list[rindex2].first+=(diff*diff);
    }
  }
  return rmsd_data;
}


void drmsdt_check_and_swap(const GlobalRDMap& glob_dist_list, const EntityView& mdl, int sequence_separation,  Real cap_distance, std::vector<std::pair<Real, long int> > local_rmsd_data_list)
{
  ChainView mdl_chain=mdl.GetChainList()[0];
  XCSEditor edi=mdl.GetHandle().EditXCS(BUFFERED_EDIT);
  for (GlobalRDMap::const_iterator i=glob_dist_list.begin(); i!=glob_dist_list.end(); ++i) {
    ResNum rnum =  i->first;
    if (i->second.size()==0) {
      continue;
    }
    ResidueView mdl_res=mdl_chain.FindResidue(rnum);
    if (!mdl_res) {
      continue;
    }
    String rname = mdl_res.GetName();
    if (!(rname=="GLU" || rname=="ASP" || rname=="VAL" || rname=="TYR" ||
          rname=="PHE" || rname=="LEU" || rname=="ARG")) {
      continue;
    }
//    std::pair<long int, long int> ov1=calc_rmsd(i->second, rnum,mdl_chain, sequence_separation,
//                                          cap_distance, true, false, local_rmsd_data_list,false);

//    std::pair<long int, long int> ov2=calc_rmsd(i->second, rnum, mdl_chain, sequence_separation,
//                                          cap_distance, true, true, local_rmsd_data_list,false);

    std::pair<long int, long int> ov1=calc_rmsd(i->second, rnum,mdl_chain, sequence_separation,
                                            cap_distance, true, false, local_rmsd_data_list,true);

    std::pair<long int, long int> ov2=calc_rmsd(i->second, rnum, mdl_chain, sequence_separation,
                                            cap_distance, true, true, local_rmsd_data_list,true);


    if (std::sqrt(ov1.first/static_cast<Real>(ov1.second))>(std::sqrt(ov2.first/static_cast<Real>(ov2.second)))) {

      AtomViewList atoms=mdl_res.GetAtomList();
      for (AtomViewList::iterator j=atoms.begin(), e2=atoms.end(); j!=e2; ++j) {
        if (Swappable(rname, j->GetName())) {
          edi.RenameAtom(j->GetHandle(), SwappedName(j->GetName()));
        }
      }
    }
  }
}


}

std::pair<Real,long int> DistanceRMSDTest(const EntityView& mdl, const GlobalRDMap& glob_dist_list,
                           Real cap_distance, int sequence_separation, const String& local_drmsdt_property_string)
{
  if (!mdl.GetResidueCount()) {
    LOG_WARNING("model structures doesn't contain any residues");
    return std::make_pair<long int,long int>(0,0);
  }
  if (glob_dist_list.size()==0) {
    LOG_WARNING("global reference list is empty");
    return std::make_pair<long int,long int>(0,0);
  }
  std::vector<std::pair<Real, long int> > local_rmsd_data_list(mdl.GetResidueCount(), std::pair<Real, long int>(0, 0));
  drmsdt_check_and_swap(glob_dist_list,mdl,sequence_separation, cap_distance, local_rmsd_data_list);
  ChainView mdl_chain=mdl.GetChainList()[0];
  local_rmsd_data_list.clear();
  std::pair<Real, long int> total_ov(0, 0);
  for (GlobalRDMap::const_iterator i=glob_dist_list.begin(), e=glob_dist_list.end(); i!=e; ++i) {
    ResNum rn = i->first;
    if (i->second.size()!=0) {
      std::pair<Real, long int> ov1=calc_rmsd(i->second, rn, mdl_chain, sequence_separation, cap_distance, false, false, local_rmsd_data_list,true);
      total_ov.first+=ov1.first;
      total_ov.second+=ov1.second;
     }
  }


  for (GlobalRDMap::const_iterator i=glob_dist_list.begin(),
       e=glob_dist_list.end();i!=e; ++i) {
    ResNum rn = i->first;
    if(local_drmsdt_property_string!="") {
      ResidueView mdlr=mdl_chain.FindResidue(rn);
      if (mdlr.IsValid()) {
        int mdl_res_index =mdlr.GetIndex();
        Real local_rmsd=sqrt(local_rmsd_data_list[mdl_res_index].first/(static_cast<Real>(local_rmsd_data_list[mdl_res_index].second) ? static_cast<Real>(local_rmsd_data_list[mdl_res_index].second) : 1));
        mdlr.SetFloatProp(local_drmsdt_property_string, local_rmsd);
        mdlr.SetIntProp(local_drmsdt_property_string+"_sum", local_rmsd_data_list[mdl_res_index].first);
        mdlr.SetIntProp(local_drmsdt_property_string+"_count", local_rmsd_data_list[mdl_res_index].second);
      }
    }
  }
  local_rmsd_data_list.clear();
  return std::make_pair<Real,long int>(total_ov.first,total_ov.second);
}


Real DRMSD(const EntityView& v, const GlobalRDMap& global_dist_list, Real cap_distance, int sequence_separation)
{
    String label="localdrmsd";
    std::pair<Real,long int> total_ov=alg::DistanceRMSDTest(v, global_dist_list, cap_distance, sequence_separation, label);

    Real calcdrmsd = static_cast<Real>(total_ov.first)/(static_cast<Real>(total_ov.second) ? static_cast<Real>(total_ov.second) : 1);

    return std::sqrt(calcdrmsd);
}






}}}
