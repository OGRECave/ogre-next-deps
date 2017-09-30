project(Remotery)

set( Remotery_SOURCES src/Remotery.c src/Remotery.h )

if( APPLE )
	set( Remotery_SOURCES ${Remotery_SOURCES} src/Remotery.mm )
endif()

add_library( Remotery STATIC ${Remotery_SOURCES} )

install_dep( Remotery include src/Remotery.h )
