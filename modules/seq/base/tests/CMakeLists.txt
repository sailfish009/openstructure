set(OST_SEQ_UNIT_TESTS
  test_sequence.cc
  test_aligned_column.cc
  test_aligned_region.cc
  test_alignment.cc
  tests.cc
)

ost_unittest(seq "${OST_SEQ_UNIT_TESTS}")

