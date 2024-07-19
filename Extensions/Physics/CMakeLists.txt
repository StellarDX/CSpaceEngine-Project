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
AddHeaders(${Name} ${FMT_HEADERS_DIR} ${BOOST_ROOT_DIR})
CreateModule()
