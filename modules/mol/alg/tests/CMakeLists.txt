set(OST_MOL_ALG_UNIT_TESTS
  test_superposition.cc
  tests.cc
  test_consistency_checks.cc
  test_pdbize.py
  test_convenient_superpose.py
)

ost_unittest(MODULE mol_alg SOURCES "${OST_MOL_ALG_UNIT_TESTS}")

