qt_standard_project_setup(I18N_TRANSLATED_LANGUAGES en fi)

if(MSVC)
  set(RCFILE isplitter.rc)
  set(WFILE windows/wfile.cpp
				windows/wfile_win.cpp
				windows/wutil_msvc.cpp
				windows/vswscanf.c
				windows/_filwbuf.c
				windows/_getbuf.c
  )

  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:msvcrt")
else()
  set(WFILE unix/wfile.cpp
				unix/wfile_unix.cpp
				unix/wutil_unix.cpp
				unix/wcsdup.c
				unix/wcslwr.c
				unix/wcsupr.c
  )
endif()

qt_add_executable(ImageSplitter
  $<$<CONFIG:Debug>:debugimpl.cpp>
  main.cpp
  mainwindow.ui
  mainwindowimpl.cpp
  menubar.cpp
  platform.cpp
  popup.cpp
  previewimpl.cpp
  util.cpp
  wstring.cpp
  ${WFILE}
  isplitter.qrc
  ${RCFILE}
)

target_include_directories(ImageSplitter PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} Qt6::Core Qt6::Gui Qt6::Widgets)
target_link_libraries(ImageSplitter PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets ZLIB::ZLIB)
set_target_properties(ImageSplitter PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

install(TARGETS ImageSplitter)
qt_generate_deploy_app_script(
    TARGET ImageSplitter
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

qt_add_translations(ImageSplitter QM_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} QM_FILES_OUTPUT_VARIABLE qm_files)
install(FILES ${qm_files} DESTINATION "translations")