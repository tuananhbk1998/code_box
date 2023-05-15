/*
    Module: ITS.cpp
    Author: TuND
    Date: Nov 02, 2022
*/

#ifndef ITS_hpp
#define ITS_hpp

#include <regex>
// #include "ocr_db_crnn.hpp"
#include "AnprDetector.hpp"
#include "ObjectTracking.hpp"
#include "MotionDetector.hpp"
#include "../AiCore/paddle/OCR.hpp"

using namespace std;
using namespace cv;
#define MAX_COUNT_UNKNOWN 5
#define MAX_COUNT_MATCH 5

#define WIDTH_4K 3840
#define HEIGHT_4K 2160
#define SCALE_FHD_4K 2
using namespace std;
enum TypeOutput
{
    Both = 2,
    OnlyPlate = 1,
    OnlyVehicle = 3,
    Undefine = 0
};
struct PlateInfor
{
    cv::Mat imgPlate;
    cv::Rect rect;
    std::string license = "";
    std::string typePlate = "";
    float score;
    int track_id = 0;
    bool bOutOfFrame = false;
    bool bRecognized = false;
    uint8_t countUnknown = 0;
};
struct VehicleInfor
{
    std::string typeVehicle = "";
    cv::Rect rectVehicle;
    int iTrackID = 0;
    bool bOutOfFrame = false;
    uint8_t countMatch = 0;
    bool isMatch  = 0;
    unsigned short matchID = 0;
};
struct ITS_Output
{
    PlateInfor plate;
    VehicleInfor vehicle;
    int matchID;
    TypeOutput typeOutput;
};
class ITS
{
    
private:
    /* data */
    AnprDetector* m_PlateDetect = nullptr;
    OCR ocr;
    ObjectDetector* m_VehicleDetector = nullptr;
    ObjectTracking* m_PlateTracker = nullptr;
    ObjectTracking* m_VehicleTracker = nullptr;
    std::vector<VehicleInfor> m_listVehicles;
    std::vector<PlateInfor> m_listPlateTracks;
    std::vector<ITS_Output> m_listOutputs;

    std::string readText( cv::Mat& img, Nations nation, float& confidence);
    bool isValidPlate(cv::Mat& img);
    bool checkPlate(PlateInfor& plate, int cols, int rows);
    cv::Rect2f getRect4K(cv::Rect2f rect_fhd, int cols_4K, int rows_4K);

public:
    ITS();
    ~ITS();
    int init(Nations nation);

    int Execute(cv::Mat& mFHDImg, cv::Mat mUHDImg, std::vector<ITS_Output>& outputs);
    int Execute2(cv::Mat& mFHDImg, cv::Mat mUHDImg, std::vector<ITS_Output>& outputs);
    bool MatchingVehicle(PlateInfor& stPlate, std::vector<VehicleInfor>& stVehicleTracks, std::vector<ITS_Output>& outputs);

};

#endif