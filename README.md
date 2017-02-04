# pedestrian_flow_counter

[![Track you](
https://mega.nz/#!95kFAb4Q!hY1lIi_7NgNZWbr60T27zXr_W1iPCh-TCw1K-y1yNb0
)](http://www.youtube.com/watch?v=OpL0ivj0QRI "Track human")

This is a simple example to show you how to count the flow of pedestrian by opencv.
Since this algorithm rely on tracking algorithm, we can expect the limitation of
tracking algorithms apply on it too.

## Limitation of this project

1. Camera must be fixed
2. Occlusion
3. Crowded scenes
4. Object cannot move too fast
5. Frame rate cannot be too low and need to be stable

Although there are many limitations, I think this is still a good start for pedestrian flow counter.
To make things easier to understand, I design the api careful and separate the operations into different
modules.

##How to compile
0. This project rely on makefile generator of Qt5, please install latest Qt5 in your pc
1. Create a folder on your pc, let us assume it called my_folder
2. Go to my_folder, open your terminal and type "git clone https://github.com/stereomatchingkiss/ocv_libs",
you do not do anything with this lib, I use the header of this lib only
3. Type "git clone https://github.com/stereomatchingkiss/pri"
4. Type "git clone https://github.com/stereomatchingkiss/pedestrian_flow_counter_by_opencv"
5. If your opencv do not compile with tbb, remove the "include(../pri/tbb.pri)" in "people_counter.pro"
6. If your opencv do not compile with cuda, remove "DEFINES += OPENCV_USE_CUDA" in "people_counter.pro"
7. Install opencv3.2 on your pc, set the include path and link to the libraries. The keywords you need to know 
are "qmake INCLUDEPATH" and "qmake LIBS"
8. If you still cannot figure out how to compile this project, please open an issue on github

##Links

1. [Example on youtube](https://www.youtube.com/watch?v=OpL0ivj0QRI)
2. [Original video of youtube](
 https://mega.nz/#!w59VUCjI!qM5xMwFYK5_ZDYpM6YVlRGqG_Mrs9B9W6P3x2xmuYjk
)
3. [qmake tutorial](http://doc.qt.io/qt-4.8/qmake-tutorial.html)

