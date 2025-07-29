set(BUILD_CSE_Physics_INIT OFF)

AddModule(Physics CSE_Base CSE_Object)
AddSources()
AddHeaders(${Name} ${FMT_HEADERS_DIR} ${BOOST_ROOT_DIR})
CreateModule()
