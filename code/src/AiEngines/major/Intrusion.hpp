#ifndef INTRUSION_HPP
#define INTRUSION_HPP

#include <vector>
#include <unistd.h>

#include "ObjectTracking.hpp"
#include "ObjectDetector.hpp"

#define THRES_DETECT_INTRUSION_NIGHT 0.35
#define THRES_DETECT_INTRUSION_DAY 0.5
#define THRES_MOVE 20

struct settingIntrusion
{
    std::vector<std::string> objectsTrack;
    std::vector<cv::Point> regionSet;
    uint32_t allowed_amount = 1;
    uint32_t allowed_time = 0;
    uint32_t repeated_time = 0;
};

struct outputIntrusion
{
    cv::Rect rect;
    std::string label = "";
    float score;
    double timeIn = 0;
    double timeAccessed = 0;
    int track_id = 0;
    bool isOutOfFrame = false;
    bool isNew = false;
};

enum TypeWarning
{
    No = -1,
    First = 0,
    Repeat = 1
};

class Intrusion
{
    private:
        ObjectDetector* detector = nullptr;
        ObjectTracking* tracker = nullptr;
        
        double frequency = 0;

        std::vector<std::string> m_objects;
        std::vector<cv::Point> m_region;
        uint32_t m_allowedAmount = 1;
        uint32_t m_allowedTime = 0;
        uint32_t m_repeatedTime = 0;

        double m_timeWarning = 0;

        std::vector<outputIntrusion> listTracked;
    public:
        Intrusion();
        ~Intrusion();
        int initIntrusion();
        int setIntrusion(settingIntrusion settings);
        int updateIntrusion(cv::Mat &frame, std::vector<outputIntrusion> &output);
        int updateIntrusion2(cv::Mat &frame, std::vector<outputIntrusion> &outputs);
        bool isWarning = false;
        TypeWarning typeWarning = TypeWarning::No;

    private:
        bool isInside(TrackingTrace track, std::vector<cv::Point> area);
        bool isMove(TrackingTrace track);
        bool isObject(TrackingTrace track, int width, int height);
        bool isPerson(TrackingTrace track, int width, int height);
        bool isGrayscale(cv::Mat crop);
};

#endif // INTRUSION_HPP
