QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    ../../Ivium_Software Development Driver/IVIUM_remdriver.h \
    ../../Ivium_Software Development Driver/IVIUM_remdriver.h \
    ../../Ivium_Software Development Driver/Ivium_remdriver64.h \
    ../../Ivium_Software Development Driver/Ivium_remdriver64.h \
    ../../Ivium_Software Development Driver/Ivium_remdriver64.h \
    Ivium_remdriver64.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
	../../Ivium_Software Development Driver/IVIUM_remdriver.dll \
	../../Ivium_Software Development Driver/Ivium_remdriver64.dll \
	../../Ivium_Software Development Driver/Ivium_remdriver64.dll \
	IVIUM_remdriver.dll \
	Ivium_remdriver64.dll
