add_subdirectory(boost)
add_subdirectory(detail)

set(OST_IO_MSGPACK_MSGPACK_V2_ADAPTOR_HEADERS
map_decl.hpp
int_decl.hpp
adaptor_base_decl.hpp
fixint_decl.hpp
raw_decl.hpp
v4raw_decl.hpp
array_ref_decl.hpp
define_decl.hpp
check_container_size_decl.hpp
msgpack_tuple_decl.hpp
ext_decl.hpp
nil_decl.hpp
size_equal_only_decl.hpp
adaptor_base.hpp
PARENT_SCOPE
)

set(OST_IO_MSGPACK_MSGPACK_V2_ADAPTOR_BOOST_HEADERS
${OST_IO_MSGPACK_MSGPACK_V2_ADAPTOR_BOOST_HEADERS}
PARENT_SCOPE
)

set(OST_IO_MSGPACK_MSGPACK_V2_ADAPTOR_DETAIL_HEADERS
${OST_IO_MSGPACK_MSGPACK_V2_ADAPTOR_DETAIL_HEADERS}
PARENT_SCOPE
)

