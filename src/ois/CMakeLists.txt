project(OIS)

set(OIS_HEADERS
  includes/OISConfig.h
  includes/OISEffect.h
  includes/OISEvents.h
  includes/OISException.h
  includes/OISFactoryCreator.h
  includes/OISForceFeedback.h
  includes/OIS.h
  includes/OISInputManager.h
  includes/OISInterface.h
  includes/OISJoyStick.h
  includes/OISKeyboard.h
  includes/OISMouse.h
  includes/OISMultiTouch.h
  includes/OISObject.h
  includes/OISPrereqs.h
)
set(OIS_SOURCES
  src/OISEffect.cpp
  src/OISException.cpp
  src/OISForceFeedback.cpp
  src/OISInputManager.cpp
  src/OISJoyStick.cpp
  src/OISKeyboard.cpp
  src/OISObject.cpp
  src/extras/LIRC/OISLIRC.cpp
  src/extras/LIRC/OISLIRCFactoryCreator.cpp
)
if (WIN32)
  set(OIS_PLATFORM_HEADERS
    includes/win32/Win32ForceFeedback.h
    includes/win32/Win32InputManager.h
    includes/win32/Win32JoyStick.h
    includes/win32/Win32KeyBoard.h
    includes/win32/Win32Mouse.h
    includes/win32/Win32Prereqs.h
  )
  set(OIS_PLATFORM win32)
  set(OIS_PLATFORM_SOURCES
    src/win32/Win32ForceFeedback.cpp
    src/win32/Win32InputManager.cpp
    src/win32/Win32JoyStick.cpp
    src/win32/Win32KeyBoard.cpp
    src/win32/Win32Mouse.cpp
    src/win32/extras/WiiMote/hiddevice.cpp
    src/win32/extras/WiiMote/OISWiiMote.cpp
    src/win32/extras/WiiMote/OISWiiMoteFactoryCreator.cpp
    src/win32/extras/WiiMote/OISWiiMoteForceFeedback.cpp
    src/win32/extras/WiiMote/OISWiiMoteRingBuffer.cpp
    src/win32/extras/WiiMote/wiimote.cpp
  )
  # need DirectX includes and libraries
  include_directories(${DirectX_INCLUDE_DIRS})
  set(OIS_PLATFORM_LIBS ${DirectX_DINPUT8_LIBRARY} ${DirectX_DXGUID_LIBRARY})
elseif (APPLE)
  set(OIS_PLATFORM_HEADERS
    includes/mac/MacHelpers.h
    includes/mac/MacHIDManager.h
    includes/mac/MacInputManager.h
    includes/mac/MacJoyStick.h
    includes/mac/MacKeyboard.h
    includes/mac/MacMouse.h
    includes/mac/MacPrereqs.h
  )
  set(OIS_PLATFORM mac)
  set(OIS_PLATFORM_SOURCES
    src/mac/MacHelpers.cpp
    src/mac/MacHIDManager.cpp
    src/mac/MacInputManager.cpp
    src/mac/MacJoyStick.cpp
    src/mac/MacKeyboard.cpp
    src/mac/MacMouse.cpp
  )
else ()
  set(OIS_PLATFORM_HEADERS
    includes/linux/EventHelpers.h
    includes/linux/LinuxForceFeedback.h
    includes/linux/LinuxInputManager.h
    includes/linux/LinuxJoyStickEvents.h
    includes/linux/LinuxKeyboard.h
    includes/linux/LinuxMouse.h
    includes/linux/LinuxPrereqs.h
  )
  set(OIS_PLATFORM linux)
  set(OIS_PLATFORM_SOURCES
    src/linux/EventHelpers.cpp
    src/linux/LinuxForceFeedback.cpp
    src/linux/LinuxInputManager.cpp
    src/linux/LinuxJoyStickEvents.cpp
    src/linux/LinuxKeyboard.cpp
    src/linux/LinuxMouse.cpp
  )
endif ()


include_directories(includes)
add_definitions(-DOIS_NONCLIENT_BUILD -DOIS_DYNAMIC_LIB)

add_library(OIS SHARED ${OIS_HEADERS} ${OIS_SOURCES} ${OIS_PLATFORM_HEADERS} ${OIS_PLATFORM_SOURCES})

if (MINGW)
	  # remove lib prefix from DLL outputs
	  set_target_properties(OIS PROPERTIES PREFIX "")
endif ()

target_link_libraries(OIS ${OIS_PLATFORM_LIBS})
install_dep(OIS include/OIS ${OIS_HEADERS})
if (OGREDEPS_INSTALL_DEV)
  install(FILES ${OIS_PLATFORM_HEADERS} DESTINATION include/OIS/${OIS_PLATFORM})
endif ()

set(OIS_INCLUDE_DIR "${OIS_SOURCE_DIR}/includes" CACHE PATH "" FORCE)
set(OIS_LIBRARY_DBG "OIS" CACHE STRING "" FORCE)
set(OIS_LIBRARY_REL "OIS" CACHE STRING "" FORCE)
mark_as_advanced(OIS_INCLUDE_DIR OIS_LIBRARY_DBG OIS_LIBRARY_REL)
