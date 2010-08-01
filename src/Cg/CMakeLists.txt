project(Cg)

if (WIN32)
  # manually copy to build and install the correct DLL
elseif (UNIX)
  # manually install the correct .so
  if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
    install(FILES lib64/libCg.so DESTINATION ${OGREDEPS_LIB_DIRECTORY})
  else ()
    install(FILES lib/libCg.so DESTINATION ${OGREDEPS_LIB_DIRECTORY})
  endif ()
endif ()

if (OGREDEPS_INSTALL_DEV)
  install(DIRECTORY include/ DESTINATION include)
  if (WIN32)
    # install the correct lib
  endif ()
endif ()
