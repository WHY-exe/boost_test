#----------------------------------------------------------------
# Generated CMake target import file for configuration "MinSizeRel".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "LibreSSL::SSL" for configuration "MinSizeRel"
set_property(TARGET LibreSSL::SSL APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(LibreSSL::SSL PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_MINSIZEREL "C"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/lib/ssl.lib"
  )

list(APPEND _cmake_import_check_targets LibreSSL::SSL )
list(APPEND _cmake_import_check_files_for_LibreSSL::SSL "${_IMPORT_PREFIX}/lib/ssl.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
