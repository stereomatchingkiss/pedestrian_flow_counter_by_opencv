TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

msvc:QMAKE_CXXFLAGS_RELEASE += /openmp /arch:AVX

DEFINES += OPENCV_USE_CUDA
#DEFINES += HAS_DLIB

contains(DEFINES, HAS_DLIB) {
   message(Support dlib)
   include(../pri/dlibs.pri)
   HEADERS += dlib_tracker.hpp
   SOURCES += dlib_tracker.cpp
}

include(../pri/cv_dev.pri)
include(../pri/tbb.pri)

INCLUDEPATH += ..

SOURCES += main.cpp \
    people_count_test.cpp \
    ../ocv_libs/core/contour_utility.cpp \
    tracker_base.cpp \
    opencv_trackers.cpp \
    blob_detector_base.cpp \
    bg_subtract_detector.cpp

HEADERS += \
    people_count_test.hpp \
    ../ocv_libs/core/contour_utility.hpp \
    tracker_base.hpp \
    opencv_trackers.hpp \
    ../ocv_libs/core/rect_utility.hpp \    
    blob_detector_base.hpp \
    bg_subtract_detector.hpp

linux-g++ {
    LIBS += -lopencv_video -lopencv_videoio
    LIBS += -L/usr/local/lib -lopencv_objdetect -lopencv_bgsegm -lopencv_tracking
}
