# Scan Files
file(GLOB_RECURSE Sources CONFIGURE_DEPENDS Sources/*.cc Headers/*.h)
add_library(Object STATIC "${Sources}")
add_library(CSE::Object ALIAS Object)

target_include_directories(Object PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Headers>
    $<INSTALL_INTERFACE:Headers>)
target_link_libraries(Object PUBLIC CSEBase)

if(BUILD_SCStream)
    target_link_libraries(Object PUBLIC SCStream)
endif()

# Installations
include(Installation)
Installation(Object)
