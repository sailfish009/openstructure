set(OST_CONOP_UNIT_TESTS
  test_heuristic_conop.cc
  tests.cc
  test_rule_based_conop.cc 
  helper.cc
  test_cleanup.py
  test_processor.py
  test_nonstandard.py
)

if (COMPOUND_LIB)
  list(APPEND OST_CONOP_UNIT_TESTS test_compound.py)
endif()

ost_unittest(MODULE conop
             SOURCES "${OST_CONOP_UNIT_TESTS}")
