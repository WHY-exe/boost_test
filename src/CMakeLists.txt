cmake_minimum_required(VERSION 3.12...3.20)
set(PROJECT_COMPILE_FLAG )

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    add_definitions(-DNOMINMAX)
    set(PROJECT_COMPILE_FLAG "${CMAKE_CXX_FLAGS} /W4 ")
    if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
        set(PROJECT_COMPILE_FLAG "${PROJECT_COMPILE_FLAG} /Ox")
    endif()
else()
    set(PROJECT_COMPILE_FLAG "${CMAKE_CXX_FLAGS} -rdynamic -g -Wall -Werror")
    if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        set(PROJECT_COMPILE_FLAG "${PROJECT_COMPILE_FLAG} -O0")
    else()
        set(PROJECT_COMPILE_FLAG "${PROJECT_COMPILE_FLAG} -O3")
    endif()
endif()

find_package(Threads REQUIRED)
link_libraries(${CMAKE_THREAD_LIBS_INIT})
add_compile_definitions(BOOST_STACKTRACE_LINK)

# if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
#     add_link_options(
#         -static-libgcc 
#         -static-libstdc++ 
#     )
# endif()
add_subdirectory(util)
add_subdirectory(prj_exec1)