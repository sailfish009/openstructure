set(OST_GFX_UNIT_TESTS
  tests.cc
)
if (ENABLE_IMG)
  list(APPEND OST_GFX_UNIT_TESTS test_map_octree.cc)
endif()

ost_unittest(gfx "${OST_GFX_UNIT_TESTS}")

target_link_libraries(gfx_tests ost_io)
