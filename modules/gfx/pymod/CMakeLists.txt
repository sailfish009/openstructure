set(OST_GFX_PYMOD_SOURCES
  wrap_gfx.cc
  export_scene.cc
  export_gfx_node.cc
  export_symmetry_node.cc
  export_gfx_obj.cc
  export_entity.cc
  export_surface.cc
  export_primitives.cc
  export_scene_observer.cc
  export_render_options.cc
  export_color_ops.cc
)

if (ENABLE_IPLT)
  set(OST_GFX_PYMOD_SOURCES ${OST_GFX_PYMOD_SOURCES} export_map.cc)
endif()

pymod(NAME gfx CPP ${OST_GFX_PYMOD_SOURCES} PY __init__.py)