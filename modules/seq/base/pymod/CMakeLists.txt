set(OST_SEQ_PYMOD_SOURCES
  export_sequence.cc
  wrap_seq.cc
)

pymod(NAME seq CPP ${OST_SEQ_PYMOD_SOURCES} PY __init__.py)
