get_ost_rev()
include(Deployment)
set(SUBST_DIC OST_VERSION="${OST_VERSION}")
add_custom_target(deploymnt ALL)
substitute(IN_FILE README.html.in OUT_FILE README.html 
               DICT ${SUBST_DIC})
add_subdirectory(win)
