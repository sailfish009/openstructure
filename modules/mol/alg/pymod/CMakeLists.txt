set(OST_MOL_ALG_PYMOD_SOURCES
  wrap_mol_alg.cc
  export_svd_superpose.cc
)

set(OST_MOL_ALG_PYMOD_MODULES
  "__init__.py"
  views.py
)
pymod(NAME mol_alg OUTPUT_DIR ost/mol/alg CPP ${OST_MOL_ALG_PYMOD_SOURCES}
      PY ${OST_MOL_ALG_PYMOD_MODULES})

