# CSpaceEngine Debugger Defines
option(LogTimeStamp "print time stamp into log file" true)
option(LogThreadStamp "print thread ID into log file" true)

# Level of logging for the System files:
# 0 - do not log
# 1 - log errors and warnings
# 2 - log everything
set(SysLogLevel 1 CACHE STRING
    "Level of logging for the System files:
    0 - do not log
    1 - log errors and warnings
    2 - log everything")

add_definitions(-DLOG_TIME_STAMP=${LogTimeStamp})
add_definitions(-DLOG_THRESAD_STAMP=${LogThreadStamp})
add_definitions(-DSYS_LOG_LEVEL=${SysLogLevel})

# Scan Files
file(GLOB_RECURSE Sources CONFIGURE_DEPENDS Sources/*.cc Headers/*.h)
add_library(CSEBase STATIC "${Sources}")
add_library(CSE::Core ALIAS CSEBase)

target_include_directories(CSEBase PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Headers>
    $<INSTALL_INTERFACE:Headers>)

# std::format is still not supported in som compilers while enable C++20
# so a backup solution is needed for using format functions
option (FMT_BACKUP_SOLUTION "std::format is still not supported in som compilers while enable C++20,
    so a backup solution is needed for using format functions" OFF)
if (FMT_BACKUP_SOLUTION)
    add_definitions(-DUSE_FMTLIB)
    option (FMT_AUTOFIND "Use 'find_package' function to find fmtlib automatically" OFF)

    if (FMT_AUTOFIND)
        set(FMT_FIND_DIRECTORY "" CACHE STRING "fmtlib directory")
        set(fmt_DIR ${FMT_FIND_DIRECTORY})

        # Find fmtlib
        message (STATUS "Searching fmtlib in directory: ${fmt_DIR}")
        find_package(fmt REQUIRED PATHS ${fmt_DIR} NO_DEFAULT_PATH)
        message (STATUS "fmtlib configs are found at: ${fmt_DIR}")
        message (STATUS "fmtlib version: ${fmt_VERSION}")
        message (STATUS "fmtlib headers at: ${FMT_FIND_DIRECTORY}/include")
        message (STATUS "fmtlib library at: ${FMT_FIND_DIRECTORY}/lib")
        include_directories(${FMT_FIND_DIRECTORY}/include)
        target_link_directories(CSEBase PUBLIC ${FMT_FIND_DIRECTORY}/lib)
    else()
        set(FMT_HEADERS_DIR "" CACHE STRING "fmtlib headers directory")
        set(FMT_LIBRARY_DIR "" CACHE STRING "fmtlib library directory")
        include_directories(${FMT_DIR_HEADERS})
        target_link_directories(CSEBase PUBLIC ${FMT_DIR_LIBRARY})
    endif()
endif()

# Installations
install(TARGETS CSEBase EXPORT CSEBase-targets
  COMPONENT CSEBase
  LIBRARY DESTINATION Libraries
  ARCHIVE DESTINATION Libraries
  RUNTIME DESTINATION Binaries
  INCLUDES DESTINATION Headers
)

install(EXPORT CSEBase-targets
  FILE "CSE-CSEBase-targets.cmake"
  NAMESPACE CSE::
  DESTINATION lib/cmake/CSE
  COMPONENT CSEBase
)

# Pack Configurations
configure_file("CSEBase-config.cmake.in"
  "${CMAKE_BINARY_DIR}/CSE-CSEBase-config.cmake"
  @ONLY
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_BINARY_DIR}/CSE-CSEBase-version.cmake"
  VERSION 2023
  COMPATIBILITY AnyNewerVersion
)

# Installation
install(FILES
    "${CMAKE_BINARY_DIR}/CSE-CSEBase-config.cmake"
    "${CMAKE_BINARY_DIR}/CSE-CSEBase-version.cmake"
  DESTINATION lib/cmake/CSE
  COMPONENT CSEBase
)

# DESTINATION will be automatically prefixed by ${CMAKE_INSTALL_PREFIX}
install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Headers
  COMPONENT CSEBase
  DESTINATION ${CMAKE_INSTALL_PREFIX}
)
