set(OST_BASE_UNIT_TESTS
  test_generic_property.cc
  test_string_ref.cc
  test_pod_vector.cc
  test_stutil.py
  test_table.py
  tests.cc
)

ost_unittest(MODULE base 
             SOURCES "${OST_BASE_UNIT_TESTS}")
target_link_libraries(base_tests ost_mol)
