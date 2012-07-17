set(OST_GFX_UNIT_TESTS
  tests.cc
  test_gfx_node.cc
  test_gfx.py
)
if (ENABLE_IMG)
  list(APPEND OST_GFX_UNIT_TESTS test_map_octree.cc)
endif()

ost_unittest(MODULE gfx
             SOURCES "${OST_GFX_UNIT_TESTS}"
             LINK ost_io)

