/*
* To compile: g++ opencv_test2.cpp -o opencv_test2 $(pkg-config --cflags --libs opencv)
*/
#ifndef  MotionDetector_hpp
#define MotionDetector_hpp
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core.hpp"
#include "opencv2/core/utility.hpp"
#include <iostream>
#include <unistd.h>

using namespace cv;
using namespace std;
#define AREA_DEFAULT 300
class MotionDetector{

    private:
        cv::Mat lastFrame;
        cv::Mat previousImage;
    public:
        MotionDetector();
        ~MotionDetector();
        int isMotionDetected(cv::Mat& frame, cv::Rect& rect);
        std::vector<cv::Rect> getMotionContours(cv::Mat image); 
        void SetThreshold(int threshold);
};

#endif