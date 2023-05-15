#ifndef VEHICLE_TRACKING_HPP
#define VEHICLE_TRACKING_HPP

#include <vector>

#include "ObjectDetector.hpp"
#include "ObjectTracking.hpp"

#include "config.h"
#include "ocr_det.h"
#include "ocr_rec.h"
#include "utility.h"

#define CONFIG_FILE "/home/na/Desktop/tracking/AiCorex86_64/src/AiEngines/traffic/Vehicle/PaddleOCR-Linux-x64/config.txt"

enum Nation
{
    VN = 1,
    VnSquare = 2,
    VnRect = 3,
    US = 4,
    MALAY = 5,
    UNDEFINED = -1
};

struct settingVehicleTracking
{
    std::vector<std::string> ObjectsTrack;
    bool TrackAll = false;
    bool OptLicense = false;
    Nation nation;
};

struct outputVehicleTracking
{
    cv::Rect rect;
    cv::Mat imgVehicle;
    std::string label = "";
    float scoreVehicle;
    cv::Mat imgPlate;
    bool havePlate = false;
    cv::Rect2f rectPlate;
    std::string license = "";
    std::string typePlate = "";
    float scorePlate;
    int track_id = 0;
    bool isOutOfFrame = false;
};

class VehicleTracking
{
    private:
        ObjectDetector* detector = nullptr;
        ObjectTracking* tracker = nullptr;
        ObjectDetector* plateDetect = nullptr;
        // OcrConfig* ocrVN = nullptr;
        // OcrConfig* ocrUS = nullptr;
        // OcrConfig* ocrMalay = nullptr;

        int m_width = 0;
        int m_height = 0;
        PaddleOCR::DBDetector* m_det;
        PaddleOCR::Classifier* m_cls;
        PaddleOCR::CRNNRecognizer* m_rec;

        settingVehicleTracking settings;
        std::vector<outputVehicleTracking> listTracked;
    public:
        VehicleTracking();
        ~VehicleTracking();
        int initVehicleTracking(settingVehicleTracking settings);
        // int setVehicleTracking(settingVehicleTracking settings);
        int updateVehicleTracking(cv::Mat &frame, std::vector<outputVehicleTracking> &output);
    private:
        cv::Mat getPlate(cv::Mat& imgVehicle, ObjectTrace& plate);
        bool isValidPlate(cv::Mat& imgPlate);
        int initPaddleOCR();
        std::string runPaddleOCR(cv::Mat& imgPlate);
};

#endif // VEHICLE_TRACKING_HPP