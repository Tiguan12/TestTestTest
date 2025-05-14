QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
	main.cpp \
	mainwindow.cpp \
	rechenoperation.cpp

HEADERS += \
	mainwindow.h \
	rechenoperation.h

FORMS += \
	mainwindow.ui


DISTFILES +=

RESOURCES += \
	Bild.qrc



#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../Einbindungsversuch/release/ -lIvium_remdriver64
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../Einbindungsversuch/debug/ -lIvium_remdriver64

#INCLUDEPATH += $$PWD/../../../../Einbindungsversuch/debug
#DEPENDPATH += $$PWD/../../../../Einbindungsversuch/debug

#win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../Einbindungsversuch/release/libIvium_remdriver64.a
#else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../Einbindungsversuch/debug/libIvium_remdriver64.a
#else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../Einbindungsversuch/release/Ivium_remdriver64.lib
#else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../Einbindungsversuch/debug/Ivium_remdriver64.lib





win32:CONFIG(release, debug|release): LIBS += -L$$PWD/./release/ -lIvium_remdriver64
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/./debug/ -lIvium_remdriver64

INCLUDEPATH += $$PWD/debug
DEPENDPATH += $$PWD/debug
