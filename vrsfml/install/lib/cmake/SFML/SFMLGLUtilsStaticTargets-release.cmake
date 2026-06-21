#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "SFML::GLUtils" for configuration "Release"
set_property(TARGET SFML::GLUtils APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(SFML::GLUtils PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libsfml-glutils-s.html"
  )

list(APPEND _cmake_import_check_targets SFML::GLUtils )
list(APPEND _cmake_import_check_files_for_SFML::GLUtils "${_IMPORT_PREFIX}/lib/libsfml-glutils-s.html" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
