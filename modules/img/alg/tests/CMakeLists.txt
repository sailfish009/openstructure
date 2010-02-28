set(IPLT_ALG_UNIT_TESTS
test_circular_mask.cc
test_clear.cc
test_conj.cc
test_convolute.cc
test_discrete_shrink.cc
test_fft.cc
test_fill.cc
test_filter.cc

test_histogram.cc
test_mirror.cc
test_negate.cc
test_power_spectrum.cc
test_shift.cc
test_stat.cc
test_transform.cc
tests.cc
)

ost_unittest(img_alg "${IPLT_ALG_UNIT_TESTS}")
