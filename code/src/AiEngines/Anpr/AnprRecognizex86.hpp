/*
    Module: AnprRecognizer.cpp
    Author: ThangLmb
    Date: June 24, 2021
*/

#ifndef AnprRecognizerx86_hpp
#define AnprRecognizerx86_hpp

#include <regex>
// #include "ocr_db_crnn.hpp"
#include "AnprDetector.hpp"
#include "ObjectTracking.hpp"
#include "MotionDetector.hpp"
#include "../AiCore/paddle/OCR.hpp"

#define MAX_COUNT_UNKNOWN 10
#define THRESHOLD_ANPR 0.8

using namespace std;
using namespace cv;

typedef struct OcrConfig
{
    std::vector<std::string> dict;
    std::map<std::string, double> configOCR;
    std::shared_ptr<PaddlePredictor> detector;
    std::shared_ptr<PaddlePredictor> recog;
}OcrConfig;


struct PlateInfor
{
    cv::Mat imgPlate;
    cv::Rect rect;
    std::string license = "";
    std::string typePlate = "";
    float score;
    int track_id = 0;
    bool isOutOfFrame = false;
    bool isNewEvent = false;
    uint8_t countUnknown = 0;
};
struct VehicleInfor
{
    std::string typeVehicle = "";
    cv::Rect rectVehicle;
    int track_id;
};
struct ITS_Output
{
    PlateInfor plate;
    VehicleInfor vehicle;
};
class AnprRecognizerx86
{
    
private:
    /* data */
    AnprDetector* detector = nullptr;
    std::vector<PlateInfor> listPlateTracks;
    OCR ocr;
    std::string readText( cv::Mat& img, Nations nation, float& confidence);
    bool isValidPlate(cv::Mat& img);
    ObjectDetector* m_detector = nullptr;
    ObjectTracking* m_tracker = nullptr;
    std::vector<ObjectTrace> m_listObject;
public:
    AnprRecognizerx86(/* args */);
    ~AnprRecognizerx86();
    // int init(Nations nation);
    int recognize( cv::Mat& img, std::vector<PlateInfor>& plates);
    int only_recognize(cv::Mat& img, std::vector<PlateInfor>& plates);

    int recognize_traffic(cv::Mat mFHDImg, cv::Mat mUHDImg, ObjectTrace& objects, PlateInfor& plates);
    int detect_traffic(cv::Mat& img,std::vector<ObjectTrace>& objs);
    int its(cv::Mat mFHDImg, cv::Mat mUHDImg, ITS_Output outputs);
    // int recognize( cv::Mat& img, std::vector<PlateInfor>& plates, bool checkIsMotion, Rect& rectMotion);
    // int trackAnpr( Mat &img, std::vector<PlateInfor>& plates);
};


#endif