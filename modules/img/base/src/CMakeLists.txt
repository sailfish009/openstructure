add_subdirectory(raster_image)
add_subdirectory(image_state)


set(OST_IMG_SOURCES 
data.cc
data_algorithm.cc
data_observer.cc
extent.cc
extent_iterator.cc
function_base.cc
function_impl.cc
image_factory.cc
image_handle.cc
image_impl.cc
paste_impl.cc
peak.cc
pixel_sampling.cc
isymop.cc
phase.cc
point.cc
point_list.cc
size.cc
mask.cc
mask_base.cc
composite_mask.cc
inverted_mask.cc
polygon_mask.cc
extent_mask.cc
spherical_mask.cc
mask_op.cc
xtal_map.cc
circle_mask.cc
mask_info_convert.cc
image_list.cc
physical_units.cc
progress.cc
map.cc
)

set(OST_IMG_HEADERS
algorithm.hh
data_algorithm.hh
data.hh
xtal_map.hh
data_types.hh
isymop.hh
data_observer.hh
debug.hh
dllexport.hh
extent.hh
extent_iterator.hh
function.hh
function_base.hh
function_fw.hh
function_impl.hh
image.hh
image_factory.hh
image_fw.hh
image_handle.hh
image_handle_fw.hh
image_impl.hh
image_state.hh
image_state_fw.hh
mask_visitor.hh
normalizer_fw.hh
normalizer_impl.hh
null_data.hh
null_function.hh
observable.hh
paste_impl.hh
peak.hh
pixel_sampling.hh
phase.hh
point.hh
point_list.hh
size.hh
util.hh
util.cc
module_config.hh
value_util.hh
vecmat.hh
mask_base_fw.hh
mask_base.hh
composite_mask.hh
inverted_mask.hh
polygon_mask.hh
extent_mask.hh
spherical_mask.hh
mask_op.hh
mask.hh
circle_mask.hh
mask_info_convert.hh
image_list.hh
physical_units.hh
progress.hh
map.hh
)


foreach(fname ${OST_IMG_IMAGE_STATE_SOURCES})
  set(OST_IMG_SOURCES ${OST_IMG_SOURCES} image_state/${fname})
endforeach(fname ${OST_IMG_IMAGE_STATE_SOURCES})

foreach(fname ${OST_IMG_RASTER_IMAGE_SOURCES})
  set(OST_IMG_SOURCES ${OST_IMG_SOURCES} raster_image/${fname})
endforeach(fname ${OST_IMG_RASTER_IMAGE_SOURCES})

module(NAME img SOURCES "${OST_IMG_SOURCES}"
       HEADERS ${OST_IMG_RASTER_IMAGE_HEADERS} IN_DIR raster_image
                ${OST_IMG_IMAGE_STATE_HEADERS} IN_DIR image_state
                ${OST_IMG_HEADERS}
       DEPENDS_ON geom base info)
