# Level of logging while loading catalogs:
# 0 - do not log
# 1 - log errors and warnings, using min("CatalogLogLevel" in this config, "LogLevel" in the catalog file)
# 2 - log everything,          using min("CatalogLogLevel" in this config, "LogLevel" in the catalog file)
# 3 - log errors and warnings, ignoring "LogLevel" in the catalog file
# 4 - log everything,          ignoring "LogLevel" in the catalog file
set(CatalogLogLevel 2 CACHE STRING
    "Level of logging while loading catalogs:
    0 - do not log
    1 - log errors and warnings, using min(\"CatalogLogLevel\" in this config, \"LogLevel\" in the catalog file) (TODO)
    2 - log everything,          using min(\"CatalogLogLevel\" in this config, \"LogLevel\" in the catalog file) (TODO)
    3 - log errors and warnings, ignoring \"LogLevel\" in the catalog file
    4 - log everything,          ignoring \"LogLevel\" in the catalog file")

# Level of logging for the *.csv catalog files:
# 0 - do not log
# 1 - log errors and warnings
# 2 - log everything
# set(CsvLogLevel 1 CACHE STRING
#     "Level of logging for the *.csv catalog files:
#     0 - do not log
#     1 - log errors and warnings
#     2 - log everything")

# Print a warning into log file if some *.csv catalog is going to update/patch some *.sc catalog
# set(CsvPatchWarning true CACHE STRING
# "Print a warning into log file if some *.csv catalog is going to update/patch some *.sc catalog")

# Parser algorithm, default is LR(1)
set(ParserAlgorithm LR1 CACHE STRING
    "Parser algorithm, default is LR(1)")

add_definitions(-DLOG_TIME_STAMP=${LogTimeStamp})
add_definitions(-DLOG_THRESAD_STAMP=${LogThreadStamp})
add_definitions(-DCAT_LOG_LEVEL=${CatalogLogLevel})
#add_definitions(-DCSV_LOG_LEVEL=${CsvLogLevel})
#add_definitions(-DCSV_PATCH_WARNING=${CsvPatchWarning})

if (ParserAlgorithm STREQUAL "LALR1")
    add_definitions(-DSC_PARSER_LR=0)
elseif (ParserAlgorithm STREQUAL "LR1")
    add_definitions(-DSC_PARSER_LR=1)
elseif (ParserAlgorithm STREQUAL "IELR1")
    add_definitions(-DSC_PARSER_LR=2)
endif()

# Scan Files
file(GLOB_RECURSE Sources CONFIGURE_DEPENDS Sources/*.cc Headers/*.h)
if (ParserAlgorithm STREQUAL "LALR1")
    list(APPEND Sources "Sources/Grammar/LALR1.tbl")
elseif (ParserAlgorithm STREQUAL "LR1")
    list(APPEND Sources "Sources/Grammar/LR1.tbl")
elseif (ParserAlgorithm STREQUAL "IELR1")
    list(APPEND Sources "Sources/Grammar/IELR1.tbl")
endif()

add_library(SCStream STATIC "${Sources}")
add_library(CSE::SCStream ALIAS SCStream)

target_link_libraries(SCStream PUBLIC stdc++fs)

target_include_directories(SCStream PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Headers>
    $<INSTALL_INTERFACE:Headers>)
target_link_libraries(SCStream PUBLIC CSEBase)

# Include fmtlib
if (FMT_BACKUP_SOLUTION)
    add_definitions(-DUSE_FMTLIB)
    AddFmtLib(SCStream)
endif()

# Installations
include(Installation)
Installation(SCStream)
