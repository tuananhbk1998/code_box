#ifndef Paddle_H
#define Paddle_H
// #include "glog/logging.h"
#include "omp.h"
#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <vector>

#include <cstring>
#include <fstream>
#include <numeric>

// #include <glog/logging.h>
#include "paddle-core/config.h"
#include "paddle-core/ocr_det.h"
#include "paddle-core/ocr_rec.h"
#include "paddle-core/utility.h"
#include <sys/stat.h>

using namespace std;
using namespace cv;
using namespace PaddleOCR;

class OCR{
    private:

        DBDetector det;
        Classifier *cls = nullptr;
        CRNNRecognizer rec;
    public:
        OCR();
        ~OCR();
        // int initPaddle();
        int RunOCR(cv::Mat frame, std::string& out );
};
#endif