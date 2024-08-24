set(BUILD_CSE_PlanSystem_INIT OFF)

AddModule(PlanSystem CSE_Base CSE_Object CSE_MathExtras_Sexagesimal OPTIONAL CSE_Parser)
AddSources()
AddHeaders(${Name} ${FMT_HEADERS_DIR} ${BOOST_ROOT_DIR})
CreateModule()
