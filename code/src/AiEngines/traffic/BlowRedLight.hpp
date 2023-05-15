#ifndef BlowRedLight_hpp
#define BlowRedLight_hpp

#include <vector>

#include "ObjectTracking.hpp"
#include "ObjectDetector.hpp"

struct settingBlowRedLight
{
    std::vector<std::string> allowedObjects;
    std::vector<std::vector<cv::Point>> regionSet;
    std::vector<std::string> labelSet;
    int allowLeft;
    int allowRight;
};

enum LightState
{
    RED = 0,
    YELLOW = 1,
    GREEN = 2,
    OFF = 3,
    UNDEFINED = -1
};

enum TypeError
{
    RedLight = 0,
    YellowLight = 1,
    OverTheLine = 2
};

struct outputBlowRedLight
{
    cv::Rect rect;
    std::string label = "";
    float score;
    int track_id = 0;
    bool isOutOfFrame = false;
    bool isBlowRedLight = false;
    // bool isBlowYellowLight = false;
    bool isOverTheLine = false;
    // bool isStopYellowLight = false;
    TypeError typeError;
    bool left = true;
    bool right = true;
    bool straight = true;
    // std::vector<cv::Mat> images;
    cv::Mat imgEvent;
};

class BlowRedLight
{
    private:
        ObjectDetector* detector = nullptr;
        ObjectTracking* tracker = nullptr;
        settingBlowRedLight settings;
        std::vector<outputBlowRedLight> listTracked;

        std::vector<std::vector<cv::Point>> leftLight, standardLight;
        std::vector<cv::Point> beginLine, endLine, leftLine, rightLine;
        bool isBeginLine, isEndLine, isLeftLine, isRightLine;
        std::vector<Point> fullLane, straightLane, leftLane, rightLane, straight_leftLane, straight_rightLane;
        LightState leftSignal = LightState::UNDEFINED;
        LightState standardSignal = LightState::UNDEFINED;
        int allow2TurnRight, allow2TurnLeft;

        LightState getCurrentLightState(const cv::Mat &input, std::vector<std::vector<cv::Point>> light);
        std::string getStatus(LightState standardSignal, LightState leftSignal);
        bool checkLane(TrackingTrace track, std::vector<cv::Point> area);
        bool allow2turn(int allowTurn, std::string type);

    public:
        BlowRedLight();
        ~BlowRedLight();
        int initBlowRedLight();
        int setBlowRedLight(settingBlowRedLight settings);
        int updateBlowRedLight(cv::Mat &frame, std::vector<outputBlowRedLight> &output);
};

#endif // BlowRedLight_hpp