set(OST_MOL_ALG_UNIT_TESTS
  test_superposition.cc
  tests.cc
  test_consistency_checks.cc
  test_pdbize.py
  test_convenient_superpose.py
  test_hbond.py
  test_accessibility.py
)

if (COMPOUND_LIB)
  list(APPEND OST_MOL_ALG_UNIT_TESTS test_qsscoring.py)
endif()

ost_unittest(MODULE mol_alg SOURCES "${OST_MOL_ALG_UNIT_TESTS}")

