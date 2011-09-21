set(OST_CONOP_UNIT_TESTS
  test_heuristic_builder.cc
  test_rule_based_builder.cc
  tests.cc
  test_builder.cc
  test_compound.py
  test_cleanup.py
)

ost_unittest(MODULE conop
             SOURCES "${OST_CONOP_UNIT_TESTS}")

