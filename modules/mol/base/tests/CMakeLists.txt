set(OST_MOL_BASE_UNIT_TESTS
  test_atom_groups.cc
  test_chain.cc
  test_conn.cc
  test_coord_group.cc
  test_delete.cc
  test_entity.cc
  test_ics.cc
  test_iterators.cc
  test_query.cc
  test_surface.cc
  test_residue.cc
  test_view.cc
  test_view_op.cc
  tests.cc
  test_numpy.py
)

ost_unittest(MODULE mol SOURCES "${OST_MOL_BASE_UNIT_TESTS}")

# for valgrind debugging
# executable(NAME test_query_standalone SOURCES test_query_standalone.cc DEPENDS_ON mol)

