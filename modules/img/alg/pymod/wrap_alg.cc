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

/*
  Authors: Ansgar Philippsen, Andreas Schenk
*/

#include <boost/python.hpp>
#include <ost/img/alg/auto_correlate.hh>
#include <ost/img/alg/clear.hh>
#include <ost/img/alg/conjugate.hh>
#include <ost/img/alg/correlate.hh>
#include <ost/img/alg/cross_correlate.hh>
#include <ost/img/alg/convolute.hh>
#include <ost/img/alg/fft.hh>
#include <ost/img/alg/dft.hh>
#include <ost/img/alg/fill.hh>
#include <ost/img/alg/threshold.hh>
#include <ost/img/alg/density_slice.hh>
#include <ost/img/alg/histogram.hh>
#include <ost/img/alg/negate.hh>
#include <ost/img/alg/stat.hh>
#include <ost/img/alg/stat_min_max.hh>
#include <ost/img/alg/randomize.hh>
#include <ost/img/alg/power_spectrum.hh>
#include <ost/img/alg/mask_image.hh>
#include <ost/img/alg/smooth_mask_image.hh>
#include <ost/img/alg/clip_min_max.hh>
#include <ost/img/alg/local_sigma_threshold.hh>
#include <ost/img/extent.hh>
#include <ost/img/alg/transform.hh>
#include <ost/img/alg/discrete_shrink.hh>
#include <ost/img/alg/fractional_shift.hh>
#include <ost/img/alg/line_average.hh>
#include <ost/img/alg/rscrosscorr.hh>
#include <ost/img/alg/highest_peak_search_3d.hh>

using namespace ost;
using namespace ost::img;
using namespace ost::img::alg;
using namespace boost::python;

// fw decl
void export_Filter();
void export_Normalizer();
void export_Transcendentals();
void export_Polar();

using namespace ost::img;
using namespace ost::img::alg;

namespace {
list get_histo_bins(const Histogram& hist) {
  list result;
  const std::vector<int>& bins = hist.GetBins();
  for(std::vector<int>::const_iterator it=bins.begin();it!=bins.end();++it) {
    result.append(*it);
  }
  return result;
}

void frac_shift0(FractionalShift* s) 
{
  s->SetShift();
}
  
void frac_shift1(FractionalShift* s, Real sx) 
{
  s->SetShift(sx);
}

void frac_shift2(FractionalShift* s, Real sx, Real sy) 
{
  s->SetShift(sx,sy);
}
  
void frac_shift3(FractionalShift* s, Real sx, Real sy, Real sz) 
{
  s->SetShift(sx,sy,sz);
}
  
void frac_shift4(FractionalShift* s, const Vec2& v) 
{
  s->SetShift(v);
}
  
void frac_shift5(FractionalShift* s, const Vec3& v) 
{
  s->SetShift(v);
}
  
Real real_spatial_cross_correlation_1(const ConstImageHandle& image1, 
                                      const ConstImageHandle& image2)
{
  return RealSpatialCrossCorrelation(image1,image2);
}


Real real_spatial_cross_correlation_2(const ConstImageHandle& image1, 
                                      const ConstImageHandle& image2, 
                                      const Extent& extent)
{
  return RealSpatialCrossCorrelation(image1,image2,extent);
}

} //anon ns

BOOST_PYTHON_MODULE(_img_alg)
{
  class_<AutoCorrelate, bases<ConstModOPAlgorithm> >("AutoCorrelate",init<>());

  class_<Clear,bases<ConstModIPAlgorithm> >("Clear", init<>());

  class_<Conj,bases<ConstModIPAlgorithm> >("Conj", init<>());

  class_<ExplicitConvolute,bases<ConstModOPAlgorithm> >("ExplicitConvolute", init<const ConstImageHandle&,optional<bool> >());

  class_<ExplicitCorrelate,bases<ConstModOPAlgorithm> >("ExplicitCorrelate", init<const ConstImageHandle&>());

  class_<CrossCorrelate,bases<ConstModIPAlgorithm> >("CrossCorrelate", init<const ConstImageHandle&>());

  class_<DFT,bases<ConstModOPAlgorithm> >("DFT", init<>());

  class_<FFT,bases<ConstModOPAlgorithm> >("FFT", init<>());

  class_<PowerSpectrum,bases<ConstModOPAlgorithm> >("PowerSpectrum",init<>());

  class_<Fill,bases<ConstModIPAlgorithm> >("Fill", init<const Complex&>())
    .def(init<Real>())
    ;

  class_<Threshold,bases<ConstModIPAlgorithm> >("Threshold", init<const Complex&>())
    .def(init<Real>())
    ;

  class_<DensitySlice,bases<ConstModIPAlgorithm> >("DensitySlice",
						   init<const Complex&,const Complex&>())
    .def(init<Real,Real>())
    ;

  class_<ClipMinMax,bases<ConstModIPAlgorithm> >("ClipMinMax", 
						 init<const Complex&,const Complex&>())
    .def(init<Real,Real>())
    ;

  class_<Histogram, bases<NonModAlgorithm> >("Histogram", init<int,Real,Real>() )
    .def("GetBins",get_histo_bins)
    ;

  class_<Negate,bases<ConstModIPAlgorithm> >("Negate", init<>());

  class_<Stat, bases<NonModAlgorithm> >("Stat", init<>() )
    .def("GetMean",&Stat::GetMean)
    .def("SetMean",&Stat::SetMean)
    .def("GetMinimum",&Stat::GetMinimum)
    .def("GetMinimumPosition",&Stat::GetMinimumPosition)
    .def("SetMinimum",&Stat::SetMinimum)
    .def("GetMaximum",&Stat::GetMaximum)
    .def("GetMaximumPosition",&Stat::GetMaximumPosition)
    .def("SetMaximum",&Stat::SetMaximum)
    .def("GetSum",&Stat::GetSum)
    .def("GetRootMeanSquare",&Stat::GetRootMeanSquare)
    .def("GetSkewness",&Stat::GetSkewness)
    .def("GetKurtosis",&Stat::GetKurtosis)
    .def("GetVariance",&Stat::GetVariance)
    .def("SetVariance",&Stat::SetVariance)
    .def("GetStandardDeviation",&Stat::GetStandardDeviation)
    .def("SetStandardDeviation",&Stat::SetStandardDeviation)
    .def("GetCenterOfMass",&Stat::GetCenterOfMass)
    .def(self_ns::str(self))
  ;
  class_<StatMinMax, bases<NonModAlgorithm> >("StatMinMax", init<>() )
    .def("GetMinimum",&Stat::GetMinimum)
    .def("GetMinimumPosition",&Stat::GetMinimumPosition)
    .def("SetMinimum",&Stat::SetMinimum)
    .def("GetMaximum",&Stat::GetMaximum)
    .def("GetMaximumPosition",&Stat::GetMaximumPosition)
    .def("SetMaximum",&Stat::SetMaximum)
    .def(self_ns::str(self))
  ;

  class_<Randomize,bases<ConstModIPAlgorithm> >("Randomize", init<>());
  
  class_<MaskImage, bases<ConstModIPAlgorithm> >("MaskImage",init<const MaskPtr&,optional<bool> >())
    ;

  class_<SmoothMaskImage, bases<ModIPAlgorithm> >("SmoothMaskImage",init<const MaskPtr&, Real, optional<bool> >())
    ;

  class_<LocalSigmaThreshold, bases<ConstModOPAlgorithm> >("LocalSigmaThreshold", init<int,Real>() );

  export_Filter();
  export_Normalizer();
  export_Polar();

  export_Transcendentals();

  class_<Transformation>("Transformation", init<>())
    .def(init<const Mat4&>())
    .def(init<const Mat4&, const Mat4&>())
    ;

  class_<Rotate2D, bases<Transformation> >("Rotate2D", init<Real>() );
  class_<Rotate2D90, bases<Transformation> >("Rotate2D90", init<int>() );
  class_<Scale, bases<Transformation> >("Scale", init<Real>() );
  class_<Translate, bases<Transformation> >("Translate", init<const Vec3&>() );

  class_<Transform,bases<ConstModOPAlgorithm> >("Transform",init<>())
    .def(init<const Transformation&, optional<const Vec3&> >())
    ;

  class_<Shift,bases<ConstModOPAlgorithm> >("Shift",init<>())
    .def(init<const Point&>())
    ;

  class_<Mirror,bases<ConstModOPAlgorithm> >("Mirror",init<>())
    .def(init<int>())
    ;

  class_<DiscreteShrink, bases<ConstModOPAlgorithm> >("DiscreteShrink",init<optional<const Size&> >())
    .def("SetBlocksize",&DiscreteShrink::SetBlocksize)
    .def("GetBlocksize",&DiscreteShrink::GetBlocksize)
    ;

  class_<FractionalShift, bases<ModIPAlgorithm> >("FractionalShift", init<optional <Real,Real,Real> >() )
    .def(init<const Vec3&>())
    .def("SetShift",frac_shift0)
    .def("SetShift",frac_shift1)
    .def("SetShift",frac_shift2)
    .def("SetShift",frac_shift3)
    .def("SetShift",frac_shift4)
    .def("SetShift",frac_shift5)
    .def("GetShift",&FractionalShift::GetShift)
    ;
  class_<LineAverage, bases<ConstModOPAlgorithm> >("LineAverage", init<>() )
    .def(init<unsigned int>())
    .def("GetGeometry",&LineAverage::GetGeometry)
    .def("SetGeometry",&LineAverage::SetGeometry)
  ;
  def("RealSpatialCrossCorrelation",real_spatial_cross_correlation_1);
  def("RealSpatialCrossCorrelation",real_spatial_cross_correlation_2);

  class_<HighestPeakSearch3D, bases<NonModAlgorithm> >("HighestPeakSearch3D" ,init<int,int,Real>())
    .def(init<int,int>())
    .def(init<int>())
    .def(init<>())
    .def("GetPeakList",&HighestPeakSearch3D::GetPeakList)
    .def("ClearPeakList",&HighestPeakSearch3D::ClearPeakList)
  ;

}
