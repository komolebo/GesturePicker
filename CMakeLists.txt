cmake_minimum_required(VERSION 3.6)
project(GesturesPicker)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp handGesture.cpp data.cpp Functions.cpp Starter.cpp)


include_directories(${DIR}/headers /home/oleh/ClionProjects/OpenCVTest/handDetectionCV-master/opencv/opencv/include)
link_directories (${DIR}/src/utils /home/oleh/ClionProjects/OpenCVTest/handDetectionCV-master/opencv/opencv/build)

find_package(OpenCV REQUIRED COMPONENTS core imgproc highgui)

ADD_EXECUTABLE(main main.cpp)
target_link_libraries(main ${OpenCV})


add_executable(GesturesPicker ${SOURCE_FILES})