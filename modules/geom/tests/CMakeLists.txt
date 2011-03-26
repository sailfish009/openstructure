set(GEOM_UNITTESTS
  test_composite3.cc
  tests.cc
  test_repr.py
)

ost_unittest(geom "${GEOM_UNITTESTS}")
