set(GEOM_UNITTESTS
  test_composite3.cc
  test_composite2.cc
  test_mat2.cc
  test_mat3.cc
  test_mat4.cc
  test_op2.cc
  test_op3.cc
  test_op4.cc
  test_vec2.cc
  test_vec3.cc
  test_vec4.cc
  tests.cc
)

ost_unittest(geom "${GEOM_UNITTESTS}")
