# todo implement get_ost_rev for git
#get_ost_rev()
include(Deployment)
set(SUBST_DIC OST_VERSION="${OST_VERSION}")
add_custom_target(deploymnt ALL)
substitute(IN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/README.html.in OUT_FILE README.html 
               DICT ${SUBST_DIC})
add_subdirectory(win)
