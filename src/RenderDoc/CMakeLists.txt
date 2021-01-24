project(RenderDoc)

set( RenderDoc_HEADERS include/renderdoc_app.h )

if (OGREDEPS_INSTALL_DEV)
	install( DIRECTORY include/ DESTINATION include/renderdoc )
endif()
