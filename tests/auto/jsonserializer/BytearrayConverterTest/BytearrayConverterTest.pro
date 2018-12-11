TEMPLATE = app

QT = core testlib jsonserializer
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_bytearrayconverter

include(../convlib.pri)

SOURCES += \
	tst_bytearrayconverter.cpp

include(../../testrun.pri)
