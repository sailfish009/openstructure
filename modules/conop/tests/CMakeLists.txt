set(OST_CONOP_UNIT_TESTS
  test_heuristic_builder.cc
  tests.cc
  test_rule_based_conop.cc
  test_builder.cc
  helper.cc
  test_compound.py
  test_cleanup.py
  test_nonstandard.py
)

ost_unittest(MODULE conop
             SOURCES "${OST_CONOP_UNIT_TESTS}")

