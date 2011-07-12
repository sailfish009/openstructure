set(OST_IO_UNIT_TESTS
  test_io_pdb.py
  test_clustal.cc
  test_io_pdb.cc
  test_io_crd.cc
  test_io_sdf.cc
  test_io_img.cc
  test_pir.cc
  test_iomanager.cc
  tests.cc
  test_star_parser.cc
)

ost_unittest(MODULE io
             SOURCES "${OST_IO_UNIT_TESTS}"
             LINK ost_mol ost_seq)

add_executable(test_mae_standalone test_mae_standalone.cc)
target_link_libraries(test_mae_standalone ost_mol)
target_link_libraries(test_mae_standalone ost_io)
