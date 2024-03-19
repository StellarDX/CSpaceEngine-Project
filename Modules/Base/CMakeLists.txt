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

option(TrigonoUseRadians "Use radians instead of degrees in trigonometry,
    will affect the behavior of trigonometric functions
    NOTICE: This maybe cause precision loss!" false)

if (TrigonoUseRadians)
    add_definitions(-DTRIGONOMETRY_USE_RADIANS)
endif()

option(EnableAdditionalTriFuncs "Enable additional trigonometric functions,
    like chords, versines and exsecants." false)

if (EnableAdditionalTriFuncs)
    add_definitions(-D_USE_FULL_TRIGONOMETRY_SYSTEM)
endif()

AddModule(Base)
AddSources()
if(FMT_BACKUP_SOLUTION AND FMT_HEADERS_DIR)
    message(STATUS "${Name} : FMT_BACKUP_SOLUTION is enabled, adding fmtlib... (At ${FMT_HEADERS_DIR})")
    AddHeaders(${Name} ${FMT_HEADERS_DIR})
else()
    AddHeaders(${Name})
endif()
CreateModule()
