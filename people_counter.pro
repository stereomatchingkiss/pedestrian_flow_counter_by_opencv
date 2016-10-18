TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

msvc:QMAKE_CXXFLAGS_RELEASE += /openmp /arch:AVX
gcc:QMAKE_CXXFLAGS_RELEASE += -O3 -march=native -fopenmp -D_GLIBCXX_PARALLEL

include(../pri/cv_dev.pri)
include(../pri/dlibs.pri)
include(../pri/tbb.pri)

INCLUDEPATH += ..

SOURCES += main.cpp \
    people_count_test.cpp \
    ../ocv_libs/core/contour_utility.cpp \
    tracker_base.cpp \
    opencv_trackers.cpp \
    dlib_tracker.cpp

HEADERS += \
    people_count_test.hpp \
    ../ocv_libs/core/contour_utility.hpp \
    tracker_base.hpp \
    opencv_trackers.hpp \
    ../ocv_libs/core/rect_utility.hpp \
    dlib_tracker.hpp
