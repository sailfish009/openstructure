set(OST_BASE_PYMOD_SOURCES
  wrap_base.cc
  export_logger.cc
  export_range.cc
  export_units.cc
)
if (NOT ENABLE_STATIC)
  pymod(NAME base OUTPUT_DIR ost 
        CPP ${OST_BASE_PYMOD_SOURCES} 
        PY settings.py stutil.py table.py xmlrunner.py testutils.py)
endif() 

set(SQ "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\'")
set(SUBST_DICT "ALL_ADDITIONAL_MODULES=")
if(ENABLE_GUI)
  set(SUBST_DICT "${SUBST_DICT},${SQ}gui${SQ}")
endif(ENABLE_GUI)
if(ENABLE_GFX)
  set(SUBST_DICT "${SUBST_DICT},${SQ}gfx${SQ}")
endif(ENABLE_GFX)
if(ENABLE_IMG)
  set(SUBST_DICT "${SUBST_DICT},${SQ}img${SQ}")
endif(ENABLE_IMG)
if(ENABLE_INFO)
  set(SUBST_DICT ${SUBST_DICT},${SQ}info${SQ})
endif(ENABLE_INFO)


script(NAME __init__.py
       INPUT __init__.py.in
       SUBSTITUTE "${SUBST_DICT}"
       TARGET ost_base_pymod
       OUTPUT_DIR "${LIB_DIR}/python${PYTHON_VERSION}/site-packages/ost")

