set(BUILD_CSE_Astronomy_INIT OFF)

AddModule(Astronomy CSE_Base CSE_Object OPTIONAL CSE_Parser)
AddSources()
AddHeaders(${Name} ${FMT_HEADERS_DIR} ${BOOST_ROOT_DIR})
CreateModule()
