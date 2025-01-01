# cmake root file
cmake_minimum_required(VERSION 3.12...3.20)
# enter your project name heres
project(boost_test VERSION 0.1.0)
configure_file(
    version.h.in
    generated/version.h
    @ONLY
)

option(BUILD_EXAMPLE OFF)
option(BUILD_TEST "Whether compile unit test" false)
# needs to specify BOOST_SRC_PATH
option(BUILD_BOOST_FROM_SRC "Building boost libs from src" true)

if (NOT BUILD_BOOST_FROM_SRC)
    find_package(Boost 1.87 REQUIRED)
    if(NOT Boost_FOUND)
        message(FATAL "failed to find boost")
    endif()
    
    link_directories(${Boost_LIBRARY_DIRS})
    include_directories(${Boost_INCLUDE_DIRS})
endif()

# specify c++ standar
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# msvc compile option
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # set debug compile option
    if (POLICY CMP0141)
        cmake_policy(SET CMP0141 NEW)
        set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF: $< AND: $<C_COMPILER_ID:MSVC>, $<CXX_COMPILER_ID:MSVC> >, $< $<CONFIG:Debug,RelWithDebInfo>:EditAndContinue >, $< $<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
    endif()
    # active multi-thread compliation flag on msvc
    cmake_host_system_information(RESULT CPU_NUMBER_OF_LOGICAL_CORES QUERY NUMBER_OF_LOGICAL_CORES)
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP${CPU_NUMBER_OF_LOGICAL_CORES}")
    add_compile_definitions(
        _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
    )
endif()
# manually specify windows api version to 
# avoid conflict between boost.log and boost.asio
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    # macro see @ https://stackoverflow.com/a/40217291/1746503
    macro(get_win32_winnt version)
        if(CMAKE_SYSTEM_VERSION)
            set(ver ${CMAKE_SYSTEM_VERSION})
            string(REGEX MATCH "^([0-9]+).([0-9])" ver ${ver})
            string(REGEX MATCH "^([0-9]+)" verMajor ${ver})
            # Check for Windows 10, b/c we'll need to convert to hex 'A'.
            if("${verMajor}" MATCHES "10")
                set(verMajor "A")
                string(REGEX REPLACE "^([0-9]+)" ${verMajor} ver ${ver})
            endif("${verMajor}" MATCHES "10")
            # Remove all remaining '.' characters.
            string(REPLACE "." "" ver ${ver})
            # Prepend each digit with a zero.
            string(REGEX REPLACE "([0-9A-Z])" "0\\1" ver ${ver})
            set(${version} "0x${ver}")
        endif()
    endmacro()

    if(NOT DEFINED _WIN32_WINNT)
        get_win32_winnt(ver)
        set(_WIN32_WINNT ${ver})
    endif()

    message(STATUS "Set _WIN32_WINNET=${_WIN32_WINNT}")

    add_compile_definitions(_WIN32_WINNT=${_WIN32_WINNT} WIN32_LEAN_AND_MEAN)
endif()
# public include dir
set(THIRD_PARTY_INCLUDE "${PROJECT_SOURCE_DIR}/third-party")

# binary output dir
set(BIN_DIR "${CMAKE_BINARY_DIR}/bin")
set(EXAMPLE_DIR "${CMAKE_BINARY_DIR}/example")
set(TEST_BIN_DIR "${CMAKE_BINARY_DIR}/test")

add_subdirectory(src)
add_subdirectory(third-party)

if (BUILD_EXAMPLE)
    message(STATUS "${PROJECT_NAME}: building example for the project")
    add_subdirectory(example)
endif()

if(BUILD_TEST)
    message(STATUS "${PROJECT_NAME}: building unitest for the project")
    enable_testing()
    add_subdirectory(test)
    add_test(NAME unit_test COMMAND unit_test)
endif()

# copy compile_commands.json to SOURCE_DIR to enable clangd magic
if (EXISTS "${CMAKE_BINARY_DIR}/compile_commands.json")
    file(COPY "${CMAKE_BINARY_DIR}/compile_commands.json" DESTINATION "${CMAKE_SOURCE_DIR}")
endif()