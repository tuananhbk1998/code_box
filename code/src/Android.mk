# Copyright (C) 2020, LE MANH THANG. All rights reserved.
# Module: Android.mk
# Author: LE MANH THANG
# Created: 21/12/2020
# Description: 

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
#inlcude linopencv-android-armv7a
LOCAL_C_INCLUDES := $(LOCAL_PATH)/libopencv-4.5.0-android-armv7a-prebuilt/sdk/native/jni/include
#include snpe
LOCAL_C_INCLUDES += $(LOCAL_PATH)/libSNPE-prebuilt/include/zdl
# include libncnn-android-armv7a
LOCAL_C_INCLUDES += $(LOCAL_PATH)/libncnn-android-armv7/include $(LOCAL_PATH)/libncnn-android-armv7/include/ncnn
#include libcurl-android-armv7a
LOCAL_C_INCLUDES += $(LOCAL_PATH)/libcurl-android/prebuilt-libssl/android-armeabi-v7a/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/libcurl-android/prebuilt-libcurl-master/android-armeabi-v7a/include
#include libevent
LOCAL_C_INCLUDES += $(LOCAL_PATH)/libevent/include
# include ffmpeg
LOCAL_C_INCLUDES += $(LOCAL_PATH)/libffmpeg/include/armeabi-v7a
#include curl
LOCAL_C_INCLUDES += $(LOCAL_PATH)/libcurl-android/prebuilt-libssl/android-armeabi-v7a/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/libcurl-android/prebuilt-libcurl-master/android-armeabi-v7a/include
# include mxnet
LOCAL_C_INCLUDES += $(LOCAL_PATH)/mxnet/include
#sqlite3
LOCAL_C_INCLUDES += $(LOCAL_PATH)/sqlite3/include
#inlcude libmnn-android-armv7a
LOCAL_C_INCLUDES += $(LOCAL_PATH)/libmnn-android/include
# include tflite-dist
LOCAL_C_INCLUDES += $(LOCAL_PATH)/tflite-dist/include
# include paddle-lite
LOCAL_C_INCLUDES += $(LOCAL_PATH)/libpaddle-lite-2.9/include

#include source
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src/AiCore
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src/AiCore/utils
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src/AiCore/ncnn
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src/AiCore/mnn
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src/AiCore/snpe
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src/AiCore/snpe/detector
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src/AiCore/snpe/classifier
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src/AiCore/face_recognizer
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src/AiCore/tflite

# include libtracker header
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src/AiCore/tracker/libtracker/common
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src/AiCore/tracker/libtracker
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src/AiCore/tracker/libtracker/graph
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src/AiCore/tracker/libtracker/graph/GTL/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src/AiCore/tracker/libtracker/graph/GTL/include/GTL
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src/AiCore/tracker/libtracker/dat
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src/AiCore/tracker

LOCAL_C_INCLUDES += $(LOCAL_PATH)/src/AiEngines/CoreObject
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src/AiEngines/Anpr

# share lib
#opencv
LOCAL_SHARED_LIBRARIES := libopencv_world-prebuilt
LOCAL_SHARED_LIBRARIES += libopencv_img_hash-prebuilt
#snpe
LOCAL_SHARED_LIBRARIES += libsnpe-prebuilt
#tflite
LOCAL_SHARED_LIBRARIES += libtensorflowlite
LOCAL_SHARED_LIBRARIES += libtensorflowlite_gpu_delegate
#ffmpeg
LOCAL_SHARED_LIBRARIES += libffmpeg
#libevnet
LOCAL_SHARED_LIBRARIES += event
# mxnet
LOCAL_SHARED_LIBRARIES += mxnet
#sqlite3
LOCAL_SHARED_LIBRARIES += sqlite3
#mnn
LOCAL_SHARED_LIBRARIES += libMNN-prebuilt
#paddle-lite
LOCAL_SHARED_LIBRARIES += libpaddle-lite-prebuilt
# static lib
#ncnn
LOCAL_STATIC_LIBRARIES := libncnn
LOCAL_STATIC_LIBRARIES += libglslang
LOCAL_STATIC_LIBRARIES += libOGLCompiler
LOCAL_STATIC_LIBRARIES += libOSDependent
LOCAL_STATIC_LIBRARIES += libSPIRV
#curl
LOCAL_STATIC_LIBRARIES += libcurl
LOCAL_STATIC_LIBRARIES += libssl 
LOCAL_STATIC_LIBRARIES += libcrypto

LOCAL_MODULE := ai_inference
#LOCAL_MODULE := poc
LOCAL_SRC_FILES := \
				src/AiCore/utils/ImageProcessor.cpp \
				src/AiCore/snpe/Util.cpp \
				src/AiCore/snpe/SnpeCommLib.cpp \
				src/AiCore/snpe/classifier/SnpeClassifier.cpp \
				src/AiCore/snpe/detector/SnpeMobilenetSSD.cpp \
				src/AiCore/snpe/detector/SnpeRetinanet.cpp \
				src/AiCore/snpe/detector/SnpeDetector.cpp \
				src/AiCore/mnn/UltraFace.cpp \
				src/AiCore/mnn/MnnFaceDetector.cpp \
				src/AiCore/mnn/MnnFaceLandmark.cpp \
				src/AiCore/mnn/MnnRetinaface.cpp  \
				src/AiCore/mnn/MnnDetector.cpp  \
				src/AiCore/face_recognizer/comm_lib.cpp \
				src/AiCore/face_recognizer/SnpeInsightface.cpp \
				src/AiCore/face_detection/retinaface/FaceDetector.cpp \
				src/AiCore/face_recognizer/SnpeMtcnn.cpp \
				src/AiCore/face_recognizer/SnpeRetinaFace.cpp \
				src/AiCore/ncnn/NcnnDetector.cpp \
				src/AiCore/tflite/TfliteDetector.cpp \
				src/AiCore/ObjectDetector.cpp \
				src/AiCore/ObjectClassifier.cpp  \
\
				src/AiCore/tracker/libtracker/Ctracker.cpp \
				src/AiCore/tracker/libtracker/ShortPathCalculator.cpp \
				src/AiCore/tracker/libtracker/track.cpp \
				src/AiCore/tracker/libtracker/Kalman.cpp \
				src/AiCore/tracker/libtracker/HungarianAlg/HungarianAlg.cpp \
				src/AiCore/tracker/libtracker/dat/dat_tracker.cpp \
				src/AiCore/tracker/libtracker/graph/tokenise.cpp \
				src/AiCore/tracker/libtracker/graph/mytree.cpp \
				src/AiCore/tracker/libtracker/graph/mygraph.cpp \
				src/AiCore/tracker/libtracker/graph/mwbmatching.cpp \
				src/AiCore/tracker/libtracker/graph/mincut.cpp \
				src/AiCore/tracker/libtracker/graph/gport.cpp \
				src/AiCore/tracker/libtracker/graph/fheap.c \
				src/AiCore/tracker/libtracker/graph/GTL/src/bellman_ford.cpp \
				src/AiCore/tracker/libtracker/graph/GTL/src/bfs.cpp \
				src/AiCore/tracker/libtracker/graph/GTL/src/biconnectivity.cpp \
				src/AiCore/tracker/libtracker/graph/GTL/src/bid_dijkstra.cpp \
				src/AiCore/tracker/libtracker/graph/GTL/src/components.cpp \
				src/AiCore/tracker/libtracker/graph/GTL/src/debug.cpp \
				src/AiCore/tracker/libtracker/graph/GTL/src/dfs.cpp \
				src/AiCore/tracker/libtracker/graph/GTL/src/dijkstra.cpp \
				src/AiCore/tracker/libtracker/graph/GTL/src/edge.cpp \
				src/AiCore/tracker/libtracker/graph/GTL/src/embedding.cpp \
				src/AiCore/tracker/libtracker/graph/GTL/src/fm_partition.cpp \
				src/AiCore/tracker/libtracker/graph/GTL/src/gml_parser.cpp \
				src/AiCore/tracker/libtracker/graph/GTL/src/gml_scanner.cpp \
				src/AiCore/tracker/libtracker/graph/GTL/src/graph.cpp \
				src/AiCore/tracker/libtracker/graph/GTL/src/maxflow_ff.cpp \
				src/AiCore/tracker/libtracker/graph/GTL/src/maxflow_pp.cpp \
				src/AiCore/tracker/libtracker/graph/GTL/src/maxflow_sap.cpp \
				src/AiCore/tracker/libtracker/graph/GTL/src/min_tree.cpp \
				src/AiCore/tracker/libtracker/graph/GTL/src/node.cpp \
				src/AiCore/tracker/libtracker/graph/GTL/src/planarity.cpp \
				src/AiCore/tracker/libtracker/graph/GTL/src/pq_node.cpp \
				src/AiCore/tracker/libtracker/graph/GTL/src/pq_tree.cpp \
				src/AiCore/tracker/libtracker/graph/GTL/src/ratio_cut_partition.cpp \
				src/AiCore/tracker/libtracker/graph/GTL/src/st_number.cpp \
				src/AiCore/tracker/libtracker/graph/GTL/src/topsort.cpp \
\
				src/AiCore/tracker/ObjectTracking.cpp \
				src/AiCore/tracker/RoadLine.cpp \
				src/AiEngines/CoreObject/Detector.cpp \
				src/AiEngines/CoreObject/Utility.cpp \
				src/AiEngines/CoreObject/CrosslineDetector.cpp \
				src/AiEngines/CoreObject/IntrusionDetector.cpp \
				src/AiEngines/CoreObject/CrowdDetector.cpp \
				src/AiEngines/CoreObject/DisappearanceDetector.cpp \
\
				src/AiEngines/Anpr/clipper.cpp \
				src/AiEngines/Anpr/db_post_process.cpp \
				src/AiEngines/Anpr/crnn_process.cpp \
				src/AiEngines/Anpr/ocr_db_crnn.cpp \
				src/AiEngines/Anpr/VehicleDetector.cpp \
				src/AiEngines/Anpr/AnprDetector.cpp \
				src/AiEngines/Anpr/TextDetection.cpp \
				src/AiEngines/Anpr/TextRecognizer.cpp \
				src/AiEngines/Anpr/AnprRecognizer.cpp \
\
				src/main_AICore.cpp \
				#				src/main_thanglmb.cpp\
#				src/AiEngine/AiFaceMaskdspProcess.cpp\
#				src/AiEngine/AiFaceMaskdsp.cpp\

LOCAL_CFLAGS += -fPIE -fPIC
LOCAL_CPPFLAGS += -fPIE -fPIC 
LOCAL_LDFLAGS += -fPIE -fPIC -Wl,--gc-sections,--allow-multiple-definition
LOCAL_LDLIBS := -lz -llog -ljnigraphics -landroid -lGLESv2 -lEGL -lvulkan  

include $(BUILD_EXECUTABLE)
#include $(BUILD_SHARED_LIBRARY)

#add lib opencv prebuilt
include $(CLEAR_VARS)
LOCAL_MODULE := libopencv_world-prebuilt
LOCAL_SRC_FILES := $(LOCAL_PATH)/libopencv-4.5.0-android-armv7a-prebuilt/sdk/native/libs/armeabi-v7a/libopencv_world.so
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/libopencv-4.5.0-android-armv7a-prebuilt/sdk/native/jni/include
include $(PREBUILT_SHARED_LIBRARY)
include $(CLEAR_VARS)

#add lib snpe prebuilt
include $(CLEAR_VARS)
LOCAL_MODULE := libsnpe-prebuilt
LOCAL_SRC_FILES := $(LOCAL_PATH)/libSNPE-prebuilt/lib/arm-android-clang6.0/libSNPE.so
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/libSNPE-prebuilt/include/zdl
include $(PREBUILT_SHARED_LIBRARY)
include $(CLEAR_VARS)

#  add lib ncnn
include $(CLEAR_VARS)
LOCAL_MODULE := libncnn
LOCAL_SRC_FILES := $(LOCAL_PATH)/libncnn-android-armv7/lib/libncnn.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libglslang
LOCAL_SRC_FILES := $(LOCAL_PATH)/libncnn-android-armv7/lib/libglslang.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libOGLCompiler
LOCAL_SRC_FILES := $(LOCAL_PATH)/libncnn-android-armv7/lib/libOGLCompiler.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libOSDependent
LOCAL_SRC_FILES := $(LOCAL_PATH)/libncnn-android-armv7/lib/libOSDependent.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libSPIRV
LOCAL_SRC_FILES := $(LOCAL_PATH)/libncnn-android-armv7/lib/libSPIRV.a
include $(PREBUILT_STATIC_LIBRARY)
include $(CLEAR_VARS)

#add lib openssl
include $(CLEAR_VARS)
LOCAL_MODULE := libssl
LOCAL_SRC_FILES := $(LOCAL_PATH)/libcurl-android/prebuilt-libssl/android-armeabi-v7a/lib/libssl.a
include $(PREBUILT_STATIC_LIBRARY)

#add lib crypto
include $(CLEAR_VARS)
LOCAL_MODULE := libcrypto
LOCAL_SRC_FILES := $(LOCAL_PATH)/libcurl-android/prebuilt-libssl/android-armeabi-v7a/lib/libcrypto.a
include $(PREBUILT_STATIC_LIBRARY)

#add lib libcurl
include $(CLEAR_VARS)
LOCAL_MODULE := libcurl
LOCAL_SRC_FILES := $(LOCAL_PATH)/libcurl-android/prebuilt-libcurl-master/android-armeabi-v7a/lib/libcurl.a
include $(PREBUILT_STATIC_LIBRARY)

#add lib event
include $(CLEAR_VARS)
LOCAL_MODULE := event
LOCAL_SRC_FILES := $(LOCAL_PATH)/libevent/lib/libevent.so
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/libevent/include
include $(PREBUILT_SHARED_LIBRARY)

#add lib libffmpeg
include $(CLEAR_VARS)
LOCAL_MODULE := libffmpeg
LOCAL_SRC_FILES := $(LOCAL_PATH)/libffmpeg/lib/armeabi-v7a/libffmpeg.so
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/libffmpeg/include/armeabi-v7a
include $(PREBUILT_SHARED_LIBRARY)

#add lib mxnet
include $(CLEAR_VARS)
LOCAL_MODULE := mxnet
LOCAL_SRC_FILES := $(LOCAL_PATH)/mxnet/lib/libmxnet.so
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/mxnet/include
include $(PREBUILT_SHARED_LIBRARY)

#add lib sqlite
include $(CLEAR_VARS)
LOCAL_MODULE := sqlite3
LOCAL_SRC_FILES := $(LOCAL_PATH)/sqlite3/lib/libsqliteX.so
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/sqlite3/include
include $(PREBUILT_SHARED_LIBRARY)
include $(CLEAR_VARS)

#add lib mnn prebuilt
include $(CLEAR_VARS)
LOCAL_MODULE := libMNN-prebuilt
LOCAL_SRC_FILES := $(LOCAL_PATH)/libmnn-android/armeabi-v7a/libMNN.so
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/libmnn-android/include
include $(PREBUILT_SHARED_LIBRARY)
include $(CLEAR_VARS)

#  add lib tensorflow
include $(CLEAR_VARS)
LOCAL_MODULE := libtensorflowlite
LOCAL_SRC_FILES := $(LOCAL_PATH)/tflite-dist/libs/android_arm/libtensorflowlite.so
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/tflite-dist/include
include $(PREBUILT_SHARED_LIBRARY)
include $(CLEAR_VARS)

include $(CLEAR_VARS)
LOCAL_MODULE := libtensorflowlite_gpu_delegate
LOCAL_SRC_FILES := $(LOCAL_PATH)/tflite-dist/libs/android_arm/libtensorflowlite_gpu_delegate.so
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/tflite-dist/include
include $(PREBUILT_SHARED_LIBRARY)
include $(CLEAR_VARS)

#add lib opencv prebuilt
include $(CLEAR_VARS)
LOCAL_MODULE := libopencv_img_hash-prebuilt
LOCAL_SRC_FILES := $(LOCAL_PATH)/libopencv-4.5.0-android-armv7a-prebuilt/sdk/native/libs/armeabi-v7a/libopencv_img_hash.so
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/libopencv-4.5.0-android-armv7a-prebuilt/sdk/native/jni/include
include $(PREBUILT_SHARED_LIBRARY)
include $(CLEAR_VARS)

#add lib paddle-lite prebuilt
include $(CLEAR_VARS)
LOCAL_MODULE := libpaddle-lite-prebuilt
LOCAL_SRC_FILES := $(LOCAL_PATH)/libpaddle-lite-2.9/lib/libpaddle_light_api_shared.so
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/libpaddle-lite-2.9/include
include $(PREBUILT_SHARED_LIBRARY)
include $(CLEAR_VARS)