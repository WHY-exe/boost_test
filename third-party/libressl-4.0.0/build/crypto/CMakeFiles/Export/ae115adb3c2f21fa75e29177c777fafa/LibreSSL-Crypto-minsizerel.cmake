#----------------------------------------------------------------
# Generated CMake target import file for configuration "MinSizeRel".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "LibreSSL::Crypto" for configuration "MinSizeRel"
set_property(TARGET LibreSSL::Crypto APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(LibreSSL::Crypto PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_MINSIZEREL "ASM_MASM;C"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/lib/crypto.lib"
  )

list(APPEND _cmake_import_check_targets LibreSSL::Crypto )
list(APPEND _cmake_import_check_files_for_LibreSSL::Crypto "${_IMPORT_PREFIX}/lib/crypto.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
