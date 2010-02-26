if (USE_SHADER AND NOT APPLE) 
  set(OST_GLEW_HEADERS
     glew.h
     glxew.h
     wglew.h
     )
  stage_headers("${OST_GLEW_HEADERS}" "ost/gfx/GL" "ost_gfx_glew" ost_glew)
endif ()
