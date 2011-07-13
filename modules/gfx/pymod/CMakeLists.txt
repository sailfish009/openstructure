set(OST_GFX_PYMOD_SOURCES
  wrap_gfx.cc
  export_scene.cc
  export_gfx_node.cc
  export_symmetry_node.cc
  export_gfx_obj.cc
  export_entity.cc
  export_surface.cc
  export_primitives.cc
  export_primlist.cc
  export_scene_observer.cc
  export_render_options.cc
  export_color_ops.cc
  export_glwin_base.cc
)

if (ENABLE_IMG)
  set(OST_GFX_PYMOD_SOURCES ${OST_GFX_PYMOD_SOURCES} export_map.cc)
endif()

pymod(NAME gfx CPP ${OST_GFX_PYMOD_SOURCES} PY __init__.py)

set(GRADIENT_FILE
  gradients.xml
)
copy_if_different("./" "${STAGE_DIR}/share/openstructure/scene" 
                  "${GRADIENT_FILE}" "PRESET GRADIENTS" _ost_gfx)
                  
install(FILES ${GRADIENT_FILE} DESTINATION "share/openstructure/scene")