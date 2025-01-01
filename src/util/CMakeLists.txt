cmake_minimum_required(VERSION 3.12...3.20)
# 这个项目输出可执行文件
project(util)

set(INCLUDE_BASE ${PROJECT_SOURCE_DIR}/include)

file(GLOB INCLUDE_FILE 
    "${INCLUDE_BASE}/${PROJECT_NAME}/*.h" 
    "${INCLUDE_BASE}/${PROJECT_NAME}/*.hpp"
)

aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC)

add_library(${PROJECT_NAME} #SHARED
    ${SRC} 
    ${INCLUDE_FILE}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${INCLUDE_BASE}
    PRIVATE
    ${INCLUDE_BASE}/${PROJECT_NAME}
)

target_link_libraries(${PROJECT_NAME} 
    PUBLIC
    Boost::asio
    Boost::log
    Boost::outcome
    Boost::log_setup
    Boost::stacktrace
)

target_precompile_headers(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/../boost_pch.hpp)

# setting up compile flags
set_property(TARGET ${PROJECT_NAME} APPEND_STRING
    PROPERTY COMPILE_FLAGS ${PROJECT_COMPILE_FLAG}
)

add_dependencies(${PROJECT_NAME}
    Boost::asio
    Boost::log
    Boost::log_setup
    Boost::stacktrace
    Boost::outcome
)

set_target_properties(${BINARY_NAME}
    PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${BIN_DIR}/../static_lib"
        ARCHIVE_OUTPUT_DIRECTORY "${BIN_DIR}"
)