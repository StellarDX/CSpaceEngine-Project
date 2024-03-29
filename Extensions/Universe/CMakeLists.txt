set(Descr "All CSpaceEngine modules")
set(SystemModule FALSE)
set(BUILD_CSE_Universe_INIT OFF)

function(IncludeModule Module)
    include("${CSE_MODULE_${Module}_LOCATION}/CMakeLists.txt")
    foreach(var CMAKE_CXX_FLAGS CMAKE_C_FLAGS) # Propagate warnings settings
        set(${var} "${${var}}" PARENT_SCOPE)
    endforeach()
endfunction()

if(NOT CSE_INITIAL_PASS)
    set(ENABLE_PRECOMPILED_HEADERS OFF CACHE INTERNAL "" FORCE)
    project(CSE)
    message(STATUS "Merging Modules...")
    foreach(Module ${CSE_MODULES_BUILD})
        set(Name ${Module})
        if(SYSTEM_MODULE_${Name})
            message(STATUS "    module ${Name}...")
            set(CMAKE_CURRENT_SOURCE_DIR "${CSE_MODULE_${Name}_LOCATION}")
            #add_subdirectory("${CSE_MODULE_${Name}_LOCATION}" ${CMAKE_CURRENT_BINARY_DIR}/${Name})
            IncludeModule(${Name})
        endif()
    endforeach()
    message(STATUS "Merging Modules... DONE")
    set(CMAKE_CURRENT_SOURCE_DIR "${CSE_MODULE_CSE_Universe_LOCATION}")
endif()

AddModule(Universe CSE_Base)

set(HeaderList)
set(SourceList)
set(LinkDeps "")
foreach(m ${CSE_MODULE_${Name}_DEPS} CSE_Universe)
    if(SYSTEM_MODULE_${m})
        list(APPEND HeaderList ${CSE_MODULE_${m}_HEADERS})
        list(APPEND SourceList ${CSE_MODULE_${m}_SOURCES})
    endif()
    if(NOT " ${CSE_MODULE_${m}_LINK_DEPS}" STREQUAL " ")
        list(APPEND LinkDeps ${CSE_MODULE_${m}_LINK_DEPS})
    endif()
endforeach()

AddSources(HEADERS ${HeaderList} SOURCES ${SourceList})

message(STATUS "${CSE_MODULE_${Name}_HEADERS}")
message(STATUS "${CSE_MODULE_${Name}_SOURCES}")
CreateModule(${LinkDeps})
