cmake_minimum_required(VERSION 3.12...3.20)
#  unit-test
project(unit_test)

file(GLOB_RECURSE files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "*.c"
    "*.cpp"
    "*.cc"            
)

foreach(SRC_FILE ${files})
    set(LINKED_LIBS  
            Boost::unit_test_framework
            util
    )
    string(REGEX REPLACE ".cpp" "" BINARY_NAME ${SRC_FILE})
    add_executable(${BINARY_NAME} ${SRC_FILE})
    set_target_properties(${BINARY_NAME}
        PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY "${TEST_BIN_DIR}/static_lib"
            ARCHIVE_OUTPUT_DIRECTORY "${TEST_BIN_DIR}/"
            RUNTIME_OUTPUT_DIRECTORY "${TEST_BIN_DIR}/"
    )
    target_link_libraries(${BINARY_NAME} PRIVATE ${LINKED_LIBS})
    add_test(
        NAME ${BINARY_NAME}\_test
        COMMAND $<TARGET_FILE:${BINARY_NAME}>
	)
    add_dependencies(${BINARY_NAME} ${LINKED_LIBS})
endforeach()