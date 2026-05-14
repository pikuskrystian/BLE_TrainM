QT += widgets
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app
TARGET = run_tests

SOURCES += test_main.cpp
LIBS += -lgtest -lgtest_main -lpthread
