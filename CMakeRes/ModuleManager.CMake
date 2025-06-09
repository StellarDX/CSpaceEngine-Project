# StellarDL: We need a module manager that not only can manage modules
# conviently but also merge all the modules into a single file. So CSE
# use OpenCV's module manager because only it meets the requirements.

# OpenCV is available under Apache-2.0 license reference to the following link:
# https://github.com/opencv/opencv/blob/master/LICENSE
# Module manager Adapted for CSpaceEngine by StellarDX

# ----------------------------------------------------------------------------
# Internal variables
# ----------------------------------------------------------------------------

set(CSE_MODULES_PUBLIC         "" CACHE INTERNAL "List of CSE modules marked for export")
set(CSE_MODULES_BUILD          "" CACHE INTERNAL "List of CSE modules included into the build")
set(CSE_MODULES_DISABLED_USER  "" CACHE INTERNAL "List of CSE modules explicitly disabled by user")
set(CSE_MODULES_DISABLED_AUTO  "" CACHE INTERNAL "List of CSE modules implicitly disabled due to dependencies")
set(CSE_MODULES_DISABLED_FORCE "" CACHE INTERNAL "List of CSE modules which can not be build in current configuration")
unset(CSE_SYSTEM_MODULES CACHE)

# ----------------------------------------------------------------------------
# Functions
# ----------------------------------------------------------------------------

macro(AddDependencies ModuleName)
    message(STATUS "Adding Dependencies for ${ModuleName} : ${ARGN} ")
    #we don't clean the dependencies here to allow this macro several times for every module
    foreach(d "REQUIRED" ${ARGN})
        if(d STREQUAL "REQUIRED")
            set(Deps CSE_MODULE_${ModuleName}_REQ_DEPS)
        elseif(d STREQUAL "OPTIONAL")
            set(Deps CSE_MODULE_${ModuleName}_OPT_DEPS)
        elseif(d STREQUAL "PRIVATE_REQUIRED")
            set(Deps CSE_MODULE_${ModuleName}_PRIVATE_REQ_DEPS)
        elseif(d STREQUAL "PRIVATE_OPTIONAL")
            set(Deps CSE_MODULE_${ModuleName}_PRIVATE_OPT_DEPS)
        elseif(d STREQUAL "WRAP")
            set(Deps CSE_MODULE_${ModuleName}_WRAPPERS)
        else()
            list(APPEND ${Deps} "${d}")
        endif()
    endforeach()
    unset(Deps)

    RemDuplicates(CSE_MODULE_${ModuleName}_REQ_DEPS)
    RemDuplicates(CSE_MODULE_${ModuleName}_OPT_DEPS)
    RemDuplicates(CSE_MODULE_${ModuleName}_PRIVATE_REQ_DEPS)
    RemDuplicates(CSE_MODULE_${ModuleName}_PRIVATE_OPT_DEPS)
    RemDuplicates(CSE_MODULE_${ModuleName}_WRAPPERS)

    set(CSE_MODULE_${ModuleName}_REQ_DEPS ${CSE_MODULE_${ModuleName}_REQ_DEPS}
        CACHE INTERNAL "Required dependencies of ${ModuleName} module")
    set(CSE_MODULE_${ModuleName}_OPT_DEPS ${CSE_MODULE_${ModuleName}_OPT_DEPS}
        CACHE INTERNAL "Optional dependencies of ${ModuleName} module")
    set(CSE_MODULE_${ModuleName}_PRIVATE_REQ_DEPS ${CSE_MODULE_${ModuleName}_PRIVATE_REQ_DEPS}
        CACHE INTERNAL "Required private dependencies of ${ModuleName} module")
    set(CSE_MODULE_${ModuleName}_PRIVATE_OPT_DEPS ${CSE_MODULE_${ModuleName}_PRIVATE_OPT_DEPS}
        CACHE INTERNAL "Optional private dependencies of ${ModuleName} module")
    set(CSE_MODULE_${ModuleName}_WRAPPERS ${CSE_MODULE_${ModuleName}_WRAPPERS}
        CACHE INTERNAL "List of wrappers supporting module ${ModuleName}")
endmacro()

macro(AddModule ModuleName)
    message(STATUS "Configuring Module: ${ModuleName} ${ARGN}")
    set(Name CSE_${ModuleName})
    set(MName ${ModuleName})
    # Phase 1 - collect modules info, Phase 2 - create targets
    if(CSE_INITIAL_PASS)
        #guard against redefinition
        if(";${CSE_MODULES_BUILD};${CSE_MODULES_DISABLED_USER};" MATCHES ";${Name};")
            message(FATAL_ERROR "Redefinition of the ${Name} module.
                                at:                    ${CMAKE_CURRENT_SOURCE_DIR}
                                previously defined at: ${CSE_MODULE_${Name}_LOCATION}
                              ")
        endif()

        if(NOT DEFINED Descr)
          set(Descr "The ${Name} CSE module")
        endif()

        if(NOT DEFINED BUILD_${Name}_INIT)
          set(BUILD_${Name}_INIT ON)
        endif()

        # create option to enable/disable this module
        option(BUILD_${Name} "Include ${Name} module into the CSE build" ${BUILD_${Name}_INIT})

        # add module details
        set(CSE_MODULE_${Name}_DESCRIPTION "${Descr}" CACHE INTERNAL "Brief description of ${Name} module")
        set(CSE_MODULE_${Name}_LOCATION    "${CMAKE_CURRENT_SOURCE_DIR}" CACHE INTERNAL "Location of ${Name} module sources")
        set(CSE_MODULE_${Name}_LINK_DEPS "" CACHE INTERNAL "")
        set(ADD_MODULE_ARGN ${ARGN})

        # parse list of dependencies
        if(" ${ARGV1}" STREQUAL " INTERNAL" OR " ${ARGV1}" STREQUAL " BINDINGS")
            set(CSE_MODULE_${Name}_CLASS "${ARGV1}" CACHE INTERNAL "The category of the module")
            set(ArgN ${ADD_MODULE_ARGN})
            list(REMOVE_AT ArgN 0)
            AddDependencies(${Name} ${ArgN})
            unset(ArgN)
        else()
            set(CSE_MODULE_${Name}_CLASS "PUBLIC" CACHE INTERNAL "The category of the module")
            AddDependencies(${Name} ${ADD_MODULE_ARGN})
            if(BUILD_${Name})
                set(CSE_MODULES_PUBLIC ${CSE_MODULES_PUBLIC} "${Name}" CACHE INTERNAL "List of CSE modules marked for export")
            endif()
        endif()

        # add module to the system dependencies
        if((NOT DEFINED SystemModule
            AND (NOT DEFINED SYSTEM_MODULE_${Name} OR SYSTEM_MODULE_${Name}))
            OR SystemModule)
            set(SYSTEM_MODULE_${Name} ON CACHE INTERNAL "")
            AddDependencies(CSE_Universe OPTIONAL ${Name})
        else()
            set(SYSTEM_MODULE_${Name} OFF CACHE INTERNAL "")
        endif()

        # Add module to module list
        if(BUILD_${Name})
          set(CSE_MODULES_BUILD ${CSE_MODULES_BUILD} "${Name}" CACHE INTERNAL "List of CSE modules included into the build")
        else()
          set(CSE_MODULES_DISABLED_USER ${CSE_MODULES_DISABLED_USER} "${Name}" CACHE INTERNAL "List of CSE modules explicitly disabled by user")
        endif()

        return()
    else()
        set(CSE_MODULE_${Name}_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}" CACHE INTERNAL "")
        if(NOT BUILD_${Name})
            return() # extra protection from redefinition
        endif()
        if(NOT SYSTEM_MODULE_${Name} OR NOT ${BUILD_CSE_Universe})
            if (NOT ${Name} STREQUAL CSE_Universe)
                project(${Name})
            endif()
        endif()
    endif()
endmacro()

function(AddSourceGroup group)
    if(BUILD_CSE_Universe AND SYSTEM_MODULE_${Name})
        set(group "${Name}\\${group}")
    endif()
    cmake_parse_arguments(SG "" "DIRBASE" "GLOB;GLOB_RECURSE;FILES" ${ARGN})
    message(STATUS "Creating source group ${group} : ${ARGN}")
    set(files "")
    if(SG_FILES)
        list(APPEND files ${SG_FILES})
    endif()
    if(SG_GLOB)
        file(GLOB srcs ${SG_GLOB})
        list(APPEND files ${srcs})
    endif()
    if(SG_GLOB_RECURSE)
        file(GLOB_RECURSE srcs ${SG_GLOB_RECURSE})
        list(APPEND files ${srcs})
    endif()
    if(SG_DIRBASE)
        foreach(f ${files})
            file(RELATIVE_PATH fpart "${SG_DIRBASE}" "${f}")
            if(fpart MATCHES "^\\.\\.")
                message(AUTHOR_WARNING "Can't detect subpath for AddSourceGroup command: Group=${group} FILE=${f} DIRBASE=${SG_DIRBASE}")
                set(fpart "")
            else()
                get_filename_component(fpart "${fpart}" PATH)
                if(fpart)
                    set(fpart "/${fpart}") # add '/'
                    string(REPLACE "/" "\\" fpart "${fpart}")
                endif()
            endif()
            source_group("${group}${fpart}" FILES ${f})
        endforeach()
    else()
        source_group(${group} FILES ${files})
    endif()
endfunction()

macro(SetSources)
    message(STATUS "Adding sources : ${ARGN} ")

    set(CSE_MODULE_${Name}_HEADERS "")
    set(CSE_MODULE_${Name}_SOURCES "")

    foreach(f "HEADERS" ${ARGN})
        if(f STREQUAL "HEADERS" OR f STREQUAL "SOURCES")
            set(Group "CSE_MODULE_${Name}_${f}")
        else()
            list(APPEND ${Group} "${f}")
        endif()
    endforeach()

    if(CSE_MODULE_${Name}_SOURCES_DISPATCHED)
        list(APPEND CSE_MODULE_${Name}_SOURCES ${CSE_MODULE_${Name}_SOURCES_DISPATCHED})
    endif()

    # use full paths for module to be independent from the module location
    Canonical(CSE_MODULE_${Name}_HEADERS)

    set(CSE_MODULE_${Name}_HEADERS ${CSE_MODULE_${Name}_HEADERS} CACHE INTERNAL "List of header files for ${Name}")
    set(CSE_MODULE_${Name}_SOURCES ${CSE_MODULE_${Name}_SOURCES} CACHE INTERNAL "List of source files for ${Name}")
endmacro()

macro(AddSources)
    message("Scanning source files of module: ${ARGN}")
    set(Arguments ${ARGN})

    # Source files
    file(GLOB_RECURSE Sources
        "${CMAKE_CURRENT_LIST_DIR}/Sources/*.cc"
    )
    # Internal headers and inlines
    file(GLOB_RECURSE Inlines
        "${CMAKE_CURRENT_LIST_DIR}/Sources/*.hh"
        "${CMAKE_CURRENT_LIST_DIR}/Sources/*.inl"
        "${CMAKE_CURRENT_LIST_DIR}/Sources/*.tbl" # Table files
    )
    # Headers
    file(GLOB_RECURSE Headers
        "${CMAKE_CURRENT_LIST_DIR}/Headers/CSE/*.h"
        "${CMAKE_CURRENT_LIST_DIR}/Headers/CSE/*.hh"
        "${CMAKE_CURRENT_LIST_DIR}/Headers/CSE/*.inc"
        "${CMAKE_CURRENT_LIST_DIR}/Headers/CSE/*.inl"
    )

    AddSourceGroup("Sources" DIRBASE "${CMAKE_CURRENT_LIST_DIR}/Sources" FILES ${Sources} ${Inlines})
    AddSourceGroup("Headers" DIRBASE "${CMAKE_CURRENT_LIST_DIR}/Headers" FILES ${Headers})
    SetSources(${Arguments} HEADERS ${Headers} SOURCES ${Sources} ${Inlines})
endmacro()

macro(CreateModule)
    message("${Name}: Loading... (With external dependences ${ARGN}")
    if(CSE_MODULE_${Name}_CLASS STREQUAL "BINDINGS")
        message(FATAL_ERROR "Bindings module can't call CreateModule()")
    endif()
    if(NOT " ${ARGN}" STREQUAL " ")
        set(CSE_${Name}_LINK_DEPS "${CSE_MODULE_${Name}_LINK_DEPS};${ARGN}" CACHE INTERNAL "")
    endif()
    if(BUILD_CSE_Universe AND SYSTEM_MODULE_${Name})
        # nothing
        set(TargetModule CSE_Universe)
    else()
        CreateModuleImpl(${ARGN})
        set(TargetModule ${Name})
    endif()
endmacro()

macro(FixTarget target_var)
    if(BUILD_CSE_Universe)
        if(SYSTEM_MODULE_${${target_var}})
            set(${target_var} CSE_Universe)
        endif()
    endif()
endmacro()

function(IsCSEDir result_var dir)
    set(result FALSE)
    foreach(parent ${CSpaceEngine_SOURCE_DIR} ${CSpaceEngine_BINARY_DIR} ${CSE_EXTRA_MODULES_PATH})
        IsSubDir(result "${parent}" "${dir}")
        if(result)
            break()
        endif()
    endforeach()
    set(${result_var} ${result} PARENT_SCOPE)
endfunction()

function(AddIncludeDirectories target)
    FixTarget(target)
    set(Params "")
    set(VName Params)
    foreach(dir ${ARGN})
        get_filename_component(CanonicalDir "${dir}" ABSOLUTE)
        IsCSEDir(CSEDir "${dir}")
        if(CSEDir)
            list(APPEND ${VName} "${CanonicalDir}")
        else()
            list(APPEND ${VName} "${dir}")
        endif()
    endforeach()
    if(TARGET ${target})
        if(Params)
            target_include_directories(${target} PRIVATE ${Params})
            if(CSE_DEPENDANT_TARGETS_${target})
                foreach(t ${CSE_DEPENDANT_TARGETS_${target}})
                    target_include_directories(${t} PRIVATE ${target})
                endforeach()
            endif()
        endif()
    else()
        if(Params)
            set(NewHeaderDir ${CSE_TARGET_INCLUDE_DIRS_${target}})
            list(APPEND NewHeaderDir ${Params})
            set(CSE_TARGET_INCLUDE_DIRS_${target} "${NewHeaderDir}" CACHE INTERNAL "")
        endif()
    endif()
endfunction()

macro(AddTargetIncludeModules target)
    foreach(d ${ARGN})
        if(d MATCHES "^CSE_")
            if(HAVE_${d} AND EXISTS "${CSE_MODULE_${d}_LOCATION}/Headers")
                AddIncludeDirectories(${target} "${CSE_MODULE_${d}_LOCATION}/Headers")
            endif()
        elseif(EXISTS "${d}")
            AddIncludeDirectories(${target} "${d}")
        else()
            message(WARNING "Unexpected include path: ${d} (module=${Name})")
        endif()
    endforeach()
endmacro()

macro(AddHeaders)
    AddIncludeDirectories(${Name} "${CSE_MODULE_${Name}_LOCATION}/Headers")
    AddTargetIncludeModules(${Name} ${CSE_MODULE_${Name}_DEPS} ${ARGN})
endmacro()

function(AddTargetIncludes target)
    if(DEFINED CSE_TARGET_INCLUDE_DIRS_${target})
        target_include_directories(${target} PRIVATE ${CSE_TARGET_INCLUDE_DIRS_${target}})
        if(CSE_DEPENDANT_TARGETS_${target})
            foreach(t ${CSE_DEPENDANT_TARGETS_${target}})
                target_include_directories(${t} PRIVATE ${CSE_TARGET_INCLUDE_DIRS_${target}})
            endforeach()
        endif()
        unset(CSE_TARGET_INCLUDE_DIRS_${target} CACHE)
    endif()

    if(DEFINED CSE_TARGET_INCLUDE_SYSTEM_DIRS_${target})
        target_include_directories(${target} SYSTEM PRIVATE ${CSE_TARGET_INCLUDE_SYSTEM_DIRS_${target}})
        if(CSE_DEPENDANT_TARGETS_${target})
            foreach(t ${CSE_DEPENDANT_TARGETS_${target}})
                target_include_directories(${t} SYSTEM PRIVATE ${CSE_TARGET_INCLUDE_SYSTEM_DIRS_${target}})
            endforeach()
        endif()
        unset(CSE_TARGET_INCLUDE_SYSTEM_DIRS_${target} CACHE)
    endif()
endfunction()

function(AddLibrary target)
    add_library(${target} ${ARGN})
    AddTargetIncludes(${target})
endfunction()

function(InstallTarget)
    install(TARGETS ${ARGN})
endfunction()

macro(CreateModuleImpl)
    set(ModuleHeaders ${CSE_MODULE_${Name}_HEADERS})
    if(ModuleHeaders)
        list(SORT ModuleHeaders)  # fix headers order, useful for bindings
    endif()
    set(CSE_MODULE_${Name}_HEADERS ${ModuleHeaders} CACHE INTERNAL "List of header files for ${Name}")
    set(CSE_MODULE_${Name}_SOURCES ${CSE_MODULE_${Name}_SOURCES} CACHE INTERNAL "List of source files for ${Name}")
    message(STATUS "Adding library: ${Name}")

    AddLibrary(${Name} STATIC ${CSE_MODULE_${Name}_HEADERS} ${CSE_MODULE_${Name}_SOURCES})

    set_target_properties(${Name} PROPERTIES LABELS "${CSE_MODULE_${Name}_LABEL};Module")
    set_source_files_properties(${CSE_MODULE_${Name}_HEADERS} ${CSE_MODULE_${Name}_SOURCES}
        PROPERTIES LABELS "${CSE_MODULE_${Name}_LABEL};Module")
    if(ENABLE_SOLUTION_FOLDERS)
        set_target_properties(${Name} PROPERTIES FOLDER "Modules")
    endif()

    set_target_properties(${Name} PROPERTIES
      OUTPUT_NAME "${Name}"
      DEBUG_POSTFIX "${CSE_DEBUG_POSTFIX}"
      COMPILE_PDB_NAME "${Name}"
      COMPILE_PDB_NAME_DEBUG "${Name}${CSE_DEBUG_POSTFIX}"
      ARCHIVE_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH}
      COMPILE_PDB_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH}
      LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH}
      RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
    )

    get_target_property(TargetType ${Name} TYPE)
    if(CSE_MODULE_${Name}_CLASS STREQUAL "PUBLIC")
        InstallTarget(${Name} EXPORT CSEModules OPTIONAL
        RUNTIME DESTINATION Binaries COMPONENT libs
        LIBRARY DESTINATION Libraries COMPONENT libs NAMELINK_SKIP
        ARCHIVE DESTINATION Libraries COMPONENT dev
        INCLUDES DESTINATION Headers COMPONENT dev
        )
    endif()

    if(CSE_MODULE_${Name}_HEADERS AND ";${CSE_MODULES_PUBLIC};" MATCHES ";${Name};")
        foreach(Header ${CSE_MODULE_${Name}_HEADERS})
            string(REGEX REPLACE "^.*CSE/" "CSE/" Header2 "${Header}")
            message(STATUS "Installing header : ${Header} -> ${Header2}")
            if(NOT Header2 MATCHES "private" AND Header2 MATCHES "^(CSE/?.*)/[^/]+.[(h)(hh)(inc)(inl)]$" )
                message(STATUS "Header installed : ${Header}")
                install(FILES ${Header} OPTIONAL DESTINATION "Headers/${CMAKE_MATCH_1}" COMPONENT dev)
            else()
                message(WARNING "Header file will be NOT installed: ${Header}")
            endif()
        endforeach()
    endif()
endmacro()

# ----------------------------------------------------------------------------
# Main Process functions
# ----------------------------------------------------------------------------

function(ScanDirectory OutputPath OutputName)
    set(PATHS ${ARGN})
    foreach(path ${PATHS})
        message(STATUS "Scanning: ${path}")
        list(LENGTH paths before)
        get_filename_component(path "${path}" ABSOLUTE)
        # Either module itself
        if(NOT path STREQUAL "${CSE_SOURCE_DIR}/Modules" AND EXISTS "${path}/CMakeLists.txt")
            get_filename_component(name "${path}" NAME)
            list(APPEND paths "${path}")
            list(APPEND names "${name}")
        else()
            # Either flat collection of modules
            file(GLOB subdirs RELATIVE "${path}" "${path}/*")
            foreach(subdir ${subdirs})
                message(STATUS "Scanning: ${path}/${subdir}")
                if(EXISTS "${path}/${subdir}/CMakeLists.txt")
                    list(APPEND paths "${path}/${subdir}")
                    list(APPEND names "${subdir}")
                endif()
            endforeach()
        endif()
        list(LENGTH paths after)
        if(before EQUAL after)
            message(SEND_ERROR "No modules has been found: ${path}")
        endif()
    endforeach()
    # Return
    set(${OutputPath} ${paths} PARENT_SCOPE)
    set(${OutputName} ${names} PARENT_SCOPE)
endfunction()

function(CheckDuplication)
    GetDuplicates(Duplicates ${ARGN})
    if(Duplicates)
        foreach(e ${ARGN})
            list(FIND Duplicates "${e}" idx)
            if(NOT idx EQUAL -1)
                set(prefix " > ")
            else()
                set(prefix "   ")
            endif()
            message("${prefix}${e}")
        endforeach()
        message(FATAL_ERROR "${msg}")
    endif()
endfunction()

function(AddModulePhase1 paths names)
    message(STATUS "Initializing: Loading module phase 1 - paths=${paths}, names=${names}, ... ${ARGN}")
    list(LENGTH ${paths} Len)
    if(NOT Len EQUAL 0)
        list(LENGTH ${names} LenVerify)
        if(NOT Len EQUAL LenVerify)
            message(FATAL_ERROR "FATAL: Number of directories(${Len}) is not equal to Modules(${LenVerify}),
                                        are there any modules failed to loaded?")
        endif()
        math(EXPR Len "${Len} - 1")
        foreach(i RANGE ${Len})
        list(GET ${paths} ${i} Path)
        list(GET ${names} ${i} Name)
        message(STATUS "Initializing: Loading module phase 1 - ${Name} => ${Path}")
        add_subdirectory("${Path}" "${CSpaceEngine_BINARY_DIR}/Modules/.firstpass/${Name}")
        endforeach()
    endif()
endfunction()

function(DisableModule Module)
    list(REMOVE_ITEM CSE_MODULES_DISABLED_AUTO "${Module}")
    list(APPEND CSE_MODULES_DISABLED_AUTO "${Module}")
    list(REMOVE_ITEM CSE_MODULES_BUILD "${Module}")
    list(REMOVE_ITEM CSE_MODULES_PUBLIC "${Module}")
    set(HAVE_${Module} OFF CACHE INTERNAL "Module ${Module} can not be built in current configuration")
    set(CSE_MODULES_DISABLED_AUTO "${CSE_MODULES_DISABLED_AUTO}" CACHE INTERNAL "")
    set(CSE_MODULES_BUILD "${CSE_MODULES_BUILD}" CACHE INTERNAL "")
    set(CSE_MODULES_PUBLIC "${CSE_MODULES_PUBLIC}" CACHE INTERNAL "")
endfunction()

# sort modules by dependencies
function(SortDeps Lst)
    Sort(${Lst})
    set(input ${${Lst}})
    set(result "")
    set(result_extra "")
    while(input)
        list(LENGTH input length_before)
        foreach (m ${input})
            # check if module is in the result already
            if (NOT ";${result};" MATCHES ";${m};")
                # scan through module dependencies...
                set(unresolved_deps_found FALSE)
                foreach (d ${CSE_MODULE_${m}_DEPS})
                    # ... which are not already in the result and are enabled
                    if ((NOT ";${result};" MATCHES ";${d};") AND HAVE_${d})
                        set(unresolved_deps_found TRUE)
                        break()
                    endif()
                endforeach()
                # check if all dependencies for this module has been resolved
                if (NOT unresolved_deps_found)
                    list(APPEND result ${m})
                    list(REMOVE_ITEM input ${m})
                endif()
            endif()
        endforeach()
        list(LENGTH input length_after)
        # check for infinite loop or unresolved dependencies
        if (NOT length_after LESS length_before)
            if (";${input};" MATCHES ";CSE_Universe;")
                list(REMOVE_ITEM input "CSE_Universe")
                list(APPEND result_extra "CSE_Universe")
            else()
                message(FATAL_ERROR "FATAL: Unresolved dependencies or loop in dependency graph (${length_after})\n"
                "Processed $Lst: ${${Lst}}\n"
                "Good modules: ${result}\n"
                "Bad modules: ${input}")
                list(APPEND result ${input})
                break()
            endif()
        endif()
    endwhile()
    set(${Lst} "${result};${result_extra}" PARENT_SCOPE)
endfunction()

function(ResolveDeps)
    foreach(Module ${CSE_MODULES_DISABLED_USER})
        set(HAVE_${Module} OFF CACHE INTERNAL "Module ${m} will not be built in current configuration")
    endforeach()
    foreach(Module ${CSE_MODULES_BUILD})
        set(HAVE_${Module} ON CACHE INTERNAL "Module ${m} will be built in current configuration")
    endforeach()

    if(BUILD_LIST)
        # Prepare the list
        string(REGEX REPLACE "[ ,:]+" ";" whitelist "${BUILD_LIST}" )
        if(BUILD_CSE_Universe)
            list(APPEND whitelist world)
        endif()
        AddPrefix(whitelist "CSE_")
        Sort(whitelist)
        RemDuplicates(whitelist)
        message(STATUS "Using whitelist: ${whitelist}")
        # Expand the list
        foreach(depth RANGE 10)
            set(new_whitelist ${whitelist})
            foreach(Module ${whitelist})
                list(APPEND new_whitelist ${CSE_MODULE_${Module}_REQ_DEPS})
                list(APPEND new_whitelist ${CSE_MODULE_${Module}_PRIVATE_REQ_DEPS})
            endforeach()
            Sort(new_whitelist)
            RemDuplicates(new_whitelist)
            if("${whitelist}" STREQUAL "${new_whitelist}")
                break()
            endif()
            set(whitelist "${new_whitelist}")
        endforeach()
        # Disable modules not in whitelist
        foreach(Module ${CSE_MODULES_BUILD})
            list(FIND whitelist ${Module} idx)
            if(idx EQUAL -1)
                message(STATUS "Module ${Module} disabled by whitelist")
                DisableModule(${Module})
            endif()
        endforeach()
    endif()

    # add reverse wrapper dependencies (BINDINDS)
    foreach(Module ${CSE_MODULES_BUILD})
        foreach (wrapper ${CSE_MODULE_${Module}_WRAPPERS})
            AddDependencies(CSE_${wrapper} OPTIONAL ${Module})
        endforeach()
    endforeach()

    # disable MODULES with unresolved dependencies
    set(has_changes ON)
    while(has_changes)
        set(has_changes OFF)
        foreach(Module ${CSE_MODULES_BUILD})
            set(Deps ${CSE_MODULE_${Module}_REQ_DEPS} ${CSE_MODULE_${Module}_PRIVATE_REQ_DEPS})
            while(Deps)
                PopFront(Deps dep)
                if(NOT (HAVE_${dep} OR TARGET ${dep} OR EXISTS ${dep}))
                    if(dep MATCHES "^CSE_")
                        message(WARNING "Module ${Module} disabled because ${dep} dependency can't be resolved!")
                        DisableModule(${Module})
                        set(has_changes ON)
                        break()
                    else()
                        message(STATUS "Assume that non-module dependency is available: ${dep} (for module ${Module})")
                    endif()
                endif()
            endwhile()
        endforeach()
    endwhile()

    message(STATUS "List of active modules: ${CSE_MODULES_BUILD}")

    foreach(Module ${CSE_MODULES_BUILD})
        set(deps_${Module} ${CSE_MODULE_${Module}_REQ_DEPS})
            foreach(dep ${CSE_MODULE_${Module}_OPT_DEPS})
            if(NOT (";${deps_${Module}};" MATCHES ";${dep};"))
                if(HAVE_${dep} OR TARGET ${dep})
                    list(APPEND deps_${Module} ${dep})
                endif()
            endif()
        endforeach()
        message(STATUS "Initial deps of ${Module} (w/o private deps): ${deps_${Module}}")
    endforeach()

    # propagate dependencies
    set(has_changes ON)
    while(has_changes)
        set(has_changes OFF)
        foreach(m2 ${CSE_MODULES_BUILD}) # transfer deps of m2 to m
            foreach(m ${CSE_MODULES_BUILD})
                if((NOT m STREQUAL m2) AND ";${deps_${m}};" MATCHES ";${m2};")
                    foreach(d ${deps_${m2}})
                        if(NOT (";${deps_${m}};" MATCHES ";${d};"))
                            message(STATUS "Transfer dependency ${d} from ${m2} to ${m}")
                            list(APPEND deps_${m} ${d})
                            set(has_changes ON)
                        endif()
                        if(BUILD_CSE_Universe
                          AND NOT "${m}" STREQUAL "CSE_Universe"
                          AND NOT "${m2}" STREQUAL "CSE_Universe"
                          AND SYSTEM_MODULE_${m2}
                          AND NOT SYSTEM_MODULE_${m})
                            if(NOT (";${deps_${m}};" MATCHES ";CSE_Universe;"))
                                message(STATUS "  Transfer dependency CSE_Universe alias ${m2} to ${m}")
                                list(APPEND deps_${m} CSE_Universe)
                                set(has_changes ON)
                            endif()
                        endif()
                    endforeach()
                endif()
            endforeach()
        endforeach()
    endwhile()

    # process private deps
    foreach(m ${CSE_MODULES_BUILD})
        foreach(d ${CSE_MODULE_${m}_PRIVATE_REQ_DEPS})
            if(NOT (";${deps_${m}};" MATCHES ";${d};"))
                list(APPEND deps_${m} ${d})
            endif()
        endforeach()
        foreach(d ${CSE_MODULE_${m}_PRIVATE_OPT_DEPS})
            if(NOT (";${deps_${m}};" MATCHES ";${d};"))
                if(HAVE_${d} OR TARGET ${d})
                    list(APPEND deps_${m} ${d})
                endif()
            endif()
        endforeach()
    endforeach()

    Sort(CSE_MODULES_BUILD)

    foreach(m ${CSE_MODULES_BUILD})
        message(STATUS "FULL deps of ${m}: ${deps_${m}}")
        set(CSE_MODULE_${m}_DEPS ${deps_${m}})
        set(CSE_MODULE_${m}_DEPS_EXT ${deps_${m}})
        EraseIf(CSE_MODULE_${m}_DEPS_EXT "^CSE_[^ ]+$")
        if(CSE_MODULE_${m}_DEPS_EXT AND CSE_MODULE_${m}_DEPS)
            list(REMOVE_ITEM CSE_MODULE_${m}_DEPS ${CSE_MODULE_${m}_DEPS_EXT})
        endif()
    endforeach()

    # reorder dependencies
    foreach(m ${CSE_MODULES_BUILD})
        SortDeps(CSE_MODULE_${m}_DEPS)
        set(LINK_DEPS ${CSE_MODULE_${m}_DEPS})
        # process world
        if(BUILD_CSE_Universe)
            if(SYSTEM_MODULE_${m})
                list(APPEND CSE_SYSTEM_MODULES ${m})
            endif()
            foreach(m2 ${CSE_MODULES_BUILD})
                if(SYSTEM_MODULE_${m2})
                    if(";${LINK_DEPS};" MATCHES ";${m2};")
                        list(REMOVE_ITEM LINK_DEPS ${m2})
                        if(NOT (";${LINK_DEPS};" MATCHES ";CSE_Universe;") AND NOT (${m} STREQUAL CSE_Universe))
                            list(APPEND LINK_DEPS CSE_Universe)
                        endif()
                    endif()
                    if("${m}" STREQUAL CSE_Universe)
                        list(APPEND CSE_MODULE_CSE_Universe_DEPS_EXT ${CSE_MODULE_${m2}_DEPS_EXT})
                    endif()
                endif()
            endforeach()
        endif()
        set(CSE_MODULE_${m}_DEPS ${CSE_MODULE_${m}_DEPS} CACHE INTERNAL "Flattened dependencies of ${m} module")
        set(CSE_MODULE_${m}_DEPS_EXT ${CSE_MODULE_${m}_DEPS_EXT} CACHE INTERNAL "Extra dependencies of ${m} module")
        set(CSE_MODULE_${m}_DEPS_TO_LINK ${LINK_DEPS} CACHE INTERNAL "Flattened dependencies of ${m} module (for linker)")
        message(STATUS "  module deps of ${m}: ${CSE_MODULE_${m}_DEPS}")
        message(STATUS "  module link deps of ${m}: ${CSE_MODULE_${m}_DEPS_TO_LINK}")
        message(STATUS "  extra deps of ${m}: ${CSE_MODULE_${m}_DEPS_EXT}")
        message(STATUS "")
    endforeach()
    SortDeps(CSE_MODULES_BUILD)
    set(CSE_MODULES_PUBLIC        ${CSE_MODULES_PUBLIC}        CACHE INTERNAL "List of CSE modules marked for export")
    set(CSE_MODULES_BUILD         ${CSE_MODULES_BUILD}         CACHE INTERNAL "List of CSE modules included into the build")
    set(CSE_MODULES_DISABLED_AUTO ${CSE_MODULES_DISABLED_AUTO} CACHE INTERNAL "List of CSE modules implicitly disabled due to dependencies")
    set(CSE_WORLD_MODULES         ${CSE_WORLD_MODULES}         CACHE INTERNAL "List of CSE modules included into the world")
endfunction()

function(AddModulePhase2)
    message(STATUS "Initializing: Loading module phase 2 - ${ARGN}")
    foreach(Module ${ARGN})
        set(ModuleName "${Module}")
        if(BuildAsSingleFile AND Module STREQUAL "Universe"
          OR NOT BUILD_CSE_Universe
          OR NOT SYSTEM_MODULE_${Module})
            if(NOT Module MATCHES "^CSE_")
                message(WARNING "Incorrect module name: ${Module}")
            endif()
            string(REGEX REPLACE "^CSE_" "" name "${Module}")
            message(STATUS "Initializing: Loading module phase 2 - ${name} => ${CSE_MODULE_${Module}_LOCATION}")
            add_subdirectory("${CSE_MODULE_${Module}_LOCATION}" "${CSpaceEngine_BINARY_DIR}/Modules/${name}")
        endif()
    endforeach()
    unset(ModuleName)
endfunction()

macro(DetectModuleList MainList)
    if(DEFINED CSE_INITIAL_PASS)
        message(FATAL_ERROR "Modules is already loaded.")
    endif()
    set(CSE_INITIAL_PASS ON)
    ScanDirectory(ModulePath Modules ${MainList})
    ScanDirectory(ExtensionPath Extensions ${ARGN})
    CheckDuplication("Duplicated modules LOCATIONS has been found" ${ModulePath} ${ExtensionPath})
    CheckDuplication("Duplicated modules LOCATIONS has been found" ${Modules} ${Extensions})
    set(CSE_PROCESSING_EXTENSIONS 0)
    # OpenCV separated the process of loading modules to two phases
    # Load modules and extensions phase 1
    AddModulePhase1(ModulePath Modules)
    set(CSE_PROCESSING_EXTENSIONS 1)
    AddModulePhase1(ExtensionPath Extensions)
    DeleteVariable(ModulePath Modules ExtensionPath Extensions)
    # Resolve Dependences
    message(STATUS "Resolving dependencies...")
    ResolveDeps()
    # Load modules and extensions phase 2
    set(CSE_INITIAL_PASS OFF)
    AddModulePhase2(${CSE_MODULES_BUILD})
endmacro()

# ----------------------------------------------------------------------------
# Main
# ----------------------------------------------------------------------------

function(ModuleManagerMain)
    if(NOT CSE_MODULES_PATH)
        set(CSE_MODULES_PATH "${CSpaceEngine_SOURCE_DIR}/Modules")
    endif()
    if(NOT CSE_EXTENSIONS_PATH)
        set(CSE_EXTENSIONS_PATH "${CSpaceEngine_SOURCE_DIR}/Extensions")
    endif()
    DetectModuleList(${CSE_MODULES_PATH} ${CSE_EXTENSIONS_PATH})
endfunction()
