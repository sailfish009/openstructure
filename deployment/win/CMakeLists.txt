if(DEPLOYMENT)
#  get_ost_rev()
  include(Deployment)
  set(SUBST_DICT OST_VERSION="${OST_VERSION}")
  add_custom_target(deployment ALL)



  if(WIN32)
    script(NAME create_archive.bat INPUT create_archive.bat.in SUBSTITUTE ${SUBST_DICT} TARGET deployment)
  endif()
endif()

