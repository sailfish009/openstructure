set(OST_IPLT_BASE_UNIT_TESTS
test_data.cc
test_extent.cc
test_function.cc
test_image.cc
test_image_factory.cc
test_image_state.cc
test_image_state_visitor.cc
test_index.cc
test_point.cc
test_sampling.cc
test_domains.cc
test_size.cc
test_value_holder.cc
tests.cc
)


ost_unittest(iplt "${OST_IPLT_BASE_UNIT_TESTS}")
if(NOT WIN32)
target_link_libraries(iplt_tests ost_iplt_alg)
endif()
