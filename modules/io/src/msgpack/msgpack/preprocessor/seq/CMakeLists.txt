add_subdirectory(detail)

set(OST_IO_MSGPACK_MSGPACK_PREPROCESSOR_SEQ_HEADERS
to_array.hpp
cat.hpp
size.hpp
fold_left.hpp
reverse.hpp
for_each_i.hpp
insert.hpp
first_n.hpp
transform.hpp
to_tuple.hpp
for_each_product.hpp
variadic_seq_to_seq.hpp
pop_back.hpp
fold_right.hpp
replace.hpp
push_back.hpp
rest_n.hpp
enum.hpp
remove.hpp
for_each.hpp
filter.hpp
pop_front.hpp
push_front.hpp
subseq.hpp
elem.hpp
seq.hpp
to_list.hpp
PARENT_SCOPE
)

set(OST_IO_MSGPACK_MSGPACK_PREPROCESSOR_SEQ_DETAIL_HEADERS
${OST_IO_MSGPACK_MSGPACK_PREPROCESSOR_SEQ_DETAIL_HEADERS}
PARENT_SCOPE
)

