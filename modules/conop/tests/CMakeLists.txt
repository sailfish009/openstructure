set(OST_CONOP_UNIT_TESTS
  test_heuristic_builder.cc
  tests.cc
  test_builder.cc
  test_compound.py
)

ost_unittest(conop "${OST_CONOP_UNIT_TESTS}")

