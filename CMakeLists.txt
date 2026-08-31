cmake_minimum_required(VERSION 3.25)
project(pa3q VERSION 0.2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    set(COMPILE_WARNINGS /W4 /WX)
else()
    set(COMPILE_WARNINGS -Wall -Wextra -Wpedantic -Werror)
endif()

set(CORE_SOURCES
    src/predictor.cpp
    src/encoder.cpp
    src/compressor.cpp
    src/decompressor.cpp
)

add_executable(pa3q src/main.cpp ${CORE_SOURCES})
target_include_directories(pa3q PRIVATE src)
target_compile_options(pa3q PRIVATE ${COMPILE_WARNINGS})

set_target_properties(pa3q PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(pa3q_tests tests/tests.cpp ${CORE_SOURCES})
target_include_directories(pa3q_tests PRIVATE src)
target_link_libraries(pa3q_tests PRIVATE gtest_main)
target_compile_options(pa3q_tests PRIVATE ${COMPILE_WARNINGS})

set_target_properties(pa3q_tests PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

include(GoogleTest)
gtest_discover_tests(pa3q_tests)