#include <ost/iplt/image_state.hh>

namespace ost { namespace iplt { namespace alg {
Real RealSpatialCrossCorrelation(const iplt::ConstImageHandle& image1, const iplt::ConstImageHandle& image2, const iplt::Extent& extent)
{
  iplt::Extent image1_overlap=Overlap(image1.GetExtent(),extent);
  iplt::Extent image2_overlap=Overlap(image2.GetExtent(),extent);

  if ( (image1.IsReal() && image2.IsReal())!=true ) {

    throw(Error("One of the input images is not of Real type"));

  } else if ( (image1.IsSpatial() && image2.IsSpatial()) !=true) {

    throw(Error("One of the input images is not in Spatial domain"));

  } else if (image1_overlap != image2_overlap) {

    throw(Error("Input images and extent are not compatible with each other"));

  } else {

    ImageStateBasePtr base_is_pointer_image1 = image1.ImageStatePtr();
    boost::shared_ptr<iplt::RealSpatialImageState> image1_pointer = boost::dynamic_pointer_cast<iplt::RealSpatialImageState>(base_is_pointer_image1);

    ImageStateBasePtr base_is_pointer_image2 = image2.ImageStatePtr();
    boost::shared_ptr<iplt::RealSpatialImageState> image2_pointer = boost::dynamic_pointer_cast<iplt::RealSpatialImageState>(base_is_pointer_image2);

    Real corr=0.0;
    Real sum1=0.0;
    Real sum2=0.0;

    // total of values
    for(ExtentIterator img_ov_it(image1_overlap);!img_ov_it.AtEnd();++img_ov_it) {
      Real v1=Val2Val<Real,Real>(image1_pointer->Value(img_ov_it));
      Real v2=Val2Val<Real,Real>(image2_pointer->Value(img_ov_it));
      corr += v1 * v2;
      sum1 += v1 * v1;
      sum2 += v2 * v2;  
    }
    
    return corr/(sqrt(sum1) * sqrt (sum2));

  }
}

Real RealSpatialCrossCorrelation(const iplt::ConstImageHandle& image1, const iplt::ConstImageHandle& image2)
{
  return RealSpatialCrossCorrelation(image1, image2, image2.GetExtent());
}

}}} // ns

