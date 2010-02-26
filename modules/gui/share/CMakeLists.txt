
set(GUI_ICONS
  icons/add_icon.png
  icons/close_icon.png
  icons/distance_icon.png
  icons/map_icon.png
  icons/rigid_body_manipulator_icon.png
  icons/selection_icon.png
  icons/show_sidebar_icon.png
  icons/site_icon.png
  icons/split_icon.png
)
add_custom_target(scene_icons ALL)
copy_if_different("./" "${STAGE_DIR}/share/openstructure/gui/icons" 
                  "${GUI_ICONS}" "ICONS_FOR_GUI" scene_icons)
install(FILES ${GUI_ICONS} DESTINATION "share/openstructure/gui/icons")
