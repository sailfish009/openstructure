//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2011 by the OpenStructure authors
// Copyright (C) 2003-2010 by the IPLT authors
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


#include <sstream>
#include <cassert>

#include <boost/shared_ptr.hpp>

#include <fftw3.h>
#include <QThread>

#include <ost/message.hh>
#include <ost/img/value_util.hh>
#include <ost/img/image_state/image_state_def.hh>
#include "fft.hh"
#include <ost/img/alg/fftw_helper.hh>


namespace ost { namespace img { namespace alg {
namespace {

template <class D>
String Domain2String();

template<>
String Domain2String<image_state::SpatialDomain>() {return "SPATIAL";}

template<>
String Domain2String<image_state::FrequencyDomain>() {return "FREQUENCY";}

template <class D>
String Domain2String() {return "HALF_FREQUENCY";}

int half_plus_one(int n) {
  return (n&0x1) ? (n-1)/2+1 : n/2+1;
}

} // anon ns

FFTFnc::FFTFnc(): ori_flag_(false)
{
  OST_FFTW_fftw_init_threads();
}
FFTFnc::FFTFnc(bool f): ori_flag_(f)
{
  OST_FFTW_fftw_init_threads();
}


// real spatial -> complex half-frequency
template <>
ImageStateBasePtr FFTFnc::VisitState<Real,SpatialDomain>(const RealSpatialImageState& in_state) const
{
  int rank,n[3]; // for actual fftw call
  size_t block_count,src_size,dst_size; // padding parameters

  Size in_size = in_state.GetExtent().GetSize();

  switch (in_size.GetDim()) {
  case 1: 
    rank = 1; 
    n[0] = in_size.GetWidth();
    block_count=1;
    src_size=n[0]; // real values
    dst_size=half_plus_one(n[0]); // complex values
    break;
  case 2: 
    rank = 2; 
    n[0] = in_size.GetWidth();
    n[1] = in_size.GetHeight();
    block_count=n[0];
    src_size=n[1]; // real values
    dst_size=half_plus_one(n[1]); // complex values
    break;
  case 3:
    rank = 3; 
    n[0] = in_size.GetWidth();
    n[1] = in_size.GetHeight();
    n[2] = in_size.GetDepth();
    block_count=n[0]*n[1];
    src_size=n[2]; // real values
    dst_size=half_plus_one(n[2]); // complex values
    break;
  default:
    throw(FFTException("unexpected dimension in FFT R2C"));
  }


  PixelSampling ps=in_state.GetSampling();
  ps.SetDomain(FREQUENCY);

  boost::shared_ptr<ComplexHalfFrequencyImageState> out_state(new ComplexHalfFrequencyImageState(in_size,ps));

  out_state->SetSpatialOrigin(in_state.GetSpatialOrigin());

  assert(sizeof(OST_FFTW_fftw_complex)==sizeof(Complex));
  OST_FFTW_fftw_complex* fftw_out =
reinterpret_cast<OST_FFTW_fftw_complex*>(out_state->Data().GetData());

  /*
    Since the input array is not preserved in fftw3, a copy is made, and
    then an in-place transform is performed. The real array must be padded
    correctly for this to work. 
  */
  Real* fftw_in = reinterpret_cast<Real*>(fftw_out);
  Real* in_ptr = reinterpret_cast<Real*>(in_state.Data().GetData());

  for(size_t i=0;i<block_count;i++) {
    std::copy(&in_ptr[i*src_size],&in_ptr[(i+1)*src_size],&fftw_in[i*2*dst_size]);
  }
  OST_FFTW_fftw_plan_with_nthreads(std::max<int>(1,QThread::idealThreadCount()));
  OST_FFTW_fftw_plan plan = OST_FFTW_fftw_plan_dft_r2c(rank,n,
				     fftw_in,fftw_out,
				     FFTW_ESTIMATE);

  OST_FFTW_fftw_execute(plan);
  OST_FFTW_fftw_destroy_plan(plan);

  if(ori_flag_) out_state->AdjustPhaseOrigin(in_state.GetSpatialOrigin());

  return out_state;
}

// complex half-frequency -> real spatial
template <>
ImageStateBasePtr FFTFnc::VisitState<Complex,HalfFrequencyDomain>(const ComplexHalfFrequencyImageState& in_state) const
{
  int rank,n[3]; // for actual fftw call
  size_t block_count,src_size,dst_size; // padding parameters

  Size in_size=in_state.GetLogicalExtent().GetSize();
  // copy
  ComplexHalfFrequencyImageState tmp_state(in_state);
  // correct phase origin
  if (ori_flag_) tmp_state.AdjustPhaseOrigin(-tmp_state.GetSpatialOrigin());

  switch (in_size.GetDim()) {
  case 1: 
    rank=1; 
    n[0]=in_size.GetWidth();
    block_count=0; // no un-padding for 1D necessary
    src_size=half_plus_one(n[0]); // complex values
    dst_size=n[0]; // real values
    break;
  case 2: 
    rank=2; 
    n[0]=in_size.GetWidth();
    n[1]=in_size.GetHeight();
    block_count=n[0]; 
    src_size=half_plus_one(n[1]); // complex values
    dst_size=n[1]; // real values
    // complete zero line
    for(int i=1;i<half_plus_one(n[0]);++i){
        tmp_state.Data().Value(Index(n[0]-i,0,0))=conj(tmp_state.Data().Value(Index(i,0,0)));
    }
    break;
  case 3:
    rank=3; 
    n[0]=in_size.GetWidth();
    n[1]=in_size.GetHeight();
    n[2]=in_size.GetDepth();
    block_count=n[0]*n[1];
    src_size=half_plus_one(n[2]); // complex values
    dst_size=n[2]; // real values
    // complete zero line
    for(int i=1;i<half_plus_one(n[0]);++i){
        tmp_state.Data().Value(Index(n[0]-i,0,0))=conj(tmp_state.Data().Value(Index(i,0,0)));
    }
    // complete rest of zero plane
    for(int i=0;i<n[0];++i){
      for(int j=1;j<half_plus_one(n[1]);++j){
        tmp_state.Data().Value(Index((n[0]-i)%n[0],n[1]-j,0))=conj(tmp_state.Data().Value(Index(i,j,0)));
      }
    }
    break;
  default:
    throw(FFTException("unexpected dimension in FFT C2R"));
  }


  PixelSampling ps=in_state.GetSampling();
  ps.SetDomain(SPATIAL);

  // convert (!) to real spatial image state
  Size out_size = in_state.GetLogicalExtent().GetSize();
  boost::shared_ptr<RealSpatialImageState> out_state(new RealSpatialImageState(out_size,tmp_state.Data(),ps ));
  out_state->SetSpatialOrigin(in_state.GetSpatialOrigin());


  Real* out_ptr = out_state->Data().GetData();
  Real* fftw_out = reinterpret_cast<Real*>(out_ptr);
  
  assert(sizeof(OST_FFTW_fftw_complex)==sizeof(Complex));
  OST_FFTW_fftw_plan_with_nthreads(std::max<int>(1,QThread::idealThreadCount()));
  OST_FFTW_fftw_complex* fftw_in =
reinterpret_cast<OST_FFTW_fftw_complex*>(out_ptr);

  OST_FFTW_fftw_plan plan = OST_FFTW_fftw_plan_dft_c2r(rank,n,
				     fftw_in,fftw_out,
				     FFTW_ESTIMATE);

  OST_FFTW_fftw_execute(plan);
  OST_FFTW_fftw_destroy_plan(plan);

  // un-pad (leftover from in-place transform)
  Real* dout_ptr = reinterpret_cast<Real*>(out_ptr);
  for(size_t i=1;i<block_count;++i) {
    std::copy(&dout_ptr[i*2*src_size],&dout_ptr[i*2*src_size+dst_size],&dout_ptr[i*dst_size]);
  }

  Real fac = 1.0/static_cast<Real>(out_state->GetSize().GetVolume());
  for(Real* ptr = out_state->Data().GetData(); ptr<out_state->Data().GetEnd(); ++ptr) {
    (*ptr)*=fac;
  }

  return out_state;
}


// complex spatial -> complex frequency
template <>
ImageStateBasePtr FFTFnc::VisitState<Complex,SpatialDomain>(const ComplexSpatialImageState& in_state) const
{
  Size size=in_state.GetExtent().GetSize();
  PixelSampling ps=in_state.GetSampling();
  ps.SetDomain(FREQUENCY);
  boost::shared_ptr<ComplexFrequencyImageState> out_state(new ComplexFrequencyImageState(size,ps));
  out_state->SetSpatialOrigin(in_state.GetSpatialOrigin());

  out_state->Data()=in_state.Data(); // use assignement op to copy data area to new state
  int rank = size.GetDim();
  int n[3] = {size[0],size[1],size[2]};
  int dir = FFTW_FORWARD;

  assert(sizeof(OST_FFTW_fftw_complex)==sizeof(Complex));
  OST_FFTW_fftw_complex* fftw_out =
reinterpret_cast<OST_FFTW_fftw_complex*>(out_state->Data().GetData());

  // in place transform
  OST_FFTW_fftw_plan_with_nthreads(std::max<int>(1,QThread::idealThreadCount()));
  OST_FFTW_fftw_plan plan = OST_FFTW_fftw_plan_dft(rank,n,
				 fftw_out, fftw_out, 
				 dir, 
				 FFTW_ESTIMATE);

  OST_FFTW_fftw_execute(plan);
  OST_FFTW_fftw_destroy_plan(plan);

  if(ori_flag_) out_state->AdjustPhaseOrigin(in_state.GetSpatialOrigin());

  return out_state;
}

// complex frequency -> complex spatial
template <>
ImageStateBasePtr FFTFnc::VisitState<Complex,FrequencyDomain>(const ComplexFrequencyImageState& in_state) const
{
  // copy in state
  ComplexFrequencyImageState tmp(in_state);
  if(ori_flag_) tmp.AdjustPhaseOrigin(-in_state.GetSpatialOrigin());

  Size size=in_state.GetExtent().GetSize();
  PixelSampling ps=in_state.GetSampling();
  ps.SetDomain(SPATIAL);
  // use memory located for tmp
  boost::shared_ptr<ComplexSpatialImageState> out_state(new ComplexSpatialImageState(size,tmp.Data(),ps));
  out_state->SetSpatialOrigin(in_state.GetSpatialOrigin());

  int rank = size.GetDim();
  int n[3] = {size[0],size[1],size[2]};
  int dir = FFTW_BACKWARD;

  assert(sizeof(OST_FFTW_fftw_complex)==sizeof(Complex));
  OST_FFTW_fftw_complex* fftw_out =
reinterpret_cast<OST_FFTW_fftw_complex*>(out_state->Data().GetData());

  // in place transform
  OST_FFTW_fftw_plan_with_nthreads(std::max<int>(1,QThread::idealThreadCount()));
  OST_FFTW_fftw_plan plan = OST_FFTW_fftw_plan_dft(rank,n,
				 fftw_out, fftw_out, 
				 dir, 
				 FFTW_ESTIMATE);

  OST_FFTW_fftw_execute(plan);
  OST_FFTW_fftw_destroy_plan(plan);

  Real fac = 1.0/static_cast<Real>(size.GetVolume());
  for(Complex* ptr = out_state->Data().GetData(); ptr<out_state->Data().GetEnd(); ++ptr) {
    (*ptr)*=fac;
  }

  return out_state;
}

// default impl
template <typename T, class D>
ImageStateBasePtr FFTFnc::VisitState(const ImageStateImpl<T,D>& s) const
{
  std::ostringstream ostr;
  ostr << "FFT  not supported for <" << Val2String<T>() << "," << Domain2String<D>() << ">";
  throw FFTException(ostr.str());
}

}  //alg ns

template class TEMPLATE_DEF_EXPORT image_state::ImageStateConstModOPAlgorithm<alg::FFTFnc>;

}} // img ns

