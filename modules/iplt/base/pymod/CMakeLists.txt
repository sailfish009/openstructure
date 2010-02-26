set(OST_IPLT_PYMOD_SOURCES
export_data.cc
export_data_algorithm.cc
export_extent.cc
export_function.cc
export_image_handle.cc
export_peak.cc
export_phase.cc
export_point.cc
export_progress.cc
export_raster_image.cc
export_size.cc
export_mask.cc
export_image_list.cc
export_map.cc
wrap_iplt.cc
)

pymod(NAME iplt OUTPUT_DIR ost/iplt 
      CPP ${OST_IPLT_PYMOD_SOURCES} PY __init__.py)
