#-------------------------------------------------
#
# Project created by QtCreator 2013-02-03T20:28:03
#
#-------------------------------------------------

#QT       += core
#QT       -= gui

TARGET = 10sectaxi
CONFIG += console
CONFIG -= app_bundle

INCLUDEPATH += \
	include \
	include/libc \
	include/libcxx

TEMPLATE = app


SOURCES += \
	app.cpp \
	useful.cpp \
	settings.cpp \
	fontmanager.cpp \
	audiomanager.cpp \
	entitymanager.cpp \
	texturemanager.cpp \
	boundedentity.cpp \
	entity.cpp \
	camera.cpp \
	spriteentity.cpp \
    carentity.cpp \
    playercarentity.cpp \
    video.cpp

OTHER_FILES += \
	Makefile \
	build/10sectaxi.html \
	data/settings.cfg \
    ideas.txt

HEADERS += \
	app.h \
	useful.h \
	settings.h \
	fontmanager.h \
	audiomanager.h \
	entitymanager.h \
	texturemanager.h \
	boundedentity.h \
	entity.h \
	camera.h \
	spriteentity.h \
    carentity.h \
    playercarentity.h \
    video.h
