set(OST_CONOP_PYMOD_SOURCES
  wrap_conop.cc
  export_builder.cc
  export_compound.cc
  export_conop.cc
  export_ring_finder.cc
)

pymod(NAME conop CPP ${OST_CONOP_PYMOD_SOURCES} PY __init__.py)