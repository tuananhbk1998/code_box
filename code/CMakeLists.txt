
# Module: CMakelists.txt
# Author: LE MANH THANG
# Created: 10/01/2021
# Description: 
cmake_minimum_required(VERSION 3.10.2)

enable_language(C)
enable_language(CXX)

project (AI_Core)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#opencv
#find_package( OpenCV REQUIRED )
#include_directories( ${OpenCV_INCLUDE_DIRS})

#  # #add lib opencv
set( OPENCV_LIB libopencv_world.so)
link_directories(${CMAKE_SOURCE_DIR}/libopencv-4.5.0-linux-x64-prebuilt/lib)
include_directories( ${CMAKE_SOURCE_DIR}/libopencv-4.5.0-linux-x64-prebuilt/include/opencv4)

# #add lib snpe
set( SNPE_LIB libSNPE.so)
link_directories(${CMAKE_SOURCE_DIR}/libSNPE-prebuilt/lib/x86_64-linux-clang)
include_directories(${CMAKE_SOURCE_DIR}/libSNPE-prebuilt/include/zdl)

set( libomp libomp.so)
link_directories(${CMAKE_SOURCE_DIR}/libSNPE-prebuilt/lib/x86_64-linux-clang)
set( libHtpPrepare libHtpPrepare.so)
link_directories(${CMAKE_SOURCE_DIR}/libSNPE-prebuilt/lib/x86_64-linux-clang)

#add lib ncnn
set( NCNN_LIB libncnn.a)
link_directories(${CMAKE_SOURCE_DIR}/libncnn-linux_x86_64/lib)
include_directories(${CMAKE_SOURCE_DIR}/libncnn-linux_x86_64/include)

#add lib paddle
#set(PADDLE libpaddle_inference.so)
#link_directories(${CMAKE_SOURCE_DIR}/libs/paddle_inference/paddle/lib)
#include_directories(${CMAKE_SOURCE_DIR}/libs/paddle_inference/paddle/include)

#add lib paddle-lite
set( PADDLE_LIB libpaddle_inference.so)
link_directories(${CMAKE_SOURCE_DIR}/paddle/paddle_inference/paddle/lib)
include_directories(${CMAKE_SOURCE_DIR}/paddle/paddle_inference/paddle/include)


include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/src/AiCore)
include_directories(${CMAKE_SOURCE_DIR}/src/AiCore/utils)
include_directories(${CMAKE_SOURCE_DIR}/src/AiCore/snpe)
include_directories(${CMAKE_SOURCE_DIR}/src/AiCore/snpe/detector)
include_directories(${CMAKE_SOURCE_DIR}/src/AiCore/ncnn)
include_directories(${CMAKE_SOURCE_DIR}/src/AiCore/sort-tracker)
include_directories(${CMAKE_SOURCE_DIR}/src/AiEngines/traffic)
include_directories(${CMAKE_SOURCE_DIR}/src/AiEngines/major)
#include_directories(${CMAKE_SOURCE_DIR}/src/AiEngines/traffic/Vehicle)
#include_directories(${CMAKE_SOURCE_DIR}/src/AiEngines/traffic/Vehicle/PaddleOCR-Linux-x64)


file( GLOB SOURCES  
src/AiCore/utils/*.cpp
src/AiCore/ncnn/*.cpp	
src/AiCore/snpe/*.cpp	
src/AiCore/snpe/detector/*.cpp	
src/AiCore/sort-tracker/*.cpp	
src/AiCore/*.cpp
src/AiCore/paddle/*.cpp
src/AiCore/paddle/paddle-core/*.cpp
src/AiEngines/traffic/Oppose.cpp
src/AiEngines/traffic/Invade.cpp
src/AiEngines/traffic/Speed.cpp
src/AiEngines/major/*.cpp
src/AiEngines/major/Litter/*.cpp
src/AiEngines/major/Children/*.cpp
src/AiEngines/Novaland/*.cpp
src/AiEngines/Anpr/*.cpp
src/AiEngines/AIBenchmark/*.cpp
src/AiEngines/Tracking/*.cpp
# src/main/main_trash.cpp
src/main/main_children.cpp



src/Decoder/OpencvDecoder.cpp
src/Decoder/DecoderThread.cpp
src/Decoder/FrameManager.cpp

src/AiEngines/Intrustion/IntrustionManager.cpp
)
                    
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}

${OPENCV_LIB}
${SNPE_LIB}
${NCNN_LIB}
${PADDLE_LIB}
#${PADDLE_LITE}
-Wall -lm -ldl -lpthread -lstdc++ -std=c++14 -lgomp -Wcpp -fPIC -march=x86-64
) 
