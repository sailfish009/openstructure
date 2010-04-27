set(OST_BASE_SOURCES 
generic_property.cc
invalid_handle.cc
integrity_error.cc
log.cc
profile.cc
units.cc
string_ref.cc
platform.cc
message.cc
)

set(OST_BASE_HEADERS
generic_property.hh
integrity_error.hh
invalid_handle.hh
ptr_observer.hh
log.hh
profile.hh
dyn_cast.hh
stdint.hh
stdint_msc.hh
range.hh
module_config.hh
units.hh
message.hh
platform.hh
string_ref.hh
pod_vector.hh
fixed_string.hh
tri_matrix.hh
)

module(NAME base SOURCES ${OST_BASE_SOURCES} 
       HEADERS generic_property_def.hh IN_DIR export_helper ${OST_BASE_HEADERS}
       DEPENDS_ON geom
       HEADER_OUTPUT_DIR ost)

