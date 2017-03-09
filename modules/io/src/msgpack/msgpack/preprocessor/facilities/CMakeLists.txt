add_subdirectory(detail)

set(OST_IO_MSGPACK_MSGPACK_PREPROCESSOR_FACILITIES_HEADERS
empty.hpp
identity.hpp
is_1.hpp
apply.hpp
is_empty.hpp
overload.hpp
intercept.hpp
expand.hpp
is_empty_or_1.hpp
is_empty_variadic.hpp
PARENT_SCOPE
)

set(OST_IO_MSGPACK_MSGPACK_PREPROCESSOR_FACILITIES_DETAIL_HEADERS
${OST_IO_MSGPACK_MSGPACK_PREPROCESSOR_FACILITIES_DETAIL_HEADERS}
PARENT_SCOPE
)

