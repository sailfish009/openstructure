//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2020 by the OpenStructure authors
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


#include <boost/python.hpp>
#include <ost/db/linear_indexer.hh>
#include <ost/db/binary_container.hh>
#include <ost/db/extract_data_helper.hh>

using namespace boost::python;
using namespace ost::db;


namespace{

template<typename T>
void ListToVec(const list& l, std::vector<T>& v) {
  uint num_elements = len(l);
	v.resize(num_elements);
	for(uint i = 0; i < num_elements; ++i) {
		v[i] = extract<T>(l[i]);
	}
}


template<typename T>
void VecToList(const std::vector<T>& v, list& l) {
  l = list();
  for(typename std::vector<T>::const_iterator i = v.begin(); 
  	  i != v.end(); ++i) {
  	l.append(*i);
  }
}


void WrapAddAssembly(LinearIndexerPtr indexer,
	                   const String& name,
	                   const list& chain_names,
	                   const list& chain_lengths) {

  std::vector<String> v_chain_names;
  std::vector<uint> v_chain_lengths;
  ListToVec(chain_names, v_chain_names);
  ListToVec(chain_lengths, v_chain_lengths);
  indexer->AddAssembly(name, v_chain_names, v_chain_lengths);
}


list WrapGetAssemblies(LinearIndexerPtr indexer) {

  std::vector<String> entry_names = indexer->GetAssemblies();
  list return_list;
  VecToList(entry_names, return_list);
  return return_list;
}


list WrapGetChainNames(LinearIndexerPtr indexer, const String& name) {

  std::vector<String> chain_names = indexer->GetChainNames(name);
  list return_list;
  VecToList(chain_names, return_list);
  return return_list;
}


list WrapGetChainLengths(LinearIndexerPtr indexer, const String& name) {

  std::vector<uint> chain_lengths = indexer->GetChainLengths(name);
  list return_list;
  VecToList(chain_lengths, return_list);
  return return_list;
}


tuple WrapGetDataRangeAssembly(LinearIndexerPtr indexer,
	                             const String& name) {

  std::pair<uint64_t, uint64_t> range = indexer->GetDataRange(name);
  tuple return_tuple = make_tuple(range.first, range.second);
  return return_tuple;
}


tuple WrapGetDataRange(LinearIndexerPtr indexer,
	                     const String& name,
	                     const String& chain_name) {

  std::pair<uint64_t, uint64_t> range = indexer->GetDataRange(name, chain_name);
  tuple return_tuple = make_tuple(range.first, range.second);
  return return_tuple;
}

void WrapClearCharacterRange(LinearCharacterContainerPtr container,
                             tuple range) {

  std::pair<uint64_t, uint64_t> p_range;
  p_range.first = extract<uint64_t>(range[0]);
  p_range.second = extract<uint64_t>(range[1]);

  container->ClearRange(p_range);
}

String WrapGetCharacters(LinearCharacterContainerPtr container,
                         tuple& range) {

  std::pair<uint64_t, uint64_t> p_range;
  p_range.first = extract<uint64_t>(range[0]);
  p_range.second = extract<uint64_t>(range[1]);
  String return_string;
  container->GetCharacters(p_range, return_string);
  return return_string;
}

void WrapClearPositionRange(LinearPositionContainerPtr container,
                            tuple& range) {

  std::pair<uint64_t, uint64_t> p_range;
  p_range.first = extract<uint64_t>(range[0]);
  p_range.second = extract<uint64_t>(range[1]);
  container->ClearRange(p_range);
}

void WrapGetPositions(LinearPositionContainerPtr container,
                      tuple& range,
                      geom::Vec3List& positions) {

  std::pair<uint64_t, uint64_t> p_range;
  p_range.first = extract<uint64_t>(range[0]);
  p_range.second = extract<uint64_t>(range[1]);
  container->GetPositions(p_range, positions);  
}

// helper struct to reduce number of input parameters
struct DisCoDataContainers {

  DisCoDataContainers(const String& indexer_path,
                      const String& seqres_container_path,
                      const String& atomseq_container_path,
                      const String& position_container_path) {
    indexer = LinearIndexer::Load(indexer_path);
    seqres_container = LinearCharacterContainer::Load(seqres_container_path);
    atomseq_container = LinearCharacterContainer::Load(atomseq_container_path);
    position_container = LinearPositionContainer::Load(position_container_path);
  }

  LinearIndexerPtr indexer;
  LinearCharacterContainerPtr seqres_container;
  LinearCharacterContainerPtr atomseq_container;
  LinearPositionContainerPtr position_container;
};

// helper struct to wrap output

struct ExtractedDisCoData{
  std::vector<int> residue_numbers;
  geom::Vec3List ca_positions;
};
typedef boost::shared_ptr<ExtractedDisCoData> ExtractedDisCoDataPtr;
list WrapExtractedDisCoDataGetResNums(ExtractedDisCoDataPtr ptr) {
  list residue_numbers;
  VecToList(ptr->residue_numbers, residue_numbers); 
  return residue_numbers;
}

ExtractedDisCoDataPtr WrapExtractTemplateDataDisCo(const String& entry_name, 
                                                   const String& chain_name,
                                                   const ost::seq::AlignmentHandle& aln,
                                                   DisCoDataContainers& data_containers) {
  ExtractedDisCoDataPtr ptr(new ExtractedDisCoData);
  ost::db::ExtractTemplateDataDisCo(entry_name, chain_name, aln, 
                                    data_containers.indexer, 
                                    data_containers.seqres_container, 
                                    data_containers.atomseq_container, 
                                    data_containers.position_container, 
                                    ptr->residue_numbers,
                                    ptr->ca_positions);
  return ptr;
} 


// helper struct to reduce number of input parameters
struct GMQEDataContainers {

  GMQEDataContainers(const String& indexer_path,
                     const String& seqres_container_path,
                     const String& atomseq_container_path,
                     const String& dssp_container_path,
                     const String& n_position_container_path,
                     const String& ca_position_container_path,
                     const String& c_position_container_path,
                     const String& cb_position_container_path) {
    indexer = LinearIndexer::Load(indexer_path);
    seqres_container = LinearCharacterContainer::Load(seqres_container_path);
    atomseq_container = LinearCharacterContainer::Load(atomseq_container_path);
    dssp_container = LinearCharacterContainer::Load(dssp_container_path);
    n_position_container = LinearPositionContainer::Load(n_position_container_path);
    ca_position_container = LinearPositionContainer::Load(ca_position_container_path);
    c_position_container = LinearPositionContainer::Load(c_position_container_path);
    cb_position_container = LinearPositionContainer::Load(cb_position_container_path);
  }

  LinearIndexerPtr indexer;
  LinearCharacterContainerPtr seqres_container;
  LinearCharacterContainerPtr atomseq_container;
  LinearCharacterContainerPtr dssp_container;
  LinearPositionContainerPtr n_position_container;
  LinearPositionContainerPtr ca_position_container;
  LinearPositionContainerPtr c_position_container;
  LinearPositionContainerPtr cb_position_container;
};

// helper struct to wrap output
struct ExtractedGMQEData{
  std::vector<int> residue_numbers;
  String dssp;
  geom::Vec3List n_positions;
  geom::Vec3List ca_positions;
  geom::Vec3List c_positions;
  geom::Vec3List cb_positions; 
};
typedef boost::shared_ptr<ExtractedGMQEData> ExtractedGMQEDataPtr;
list WrapExtractedGMQEDataGetResNums(ExtractedGMQEDataPtr ptr) {
  list residue_numbers;
  VecToList(ptr->residue_numbers, residue_numbers);
  return residue_numbers;
}

ExtractedGMQEDataPtr WrapExtractTemplateDataGMQE(const String& entry_name, 
                                                 const String& chain_name,
                                                 const ost::seq::AlignmentHandle& aln,
                                                 GMQEDataContainers& data_containers) {
  ExtractedGMQEDataPtr ptr(new ExtractedGMQEData);
  ost::db::ExtractTemplateDataGMQE(entry_name, chain_name, aln, 
                                   data_containers.indexer, 
                                   data_containers.seqres_container, 
                                   data_containers.atomseq_container,
                                   data_containers.dssp_container, 
                                   data_containers.n_position_container,
                                   data_containers.ca_position_container, 
                                   data_containers.c_position_container,
                                   data_containers.cb_position_container, 
                                   ptr->residue_numbers, ptr->dssp, 
                                   ptr->n_positions, ptr->ca_positions, 
                                   ptr->c_positions, ptr->cb_positions);
  return ptr;
}

}


void export_linear_db() {

  class_<LinearIndexer, LinearIndexerPtr>("LinearIndexer", init<>())  
    .def("Load", &LinearIndexer::Load, arg("filename")).staticmethod("Load")
    .def("Save", &LinearIndexer::Save, arg("filename"))
    .def("AddAssembly", &WrapAddAssembly, (arg("name"), 
    	                                     arg("chain_names"),
    	                                     arg("chain_lengths")))
    .def("RemoveAssembly", &LinearIndexer::RemoveAssembly, (arg("name")))
    .def("GetAssemblies", &WrapGetAssemblies)
    .def("GetChainNames", &WrapGetChainNames,arg("name"))
    .def("GetChainLengths", &WrapGetChainLengths,arg("name"))
    .def("GetDataRange", &WrapGetDataRangeAssembly,arg("name"))
    .def("GetDataRange", &WrapGetDataRange,(arg("name"),
    	                                      arg("chain_name")))
    .def("GetNumResidues", &LinearIndexer::GetNumResidues)
    .def("__eq__", &LinearIndexer::operator==)
    .def("__ne__", &LinearIndexer::operator!=)
  ;


  class_<LinearCharacterContainer, 
         LinearCharacterContainerPtr>("LinearCharacterContainer", init<>())  
    .def("Load", &LinearCharacterContainer::Load, arg("filename")).staticmethod("Load")
    .def("Save", &LinearCharacterContainer::Save, arg("filename"))
    .def("ClearRange", &WrapClearCharacterRange, arg("range"))
    .def("AddCharacters", &LinearCharacterContainer::AddCharacters, arg("characters"))
    .def("GetCharacter", &LinearCharacterContainer::GetCharacter, arg("idx"))
    .def("GetCharacters", &WrapGetCharacters, arg("range"))
    .def("GetNumElements", &LinearCharacterContainer::GetNumElements)
    .def("__eq__", &LinearCharacterContainer::operator==)
    .def("__ne__", &LinearCharacterContainer::operator!=)
  ;


  class_<LinearPositionContainer,
         LinearPositionContainerPtr>("LinearPositionContainer", init<>())
    .def("Load", &LinearPositionContainer::Load, arg("filename")).staticmethod("Load")
    .def("Save", &LinearPositionContainer::Save, arg("filename"))
    .def("ClearRange", &WrapClearPositionRange, arg("range"))
    .def("AddPositions", &LinearPositionContainer::AddPositions, arg("positions"))
    .def("GetPosition", &LinearPositionContainer::GetPosition, arg("idx"), arg("pos"))
    .def("GetPositions", &WrapGetPositions, (arg("range"),arg("positions")))
    .def("GetNumElements", &LinearPositionContainer::GetNumElements)
    .def("__eq__", &LinearPositionContainer::operator==)
    .def("__ne__", &LinearPositionContainer::operator!=)
  ;

  def("ExtractValidPositions", &ExtractValidPositions, (arg("entry_name"), 
                                                        arg("chain_name"),
                                                        arg("indexer"),
                                                        arg("atomseq_container"),
                                                        arg("position_container"),
                                                        arg("seq"), arg("positions")));

  class_<DisCoDataContainers>("DisCoDataContainers", init<const String&, 
                                                          const String&, 
                                                          const String&,
                                                          const String&>())
    .def_readonly("indexer", &DisCoDataContainers::indexer)
    .def_readonly("seqres_container", &DisCoDataContainers::seqres_container)
    .def_readonly("atomseq_container", &DisCoDataContainers::atomseq_container)
    .def_readonly("ca_position_container", &DisCoDataContainers::position_container)
  ;

  class_<ExtractedDisCoData, ExtractedDisCoDataPtr>("ExtractedDisCoData", no_init)
    .add_property("residue_numbers", &WrapExtractedDisCoDataGetResNums)
    .def_readonly("ca_positions", &ExtractedDisCoData::ca_positions)
  ;

  def("ExtractTemplateDataDisCo", &WrapExtractTemplateDataDisCo, (arg("entry_name"),
                                                                  arg("chain_name"),
                                                                  arg("aln"),
                                                                  arg("data_containers")));

  class_<GMQEDataContainers>("GMQEDataContainers", init<const String&, 
                                                        const String&, 
                                                        const String&,
                                                        const String&,
                                                        const String&,
                                                        const String&,
                                                        const String&,
                                                        const String&>())
    .def_readonly("indexer", &GMQEDataContainers::indexer)
    .def_readonly("seqres_container", &GMQEDataContainers::seqres_container)
    .def_readonly("atomseq_container", &GMQEDataContainers::atomseq_container)
    .def_readonly("dssp_container", &GMQEDataContainers::dssp_container)
    .def_readonly("n_position_container", &GMQEDataContainers::n_position_container)
    .def_readonly("ca_position_container", &GMQEDataContainers::ca_position_container)
    .def_readonly("c_position_container", &GMQEDataContainers::c_position_container)
    .def_readonly("cb_position_container", &GMQEDataContainers::cb_position_container)
  ;

  class_<ExtractedGMQEData, ExtractedGMQEDataPtr>("ExtractedGMQEData", no_init)
    .add_property("residue_numbers", &WrapExtractedGMQEDataGetResNums)
    .def_readonly("dssp", &ExtractedGMQEData::dssp)
    .def_readonly("n_positions", &ExtractedGMQEData::n_positions)
    .def_readonly("ca_positions", &ExtractedGMQEData::ca_positions)
    .def_readonly("c_positions", &ExtractedGMQEData::c_positions)
    .def_readonly("cb_positions", &ExtractedGMQEData::cb_positions)
  ;

  def("ExtractTemplateDataGMQE", &WrapExtractTemplateDataGMQE, (arg("entry_name"),
                                                                arg("chain_name"),
                                                                arg("aln"),
                                                                arg("data_containers")));
}

