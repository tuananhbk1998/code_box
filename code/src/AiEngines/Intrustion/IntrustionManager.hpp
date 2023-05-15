/*
    Module: IntrustionManager.cpp
    Author: NGUYEN DUC TU
    Created: Apr 20th, 2021
*/

#ifndef IntrustionManager_hpp
#define IntrustionManager_hpp

#include<iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "../../AiCore/sort-tracker/ObjectTracking.hpp"
#include "../AiCore/ObjectDetector.hpp"

#define THRESHOLD_VEHICLE 0.5


struct PersonTrace{
    cv::Rect rect;

    float score;
    int track_id = 0;
    bool isOutOfFrame = false;
    double start;
    double duration;
    bool isNew = false;
};

class IntrustionManager
{
private:
    /* data */
    ObjectDetector* m_detector = nullptr;
    ObjectTracking* m_tracker = nullptr;
    std::vector<PersonTrace> m_listPerson;
    std::vector<PersonTrace> m_listInside;
    double frequency = 0;
public:
    IntrustionManager();
    ~IntrustionManager();
    int initIntrustion();
    int run(const cv::Mat& img, std::vector<PersonTrace>& output, vector<cv::Point> area);
    bool isInside(TrackingTrace person, vector<cv::Point> area);

};

#endif