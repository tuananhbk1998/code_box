// #ifndef __LANLAN_HPP__
// #define __LANLAN_HPP__

// #include <iostream>
// #include <vector>
// #include <string>
// #include "ObjectDetector.hpp"
// #include "ObjectTracking.hpp"

// typedef struct 
// {
//     std::vector<cv::Point> regionSet; 
//     std::vector<std::vector<std::string> > vehiclesSet;

// } settingLanlan;

// typedef struct HeSo
// {
//     float heSoX;
//     float heSoY;
//     float heSoTuDo;
// }HeSo_t;

// typedef struct 
// {
//     cv::Rect rect;
//     int track_id = 0;
//     bool isOutOfFrame = false;
//     std::string label = "";
//     bool position ;
//     bool isEvent = true;
    
// } OutputLanlan;

// class Lanlan
// {
//     private:
//         ObjectDetector* detector = nullptr;
//         ObjectTracking* tracker = nullptr;
//         std::vector<cv::Point>  m_region;
//         bool flag = false;
//         std::vector<std::vector<std::string> > m_vehicles;
//         std::vector<OutputLanlan> mListTracked;
//         std::vector<std::string> text_lane = {"Car lane", "Motorcycle lane"};
//     public:
//         Lanlan();
//         ~Lanlan();
//         int initLanlan();
//         int setLanlan(settingLanlan settings);
//         int updateLanlan(cv::Mat& frame, std::vector<OutputLanlan>& outputs);
//     private:
//         bool checkLane(TrackingTrace track, std::vector<cv::Point> area);
// };

// #endif // !__LANLAN_HPP__
