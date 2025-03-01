cmake_minimum_required(VERSION 3.11...3.20)
add_subdirectory(libressl-4.0.0)
add_subdirectory(spdlog-1.15.0)
if(BUILD_BOOST_FROM_SRC)
    set(BOOST_INCLUDE_LIBRARIES
        asio
        beast 
        outcome
        stacktrace 
    )
    if (BUILD_TEST)
        list(APPEND BOOST_INCLUDE_LIBRARIES test)
    endif()
    set(BOOST_BUILD_OUPUT_DIR "${CMAKE_BINARY_DIR}/boost")
    add_subdirectory(${BOOST_SRC_PATH} ${BOOST_BUILD_OUPUT_DIR} EXCLUDE_FROM_ALL)
endif()
