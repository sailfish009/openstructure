add_subdirectory(detail)
add_subdirectory(adaptor)

set(OST_IO_MSGPACK_MSGPACK_V1_HEADERS
unpack.hpp
version.hpp
versioning.hpp
iterator_decl.hpp
pack_decl.hpp
fbuffer_decl.hpp
sbuffer_decl.hpp
zone.hpp
unpack_exception.hpp
fbuffer.hpp
zbuffer.hpp
cpp_config_decl.hpp
object_decl.hpp
iterator.hpp
meta_decl.hpp
preprocessor.hpp
parse_return.hpp
pack.hpp
object.hpp
cpp_config.hpp
vrefbuffer.hpp
object_fwd.hpp
meta.hpp
unpack_decl.hpp
object_fwd_decl.hpp
sbuffer.hpp
vrefbuffer_decl.hpp
zbuffer_decl.hpp
zone_decl.hpp
PARENT_SCOPE
)

set(OST_IO_MSGPACK_MSGPACK_V1_DETAIL_HEADERS
${OST_IO_MSGPACK_MSGPACK_V1_DETAIL_HEADERS}
PARENT_SCOPE
)

set(OST_IO_MSGPACK_MSGPACK_V1_ADAPTOR_HEADERS
${OST_IO_MSGPACK_MSGPACK_V1_ADAPTOR_HEADERS}
PARENT_SCOPE
)

