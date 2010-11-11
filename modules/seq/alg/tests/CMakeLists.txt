set(OST_SEQ_ALG_UNIT_TESTS
  test_merge_pairwise_alignments.cc
  test_sequence_identity.cc
  tests.cc
  test_renumber.py
)

ost_unittest(seq_alg "${OST_SEQ_ALG_UNIT_TESTS}")

