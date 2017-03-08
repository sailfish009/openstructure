add_subdirectory(detail)
add_subdirectory(adaptor)

set(OST_IO_MSGPACK_MSGPACK_V2_HEADERS
unpack.hpp
x3_unpack.hpp
parse.hpp
iterator_decl.hpp
pack_decl.hpp
fbuffer_decl.hpp
sbuffer_decl.hpp
cpp_config_decl.hpp
object_decl.hpp
meta_decl.hpp
create_object_visitor.hpp
parse_return.hpp
object.hpp
object_fwd.hpp
null_visitor.hpp
unpack_decl.hpp
object_fwd_decl.hpp
vrefbuffer_decl.hpp
x3_parse.hpp
zbuffer_decl.hpp
zone_decl.hpp
PARENT_SCOPE
)

set(OST_IO_MSGPACK_MSGPACK_V2_DETAIL_HEADERS
${OST_IO_MSGPACK_MSGPACK_V2_DETAIL_HEADERS}
PARENT_SCOPE
)

set(OST_IO_MSGPACK_MSGPACK_V2_ADAPTOR_HEADERS
${OST_IO_MSGPACK_MSGPACK_V2_ADAPTOR_HEADERS}
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
