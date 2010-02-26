#ifndef IPLT_ALG_REAL_SPACE_CROSSCORR_HH
#define IPLT_ALG_REAL_SPACE_CROSSCORR_HH

#include <ost/iplt/image.hh>

namespace ost { namespace iplt { namespace alg {

//! Computes cross correlation of images
/*! Both input images must be Real Spatial their extent should be the same.              */
Real DLLEXPORT_IPLT_ALG RealSpatialCrossCorrelation(const ConstImageHandle& image1, const ConstImageHandle& image2);

//! Computes cross correlation of images
/*! the cross correlation is computed only within the given extent.  
    
    Both input images must be Real Spatial and their overlap with the given extent should be the same.              */
Real DLLEXPORT_IPLT_ALG RealSpatialCrossCorrelation(const ConstImageHandle& image1, const ConstImageHandle& image2, const iplt::Extent& extent);


}}} // ns

#endif
