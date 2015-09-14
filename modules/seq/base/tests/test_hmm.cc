//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2015 by the OpenStructure authors
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
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <iostream>

#include <ost/seq/hmm.hh>

using namespace ost::seq;


BOOST_AUTO_TEST_SUITE( hmm );


BOOST_AUTO_TEST_CASE(hmm_loading)
{
  HMMPtr hmm = HMM::Load("testfiles/test_hmm.hhm");
  BOOST_CHECK_EQUAL(hmm->size(),5);
  //check sequence with different methods of getting the columns
  BOOST_CHECK_EQUAL((*hmm)[0].GetOneLetterCode(),'E');
  BOOST_CHECK_EQUAL((hmm->columns_begin()+1)->GetOneLetterCode(),'T');
  BOOST_CHECK_EQUAL(hmm->GetColumns()[2].GetOneLetterCode(),'G');
  BOOST_CHECK_EQUAL(hmm->at(3).GetOneLetterCode(),'S');
  BOOST_CHECK_EQUAL((hmm->columns_end()-1)->GetOneLetterCode(),'P');

  //we first check, whether the null model has been read correctly
  //and then randomly pick a column to check

  HMMColumn null_model = hmm->GetNullModel();

  String olc = "ACDEFGHIKLMNPQRSTVWY";

  Real correct_aa_freqs[] = {pow(2.0,-0.001*3706), pow(2.0,-0.001*5728), 
                             pow(2.0,-0.001*4211), pow(2.0,-0.001*4064), 
                             pow(2.0,-0.001*4839), pow(2.0,-0.001*3729),  
                             pow(2.0,-0.001*4763), pow(2.0,-0.001*4308), 
                             pow(2.0,-0.001*4069), pow(2.0,-0.001*3323), 
                             pow(2.0,-0.001*5509), pow(2.0,-0.001*4640),  
                             pow(2.0,-0.001*4464), pow(2.0,-0.001*4937), 
                             pow(2.0,-0.001*4285), pow(2.0,-0.001*4423), 
                             pow(2.0,-0.001*3815), pow(2.0,-0.001*3783),  
                             pow(2.0,-0.001*6325), pow(2.0,-0.001*4665)};

  for(int i = 0; i < 20; ++i){
    BOOST_CHECK_CLOSE(null_model.GetFreq(olc[i]),correct_aa_freqs[i],Real(1e-5));
  }

  //transition frequencies should all be zero in the null model
  HMMState from,to;
  for(int i = 0; i < 3; ++i){
    from = HMMState(i);
    for(int j = 0; j < 3; ++j){
      to = HMMState(j);
      BOOST_CHECK_EQUAL(null_model.GetTransitionFreq(from,to),0.0);
    }
  }

  //NEffs should also be zero
  BOOST_CHECK_EQUAL(null_model.GetNEff(),0.0);
  BOOST_CHECK_EQUAL(null_model.GetNEffIns(),0.0);
  BOOST_CHECK_EQUAL(null_model.GetNEffDel(),0.0);


  //let's check, whether a particular column has been read correctly  
  HMMColumn col = (*hmm)[2];
  BOOST_CHECK_EQUAL(col.GetOneLetterCode(),'G');
  BOOST_CHECK_CLOSE(col.GetNEff(),3.076,Real(1e-5));
  BOOST_CHECK_CLOSE(col.GetNEffIns(),1.097,Real(1e-5));
  BOOST_CHECK_CLOSE(col.GetNEffDel(),0.0,Real(1e-5));

  memset(correct_aa_freqs,0,20*sizeof(Real));
  correct_aa_freqs[0] = pow(2.0,-0.001*3676);
  correct_aa_freqs[1] = pow(2.0,-0.001*2597);
  correct_aa_freqs[5] = pow(2.0,-0.001*5359);
  correct_aa_freqs[6] = pow(2.0,-0.001*3275);
  correct_aa_freqs[11] = pow(2.0,-0.001*3292);
  correct_aa_freqs[12] = pow(2.0,-0.001*5077);
  correct_aa_freqs[13] = pow(2.0,-0.001*3826);
  correct_aa_freqs[15] = pow(2.0,-0.001*2409);
  correct_aa_freqs[16] = pow(2.0,-0.001*3733);
  correct_aa_freqs[17] = pow(2.0,-0.001*4503);
  correct_aa_freqs[19] = pow(2.0,-0.001*3070);

  for(int i = 0; i < 20; ++i){
    BOOST_CHECK_CLOSE(col.GetFreq(olc[i]),correct_aa_freqs[i],Real(1e-5));
  }

  BOOST_CHECK_CLOSE(col.GetTransitionFreq(HMM_MATCH,HMM_MATCH),pow(2.0,-0.001*46.0),Real(1e-5));
  BOOST_CHECK_CLOSE(col.GetTransitionFreq(HMM_MATCH,HMM_INSERT),pow(2.0,-0.001*4979.0),Real(1e-5));
  BOOST_CHECK_CLOSE(col.GetTransitionFreq(HMM_MATCH,HMM_DELETE),0.0,Real(1e-5));
  BOOST_CHECK_CLOSE(col.GetTransitionFreq(HMM_INSERT,HMM_MATCH),pow(2.0,-0.001*2006.0),Real(1e-5));
  BOOST_CHECK_CLOSE(col.GetTransitionFreq(HMM_INSERT,HMM_INSERT),pow(2.0,-0.001*413.0),Real(1e-5));
  BOOST_CHECK_CLOSE(col.GetTransitionFreq(HMM_DELETE,HMM_MATCH),0.0,Real(1e-5));
  BOOST_CHECK_CLOSE(col.GetTransitionFreq(HMM_DELETE,HMM_DELETE),0.0,Real(1e-5));
  //following transitions are impossible, so frequency must be zero anyway
  BOOST_CHECK_CLOSE(col.GetTransitionFreq(HMM_DELETE,HMM_INSERT),0.0,Real(1e-5));
  BOOST_CHECK_CLOSE(col.GetTransitionFreq(HMM_INSERT,HMM_DELETE),0.0,Real(1e-5));

  BOOST_CHECK_CLOSE(col.GetNEff(),3076*0.001,Real(1e-5));
  BOOST_CHECK_CLOSE(col.GetNEffIns(),1097*0.001,Real(1e-5));
  BOOST_CHECK_CLOSE(col.GetNEffDel(),0.0,Real(1e-5));


  //check, whether the entropy of the column above gets correctly calculated
  Real entropy = 0.0;
  entropy -= correct_aa_freqs[0] * log(correct_aa_freqs[0]);
  entropy -= correct_aa_freqs[1] * log(correct_aa_freqs[1]);
  entropy -= correct_aa_freqs[5] * log(correct_aa_freqs[5]);
  entropy -= correct_aa_freqs[6] * log(correct_aa_freqs[6]);
  entropy -= correct_aa_freqs[11] * log(correct_aa_freqs[11]);
  entropy -= correct_aa_freqs[12] * log(correct_aa_freqs[12]);
  entropy -= correct_aa_freqs[13] * log(correct_aa_freqs[13]);
  entropy -= correct_aa_freqs[15] * log(correct_aa_freqs[15]);
  entropy -= correct_aa_freqs[16] * log(correct_aa_freqs[16]);
  entropy -= correct_aa_freqs[17] * log(correct_aa_freqs[17]);
  entropy -= correct_aa_freqs[19] * log(correct_aa_freqs[19]);
  BOOST_CHECK_CLOSE((*hmm)[2].GetEntropy(),entropy,Real(1e-5));

  //look at avg entropy...
  BOOST_CHECK_CLOSE(hmm->GetAverageEntropy(),1.48704576,Real(1e-5));
}



BOOST_AUTO_TEST_SUITE_END();
