set(BUILD_CSE_Physics_INIT OFF)

set(SatelliteTrackerCoordSystem "Realistic" CACHE STRING
    "Coordinate tracker coordinate system, valid value: \"Realistic\", \"SpaceEngine\"")
set(CoordSystemValidList "Realistic" "SpaceEngine")

foreach(i ${CoordSystemValidList})
    if (${SatelliteTrackerCoordSystem} STREQUAL ${i})
        add_definitions(-DSATELLITE_TRACKER_COORD_${SatelliteTrackerCoordSystem})
        break()
    endif()
endforeach()

AddModule(Physics CSE_Base CSE_Object)
AddSources()
if(FMT_BACKUP_SOLUTION AND FMT_HEADERS_DIR)
    message(STATUS "${Name} : FMT_BACKUP_SOLUTION is enabled, adding fmtlib... (At ${FMT_HEADERS_DIR})")
    AddHeaders(${Name} ${FMT_HEADERS_DIR})
else()
    AddHeaders(${Name})
endif()
CreateModule()
