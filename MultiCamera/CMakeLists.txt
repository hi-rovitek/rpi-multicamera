project(MultiCamera)
cmake_minimum_required(VERSION 2.8)

#set dir for header and library
include_directories(/usr/include)
include_directories(/usr/local/opencvARM/include)
link_directories(/usr/lib)

link_directories(/usr/local/opencvARM/lib)

#libraries
set(EXT_LIBS opencv_core opencv_imgcodecs opencv_highgui opencv_imgproc opencv_calib3d opencv_features2d opencv_videoio -lwiringPi -lraspicam_cv -lraspicam)

aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

target_link_libraries(${PROJECT_NAME} ${EXT_LIBS})
