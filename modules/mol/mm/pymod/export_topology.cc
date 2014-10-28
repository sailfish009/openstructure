#include <boost/python.hpp>
#include <ost/mol/mm/topology.hh>
#include <ost/mol/mm/topology_creator.hh>
#include <ost/mol/entity_handle.hh>
#include <vector>

using namespace boost::python;
namespace{

  template<typename T>
  int Get(T index, int i){
    return index[i];
  }

  template<typename T>
  void Set(T& index, int i, int v){
    index[i] = v;
  }

  template<typename T>
  std::vector<T> ListToVec(const boost::python::list& l){

    std::vector<T> vec;
    for (int i = 0; i < boost::python::len(l); ++i){
      vec.push_back(boost::python::extract<T>(l[i]));
    }
    return vec;
  }

  template<typename T>
  boost::python::list VecToList(std::vector<T>& vec){
    boost::python::list l;
    for(typename std::vector<T>::iterator it=vec.begin();it!=vec.end();++it){
      l.append(*it);
    }
    return l;
  }

  ost::mol::mm::TopologyPtr WrapTopologyConstructor(ost::mol::EntityHandle& handle, 
                                     const boost::python::list& masses_list){
    std::vector<Real> masses_vector = ListToVec<Real>(masses_list);
    ost::mol::mm::TopologyPtr p(new ost::mol::mm::Topology(handle,masses_vector));
    return p;
  }

  void WrapAddCMap(ost::mol::mm::TopologyPtr p, int index_one, int index_two, int index_three,
                   int index_four, int index_five, uint dimension, const boost::python::list& l){
    std::vector<Real> v = ListToVec<Real>(l);
    p->AddCMap(index_one,index_two,index_three,index_four,index_five,dimension,v);
  }

  void WrapSetSigmas(ost::mol::mm::TopologyPtr p, const boost::python::list& l){
    std::vector<Real> v = ListToVec<Real>(l);
    p->SetSigmas(v);
  }

  void WrapSetEpsilons(ost::mol::mm::TopologyPtr p, const boost::python::list& l){
    std::vector<Real> v = ListToVec<Real>(l);
    p->SetEpsilons(v);
  }

  void WrapSetGBSARadii(ost::mol::mm::TopologyPtr p, const boost::python::list& l){
    std::vector<Real> v = ListToVec<Real>(l);
    p->SetGBSARadii(v);
  }

  void WrapSetOBCScalings(ost::mol::mm::TopologyPtr p, const boost::python::list& l){
    std::vector<Real> v = ListToVec<Real>(l);
    p->SetOBCScalings(v);
  }

  void WrapSetCharges(ost::mol::mm::TopologyPtr p, const boost::python::list& l){
    std::vector<Real> v = ListToVec<Real>(l);
    p->SetCharges(v);
  }

  void WrapSetMasses(ost::mol::mm::TopologyPtr p, const boost::python::list& l){
    std::vector<Real> v = ListToVec<Real>(l);
    p->SetMasses(v);
  }


  boost::python::list WrapGetSigmas(ost::mol::mm::TopologyPtr p){
    std::vector<Real> v = p->GetSigmas();
    boost::python::list l = VecToList<Real>(v);
    return l;
  }

  boost::python::list WrapGetEpsilons(ost::mol::mm::TopologyPtr p){
    std::vector<Real> v = p->GetEpsilons();
    boost::python::list l = VecToList<Real>(v);
    return l;
  }

  boost::python::list WrapGetGBSARadii(ost::mol::mm::TopologyPtr p){
    std::vector<Real> v = p->GetGBSARadii();
    boost::python::list l = VecToList<Real>(v);
    return l;
  }

  boost::python::list WrapGetOBCScalings(ost::mol::mm::TopologyPtr p){
    std::vector<Real> v = p->GetOBCScalings();
    boost::python::list l = VecToList<Real>(v);
    return l;
  }

  boost::python::list WrapGetCharges(ost::mol::mm::TopologyPtr p){
    std::vector<Real> v = p->GetCharges();
    boost::python::list l = VecToList<Real>(v);
    return l;
  }

  boost::python::list WrapGetMasses(ost::mol::mm::TopologyPtr p){
    std::vector<Real> v = p->GetMasses();
    boost::python::list l = VecToList<Real>(v);
    return l;
  }


  boost::python::tuple WrapGetHarmonicBondParam(ost::mol::mm::TopologyPtr p,uint index){
    uint i1,i2;
    Real b,k;
    p->GetHarmonicBondParameters(index,i1,i2,b,k);
    return boost::python::make_tuple(i1,i2,b,k);
  }

  boost::python::tuple WrapGetHarmonicAngleParam(ost::mol::mm::TopologyPtr p,uint index){
    uint i1,i2,i3;
    Real a,k;
    p->GetHarmonicAngleParameters(index,i1,i2,i3,a,k);
    return boost::python::make_tuple(i1,i2,i3,a,k);
  }

  boost::python::tuple WrapGetUreyBradleyAngleParam(ost::mol::mm::TopologyPtr p,uint index){
    uint i1,i2,i3;
    Real a,a_k,b,b_k;
    p->GetUreyBradleyAngleParameters(index,i1,i2,i3,a,a_k,b,b_k);
    return boost::python::make_tuple(i1,i2,i3,a,a_k,b,b_k);
  }

  boost::python::tuple WrapGetPeriodicDihedralParam(ost::mol::mm::TopologyPtr p,uint index){
    uint i1,i2,i3,i4;
    int m;
    Real ph,k;
    p->GetPeriodicDihedralParameters(index,i1,i2,i3,i4,m,ph,k);
    return boost::python::make_tuple(i1,i2,i3,i4,m,ph,k);
  }

  boost::python::tuple WrapGetPeriodicImproperParam(ost::mol::mm::TopologyPtr p,uint index){
    uint i1,i2,i3,i4;
    int m;
    Real ph,k;
    p->GetPeriodicImproperParameters(index,i1,i2,i3,i4,m,ph,k);
    return boost::python::make_tuple(i1,i2,i3,i4,m,ph,k);
  }

  boost::python::tuple WrapGetHarmonicImproperParam(ost::mol::mm::TopologyPtr p,uint index){
    uint i1,i2,i3,i4;
    Real a,k;
    p->GetHarmonicImproperParameters(index,i1,i2,i3,i4,a,k);
    return boost::python::make_tuple(i1,i2,i3,i4,a,k);
  }

  boost::python::tuple WrapGetCMapParam(ost::mol::mm::TopologyPtr p, uint index){
    std::vector<Real> map;
    int dimension;
    uint i1,i2,i3,i4,i5;
    p->GetCMapParameters(index,i1,i2,i3,i4,i5,dimension,map);
    boost::python::list l = VecToList<Real>(map);
    return boost::python::make_tuple(i1,i2,i3,i4,i5,dimension,l);
  }

  boost::python::tuple WrapGetLJPairParam(ost::mol::mm::TopologyPtr p, uint index){
    uint i1,i2;
    Real s,e;
    p->GetLJPairParameters(index,i1,i2,s,e);
    return boost::python::make_tuple(i1,i2,s,e);
  }

  boost::python::tuple WrapGetDistanceConstraintParam(ost::mol::mm::TopologyPtr p,uint index){
    uint i1,i2;
    Real d;
    p->GetDistanceConstraintParameters(index,i1,i2,d);
    return boost::python::make_tuple(i1,i2,d);
  }

  boost::python::tuple WrapGetHarmonicPositionRestraintParam(ost::mol::mm::TopologyPtr p,uint index){
    uint atom_index;
    geom::Vec3 ref_pos;
    Real k;
    Real x_scale, y_scale, z_scale;
    p->GetHarmonicPositionRestraintParameters(index,atom_index,ref_pos,k,x_scale,y_scale,z_scale);
    return boost::python::make_tuple(atom_index,ref_pos,k,x_scale,y_scale,z_scale);
  }

  boost::python::tuple WrapGetHarmonicDistanceRestraintParam(ost::mol::mm::TopologyPtr p,uint index){
    uint i1,i2;
    Real l,k;
    p->GetHarmonicDistanceRestraintParameters(index,i1,i2,l,k);
    return boost::python::make_tuple(i1,i2,l,k);
  }

  void WrapSetCMapParameters(ost::mol::mm::TopologyPtr p, uint index, int dimension, boost::python::list l){
    std::vector<Real> v = ListToVec<Real>(l);
    p->SetCMapParameters(index,dimension,v);
  }

  boost::python::list GetHarmonicBondIndices(ost::mol::mm::TopologyPtr p, uint i1, uint i2){
    std::vector<uint> return_vec = p->GetHarmonicBondIndices(i1,i2);
    return VecToList<uint>(return_vec);
  }

  boost::python::list GetHarmonicBondIndicesSingleIndex(ost::mol::mm::TopologyPtr p, int i){
    std::vector<uint> return_vec = p->GetHarmonicBondIndices(i); 
    return VecToList<uint>(return_vec);
  }

  boost::python::list GetHarmonicAngleIndices(ost::mol::mm::TopologyPtr p, uint i1, uint i2, uint i3){
    std::vector<uint> return_vec = p->GetHarmonicAngleIndices(i1,i2,i3);
    return VecToList<uint>(return_vec);
  }

  boost::python::list GetHarmonicAngleIndicesSingleIndex(ost::mol::mm::TopologyPtr p, uint i){
    std::vector<uint> return_vec = p->GetHarmonicAngleIndices(i);
    return VecToList<uint>(return_vec);
  }

  boost::python::list GetUreyBradleyAngleIndices(ost::mol::mm::TopologyPtr p, uint i1, uint i2, uint i3){
    std::vector<uint> return_vec = p->GetUreyBradleyAngleIndices(i1,i2,i3);
    return VecToList<uint>(return_vec);
  }

  boost::python::list GetUreyBradleyAngleIndicesSingleIndex(ost::mol::mm::TopologyPtr p, uint i){
    std::vector<uint> return_vec = p->GetUreyBradleyAngleIndices(i);
    return VecToList<uint>(return_vec);
  }

  boost::python::list GetPeriodicDihedralIndices(ost::mol::mm::TopologyPtr p, uint i1, uint i2, uint i3, int i4){
    std::vector<uint> return_vec = p->GetPeriodicDihedralIndices(i1,i2,i3,i4);
    return VecToList<uint>(return_vec);
  }

  boost::python::list GetPeriodicDihedralIndicesSingleIndex(ost::mol::mm::TopologyPtr p, uint i){
    std::vector<uint> return_vec = p->GetPeriodicDihedralIndices(i);
    return VecToList<uint>(return_vec);
  }

  boost::python::list GetPeriodicImproperIndices(ost::mol::mm::TopologyPtr p, uint i1, uint i2, uint i3, uint i4){
    std::vector<uint> return_vec = p->GetPeriodicImproperIndices(i1,i2,i3,i4);
    return VecToList<uint>(return_vec);
  }

  boost::python::list GetPeriodicImproperIndicesSingleIndex(ost::mol::mm::TopologyPtr p, uint i){
    std::vector<uint> return_vec = p->GetPeriodicImproperIndices(i);
    return VecToList<uint>(return_vec);
  }

  boost::python::list GetHarmonicImproperIndices(ost::mol::mm::TopologyPtr p, uint i1, uint i2, uint i3, uint i4){
    std::vector<uint> return_vec = p->GetHarmonicImproperIndices(i1,i2,i3,i4);
    return VecToList<uint>(return_vec);
  }

  boost::python::list GetHarmonicImproperIndicesSingleIndex(ost::mol::mm::TopologyPtr p, uint i){
    std::vector<uint> return_vec = p->GetHarmonicImproperIndices(i);
    return VecToList<uint>(return_vec);
  }

  boost::python::list GetCMapIndices(ost::mol::mm::TopologyPtr p, uint i1, uint i2, uint i3, uint i4, uint i5){
    std::vector<uint> return_vec = p->GetCMapIndices(i1,i2,i3,i4,i5);
    return VecToList<uint>(return_vec);
  }

  boost::python::list GetCMapIndicesSingleIndex(ost::mol::mm::TopologyPtr p, uint i){
    std::vector<uint> return_vec = p->GetCMapIndices(i);
    return VecToList<uint>(return_vec);
  }

  boost::python::list GetLJPairIndicesSingleIndex(ost::mol::mm::TopologyPtr p, uint i){
    std::vector<uint> return_vec = p->GetLJPairIndices(i);
    return VecToList<uint>(return_vec);
  }

  boost::python::list GetDistanceConstraintIndicesSingleIndex(ost::mol::mm::TopologyPtr p, uint i){
    std::vector<uint> return_vec = p->GetDistanceConstraintIndices(i);
    return VecToList<uint>(return_vec);
  }

  boost::python::list GetHarmonicDistanceRestraintIndices(ost::mol::mm::TopologyPtr p, uint i1, uint i2){
    std::vector<uint> return_vec = p->GetHarmonicDistanceRestraintIndices(i1,i2);
    return VecToList<uint>(return_vec);
  }

  boost::python::list GetHarmonicDistanceRestraintIndicesSingleIndex(ost::mol::mm::TopologyPtr p, uint i){
    std::vector<uint> return_vec = p->GetHarmonicDistanceRestraintIndices(i);
    return VecToList<uint>(return_vec);
  }

  boost::python::list GetHarmonicPositionRestraintIndicesSingleIndex(ost::mol::mm::TopologyPtr p, uint i){
    std::vector<uint> return_vec = p->GetHarmonicPositionRestraintIndices(i);
    return VecToList<uint>(return_vec);
  }

}

long GetIndexA(ost::mol::mm::TopologyPtr top,ost::mol::AtomHandle& at) { return top->GetAtomIndex(at); }
long GetIndexB(ost::mol::mm::TopologyPtr top,int r_idx,const String& aname) { return top->GetAtomIndex(r_idx,aname); }


void export_Topology()
{

  class_<ost::mol::mm::TopologyCreator>("TopologyCreator",no_init)
    .def("Create",&ost::mol::mm::TopologyCreator::Create).staticmethod("Create")
  ;

  class_<ost::mol::mm::Topology>("Topology",no_init)

    .def("__init__",make_constructor(&WrapTopologyConstructor))
    .def("Save",&ost::mol::mm::Topology::Save)
    .def("Load",&ost::mol::mm::Topology::Load).staticmethod("Load")
    .def("GetEntity",&ost::mol::mm::Topology::GetEntity)
    //interaction adding functions
    .def("AddHarmonicBond",&ost::mol::mm::Topology::AddHarmonicBond)
    .def("AddHarmonicAngle",&ost::mol::mm::Topology::AddHarmonicAngle)
    .def("AddUreyBradleyAngle",&ost::mol::mm::Topology::AddUreyBradleyAngle)
    .def("AddPeriodicDihedral",&ost::mol::mm::Topology::AddPeriodicDihedral)
    .def("AddPeriodicImproper",&ost::mol::mm::Topology::AddPeriodicImproper)
    .def("AddHarmonicImproper",&ost::mol::mm::Topology::AddHarmonicImproper)
    .def("AddCMap",&WrapAddCMap)
    .def("AddLJPair",&ost::mol::mm::Topology::AddLJPair)
    .def("AddDistanceConstraint",&ost::mol::mm::Topology::AddDistanceConstraint)
    .def("AddExclusion",&ost::mol::mm::Topology::AddExclusion)
    .def("AddPositionConstraint",&ost::mol::mm::Topology::AddPositionConstraint)
    .def("ResetPositionConstraints",&ost::mol::mm::Topology::ResetPositionConstraints)
    .def("ResetExclusions",&ost::mol::mm::Topology::ResetExclusions)
    .def("AddHarmonicPositionRestraint",&ost::mol::mm::Topology::AddHarmonicPositionRestraint,(arg("index"),arg("ref_position"),arg("k"),arg("x_scale")=1.0,arg("y_scale")=1.0,arg("z_scale")=1.0))
    .def("AddHarmonicDistanceRestraint",&ost::mol::mm::Topology::AddHarmonicDistanceRestraint)
    .def("Merge",&ost::mol::mm::Topology::Merge)
    //single atom parameter getter and setter functions
    .def("SetSigmas",&WrapSetSigmas)
    .def("SetSigma",&ost::mol::mm::Topology::SetSigma)
    .def("SetEpsilons",&WrapSetEpsilons)
    .def("SetEpsilon",&ost::mol::mm::Topology::SetEpsilon)
    .def("SetGBSARadii",&WrapSetGBSARadii)
    .def("SetGBSARadius",&ost::mol::mm::Topology::SetGBSARadius)
    .def("SetOBCScalings",&WrapSetOBCScalings)
    .def("SetOBCScaling",&ost::mol::mm::Topology::SetOBCScaling)
    .def("SetCharges",&WrapSetCharges)
    .def("SetCharge",&ost::mol::mm::Topology::SetCharge)
    .def("SetMasses",&WrapSetMasses)
    .def("SetMass",&ost::mol::mm::Topology::SetMass)
    .def("GetSigmas",&WrapGetSigmas)
    .def("GetEpsilons",&WrapGetEpsilons)
    .def("GetGBSARadii",&WrapGetGBSARadii)
    .def("GetOBCScalings",&WrapGetOBCScalings)
    .def("GetCharges",&WrapGetCharges)
    .def("GetMasses",&WrapGetMasses)
    .def("GetCharge",&ost::mol::mm::Topology::GetCharge)
    .def("GetMass",&ost::mol::mm::Topology::GetMass)
    .def("GetOBCScaling",&ost::mol::mm::Topology::GetOBCScaling)
    .def("GetGBSARadius",&ost::mol::mm::Topology::GetGBSARadius)
    //getter and setter functions for nonbonded fudge parameters
    .def("SetFudgeLJ",&ost::mol::mm::Topology::SetFudgeLJ)
    .def("SetFudgeQQ",&ost::mol::mm::Topology::SetFudgeQQ)
    .def("GetFudgeLJ",&ost::mol::mm::Topology::GetFudgeLJ)
    .def("GetFudgeQQ",&ost::mol::mm::Topology::GetFudgeQQ)
    //getter functions for interaction parameters
    .def("GetHarmonicBondParameters",&WrapGetHarmonicBondParam)
    .def("GetHarmonicAngleParameters",&WrapGetHarmonicAngleParam)
    .def("GetUreyBradleyAngleParameters",&WrapGetUreyBradleyAngleParam)
    .def("GetPeriodicDihedralParameters",&WrapGetPeriodicDihedralParam)
    .def("GetPeriodicImproperParameters",&WrapGetPeriodicImproperParam)
    .def("GetHarmonicImproperParameters",&WrapGetHarmonicImproperParam)
    .def("GetCMapParameters",&WrapGetCMapParam)
    .def("GetLJPairParameters",&WrapGetLJPairParam)
    .def("GetDistanceConstraintParameters",&WrapGetDistanceConstraintParam)
    .def("GetHarmonicPositionRestraintParameters",&WrapGetHarmonicPositionRestraintParam)
    .def("GetHarmonicDistanceRestraintParameters",&WrapGetHarmonicDistanceRestraintParam)
    //setter functions for interaction parameters
    .def("SetHarmonicBondParameters",&ost::mol::mm::Topology::SetHarmonicBondParameters)
    .def("SetHarmonicAngleParameters",&ost::mol::mm::Topology::SetHarmonicAngleParameters)
    .def("SetUreyBradleyAngleParameters",&ost::mol::mm::Topology::SetUreyBradleyAngleParameters)
    .def("SetPeriodicDihedralParameters",&ost::mol::mm::Topology::SetPeriodicDihedralParameters)
    .def("SetPeriodicImproperParameters",&ost::mol::mm::Topology::SetPeriodicImproperParameters)
    .def("SetHarmonicImproperParameters",&ost::mol::mm::Topology::SetHarmonicImproperParameters)
    .def("SetCMapParameters",&WrapSetCMapParameters)
    .def("SetLJPairParameters",&ost::mol::mm::Topology::SetLJPairParameters)
    .def("SetDistanceConstraintParameters",&ost::mol::mm::Topology::SetDistanceConstraintParameters)
    .def("SetHarmonicPositionRestraintParameters",&ost::mol::mm::Topology::SetHarmonicPositionRestraintParameters)
    .def("SetHarmonicDistanceRestraintParameters",&ost::mol::mm::Topology::SetHarmonicDistanceRestraintParameters)
    //functions to find interactions certain atoms are involved in
    .def("GetHarmonicBondIndices",&GetHarmonicBondIndices)
    .def("GetHarmonicBondIndices",&GetHarmonicBondIndicesSingleIndex)
    .def("GetHarmonicAngleIndices",&GetHarmonicAngleIndices)
    .def("GetHarmonicAngleIndices",&GetHarmonicAngleIndicesSingleIndex)
    .def("GetUreyBradleyAngleIndices",&GetUreyBradleyAngleIndices)
    .def("GetUreyBradleyAngleIndices",&GetUreyBradleyAngleIndicesSingleIndex)
    .def("GetPeriodicDihedralIndices",&GetPeriodicDihedralIndices)
    .def("GetPeriodicDihedralIndices",&GetPeriodicDihedralIndicesSingleIndex)
    .def("GetPeriodicImproperIndices",&GetPeriodicImproperIndices)
    .def("GetPeriodicImproperIndices",&GetPeriodicImproperIndicesSingleIndex)
    .def("GetHarmonicImproperIndices",&GetHarmonicImproperIndices)
    .def("GetHarmonicImproperIndices",&GetHarmonicImproperIndicesSingleIndex)
    .def("GetDistanceConstraintIndex",&ost::mol::mm::Topology::GetDistanceConstraintIndex)
    .def("GetDistanceConstraintIndices",&GetDistanceConstraintIndicesSingleIndex)    
    .def("GetCMapIndices",&GetCMapIndices)
    .def("GetCMapIndices",&GetCMapIndicesSingleIndex)
    .def("GetLJPairIndex",&ost::mol::mm::Topology::GetLJPairIndex)
    .def("GetLJPairIndices",&GetLJPairIndicesSingleIndex)
    .def("GetHarmonicDistanceRestraintIndices",&GetHarmonicDistanceRestraintIndices)
    .def("GetHarmonicDistanceRestraintIndices",&GetHarmonicDistanceRestraintIndicesSingleIndex)
    .def("GetHarmonicPositionRestraintIndices",&GetHarmonicPositionRestraintIndicesSingleIndex)

    //functions to get amount of data in topology
    .def("GetNumAtoms",&ost::mol::mm::Topology::GetNumAtoms)
    .def("GetNumHarmonicBonds",&ost::mol::mm::Topology::GetNumHarmonicBonds)
    .def("GetNumHarmonicAngles",&ost::mol::mm::Topology::GetNumHarmonicAngles)
    .def("GetNumPeriodicDihedrals",&ost::mol::mm::Topology::GetNumPeriodicDihedrals)
    .def("GetNumPeriodicImpropers",&ost::mol::mm::Topology::GetNumPeriodicImpropers)
    .def("GetNumHarmonicImpropers",&ost::mol::mm::Topology::GetNumHarmonicImpropers)
    .def("GetNumCMaps",&ost::mol::mm::Topology::GetNumCMaps)
    .def("GetNumLJPairs",&ost::mol::mm::Topology::GetNumLJPairs)
    .def("GetNumDistanceConstraints",&ost::mol::mm::Topology::GetNumDistanceConstraints)
    .def("GetNumExclusions",&ost::mol::mm::Topology::GetNumExclusions)
    .def("GetNumHarmonicPositionRestraints",&ost::mol::mm::Topology::GetNumHarmonicPositionRestraints)
    .def("GetNumHarmonicDistanceRestraints",&ost::mol::mm::Topology::GetNumHarmonicDistanceRestraints)
    //get internal indices
    .def("GetAtomIndex",&GetIndexA)
    .def("GetAtomIndex",&GetIndexB)                                                               
  ;

  boost::python::register_ptr_to_python<ost::mol::mm::TopologyPtr>();
}
