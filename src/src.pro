unix:TARGET=isplitter
win32:TARGET=ImageSplitter
GUID={5C97BE51-DAA7-3413-915E-DC71C0E6EEAE}
DEBUG_SOURCES =

QT += widgets

#We can't rely on having "debug" in CONFIG if we set "release" as default target
CONFIG(debug, debug|release) {
	DEFINES		 += _DEBUG
	DEBUG_SOURCES = debugimpl.cpp
}
CONFIG += lrelease lupdate
CONFIG -= debug

SOURCES =	main.cpp \
			mainwindowimpl.cpp \
			menubar.cpp \
			popup.cpp \
			previewimpl.cpp \
			util.cpp \
			wstring.cpp


SOURCES +=	$$DEBUG_SOURCES

FORMS =		mainwindow.ui

HEADERS =	menubar.h \
			mainwindowimpl.h \
			previewimpl.h

RESOURCES    =  isplitter.qrc

TRANSLATIONS =	isplitter_en.ts \
				isplitter_fi.ts

CODEC =		UTF-8


win32 {
	DEFINES += _CRT_SECURE_NO_WARNINGS
	LIBS += shlwapi.lib
	SOURCES +=	windows/wfile.cpp \
				windows/wfile_win.cpp \
				windows/wutil_msvc.cpp \
				windows/vswscanf.c \
				windows/_filwbuf.c \
				windows/_getbuf.c
	RC_FILE =	isplitter.rc

	!isEmpty(DEBUG_SOURCES) {
		qtlibs.files = $$[QT_INSTALL_LIBS]\\Qt5Cored.dll \
			       $$[QT_INSTALL_LIBS]\\Qt5Guid.dll \
			       $$[QT_INSTALL_LIBS]\\Qt5Widgetsd.dll \
			       $$[QT_INSTALL_LIBS]\\libGLESv2d.dll
	} else {
		qtlibs.files = $$[QT_INSTALL_LIBS]\\Qt5Core.dll \
			       $$[QT_INSTALL_LIBS]\\Qt5Gui.dll \
			       $$[QT_INSTALL_LIBS]\\Qt5Widgets.dll \
			       $$[QT_INSTALL_LIBS]\\libGLESv2.dll
	}
	qtlibs.path = ../..
	qtlibs.CONFIG += recursive
	INSTALLS += qtlibs

	qtimageformats.path = ../../plugins/imageformats
	qtimageformats.files = $$[QT_INSTALL_PLUGINS]\\imageformats\\qsvg.dll \
			       $$[QT_INSTALL_PLUGINS]\\imageformats\\qgif.dll \
			       $$[QT_INSTALL_PLUGINS]\\imageformats\\qico.dll \
			       $$[QT_INSTALL_PLUGINS]\\imageformats\\qjpeg.dll \
			       $$[QT_INSTALL_PLUGINS]\\imageformats\\qmng.dll \
			       $$[QT_INSTALL_PLUGINS]\\imageformats\\qtga.dll \
			       $$[QT_INSTALL_PLUGINS]\\imageformats\\qtiff.dll

	qtimageformats.CONFIG += recursive
	INSTALLS += qtimageformats
}


unix {
	SOURCES +=	unix/wfile.cpp \
				unix/wfile_unix.cpp \
				unix/wutil_unix.cpp \
				unix/wcsdup.c \
				unix/wcslwr.c \
				unix/wcsupr.c
}

target.path = ../..
INSTALLS += target

!equals(TEMPLATE, vcapp) {
translations.files = $$TRANSLATIONS
translations.files ~= s/\\.ts/.qm/g
translations.path = ../../translations
translations.CONFIG += recursive
INSTALLS += translations
}