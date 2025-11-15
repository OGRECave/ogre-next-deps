project(Imgui)

set( Imgui_SOURCES
	imconfig.h
	imgui.cpp
	imgui.h
	imgui_demo.cpp
	imgui_draw.cpp
	imgui_internal.h
	imgui_tables.cpp
	imgui_widgets.cpp
	imstb_rectpack.h
	imstb_textedit.h
	imstb_truetype.h
)
set( Imgui_BACKEND
	backends/imgui_impl_osx.h
	backends/imgui_impl_osx.mm
	backends/imgui_impl_sdl2.cpp
	backends/imgui_impl_sdl2.h
	backends/imgui_impl_sdl3.cpp
	backends/imgui_impl_sdl3.h
)

install(FILES ${Imgui_SOURCES} DESTINATION include/imgui)
install(FILES ${Imgui_BACKEND} DESTINATION include/imgui/backends)
