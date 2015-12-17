#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python.hpp>
#include <ost/seq/hmm.hh>
using namespace ost::seq;
using namespace boost::python;

namespace{

boost::python::list wrap_get_names(HMMDBPtr db){
  std::vector<String> v_names = db->GetNames();
  boost::python::list names;
  for(std::vector<String>::iterator i = v_names.begin();
      i != v_names.end(); ++i){
    names.append(*i);
  }
  return names;
}
  
}

void export_hmm() 
{

  enum_<HMMState>("HMMState")
    .value("HMM_MATCH", HMM_MATCH)
    .value("HMM_INSERT", HMM_INSERT)
    .value("HMM_DELETE", HMM_DELETE)
  ;

  class_<HMMColumn>("HMMColumn", init<>())
    .add_property("n_eff", &HMMColumn::GetNEff,
                 &HMMColumn::SetNEff)
    .add_property("n_eff_ins", &HMMColumn::GetNEffIns,
                  &HMMColumn::SetNEffIns)
    .add_property("n_eff_del", &HMMColumn::GetNEffDel,
                  &HMMColumn::SetNEffDel)
    .add_property("one_letter_code", &HMMColumn::GetOneLetterCode,
                  &HMMColumn::SetOneLetterCode)
    .add_property("entropy", &HMMColumn::GetEntropy)
    .def("GetFreq", &HMMColumn::GetFreq)
    .def("SetFreq", &HMMColumn::SetFreq)
    .def("GetTransitionFreq",&HMMColumn::GetTransitionFreq)
    .def("SetTransitionFreq",&HMMColumn::SetTransitionFreq)
    .def("BLOSUMNullModel", 
         &HMMColumn::BLOSUMNullModel).staticmethod("BLOSUMNullModel")
  ;

  class_<HMMColumnList>("HMMColumnList", init<>())
    .def(vector_indexing_suite<HMMColumnList>())
  ;

  class_<HMM, HMMPtr>("HMM", init<>())
    .def("__len__",&HMM::size)
    .def("Load", &HMM::Load).staticmethod("Load")
    .def("AddColumn", &HMM::push_back)
    .def("Extract", &HMM::Extract)
    .add_property("null_model", make_function(&HMM::GetNullModel,
                  return_value_policy<copy_const_reference>()))
    .add_property("columns", 
                  make_function(&HMM::GetColumns,
                  return_value_policy<copy_const_reference>()))
    .add_property("avg_entropy", &HMM::GetAverageEntropy)
    .add_property("sequence",&HMM::GetSequence)
  ;


  class_<HMMDB, HMMDBPtr>("HMMDB", init<>())
    .def("Load", &HMMDB::Load).staticmethod("Load")
    .def("Save", &HMMDB::Save)
    .def("AddHMM", &HMMDB::AddHMM)
    .def("GetHMM", &HMMDB::GetHMM)
    .def("Size", &HMMDB::size)
    .def("GetNames",&wrap_get_names)
  ;
}
