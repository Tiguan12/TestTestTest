QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
	main.cpp \
	mainwindow.cpp \
	rechenoperation.cpp

HEADERS += \
	mainwindow.h \
	rechenoperation.h

FORMS += \
	mainwindow.ui

RESOURCES += \
	Bild.qrc

# Verwendung relativer Pfade basierend auf dem Verzeichnis der .pro-Datei
LIBS += -L$_PRO_FILE_PWD_/debug -lIvium_remdriver64

INCLUDEPATH += $_PRO_FILE_PWD_/debug
DEPENDPATH += $_PRO_FILE_PWD_/debug

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/./release/ -lIvium_remdriver64
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/./debug/ -lIvium_remdriver64
