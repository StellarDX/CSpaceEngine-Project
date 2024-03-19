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
set(CsvLogLevel 1 CACHE STRING
    "Level of logging for the *.csv catalog files:
    0 - do not log
    1 - log errors and warnings
    2 - log everything")

# Print a warning into log file if some *.csv catalog is going to update/patch some *.sc catalog
set(CsvPatchWarning true CACHE STRING
"Print a warning into log file if some *.csv catalog is going to update/patch some *.sc catalog")

# Parser algorithm, default is LR(1)
set(ParserAlgorithm LR1 CACHE STRING
    "Parser algorithm, default is LR(1)")

# Disable octal parsing
set(DisableOctalNumParsing OFF CACHE STRING
    "Disable octal parsing when parsing numbers")

add_definitions(-DLOG_TIME_STAMP=${LogTimeStamp})
add_definitions(-DLOG_THRESAD_STAMP=${LogThreadStamp})
add_definitions(-DCAT_LOG_LEVEL=${CatalogLogLevel})
add_definitions(-DCSV_LOG_LEVEL=${CsvLogLevel})
add_definitions(-DCSV_PATCH_WARNING=${CsvPatchWarning})
add_definitions(-DNO_OCTAL_NUMBER=${DisableOctalNumParsing})

if (ParserAlgorithm STREQUAL "LALR1")
    add_definitions(-DSC_PARSER_LR=0)
elseif (ParserAlgorithm STREQUAL "LR1")
    add_definitions(-DSC_PARSER_LR=1)
elseif (ParserAlgorithm STREQUAL "IELR1")
    add_definitions(-DSC_PARSER_LR=2)
endif()

AddModule(Parser CSE_Base)
AddSources()
if(FMT_BACKUP_SOLUTION AND FMT_HEADERS_DIR)
    message(STATUS "${Name} : FMT_BACKUP_SOLUTION is enabled, adding fmtlib... (At ${FMT_HEADERS_DIR})")
    AddHeaders(${Name} ${FMT_HEADERS_DIR})
else()
    AddHeaders(${Name})
endif()
CreateModule()
