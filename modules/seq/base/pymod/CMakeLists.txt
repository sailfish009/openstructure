set(OST_SEQ_PYMOD_SOURCES
  export_sequence.cc
  export_hmm.cc
  wrap_seq.cc
)

if (NOT ENABLE_STATIC)
  pymod(NAME seq CPP ${OST_SEQ_PYMOD_SOURCES} PY __init__.py)
endif()
