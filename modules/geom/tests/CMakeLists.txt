set(GEOM_UNITTESTS
  test_composite3.cc
  test_composite2.cc
  tests.cc
)

ost_unittest(geom "${GEOM_UNITTESTS}")
