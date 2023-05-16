cmake_policy(SET CMP0011 NEW)
cmake_policy(SET CMP0012 NEW)
cmake_policy(SET CMP0054 NEW)
cmake_policy(SET CMP0057 NEW) # vcpkg, pybind11 and MSVC
project(oopt-gnpy-libyang LANGUAGES CXX)
cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

include(GNUInstallDirs)
include(CTest)

if(NOT MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "-Werror ${CMAKE_CXX_FLAGS_DEBUG}")
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Woverloaded-virtual -Wimplicit-fallthrough -Wsuggest-override ${CMAKE_CXX_FLAGS}")
endif()

find_package(PkgConfig)
pkg_check_modules(LIBYANG-CPP REQUIRED IMPORTED_TARGET libyang-cpp>=1.0.0 libyang)

set(PYBIND11_FINDPYTHON ON)
find_package(Python 3.8 REQUIRED COMPONENTS Interpreter Development.Module)
find_package(pybind11 2.9.1 REQUIRED CONFIG)
message(STATUS "Using Python ${Python_VERSION}")

pybind11_add_module(oopt_gnpy_libyang ${CMAKE_CURRENT_SOURCE_DIR}/oopt-gnpy-libyang.cpp)
target_link_libraries(oopt_gnpy_libyang PRIVATE PkgConfig::LIBYANG-CPP)

function(oopt_gnpy_libyang_add_test name)
    add_test(NAME ${name} COMMAND Python::Interpreter -m pytest -vv ${CMAKE_CURRENT_SOURCE_DIR}/tests/${name}.py)
    set_property(TEST ${name} APPEND PROPERTY ENVIRONMENT
        "PYTHONPATH=$<TARGET_FILE_DIR:oopt_gnpy_libyang>"
        "CMAKE_CURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}")
endfunction()

oopt_gnpy_libyang_add_test(test_validation)

oopt_gnpy_libyang_add_test(test_context)

option(SHOW_SO_DEPENDENCIES "Show all required \"non-standard\" libraries")
option(DONT_WARN_ABOUT_SETUP_PY "magic build system hack")

if(NOT DONT_WARN_ABOUT_SETUP_PY)
    message(FATAL_ERROR "This package is designed to be built via Python's build system which invokes CMake with some magic arguments. Please read the README.md and build this via `python3 -m build --wheel`, not via invoking CMake manually.")
endif()

set(sanitizer_active OFF)
# FIXME: this just sucks. The detection is very unreliable (one could use something like
# -fsanitize=undefined,address and we are screwed), and especially clang's query for preload
# is obviously unportable because we hardcode host's architecture.
# This is super-ugly. Perhaps it would be better just to outright disable everything, but hey,
# I need to test this on my laptop where I'm using ASAN by default, and it kinda-almost-works
# there with just one patch to libyang :).
if (${CMAKE_CXX_FLAGS} MATCHES "-fsanitize=address")
    set(sanitizer_active ON)
    set(gcc_sanitizer_preload libasan.so)
    set(clang_sanitizer_preload libclang_rt.asan-x86_64.so)
endif()
if (sanitizer_active)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        execute_process(COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=${clang_sanitizer_preload}
            OUTPUT_VARIABLE LIBxSAN_FULL_PATH OUTPUT_STRIP_TRAILING_WHITESPACE)
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        execute_process(COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=${gcc_sanitizer_preload}
            OUTPUT_VARIABLE LIBxSAN_FULL_PATH OUTPUT_STRIP_TRAILING_WHITESPACE)
    else()
        message(ERROR "Cannot determine correct sanitizer library for LD_PRELOAD")
    endif()
    foreach(a_test IN LISTS TESTS)
        set_property(TEST a_test APPEND PROPERTY ENVIRONMENT
            "LD_PRELOAD=${LIBxSAN_FULL_PATH}"
            ASAN_OPTIONS=detect_leaks=0 # they look harmless, but they are annoying
            )
    endforeach()
endif()

if(SHOW_SO_DEPENDENCIES)
    if(MSVC)
        set(code_libraries [[
            list(APPEND ignored_libraries "^python")
            list(APPEND ignored_libraries "^api-ms-")
            list(APPEND ignored_libraries "^ext-ms-")
            list(APPEND ignored_libraries_post "^C:/Windows/")
            list(APPEND ignored_libraries_post "^C:\\\\Windows\\\\")
            set(extra_directories $ENV{PATH})
            ]])
    else()
        set(code_libraries [[
            list(APPEND ignored_libraries "^lib(c|dl|m|stdc\\+\\+)\\.so")
            list(APPEND ignored_libraries "^ld-linux-")
            list(APPEND ignored_libraries "^libgcc_s\\.")
            set(extra_directories "")
            ]])
    endif()
    install(CODE [[
        message(STATUS "Resolving runtime dependencies of $<TARGET_FILE:oopt_gnpy_libyang>")
        set(ignored_libraries)
        set(ignored_libraries_post)
        ]])
    install(CODE ${code_libraries})
    # Unfortunately, $<TARGET_RUNTIME_DLLS:oopt_gnpy_libyang> only lists python310.dll, so we have to pull a bigger gun.
    install(CODE [[
        file(GET_RUNTIME_DEPENDENCIES
            LIBRARIES $<TARGET_FILE:oopt_gnpy_libyang>
            RESOLVED_DEPENDENCIES_VAR libyang_cpp_deps
            UNRESOLVED_DEPENDENCIES_VAR libyang_cpp_unresolved
            CONFLICTING_DEPENDENCIES_PREFIX libyang_cpp_conflicting
            PRE_EXCLUDE_REGEXES ${ignored_libraries}
            POST_EXCLUDE_REGEXES ${ignored_libraries_post}
            DIRECTORIES ${extra_directories}
            )
        if(libyang_cpp_unresolved)
            message(STATUS " Cannot find the following required libraries to bundle them:")
            foreach(one_library IN LISTS libyang_cpp_unresolved)
                message(STATUS "  ${one_library}")
            endforeach()
        endif()
        if(libyang_cpp_conflicting_FILENAMES)
            message(STATUS " Multiple candidate libraries found for ${libyang_cpp_conflicting_FILENAMES}")
        endif()
        if(libyang_cpp_deps)
            message(STATUS " Candidates for bundling:")
            foreach(one_library IN LISTS libyang_cpp_deps)
                message(STATUS "  ${one_library}")
            endforeach()
        endif()
        ]])
endif()

install(TARGETS oopt_gnpy_libyang LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/)
