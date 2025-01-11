#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "LibreSSL::SSL" for configuration "RelWithDebInfo"
set_property(TARGET LibreSSL::SSL APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(LibreSSL::SSL PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "C"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/ssl.lib"
  )

list(APPEND _cmake_import_check_targets LibreSSL::SSL )
list(APPEND _cmake_import_check_files_for_LibreSSL::SSL "${_IMPORT_PREFIX}/lib/ssl.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
