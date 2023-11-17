foreach(component ${CSE_FIND_COMPONENTS})
    include(${CMAKE_CURRENT_LIST_DIR}/CSE-${component}-config.cmake)
endforeach()
