/*
    Module: IntrustionManager.cpp
    Author: NGUYEN DUC TU
    Created: Jul 7th, 2022
*/

#ifndef IntrustionManager_hpp
#define IntrustionManager_hpp

#include<iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "../../AiCore/sort-tracker/ObjectTracking.hpp"
#include "../../AiCore/ObjectDetector.hpp"
#include "../../AiCore/AiTypeData.hpp"
#define THRESHOLD_VEHICLE 0.5



class ObjTracking
{
private:
    /* data */
    ObjectDetector* m_detector = nullptr;
    ObjectTracking* m_tracker = nullptr;
    std::vector<ObjectTrace> m_listObject;
public:
    ObjTracking();
    ~ObjTracking();
    int init(std::string typeModel, std::string objectTarget, std::string typeRuntime);
    int run(const cv::Mat& img, std::vector<ObjectTrace>& output);
    double calDistanceTrace(TrackingTrace track);
    double calDistanceTrace(ObjectTrace object);


    // bool isInside(TrackingTrace person, vector<cv::Point> area);

};

#endif