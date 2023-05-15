/********************************************************************************
<Copyright Notice>
Product: AI Camera
Module: AI Core
Version: 2.0
File: Litter.hpp
Author: QuangNTd
Created: 14-Jul-22
Modified:
    <Name> QuangNTd
    <Date> 28-Aug-22
    <Change> 
Released: <Date>
Description: Header of class Litter
Note: <Note>
********************************************************************************/

#ifndef Litter_hpp
#define Litter_hpp

#define TagLitter "[Litter]"
#include<deque>
#include "ObjectDetector.hpp"
#include "ObjectTracking.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#define THRESH_TRASH 0.3
#define THRESH_OBJECT 0.4
#define THRESH_TRASH_MOVE 1

/**
 * @brief Structure of configuration Litter
 * 
 */
typedef struct stSettingLitter
{
    std::vector<cv::Point> ptRegionSet;
    std::vector<std::string> strObjectSet;
} stSettingLitter_t;

/**
 * @brief Structure of trash object
 * 
 */
typedef struct stTrash
{
    cv::Rect riBox;
    std::string strType = "";
    int iTrackID = 0;
    bool bOutOfFrame = false;
} stTrash_t;

/**
 * @brief Structure of output Litter
 * 
 */
typedef struct stOutputLitter
{
    cv::Rect riBox;
    std::string strType = "";
    int iTrackID = 0;
    bool bOutOfFrame = false;
    stTrash_t stTrash;
    bool bEvent = false;
    cv::Mat imgEvent;
} stOutputLitter_t;

/**
 * @brief Class that contains the functions of Litter
 * 
 */
class Litter
{
    public:
        /**
         * @brief Construct a new Litter object
         * 
         */
        Litter();

        /**
         * @brief Destroy the Litter object
         * 
         */
        ~Litter();

        /**
         * @brief Initialize the Litter object
         * 
         * @return int : 0 if initialized successfully; -1, otherwise
         */
        int LT_Init();

        /**
         * @brief Set up the Litter object
         * 
         * @param stSetting Structure of configuration Litter
         * @return int : 0 if initialized successfully; -1, otherwise
         */
        int LT_Set(stSettingLitter_t stSetting);

        /**
         * @brief Execute the Litter object
         * 
         * @param mImage Input image format cv::Mat
         * @param[in,out] stLitters A vector of stOutputLitter_t that will hold the output data
         * @return int : 0 if initialized successfully; -1, otherwise
         */
        int LT_Exe(cv::Mat& mImage, std::vector<stOutputLitter_t>& stLitters);
        int LT_Exe2(cv::Mat& mImage, std::vector<stOutputLitter_t>& stLitters);
        int LT_Exe3(cv::Mat mImage, std::vector<stOutputLitter_t>& stLitters);

    private:
        ObjectDetector* m_pObjectDetector = nullptr;
        ObjectDetector* m_pTrashDetector = nullptr;
        ObjectTracking* m_pTrashTracker = nullptr;
        std::vector<int> m_whitePixels;
        cv::Mat m_previousMat;
        cv::Mat outputResize;
        std::deque<cv::Mat> m_previousMotionList;
        cv::Rect2f m_boxRegion;
        int m_count = 0;
        float m_fChangeDelta;
        bool m_bIsRegionChanged;
        bool m_bIsMotion = false;
        bool m_bIsCalculating = false;
        int m_Color;
        std::vector<std::vector<cv::Point>> contoursColors;


        std::vector<cv::Point> m_ptRegion;
        std::vector<std::string> m_strObject;

        // std::vector<stTrash_t> m_stTrashTracks;
        std::vector<stOutputLitter_t> m_stLitterTracks;

    private:
        /**
         * @brief 
         * 
         * @param stTrack 
         * @param ptArea 
         * @return true 
         * @return false 
         */
        bool LT_bCheckInside(TrackingTrace& stTrack, std::vector<cv::Point> ptArea);

        /**
         * @brief 
         * 
         * @param stTrack 
         * @return true 
         * @return false 
         */
        bool LT_bCheckMove(TrackingTrace& stTrack);

        /**
         * @brief 
         * 
         * @param stLitter 
         * @param stObjects 
         */
        void LT_vGetObject(stOutputLitter_t& stLitter, std::vector<ObjectTrace>& stObjects);
        std::vector<cv::Rect2f> getMotionContours(cv::Mat image, cv::Mat& previousImage);
        int countFeature(cv::Mat img, std::vector<cv::Point> region);
        float _getPercentageOverlap(cv::Rect2f rectObject, cv::Rect2f rectMotion);
        bool _checkOverlapRect(cv::Rect2f rectObject, std::vector<cv::Rect2f> rectMotion);
        float getAverageFeature();
        int calcCoutour(cv::Mat image, cv::Mat previousImage, std::vector<cv::Point> region);
        cv::Rect2f pad_rectangle(cv::Mat image, cv::Rect_<float> rect, int paddingPercent);
        cv::Rect reSize(cv::Mat& image, cv::Mat& output);
        int getColor(cv::Mat &frame, std::vector<std::vector<cv::Point>> &contoursColor, std::vector<cv::Point> region_corlor);



        // int getCoutour(cv::Mat img);

};

#endif // Litter_hpp