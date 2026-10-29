cmake_minimum_required(VERSION 3.20)
project(MatrixCalculator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(MATRIX_ENABLE_ASAN "Check memory accesses with AddressSanitizer" OFF)
option(MATRIX_STATIC_RUNTIME "Link the Windows runtime statically" ON)

if(MSVC AND MATRIX_STATIC_RUNTIME AND NOT MATRIX_ENABLE_ASAN)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

add_library(project_warnings INTERFACE)
if(MSVC)
    target_compile_options(project_warnings INTERFACE /W4 /WX /utf-8)
else()
    target_compile_options(project_warnings INTERFACE -Wall -Wextra -Wpedantic -Werror)
endif()

if(MATRIX_ENABLE_ASAN)
    if(MSVC)
        string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
        target_compile_options(project_warnings INTERFACE /fsanitize=address /Zi)
        target_link_options(project_warnings INTERFACE /INCREMENTAL:NO)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(project_warnings INTERFACE -fsanitize=address -fno-omit-frame-pointer)
        target_link_options(project_warnings INTERFACE -fsanitize=address)
    else()
        message(FATAL_ERROR "AddressSanitizer is not configured for this compiler")
    endif()
elseif(MINGW AND MATRIX_STATIC_RUNTIME)
    target_link_options(project_warnings INTERFACE -static)
endif()

add_library(matrix STATIC src/matrix.cpp)
target_include_directories(matrix PUBLIC include)
target_link_libraries(matrix PRIVATE project_warnings)

add_executable(matrix_calculator src/main.cpp src/demo.cpp)
target_link_libraries(matrix_calculator PRIVATE matrix project_warnings)

include(CTest)
option(MATRIX_USE_GTEST "Build the Google Test suite" ON)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

find_package(Doxygen QUIET)
if(Doxygen_FOUND)
    add_custom_target(docs
        COMMAND "${DOXYGEN_EXECUTABLE}" Doxyfile
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        COMMENT "Generate Doxygen HTML documentation"
    )
endif()
