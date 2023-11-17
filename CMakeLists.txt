cmake_minimum_required(VERSION 3.14)

project(CSpaceEngine LANGUAGES CXX DESCRIPTION "CSpaceEngine Astronomy Library")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/")

if(NOT DEFINED CMAKE_MESSAGE_CONTEXT_SHOW)
    set(CMAKE_MESSAGE_CONTEXT_SHOW TRUE)
endif()

# set default output path
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/Binaries")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/Libraries")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/Libraries")

#file(GLOB_RECURSE Sources CONFIGURE_DEPENDS Sources/*.cc Headers/*.h)
#add_library(CSpaceEngine STATIC "${Sources}")

# Scan Sub-modules
function(CSEInternalFindModules ModuleList)
    set(Result "")
    file(GLOB Dir1 "Modules/*" LIST_DIRECTORIES true)
    message(STATUS "Scanning: /Modules/*")

    foreach(Dir IN LISTS Dir1)
        message(STATUS "Scanning: ${Dir}")
        if(IS_DIRECTORY "${Dir}" AND EXISTS "${Dir}/CMakeLists.txt")
            get_filename_component(ModuleName "${Dir}" NAME)
            message(STATUS "Found Module: ${ModuleName}")
            list(APPEND Result "${ModuleName}")
        endif()
    endforeach()

    file(GLOB Dir2 "Extensions/*" LIST_DIRECTORIES true)
    message(STATUS "Also scanning: /Extensions/*")

    foreach(Dir IN LISTS Dir2)
        message(STATUS "Scanning: ${Dir}")
        if(IS_DIRECTORY "${Dir}" AND EXISTS "${Dir}/CMakeLists.txt")
            message(STATUS "Found Extension: ${Dir}")
            get_filename_component(ModuleName "${Dir}" NAME)
            list(APPEND Result "${ModuleName}")
        endif()
    endforeach()

    message(STATUS "Module Found: ${Result}")
    set(${ModuleList} "${Result}" PARENT_SCOPE)
endfunction()

if(NOT CSESubModules)
    CSEInternalFindModules(CSESubModules)
endif()
set(CSESubModules "${CSESubModules}" CACHE STRING "Submodules to build")

foreach(Module IN LISTS CSESubModules)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Modules/${Module}/CMakeLists.txt")
        message(FATAL_ERROR "Module '${Module}' cannot be found or invalid.")
    endif()
endforeach()

# Add Sub-modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Cmake")
include(ECMOptionalAddSubdirectory)

foreach(Module IN LISTS CSESubModules)
    message(STATUS "Configuring submodule '${Module}'")
    ECM_OPTIONAL_ADD_SUBDIRECTORY("${Module}")
endforeach()

# Pack configurations
configure_file(CSE-config.cmake
  "${CMAKE_BINARY_DIR}/CSE-config.cmake"
  @ONLY
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_BINARY_DIR}/CSE-version.cmake"
  VERSION 2023
  COMPATIBILITY AnyNewerVersion
)

install(
  FILES "${CMAKE_BINARY_DIR}/CSE-config.cmake"
  DESTINATION lib/cmake/CSE
)

# Rename output file
#SET_TARGET_PROPERTIES (CSpaceEngine PROPERTIES OUTPUT_NAME "CSE")

#foreach(Module IN LISTS CSESubModules)
#    message(STATUS "Loading Library '${Module}'")
#    target_link_libraries(CSpaceEngine PUBLIC ${Module})
#endforeach()

#target_include_directories(CSpaceEngine PUBLIC Headers)
#target_compile_definitions(CSpaceEngine PRIVATE CSPACEENGINE_LIBRARY)
