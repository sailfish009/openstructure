#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python.hpp>
#include <ost/seq/hmm.hh>
using namespace ost::seq;
using namespace boost::python;

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
    .def("Load", &HMM::Load).staticmethod("Load")
    .def("AddColumn", &HMM::push_back)
    .add_property("null_model", make_function(&HMM::GetNullModel,
                  return_value_policy<copy_const_reference>()))
    .add_property("columns", 
                  make_function(&HMM::GetColumns,
                  return_value_policy<copy_const_reference>()))
    .add_property("avg_entropy", &HMM::GetAverageEntropy)
    .add_property("sequence",&HMM::GetSequence)
  ;
}
