# ----------------------------------------------------------------------------
#  Root CMake file for CSpaceEngine
# ----------------------------------------------------------------------------

# Disable in-source builds to prevent source tree corruption.
if(" ${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "
    FATAL: In-source builds are not allowed.
           You should create a separate directory for build files.
")
endif()

cmake_minimum_required(VERSION 3.25)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)  # https://cmake.org/cmake/help/latest/variable/CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT.html
    if(NOT CMAKE_TOOLCHAIN_FILE)
        if(WIN32)
            set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation Directory" FORCE)
        else()
            set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Installation Directory" FORCE)
        endif()
    else()
        set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation Directory" FORCE)
    endif()
endif()

project(CSpaceEngine LANGUAGES CXX DESCRIPTION "CSpaceEngine Astronomy Library")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
  set(CMAKE_USE_RELATIVE_PATHS ON CACHE INTERNAL "" FORCE)
endif()

if(MSVC)
    add_compile_options(/wd4190)  # MSVC 禁用警告 4190
else()
    add_compile_options(-Wno-return-type-c-linkage)  # GCC/Clang
endif()

if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wno-ignored-attributes)
endif()

# ----------------------------------------------------------------------------
# Process subdirectories
# ----------------------------------------------------------------------------

include("CMakeRes/Algorithms.CMake")

# legacy headers
add_definitions(-D_CSE_BUILD)
add_subdirectory(Headers)

# std::format is still not supported in some compilers while enable C++20
# so a backup solution is needed for using format functions
option (EnableFMT "std::format is still not supported in some compilers while enable C++20,
    so a backup solution is needed for using format functions" OFF)
if(EnableFMT)
    option (FMT_AUTOFIND "Use 'find_package' function to find fmtlib automatically" OFF)
    if (FMT_AUTOFIND)
        set(FMT_FIND_DIRECTORY "" CACHE PATH "fmtlib directory")
        set(fmt_DIR ${FMT_FIND_DIRECTORY})
        # Find fmtlib
        message (STATUS "Searching fmtlib in directory: ${fmt_DIR}")
        find_package(fmt REQUIRED PATHS ${fmt_DIR} NO_DEFAULT_PATH)
        message (STATUS "fmtlib configs are found at: ${fmt_DIR}")
        message (STATUS "fmtlib version: ${fmt_VERSION}")
        message (STATUS "fmtlib headers at: ${FMT_FIND_DIRECTORY}/include")
        message (STATUS "fmtlib library at: ${FMT_FIND_DIRECTORY}/lib")
        set(FMT_HEADERS_DIR "${FMT_FIND_DIRECTORY}/include" CACHE STRING "fmtlib headers directory" FORCE)
        set(FMT_LIBRARY_DIR "${FMT_FIND_DIRECTORY}/lib" CACHE STRING "fmtlib library directory" FORCE)
    else()
        set(FMT_HEADERS_DIR "" CACHE PATH "fmtlib headers directory")
        set(FMT_LIBRARY_DIR "" CACHE PATH "fmtlib library directory")
    endif()
    add_definitions(-DUSE_FMTLIB)
endif()

# std::regex will cause a rediculously high deley
# so a backup solution is needed for regexes
option (EnableBoost "Some of functions requires boost library support." OFF)
if(EnableBoost)
    option (BOOST_AUTOFIND "Use 'find_package' function to find boost library automatically" OFF)
    if (BOOST_AUTOFIND)
        set(BOOST_FIND_DIRECTORY "" CACHE PATH "boost library directory")
        set(boost_DIR ${BOOST_FIND_DIRECTORY})
        # Find boost library
        message (STATUS "Searching boost library in directory: ${boost_DIR}")
        set(Boost_NO_SYSTEM_PATHS ON)
        set(Boost_DEBUG ON)
        find_package(Boost REQUIRED PATHS ${boost_DIR} NO_DEFAULT_PATH COMPONENTS regex)
        message (STATUS "boost library configs are found at: ${boost_DIR}")
        message("Boost version: ${Boost_VERSION}")
        message("Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
        message("Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
        message("Boost_LIBRARIES: ${Boost_LIBRARIES}")
        set(BOOST_ROOT_DIR ${Boost_INCLUDE_DIRS} CACHE PATH "boost library root directory, used for reduce regex delay" FORCE)
    else()
        set(BOOST_ROOT_DIR "" CACHE PATH "boost library root directory, used for reduce regex delay")
    endif()
endif()

# CSE modules
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/Binaries")
set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/Libraries")
include("CMakeRes/ModuleManager.CMake")
ModuleManagerMain()

# ----------------------------------------------------------------------------
# Finalization: generate configuration-based files
# ----------------------------------------------------------------------------

set(CSE_MODULES_CONFIGCMAKE ${CSE_MODULES_PUBLIC})

# CSEConfig.cmake
set(CSpaceEngine_INCLUDE_DIRS_CONFIGCMAKE "\"${CSE_CONFIG_FILE_INCLUDE_DIR}\" \"${CSpaceEngine_SOURCE_DIR}/Headers\"")

foreach(m ${CSE_MODULES_BUILD})
    if(EXISTS "${CSE_MODULE_${m}_LOCATION}/Headers")
        set(CSpaceEngine_INCLUDE_DIRS_CONFIGCMAKE "${CSpaceEngine_INCLUDE_DIRS_CONFIGCMAKE} \"${CSE_MODULE_${m}_LOCATION}/Headers\"")
    endif()
endforeach()

export(EXPORT CSEModules FILE "${CMAKE_BINARY_DIR}/CSEModules.cmake")

# TODO CSE Config File and Module Config files
configure_file("${CSpaceEngine_SOURCE_DIR}/CMakeRes/CSEConfig.CMake" "${CMAKE_BINARY_DIR}/CSEConfig.cmake" @ONLY)

