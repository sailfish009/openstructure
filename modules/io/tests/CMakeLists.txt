set(OST_IO_UNIT_TESTS
  test_clustal.cc
  test_io_pdb.cc
  test_io_crd.cc
  test_io_sdf.cc
  test_iomanager.cc
  tests.cc
)

# missing: test_star_parser.cc
ost_unittest(io "${OST_IO_UNIT_TESTS}")
target_link_libraries(io_tests ost_mol)
target_link_libraries(io_tests ost_seq)