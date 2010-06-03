set(OST_SEQ_ALG_PYMOD_SOURCES
  wrap_seq_alg.cc
  export_align.cc
)

pymod(NAME seq_alg OUTPUT_DIR ost/seq/alg 
      CPP ${OST_SEQ_ALG_PYMOD_SOURCES}
      PY __init__.py mat.py)
