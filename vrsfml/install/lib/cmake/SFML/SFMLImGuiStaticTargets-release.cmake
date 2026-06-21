#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "SFML::ImGui" for configuration "Release"
set_property(TARGET SFML::ImGui APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(SFML::ImGui PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libsfml-imgui-s.html"
  )

list(APPEND _cmake_import_check_targets SFML::ImGui )
list(APPEND _cmake_import_check_files_for_SFML::ImGui "${_IMPORT_PREFIX}/lib/libsfml-imgui-s.html" )

# Import target "imgui" for configuration "Release"
set_property(TARGET imgui APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(imgui PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libimgui.a"
  )

list(APPEND _cmake_import_check_targets imgui )
list(APPEND _cmake_import_check_files_for_imgui "${_IMPORT_PREFIX}/lib/libimgui.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
