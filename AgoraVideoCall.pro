TEMPLATE = app

QT += core quickwidgets
CONFIG += c++11

SOURCES += main.cpp \
    agorartcengine.cpp \
    mainwindow.cpp \
    avideowidget.cpp \
    video_render_opengl.cpp \
    video_render_impl.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    agorartcengine.h \
    mainwindow.h \
    avideowidget.h \
    video_render_opengl.h \
    video_render_impl.h


INCLUDEPATH += $$PWD/lib

win32: LIBS += -L$$PWD/lib/ -lagora_rtc_sdk

macx:{
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.11
QMAKE_LFLAGS += -F/System/Library/Frameworks
#-F/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk/System/Library/Frameworks

LIBS += -L$$PWD/lib/mac/ -lmediasdk -lAgoraRTCEngine
LIBS += -framework Foundation -framework CoreAudio -framework CoreVideo -framework CoreServices -framework AppKit -framework AudioToolbox -framework SystemConfiguration -framework AVFoundation -framework CoreMedia -framework CoreWLAN -framework QTKit -framework CoreGraphics
PRE_TARGETDEPS += $$PWD/lib/mac/libmediasdk.a $$PWD/lib/mac/libAgoraRTCEngine.a
}
