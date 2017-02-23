add_subdirectory(language)
add_subdirectory(architecture)
add_subdirectory(detail)
add_subdirectory(os)
add_subdirectory(platform)
add_subdirectory(hardware)
add_subdirectory(library)
add_subdirectory(compiler)
add_subdirectory(other)

set(OST_IO_MSGPACK_MSGPACK_PREDEF_HEADERS
compiler.h
make.h
other.h
version_number.h
language.h
architecture.h
os.h
platform.h
hardware.h
library.h
version.h
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
