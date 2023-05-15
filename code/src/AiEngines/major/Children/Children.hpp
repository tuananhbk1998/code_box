/********************************************************************************
<Copyright Notice>
Product: AI Camera
Module: AI Core
Version: 2.0
File: Children.hpp
Author: MAI TUAN ANH
Created: 7-March-23
Released: <Date>
Description: Header of class Children
Note: <Note>
********************************************************************************/

#ifndef Children_hpp
#define Children_hpp

#define TagChildren "[Children]"
#include<deque>
#include <iostream>
#include <vector>
#include <string>
#include "ObjectDetector.hpp"
#include "ObjectTracking.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#define THRESH_OBJECT 0.4

typedef struct stSettingChild {
    // std::vector<cv::Point> ptRegionSet;
    std::vector<cv::Point> SetRegion_1;
    std::vector<cv::Point> SetRegion_2;
    std::vector<cv::Point> SetRegion_3;
    std::vector<cv::Point> SetRegion_4;
    std::vector<cv::Point> SetRegion_5;
    std::vector<cv::Point> SetRegion_6;
    std::vector<cv::Point> SetRegion_7;
    std::vector<cv::Point> SetRegion_8;
    std::vector<cv::Point> SetRegion_9;
    std::vector<cv::Point> SetRegion_10;
    std::vector<cv::Point> SetRegion_11;
    std::vector<cv::Point> SetRegion_12;
    std::vector<cv::Point> SetRegion_13;
    std::vector<cv::Point> SetRegion_14;
    std::vector<cv::Point> SetRegion_15;
    std::vector<cv::Point> SetRegion_16;
    std::vector<cv::Point> SetRegion_17;
    std::vector<cv::Point> SetRegion_18;
    std::vector<cv::Point> SetRegion_19;
    std::vector<cv::Point> SetRegion_20;
    std::vector<cv::Point> SetRegion_21;
    std::vector<cv::Point> line1;
    std::vector<cv::Point> line2;
    std::vector<cv::Point> line3;
    std::vector<cv::Point> line4;
    std::vector<cv::Point> line5;
    std::vector<cv::Point> line6;
    std::vector<cv::Point> line7;
    std::vector<cv::Point> line8;
    std::vector<cv::Point> line9;



    std::vector<std::string> strObjectSet;
} stSettingChild_t;

struct OutputChild {
    cv::Rect Box_Child;
    cv::Rect Box_Adult;
    cv::Rect Box_motion;

};

class Children {
    public:
        Children();
        ~Children();

        int Child_Init();
        int Child_Set(stSettingChild_t stSetting);
        int Child_Execute(cv::Mat &Image, std::vector<OutputChild>& Child);
        int Child_Execute_2(cv::Mat &Image, std::vector<OutputChild>& Child);
        int Child_Execute_3(cv::Mat &Image, std::vector<OutputChild>& Child);
        std::vector<cv::Rect> getMotion(cv::Mat &mImage, cv::Mat &preImage, std::vector<cv::Rect> &contours);
        bool CheckInside_1(cv::Rect &contour, std::vector<cv::Point> Area_1);
        bool CheckInside_2(cv::Rect &contour, std::vector<cv::Point> Area_2);
        bool CheckInside_3(cv::Rect &contour, std::vector<cv::Point> Area_3);
        int CalcChangePixel(cv::Mat image, cv::Mat previousImage, std::vector<cv::Point> region);
        

    private:
        ObjectDetector* m_ObjectDetector = nullptr;
        ObjectDetector* m_ChildDetector = nullptr;
        std::vector<cv::Point> m_Region;
        std::vector<cv::Point> Region_Check1;
        std::vector<cv::Point> Region_Check2;
        std::vector<cv::Point> Region_Check3;
        std::vector<cv::Point> Region_Check4;
        std::vector<cv::Point> Region_Check5;
        std::vector<cv::Point> Region_Check6;
        std::vector<cv::Point> Region_Check7;
        std::vector<cv::Point> Region_Check8;
        std::vector<cv::Point> Region_Check9;
        std::vector<cv::Point> Region_Check10;
        std::vector<cv::Point> Region_Check11;
        std::vector<cv::Point> Region_Check12;
        std::vector<cv::Point> Region_Check13;
        std::vector<cv::Point> Region_Check14;
        std::vector<cv::Point> Region_Check15;
        std::vector<cv::Point> Region_Check16;
        std::vector<cv::Point> Region_Check17;
        std::vector<cv::Point> Region_Check18;
        std::vector<cv::Point> Region_Check19;
        std::vector<cv::Point> Region_Check20;
        std::vector<cv::Point> Region_Check21;
        std::vector<std::string> m_strObject;
        cv::Rect2f m_boxRegion;
        cv::Mat preImage;
        std::vector<cv::Rect> contours;
        float NumberPixelChange;

        

};

#endif // Children_hpp