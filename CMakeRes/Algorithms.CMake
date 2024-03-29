# Algorithm header
# functions taken from OpenCV

macro(AddPrefix LST PREFIX)
    set(Tmp "")
    foreach(item ${${LST}})
        list(APPEND Tmp "${PREFIX}${item}")
    endforeach()
    set(${LST} ${Tmp})
    unset(Tmp)
endmacro()

macro(Sort LST)
    if(${LST})
        list(SORT ${LST})
    endif()
endmacro()

macro(RemDuplicates LST)
    if(${LST})
        list(REMOVE_DUPLICATES ${LST})
    endif()
endmacro()

macro(PopFront LST VAR)
    if(${LST})
        list(GET ${LST} 0 ${VAR})
        list(REMOVE_AT ${LST} 0)
    else()
        set(${VAR} "")
    endif()
endmacro()

macro(EraseIf lst regex)
    foreach(item ${${lst}})
        if(item MATCHES "${regex}")
            list(REMOVE_ITEM ${lst} "${item}")
        endif()
    endforeach()
endmacro()

function(GetDuplicates res)
    if(ARGC LESS 2)
        message(FATAL_ERROR "Invalid call to GetDuplicates")
    endif()
    set(lst ${ARGN})
    list(SORT lst)
    set(prev_item)
    foreach(item ${lst})
        if(item STREQUAL prev_item)
            list(APPEND dups ${item})
        endif()
        set(prev_item ${item})
    endforeach()
    set(${res} ${dups} PARENT_SCOPE)
endfunction()

macro(DeleteVariable)
    foreach(Var ${ARGN})
        unset(${Var})
        unset(${Var} CACHE)
    endforeach()
endmacro()

macro(Canonical VAR)
    if(${VAR})
        set(Tmp "")
        foreach(path ${${VAR}})
            get_filename_component(${VAR} "${path}" ABSOLUTE)
            list(APPEND Tmp "${${VAR}}")
        endforeach()
        set(${VAR} ${Tmp})
        unset(Tmp)
    endif()
endmacro()

function(IsSubDir res dir sub )
    get_filename_component(dir "${dir}" ABSOLUTE)
    get_filename_component(sub "${sub}" ABSOLUTE)
    file(TO_CMAKE_PATH "${dir}" dir)
    file(TO_CMAKE_PATH "${sub}" sub)
    set(dir "${dir}/")
    string(LENGTH "${dir}" len)
    string(LENGTH "${sub}" len_sub)
    if(NOT len GREATER len_sub)
        string(SUBSTRING "${sub}" 0 ${len} prefix)
    endif()
    if(prefix AND prefix STREQUAL dir)
        set(${res} TRUE PARENT_SCOPE)
    else()
        set(${res} FALSE PARENT_SCOPE)
    endif()
endfunction()
