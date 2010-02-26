set(OST_IPLT_ALG_PYMOD_SOURCES
wrap_alg.cc
export_filter.cc
export_normalizer.cc
export_transcendentals.cc
export_polar.cc
)

pymod(NAME iplt_alg OUTPUT_DIR ost/iplt/alg CPP ${OST_IPLT_ALG_PYMOD_SOURCES}
      PY __init__.py)
