#ifndef Invade_hpp
#define Invade_hpp

#include <vector>

#include "ObjectTracking.hpp"
#include "ObjectDetector.hpp"

struct settingInvade
{
    std::vector<std::vector<cv::Point>> regionSet;
    std::vector<std::string> labelSet;
    std::vector<std::vector<int>> invadeSet; 
    std::vector<std::vector<std::string>> vehiclesSet;
};

enum ErrorCode
{
    InvadeLaneError = 1,
    WrongLaneError = 2,
    Undefine = -1
};

struct outputInvade
{
    cv::Rect rect;
    std::string label = "";
    float score;
    int track_id = 0;
    int idLane = 0;
    bool isOutOfFrame = false;
    ErrorCode typeError;
    bool isEvent = false;
};

class Invade
{
    private:
        ObjectDetector* detector = nullptr;
        ObjectTracking* tracker = nullptr;
        settingInvade settings;
        std::vector<std::vector<cv::Point>> regions;
        std::vector<std::string> labels;
        std::vector<std::vector<int>> rules;
        std::vector<std::vector<std::string>> vehicles;
        std::vector<outputInvade> listTracked;
    public:
        Invade();
        ~Invade();
        int initInvade();
        int setInvade(settingInvade settings);
        int updateInvade(cv::Mat& frame, std::vector<outputInvade>& output);
    private:
        bool checkLane(TrackingTrace track, std::vector<cv::Point> area);
        bool onSegment(cv::Point p, cv::Point q, cv::Point r);
        int orientation(cv::Point p, cv::Point q, cv::Point r);
        bool doIntersect(cv::Point p1, cv::Point q1, cv::Point p2, cv::Point q2);
        void checkTraces(Trace trace, int& UP, int& DOWN, int& LEFT, int& RIGHT);
};

#endif // Invade_hpp