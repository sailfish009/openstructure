#include <ost/mol/mm/heuristic_block_modifiers.hh>
#include <ost/mol/mm/mm_interaction.hh>
#include <ost/geom/geom.hh>
#include <math.h>

namespace ost{ namespace mol{ namespace mm{


void HeuristicHydrogenConstructor::ApplyOnBuildingBlock(BuildingBlockPtr p){
  //we assume, that the hydrogens are already correct in the building block...
}

void HeuristicHydrogenConstructor::ApplyOnResidue(ost::mol::ResidueHandle& res, ost::mol::XCSEditor& edi){
  if(!res.IsValid()) return;
  std::vector<String> atoms=building_block_->GetAtoms();
  std::vector<Real> masses=building_block_->GetMasses();
  std::vector<MMInteractionPtr>  bonds=building_block_->GetBonds();
  //int natoms=atoms.size();
  std::vector<Real>::iterator mass;
  std::vector<String>::iterator aname;
  //We loop through all atoms and for each one seek which atoms it's bonded to
  //If there are hydrogens with missing positions, we reconstruct them
  for( aname=atoms.begin(), mass=masses.begin(); aname!=atoms.end(); aname++ , mass++){
    AtomHandle a=res.FindAtom(*aname);
    if(!a.IsValid() || *mass<1.9) continue;// if mass is smaller than 1.9 then it is a hydrogen
    std::cout<< "checking " << *aname << "\n";
    //Find all the bonds containing atom a
    std::vector<MMInteractionPtr> bond_list;
    std::vector<String> antecedent_names,hydrogen_names;
    AtomHandleList antecedents;
    String aname_new;
    for (std::vector<MMInteractionPtr>::iterator bond=bonds.begin() ; bond!=bonds.end() ; bond++){
      std::vector<String> names=(*bond)->GetNames();
      if (*aname!=names[0] && *aname!=names[1]) continue;
      if (*aname==names[0]) aname_new=names[1];
      else aname_new=names[0];
      int aindex=std::distance(atoms.begin(),std::find(atoms.begin(),atoms.end(),aname_new));
      AtomHandle anew=res.FindAtom(aname_new);
      //check whether it's a hydrogen
      if (masses[aindex]>1.9) {
        if (!anew.IsValid()){
          std::cout << "invalid heavy atom" << aname_new << "stopping reconstruction" << std::endl;
          return;
        }
        antecedents.push_back(anew);
        antecedent_names.push_back(aname_new);
      }
      else if (anew.IsValid()){
        antecedents.push_back(anew);
        antecedent_names.push_back(aname_new);        
      }
      else {
        hydrogen_names.push_back(aname_new);
        bond_list.push_back(*bond);
        }
      }
    if (hydrogen_names.size()==0){
      std::cout << "Nothing to reconstruct around " << *aname << std::endl;
      continue;
    }
    //Now we treat the different cases
    std::cout<< "number of antecedents is " << antecedents.size() << " and number of hydrogens "<< hydrogen_names.size() << std::endl;
    Real bond_length=1.08;
    String ele(1,'H');
    geom::Vec3List vl=geom::Vec3List();
    for (AtomHandleList::iterator aa=antecedents.begin(),e=antecedents.end();aa!=e;aa++) vl.push_back((*aa).GetPos());
    geom::Vec3 antecedents_cm=vl.GetCenter();
    geom::Vec3 pa=a.GetPos();
    geom::Vec3 v=geom::Normalize(pa-antecedents_cm);
    geom::Vec3 n=geom::OrthogonalVector(v),direction=geom::Vec3(),hpos=geom::Vec3();
    geom::Mat3 R=geom::Mat3();
    Real bond_angle;
    AtomHandle ha=mol::AtomHandle();
    if (antecedents.size()==0) continue; // We do not reconstruc water hydrogens
    switch (hydrogen_names.size()){
      case 1:{  
        if (antecedents.size()==1){
          bond_angle=0.4*M_PI;
          }
        else {
          bond_angle=0.0;
          }
        //AtomHandle aa=res.FindAtom(antecedents[0]);
        R=geom::AxisRotation(n,bond_angle);
        direction=R*v;
        hpos=pa+bond_length*direction;
        ha=edi.InsertAtom(res,hydrogen_names[0],hpos,ele);
        edi.Connect(a,ha);
        break;
        }
      case 2:{
        if (antecedents.size()==1){
          bond_angle=M_PI/3.;
          }
        else {
          bond_angle=0.4*M_PI;
          n=geom::Normalize(antecedents[0].GetPos()-antecedents[1].GetPos());
          }
        R=geom::AxisRotation(n,bond_angle);
        direction=R*v;
        hpos=pa+bond_length*direction;
        ha=edi.InsertAtom(res,hydrogen_names[0],hpos,ele);
        edi.Connect(a,ha);
        R=geom::AxisRotation(v,M_PI);
        direction=R*direction;
        hpos=pa+bond_length*direction;
        ha=edi.InsertAtom(res,hydrogen_names[1],hpos,ele);
        edi.Connect(a,ha);
        break;
        }
      case 3:{
        bond_angle=0.4*M_PI;
        R=geom::AxisRotation(n,bond_angle);
        direction=R*v;
        hpos=pa+bond_length*direction;
        edi.InsertAtom(res,hydrogen_names[0],hpos,ele);
        R=geom::AxisRotation(v,2.*M_PI/3.);
        direction=R*direction;
        hpos=pa+bond_length*direction;
        ha=edi.InsertAtom(res,hydrogen_names[1],hpos,ele);
        edi.Connect(a,ha);
        direction=R*direction;
        hpos=pa+bond_length*direction;
        ha=edi.InsertAtom(res,hydrogen_names[2],hpos,ele);
        edi.Connect(a,ha);
        break;
        }
      default :{
        std::cout << "number of hydrogens is " << hydrogen_names.size() << " for "<< *aname <<" not reconstructing them." <<std::endl;
        }
      }
  }
}

void HeuristicBlockModifier::ApplyOnBuildingBlock(BuildingBlockPtr p){

}

void HeuristicBlockModifier::ApplyOnResidue(ost::mol::ResidueHandle& res, ost::mol::XCSEditor& ed){

}


}}}//ns


















