set(OST_BINDINGS_UNIT_TESTS
  test_msms.py
  test_clustalw.py
  test_blast.py
)

ost_unittest(MODULE bindings 
             SOURCES "${OST_BINDINGS_UNIT_TESTS}")

