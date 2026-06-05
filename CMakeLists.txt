cmake_minimum_required(VERSION 3.30)
project(pa3q VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

if(MSVC)
    message(FATAL_ERROR "ERROR: MSVC not supported!")
endif()

set(CORE_SOURCES
    src/predictor.cpp
    src/encoder.cpp
    src/compressor.cpp
    src/decompressor.cpp
)

add_executable(pa3q src/main.cpp ${CORE_SOURCES})
target_include_directories(pa3q PRIVATE src)

target_compile_options(pa3q PRIVATE -Wall -Wextra -Werror)

target_compile_options(pa3q PRIVATE $<$<CONFIG:Release>:
    -O3
    -fomit-frame-pointer 
    -fno-stack-protector
    -fno-stack-limit 
    -fno-semantic-interposition 
    -march=native
>)
target_link_options(pa3q PRIVATE $<$<CONFIG:Release>:-s>)

target_compile_options(pa3q PRIVATE $<$<CONFIG:Debug>:
    -g3
    -O0
    -fsanitize=address,undefined
    -fno-omit-frame-pointer
    -fanalyzer
>)
target_link_options(pa3q PRIVATE $<$<CONFIG:Debug>:-fsanitize=address,undefined>)

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

target_compile_options(pa3q_tests PRIVATE -Wall -Wextra -Werror)
target_compile_options(pa3q_tests PRIVATE $<$<CONFIG:Release>:-O3 -fomit-frame-pointer -fno-stack-protector -march=native>)
target_compile_options(pa3q_tests PRIVATE $<$<CONFIG:Debug>:-g3 -O0 -fsanitize=address,undefined -fno-omit-frame-pointer>)
target_link_options(pa3q_tests PRIVATE $<$<CONFIG:Debug>:-fsanitize=address,undefined>)
target_link_options(pa3q_tests PRIVATE $<$<CONFIG:Release>:-s>)

set_target_properties(pa3q_tests PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

include(GoogleTest)
gtest_discover_tests(pa3q_tests)
