QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG -= embed_manifest_exe

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    disassembler.cpp \
    dispatcher.cpp \
    hopper/decode.cpp \
    main.cpp \
    mainwindow.cpp \
    stringparsers.cpp \
    targetmodel.cpp \
    disasmwidget.cpp \
    memory.cpp \
    memoryviewwidget.cpp \
    stringsplitter.cpp \
    symboltable.cpp \
    symboltablemodel.cpp \
    exceptionmask.cpp \
    exceptiondialog.cpp \
    graphicsinspector.cpp \
    breakpointswidget.cpp \
    breakpoint.cpp \
    addbreakpointdialog.cpp \
    rundialog.cpp

HEADERS += \
    disassembler.h \
    dispatcher.h \
    hopper/buffer.h \
    hopper/decode.h \
    hopper/instruction.h \
    mainwindow.h \
    stringparsers.h \
    targetmodel.h \
    disasmwidget.h \
    memory.h \
    remotecommand.h \
    memoryviewwidget.h \
    breakpoint.h \
    stringsplitter.h \
    symboltable.h \
    registers.h \
    symboltablemodel.h \
    exceptionmask.h \
    exceptiondialog.h \
    graphicsinspector.h \
    breakpointswidget.h \
    addbreakpointdialog.h \
    quicklayout.h \
    rundialog.h
    
RESOURCES     = hrdb.qrc    

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
