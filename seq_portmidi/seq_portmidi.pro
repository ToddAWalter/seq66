#******************************************************************************
# seq_portmidi.pro (pseq66)
#------------------------------------------------------------------------------
##
# \file       	seq_portmidi.pro
# \library    	qpseq66 application
# \author     	Chris Ahlstrom
# \date       	2018-04-08
# \update      2023-05-13
# \version    	$Revision$
# \license    	$XPC_SUITE_GPL_LICENSE$
#
# Created by and for Qt Creator. This file was created for editing the project
# sources only.  You may attempt to use it for building too, by modifying this
# file here.
#
# Important:
#
#     This project file is designed only for Qt 5 (and above?).
#
#------------------------------------------------------------------------------

message($$_PRO_FILE_PWD_)

TEMPLATE = lib
CONFIG += staticlib config_prl qtc_runnable

# These are needed to set up seq66_platform_macros:

CONFIG(debug, debug|release) {
   DEFINES += DEBUG
} else {
   DEFINES += NDEBUG
}

DEFINES += "SEQ66_MIDILIB=portmidi"
DEFINES += "SEQ66_PORTMIDI_SUPPORT=1"

TARGET = seq_portmidi

# Common:

HEADERS += \
 include/mastermidibus_pm.hpp \
 include/midibus_pm.hpp \
 include/pminternal.h \
 include/pmutil.h \
 include/portmidi.h \
 include/porttime.h

# Linux

unix:!macx {
   HEADERS += include/pmlinux.h include/pmlinuxalsa.h 
}

# Mac OSX

macx {
   HEADERS += include/pmmac.h include/pmmacosxcm.h 
}

# Windows

windows {
 HEADERS += include/pmerrmm.h include/pmwinmm.h
 DEFINES -= UNICODE
 DEFINES -= _UNICODE
 QMAKE_CFLAGS_WARN_ON += -Wno-unused-parameter
}

# Common

SOURCES += \
 src/mastermidibus.cpp \
 src/midibus.cpp \
 src/pmutil.c \
 src/portmidi.c \
 src/porttime.c

# Linux

unix:!macx {
   SOURCES += src/pmlinux.c src/pmlinuxalsa.c src/ptlinux.c
}

# Mac OSX
#
#  We have removed the readbinaryplist.c file; we use INI-style files.

macx {
   SOURCES += src/pmmac.c pmmacosxcm.c src/ptmacosx_mach.c
}

# Windows

windows: SOURCES += src/pmwin.c \
 src/pmerrmm.c \
 src/pmwinmm.c \
 src/ptwinmm.c

INCLUDEPATH = ../include/qt/portmidi include ../libseq66/include

#******************************************************************************
# seq_portmidi.pro (qpseq66)
#------------------------------------------------------------------------------
# 	vim: ts=3 sw=3 ft=automake
#------------------------------------------------------------------------------
