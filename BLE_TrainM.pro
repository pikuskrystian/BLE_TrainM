QT += quick bluetooth widgets

SOURCES += \
        bledevice.cpp \
        dataprocessing.cpp \
        deviceinfo.cpp \
        main.cpp

resources.files = main.qml Gauge.qml
resources.prefix = /$${TARGET}
resources.files += icons/batt.png
RESOURCES += resources

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    bledevice.h \
    dataprocessing.h \
    deviceinfo.h

ICON = appicon.png

ios: QMAKE_INFO_PLIST = Info.plist
macos: QMAKE_INFO_PLIST = Info.qmake.macos.plist

DISTFILES += \
    Gauge.qml \
    icons/headlights.png
