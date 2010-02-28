#ifndef IPLT_ALG_REAL_SPACE_CROSSCORR_HH
#define IPLT_ALG_REAL_SPACE_CROSSCORR_HH

#include <ost/img/image.hh>

namespace ost { namespace img { namespace alg {

//! Computes cross correlation of images
/*! Both input images must be Real Spatial their extent should be the same.              */
Real DLLEXPORT_IMG_ALG RealSpatialCrossCorrelation(const ConstImageHandle& image1, const ConstImageHandle& image2);

//! Computes cross correlation of images
/*! the cross correlation is computed only within the given extent.  
    
    Both input images must be Real Spatial and their overlap with the given extent should be the same.              */
Real DLLEXPORT_IMG_ALG RealSpatialCrossCorrelation(const ConstImageHandle& image1, const ConstImageHandle& image2, const img::Extent& extent);


}}} // ns

#endif
