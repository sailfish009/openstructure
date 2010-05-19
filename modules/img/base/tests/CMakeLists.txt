set(OST_IMG_BASE_UNIT_TESTS
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


ost_unittest(img "${OST_IMG_BASE_UNIT_TESTS}")

target_link_libraries(img_tests ost_img_alg)

