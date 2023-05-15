// Lấn làn
#ifndef Invade_hpp
#define Invade_hpp

#include <vector>

#include "ObjectTracking.hpp"
#include "ObjectDetector.hpp"
#include "../Anpr/AnprRecognizex86.hpp"
struct settingInvade
{
    std::vector<std::vector<cv::Point2f>> regionSet;
    std::vector<std::string> labelSet;
    std::vector<std::vector<int>> invadeSet; 
    std::vector<std::vector<std::string>> vehiclesSet;
};

enum ErrorCode_t
{
    InvadeLaneError = 2,
    WrongLaneError = 1,
    BothError = 3,
    Undefine = 0
};
typedef struct 
{
    std::string relativePosition;
    std::vector<cv::Point2f> zone;
    
} RegionInvade_t;

struct outputInvade
{
    cv::Rect rect;
    std::string label = "";
    float score;
    int track_id = 0;
    int idLane = 2;
    std::string typeLane = "";
    bool isOutOfFrame = false;
    ErrorCode_t typeError;
    bool isEvent = false;
    std::vector<std::string> listLicensePlate;
    std::vector<cv::Mat> listImgPlate;
    PlateInfor eventPlate;
    cv::Mat eventImage;
    cv::Rect eventRect;
};

class Invade
{
    private:
        ObjectDetector* m_Detector = nullptr;
        ObjectTracking* m_Tracker = nullptr;
        AnprRecognizerx86* m_License = nullptr;
        settingInvade m_Settings;
        std::vector<RegionInvade_t> m_Regions;
        std::vector<std::string> m_Labels;
        std::vector<std::vector<int>> m_Rules;
        std::vector<std::vector<std::string>> m_Vehicles;
        std::vector<outputInvade> m_ListTracked;
    public:
        Invade();
        ~Invade();
        int initInvade();
        int setInvade(settingInvade settings);
        int updateInvade1(cv::Mat& frame, std::vector<outputInvade>& output);
        int updateInvade2(cv::Mat& frame, std::vector<outputInvade>& output);
        
    private:
        bool checkLane(TrackingTrace track, std::vector<cv::Point2f> area);
        bool checkLaneLeft(TrackingTrace track, std::vector<cv::Point2f> area);
        bool checkLaneRight(TrackingTrace track, std::vector<cv::Point2f> area);
        bool onSegment(cv::Point p, cv::Point q, cv::Point r);
        int orientation(cv::Point p, cv::Point q, cv::Point r);
        bool doIntersect(cv::Point p1, cv::Point q1, cv::Point p2, cv::Point q2);
        void checkTraces(Trace trace, int& UP, int& DOWN, int& LEFT, int& RIGHT);
        std::string relativePosition(std::vector<std::vector<cv::Point>> region);
        void makeRelativePosition(std::vector<RegionInvade_t>& regions);
        bool isExistErrorObj(std::vector<outputInvade> listTrack);
        bool isInside(ObjectTrace object, std::vector<cv::Point2f> area);
        std::string getLegitLicense(std::vector<std::string> listLicensePlate, int &index);
};

#endif // Invade_hpp