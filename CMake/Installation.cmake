# Installation function for sub-modules

function(Installation ModuleName)
    install(TARGETS ${ModuleName} EXPORT ${ModuleName}-targets
      COMPONENT ${ModuleName}
      LIBRARY DESTINATION Libraries
      ARCHIVE DESTINATION Libraries
      RUNTIME DESTINATION Binaries
      INCLUDES DESTINATION Headers
    )

    install(EXPORT ${ModuleName}-targets
      FILE "CSE-${ModuleName}-targets.cmake"
      NAMESPACE CSE::
      DESTINATION lib/cmake/CSE
      COMPONENT ${ModuleName}
    )

    # Pack Configurations
    configure_file("${ModuleName}-config.cmake.conf"
      "${CMAKE_BINARY_DIR}/CSE-${ModuleName}-config.cmake"
      @ONLY
    )

    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
      "${CMAKE_BINARY_DIR}/CSE-${ModuleName}-version.cmake"
      VERSION ${CSE_VER}
      COMPATIBILITY AnyNewerVersion
    )

    # Installation
    install(FILES
        "${CMAKE_BINARY_DIR}/CSE-${ModuleName}-config.cmake"
        "${CMAKE_BINARY_DIR}/CSE-${ModuleName}-version.cmake"
      DESTINATION lib/cmake/CSE
      COMPONENT ${ModuleName}
    )

    # DESTINATION will be automatically prefixed by ${CMAKE_INSTALL_PREFIX}
    install(
      DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Headers
      COMPONENT ${ModuleName}
      DESTINATION ${CMAKE_INSTALL_PREFIX}
    )
endfunction()
