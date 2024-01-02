# std::format is still not supported in som compilers while enable C++20
# so a backup solution is needed for using format functions

function(FindFmtLib)
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
        set(FMT_HEADERS_DIR "${FMT_FIND_DIRECTORY}/include" CACHE STRING "fmtlib headers directory")
        set(FMT_LIBRARY_DIR "${FMT_FIND_DIRECTORY}/lib" CACHE STRING "fmtlib library directory")
    else()
        set(FMT_HEADERS_DIR "" CACHE STRING "fmtlib headers directory")
        set(FMT_LIBRARY_DIR "" CACHE STRING "fmtlib library directory")
    endif()
endfunction()

function(AddFmtLib ModuleName)
    message (STATUS "Adding Fmtlib for module: ${ModuleName}")
    include_directories(${FMT_HEADERS_DIR})
    target_link_directories(${ModuleName} PUBLIC ${FMT_LIBRARY_DIR})
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_libraries(${ModuleName} PUBLIC fmtd)
    else()
        target_link_libraries(${ModuleName} PUBLIC fmt)
    endif ()
endfunction()
