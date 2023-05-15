#ifndef __SPEED_HPP__
#define __SPEED_HPP__

#include <iostream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include "../AiCore/ObjectDetector.hpp"
#include "../AiCore/sort-tracker/ObjectTracking.hpp"

typedef struct 
{
    
    std::vector<std::vector<std::string> > vehiclesSet;
    std::vector<std::vector<cv::Point>> regionSet;
    std::vector<cv::Point2f> lineSet;
    std::vector<std::vector<int>> invadeSet; 
    int iDistancePerRegion; 
} SpeedSetting;

typedef struct 
{
    cv::Rect rect;
    int track_id = 0;
    bool isOutOfFrame = false;
    std::string label = "";
    bool isEvent = false;
    double timeAccessed = 0;
    double timeIn = 0;
    double speed = 0;
    int position;
    std::vector<double> vTempSpeed;
    std::vector<cv::Point2f> trackTrace;
    
} SpeedOutput;

typedef struct 
{
    double distance;
    std::vector<cv::Point2f> position;
    
} Region_t;
class Speed
{
    private:
        ObjectDetector* m_Detector = nullptr;
        ObjectTracking* m_Tracker = nullptr;
        std::vector<Region_t> m_Region;
        std::vector<std::vector<int>> m_Line;
        std::vector<std::vector<std::string> > m_Vehicles;
        std::vector<SpeedOutput> m_listObject;
        int m_DistancePerRegion;
        double frequency = 0;
        bool checkLane(TrackingTrace track, std::vector<cv::Point> area);
        void makeRegions(SpeedSetting setting);
        bool isInside(cv::Point2f track, std::vector<cv::Point2f> area);
        double calcSpeed(SpeedOutput object, Region_t stRegion, double timeMove);
    public:
        Speed();
        ~Speed();
        int initSpeed();
        int settingSpeed(SpeedSetting st_Settings);
        int updateSpeedv1(cv::Mat& frame, std::vector<SpeedOutput>& outputs);
        int updateSpeedv2(cv::Mat& frame, std::vector<SpeedOutput>& outputs);
        int updateSpeedv3(cv::Mat& frame, std::vector<SpeedOutput>& outputs);
        int updateSpeedv4(cv::Mat& img, std::vector<SpeedOutput>& outputs);
       
};

#endif // !__LANLAN_HPP__
