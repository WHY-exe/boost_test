#----------------------------------------------------------------
# Generated CMake target import file for configuration "MinSizeRel".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "LibreSSL::TLS" for configuration "MinSizeRel"
set_property(TARGET LibreSSL::TLS APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(LibreSSL::TLS PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_MINSIZEREL "C"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/lib/tls.lib"
  )

list(APPEND _cmake_import_check_targets LibreSSL::TLS )
list(APPEND _cmake_import_check_files_for_LibreSSL::TLS "${_IMPORT_PREFIX}/lib/tls.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
