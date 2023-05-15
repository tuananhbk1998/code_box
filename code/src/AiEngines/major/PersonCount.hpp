#ifndef PERSONCOUNT_HPP
#define PERSONCOUNT_HPP
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "../AiCore/ObjectDetector.hpp"

#define PERCENT_EDGE 0.03
#define PERCENT_AREA 0.4
using namespace cv;
typedef struct PersonCountOutput
{
    cv::Rect rect;
    float score;
    std::string type;
    float area;
}PersonCountOutput_t;

class PersonCount
{
    private:
        ObjectDetector* m_detector = nullptr;
        std::vector<std::vector<cv::Point2f>> m_edgeZone;
        std::vector<float> m_valueArea;
        bool isOnEdge(ObjectTrace object, std::vector<std::vector<cv::Point2f>> edgeZone);
        bool isInside(ObjectTrace object, std::vector<cv::Point2f> area);
        float getStandardArea();

    public:
        PersonCount();
        ~PersonCount();
        int Setting(int frameWidth, int frameHeight);
        int InitAI();
        int Execute(cv::Mat& frame, std::vector<PersonCountOutput>& outputs);
};




#endif