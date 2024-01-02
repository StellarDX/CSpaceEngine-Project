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

option (FMT_BACKUP_SOLUTION "std::format is still not supported in som compilers while enable C++20,
    so a backup solution is needed for using format functions" OFF)
include(FmtBackupSolution)
if (FMT_BACKUP_SOLUTION)
    add_definitions(-DUSE_FMTLIB)
    FindFmtLib()
    AddFmtLib(CSEBase)
endif()

# Installations
include(Installation)
Installation(CSEBase)
