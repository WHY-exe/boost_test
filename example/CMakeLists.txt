cmake_minimum_required(VERSION 3.12...3.20)
project(example)
# C++ standar
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
file(GLOB_RECURSE files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "*.c"
    "*.cpp"
    "*.cc"            
)

foreach(SRC_FILE ${files})
    set(LINKED_LIBS util Boost::asio fmt::fmt)
    string(REGEX  REPLACE ".cpp" "" BINARY_NAME ${SRC_FILE})
    add_executable(${BINARY_NAME} ${SRC_FILE})
    set_target_properties(${BINARY_NAME}
        PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY "${EXAMPLE_DIR}/static_lib"
            ARCHIVE_OUTPUT_DIRECTORY "${EXAMPLE_DIR}/"
            RUNTIME_OUTPUT_DIRECTORY "${EXAMPLE_DIR}/"
    )
    target_link_libraries(${BINARY_NAME} PRIVATE ${LINKED_LIBS})
endforeach()