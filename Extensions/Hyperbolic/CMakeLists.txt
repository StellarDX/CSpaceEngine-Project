# Scan Files
file(GLOB_RECURSE Sources CONFIGURE_DEPENDS Sources/*.cc Headers/*.h)
add_library(Hyperbolic STATIC "${Sources}")

target_include_directories(Hyperbolic PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Headers>
    $<INSTALL_INTERFACE:Headers>)
target_link_libraries(Hyperbolic PUBLIC CSEBase)

install(TARGETS Hyperbolic EXPORT Hyperbolic-targets
  COMPONENT Hyperbolic
  LIBRARY DESTINATION Libraries/Extensions
  ARCHIVE DESTINATION Libraries/Extensions
  RUNTIME DESTINATION Binaries/Extensions
  INCLUDES DESTINATION Headers
)

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Headers
  COMPONENT Hyperbolic
  DESTINATION ${CMAKE_INSTALL_PREFIX}
)
