add_subdirectory(adaptor)
add_subdirectory(predef)
add_subdirectory(preprocessor)
add_subdirectory(v2)
add_subdirectory(v1)

set(OST_IO_MSGPACK_MSGPACK_HEADERS
meta.hpp
pack_template.h
unpack.h
version_master.h
meta_decl.hpp
parse_return.hpp
unpack.hpp
versioning.hpp
cpp_config.hpp
object.h
unpack_decl.hpp
vrefbuffer.h
cpp_config_decl.hpp
object.hpp
predef.h
unpack_define.h
vrefbuffer.hpp
fbuffer.h
object_decl.hpp
unpack_exception.hpp
vrefbuffer_decl.hpp
fbuffer.hpp
object_fwd.hpp
preprocessor.hpp
unpack_template.h
zbuffer.h
fbuffer_decl.hpp
object_fwd_decl.hpp
sbuffer.h
util.h
zbuffer.hpp
gcc_atomic.h
pack.h
sbuffer.hpp
zbuffer_decl.hpp
gcc_atomic.hpp
pack.hpp
sbuffer_decl.hpp
zone.h
iterator.hpp
pack_decl.hpp
sysdep.h
version.h
zone.hpp
iterator_decl.hpp
pack_define.h
type.hpp
version.hpp
zone_decl.hpp
PARENT_SCOPE
)

# adaptor subdir
set(OST_IO_MSGPACK_MSGPACK_ADAPTOR_HEADERS
${OST_IO_MSGPACK_MSGPACK_ADAPTOR_HEADERS}
PARENT_SCOPE
)

set(OST_IO_MSGPACK_MSGPACK_ADAPTOR_BOOST_HEADERS
${OST_IO_MSGPACK_MSGPACK_ADAPTOR_BOOST_HEADERS}
PARENT_SCOPE
)

set(OST_IO_MSGPACK_MSGPACK_ADAPTOR_TR1_HEADERS
${OST_IO_MSGPACK_MSGPACK_ADAPTOR_TR1_HEADERS}
PARENT_SCOPE
)

set(OST_IO_MSGPACK_MSGPACK_ADAPTOR_CPP11_HEADERS
${OST_IO_MSGPACK_MSGPACK_ADAPTOR_CPP11_HEADERS}
PARENT_SCOPE
)

# predef subdir
set(OST_IO_MSGPACK_MSGPACK_PREDEF_HEADERS
${OST_IO_MSGPACK_MSGPACK_PREDEF_HEADERS}
PARENT_SCOPE
)

set(OST_IO_MSGPACK_MSGPACK_PREDEF_LANGUAGE_HEADERS
${OST_IO_MSGPACK_MSGPACK_PREDEF_LANGUAGE_HEADERS}
PARENT_SCOPE
)

set(OST_IO_MSGPACK_MSGPACK_PREDEF_ARCHITECTURE_HEADERS
${OST_IO_MSGPACK_MSGPACK_PREDEF_ARCHITECTURE_HEADERS}
PARENT_SCOPE
)

set(OST_IO_MSGPACK_MSGPACK_PREDEF_ARCHITECTURE_X86_HEADERS
${OST_IO_MSGPACK_MSGPACK_PREDEF_ARCHITECTURE_X86_HEADERS}
PARENT_SCOPE
)
