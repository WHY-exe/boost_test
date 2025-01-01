cmake_minimum_required(VERSION 3.12...3.20)
# 这个项目输出可执行文件
project(prj_exec1)

set(INCLUDE_BASE ${PROJECT_SOURCE_DIR}/include)

file(GLOB INCLUDE_FILE 
    "${INCLUDE_BASE}/${PROJECT_NAME}/*.h" 
    "${INCLUDE_BASE}/${PROJECT_NAME}/*.hpp"
)

aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC)

add_executable(${PROJECT_NAME} 
    ${SRC} 
    ${INCLUDE_FILE}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${INCLUDE_BASE}/${PROJECT_NAME}
)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE
    util
    Boost::stacktrace
)
target_precompile_headers(${PROJECT_NAME} REUSE_FROM util)

# setting up compile flags
set_property(TARGET ${PROJECT_NAME} APPEND_STRING
    PROPERTY COMPILE_FLAGS ${PROJECT_COMPILE_FLAG}
)

add_dependencies(${PROJECT_NAME}
    util
    Boost::stacktrace
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}/${PROJECT_NAME}"
)