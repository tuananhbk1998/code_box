/********************************************************************************
<Copyright Notice>
Product: AI Camera
Module: AI Core
Version: 2.0
File: Litter.cpp
Author: QuangNTd
Created: 14-Jul-22
Modified:
    <Name> QuangNTd
    <Date> 28-Aug-22
    <Change>
Released: <Date>
Description: Implement of class Litter
Note: <Note>
********************************************************************************/

#include "Litter.hpp"

/*-------------------------------------------------------------------------------
Function: Litter()
Purpose: Construct a new Litter object
Parameters: 
Return: 
Comments: 
Modified: 
    <Modified by>
    <Date>
    <Change>
-------------------------------------------------------------------------------*/
Litter::Litter()
{
    m_pObjectDetector = new ObjectDetector();
    m_pTrashDetector = new ObjectDetector();
    m_pTrashTracker = new ObjectTracking();
}

/*-------------------------------------------------------------------------------
Function: ~Litter()
Purpose: Destroy the Litter object
Parameters: 
Return: 
Comments: 
Modified: 
    <Modified by>
    <Date>
    <Change>
-------------------------------------------------------------------------------*/
Litter::~Litter()
{
    if (m_pObjectDetector != nullptr) delete m_pObjectDetector;
    if (m_pTrashDetector != nullptr) delete m_pTrashDetector;
    if (m_pTrashTracker != nullptr) delete m_pTrashTracker;
}

/*-------------------------------------------------------------------------------
Function: int LT_Init()
Purpose: Initialize the Litter object
Parameters: 
Return: 
Comments: 
Modified: 
    <Modified by>
    <Date>
    <Change>
-------------------------------------------------------------------------------*/
int Litter::LT_Init()
{
    if (m_pObjectDetector->initObjectDetector("snpe", "people", "CPU") != STATUS_SUCCESS)
    {
        // std::cerr << TagLitter << " Initialize Litter object failed." << std::endl;
        return -1;
    }

    // if (m_pTrashDetector->initObjectDetector("snpe", "trash", "DSP") != STATUS_SUCCESS)
    // {
    //     // std::cerr << TagLitter << " Initialize Litter object failed." << std::endl;
    //     return -1;
    // }

    return 0;
}

/*-------------------------------------------------------------------------------
Function: int LT_Set(stSettingLitter_t stSetting)
Purpose: Set up the Litter object
Parameters: stSetting
Return: 
Comments: 
Modified: 
    <Modified by>
    <Date>
    <Change>
-------------------------------------------------------------------------------*/
int Litter::LT_Set(stSettingLitter_t stSetting)
{
    m_ptRegion = stSetting.ptRegionSet;
    m_strObject = stSetting.strObjectSet;
    m_boxRegion = cv::boundingRect(m_ptRegion);
    return 0;
}

/*-------------------------------------------------------------------------------
Function: int OP_Exe(cv::Mat& mImage, std::vector<stOutputLitter_t>& stLitters)
Purpose: Execute the Litter object
Parameters: mImage, stLitters
Return: 
Comments: 
Modified: 
    <Modified by>
    <Date>
    <Change>
-------------------------------------------------------------------------------*/
int Litter::LT_Exe(cv::Mat& mImage, std::vector<stOutputLitter_t>& stLitters)
{
    stLitters.clear();
    if (mImage.empty())
    {
        // std::cerr << TagLitter << " Input invalid." << std::endl;
        return -1;
    }
    else
    {
        // detect object
        std::vector<ObjectTrace> stObjects;
        if (m_pObjectDetector->executeObjectDetector(mImage, stObjects, THRESH_OBJECT) != STATUS_SUCCESS)
        {
            // std::cerr << TagLitter << " Execute Object Detector failed." << std::endl;
            return -1;
        }
        std::cout << "stObjects.size() = " << stObjects.size() << std::endl;

        // detect trash
        std::vector<ObjectTrace> stTrashs;
        if (m_pTrashDetector->executeObjectDetector(mImage, stTrashs, THRESH_TRASH) != STATUS_SUCCESS)
        {
            // std::cerr << TagLitter << " Execute Trash Detector failed." << std::endl;
            return -1;
        }
        std::cout << "stTrashs.size() = " << stTrashs.size() << std::endl;

        // tracking trash
        std::vector<TrackingTrace> stTrashTracks;
        m_pTrashTracker->process(stTrashs, stTrashTracks);
        // delete lose trash track
        for (auto it = m_stLitterTracks.begin(); it != m_stLitterTracks.end(); )
        {
            const int iID = (*it).stTrash.iTrackID;
            const auto p = find_if(stTrashTracks.begin(), stTrashTracks.end(), 
                                    [iID] (const TrackingTrace& st) { return (st.m_ID == iID); });
            if (p == stTrashTracks.end() && it != m_stLitterTracks.end())
            {
                it = m_stLitterTracks.erase(it);
            }
            else
            {
                it ++;
            }
        }

        // update trash
        for (auto& stTrashTrack : stTrashTracks)
        {
            if (stTrashTrack.isOutOfFrame == false)
            {
                const int iID = stTrashTrack.m_ID;
                const auto p = find_if(m_stLitterTracks.begin(), m_stLitterTracks.end(), 
                                        [iID] (const stOutputLitter_t& st) { return (st.stTrash.iTrackID == iID); });
                if (p != m_stLitterTracks.end()) // if find track in list trash tracked
                {
                    int iIndex = distance(m_stLitterTracks.begin(), p);
                    m_stLitterTracks[iIndex].stTrash.riBox = stTrashTrack.m_rect;
                    m_stLitterTracks[iIndex].stTrash.bOutOfFrame = false;
                    m_stLitterTracks[iIndex].bEvent = false;
                    if (m_stLitterTracks[iIndex].strType == "Unknown")
                    {
                        LT_vGetObject(m_stLitterTracks[iIndex], stObjects);
                        if (m_stLitterTracks[iIndex].strType == "Unknown")
                        {
                            m_stLitterTracks[iIndex].bEvent = false;
                        }
                        else
                        {
                            m_stLitterTracks[iIndex].bEvent = true;
                        }
                    }
                }
                else // if find new trash track
                {
                    if (LT_bCheckMove(stTrashTrack) == true)
                    {
                        stOutputLitter_t stLitter;
                        stLitter.stTrash.iTrackID = stTrashTrack.m_ID;
                        stLitter.stTrash.riBox = stTrashTrack.m_rect;
                        stLitter.stTrash.strType = stTrashTrack.m_type;

                        LT_vGetObject(stLitter, stObjects);

                        if (stLitter.strType == "Unknown")
                        {
                            stLitter.bEvent = false;
                        }
                        else
                        {
                            stLitter.bEvent = true;
                        }

                        m_stLitterTracks.push_back(stLitter);
                    }
                }
            }
            else
            {
                const int iID = stTrashTrack.m_ID;
                const auto p = find_if(m_stLitterTracks.begin(), m_stLitterTracks.end(), 
                                        [iID] (const stOutputLitter_t& st) { return (st.stTrash.iTrackID == iID); });
                if (p != m_stLitterTracks.end())
                {
                    int iIndex = distance(m_stLitterTracks.begin(), p);
                    m_stLitterTracks[iIndex].stTrash.bOutOfFrame = true;
                }
            }
        }

        std::cout << "m_stLitterTracks.size() = " << m_stLitterTracks.size() << std::endl;

        for (auto stLitter : m_stLitterTracks)
        {
            if (stLitter.stTrash.bOutOfFrame != true)
            {
                if (stLitter.bEvent == true)
                {
                    stLitters.push_back(stLitter);
                }
            }
        }

    }

    return 0;
}

int Litter::LT_Exe2(cv::Mat& mImage, std::vector<stOutputLitter_t>& stLitters)
{
    // // std::cout << countFeature(mImage, m_ptRegion) << std::endl;
    // std::vector<cv::Rect2f> rectMotion = getMotionContours(mImage, m_previousMat);
    
    // if (_checkOverlapRect(m_boxRegion, rectMotion)) //co chuyen dong tai vung rac
    // {
    //     if (m_bIsMotion == false && m_bIsCalculating == false){
    //         m_bIsMotion = true;
    //         m_fBeforeMotion = getAverageFeature();
    //         std::cout << "Before Motion: " << m_fBeforeMotion <<std::endl;
    //         cv::putText(mImage, "Before Motion: " + std::to_string(m_fBeforeMotion), cv::Point2d(10,100), FONT_HERSHEY_COMPLEX, 2, cv::Scalar(0,0,255), 2);
    //         m_whitePixels.clear();
    //     }
    // }else { //khong co chuyen dong tai bai rac
    //     if(m_bIsMotion == true){
    //         m_bIsMotion = false;
    //         m_bIsCalculating = true;
    //     }
    //     if (m_bIsCalculating == true){
    //         m_whitePixels.push_back(countFeature(mImage, m_ptRegion));
    //         if (m_whitePixels.size() == 50){
    //             m_fAfterMotion = getAverageFeature();
    //             std::cout << "After Motion: " << m_fAfterMotion <<std::endl;
    //             cv::putText(mImage, "After Motion: " + std::to_string(m_fAfterMotion), cv::Point2d(10,100), FONT_HERSHEY_COMPLEX, 2, cv::Scalar(0,0,255), 2);
    //             m_bIsCalculating = false;
    //         }
    //     }else{
    //         if (m_whitePixels.size() == 50){
    //             m_whitePixels.erase(m_whitePixels.begin());
    //             m_whitePixels.push_back(countFeature(mImage, m_ptRegion));
    //         }else if (m_whitePixels.size() < 50){
    //             m_whitePixels.push_back(countFeature(mImage, m_ptRegion));
    //     }
    //     }
       
    // }
    
    
    // for (int i = 0 ; i < rectMotion.size(); i++)
    // {
    //     cv::rectangle(mImage, rectMotion[i], cv::Scalar(0,255,255));
    // }


    return 0;
}

int Litter::LT_Exe3(cv::Mat mImage, std::vector<stOutputLitter_t>& stLitters)
{
    // reSize(mImage, outputResize);
    m_bIsRegionChanged = false;
    std::vector<cv::Rect2f> rectMotion = getMotionContours(mImage, m_previousMat);
    if (_checkOverlapRect(m_boxRegion, rectMotion)) //co chuyen dong tai vung rac
    {
        m_bIsMotion = true;
        if (m_bIsCalculating == true)
            m_count = 0;
    }else { //khong co chuyen dong tai bai rac
        if (m_bIsMotion == true){
            m_bIsCalculating = true;
            m_bIsMotion = false;
            m_count ++;
        }else if ((m_bIsCalculating == true) && (m_bIsMotion == false)){
            if (m_count < 5){
                m_count ++;
            }
            else{
                cv::imwrite("priv.jpg",this->m_previousMotionList[0] );
                cv::imwrite("present.jpg",mImage );
                std::cout << "m_Color = " << m_Color << std::endl;
                m_Color = getColor(mImage, this->contoursColors, m_ptRegion);
                if (m_Color < 2000) {
                    m_fChangeDelta = calcCoutour(mImage,this->m_previousMotionList[0], m_ptRegion);
                    std::cout << "m_fChangeDelta: " << m_fChangeDelta << std::endl;
                    m_bIsCalculating = false;
                    m_count = 0;
                    if (m_fChangeDelta >= 2000)
                        m_bIsRegionChanged = true;

                } else {
                    std::cout << "Not Motion " << std::endl;
                }
            }
        } else if ((m_bIsMotion == false) &&  (m_bIsCalculating == false)){
            if (m_previousMotionList.size() > 3)
            {
                m_previousMotionList.pop_back();
                m_previousMotionList.push_front(mImage.clone());
            }else {
                m_previousMotionList.push_front(mImage.clone());
            }
            // std::cout << "in" << std::endl;
        }
    
    }
    std::cout << m_bIsMotion << " :  " << m_bIsCalculating << std::endl;
    for (int i = 0 ; i < rectMotion.size(); i++)
    {
        cv::rectangle(mImage, rectMotion[i], cv::Scalar(0,255,255));
    }
    
    if (m_bIsRegionChanged == true)
    {
        std::vector<ObjectTrace> objects;
        int index;
        // imwrite("ImageDetect.jpg", m_previousMotion);
        for (int i = 0; i < m_previousMotionList.size(); )
        {
            m_pObjectDetector->executeObjectDetector(m_previousMotionList[i], objects, 0.4 );
            if (objects.size() == 0){
                i++;
            }else{
                index = i;
                break;
            }
        }
        std::cout << "Detect: " << objects.size() << std::endl;
        if (objects.size() != 0) {
            for (int i = 0; i < objects.size() ; i++){
                // cv::Rect2f rectObject = pad_rectangle(m_previousMotion, objects[i].rect, 100);
                // cv::Rect rectOverlap = rectObject & m_boxRegion;
                // float percentOverlap = _getPercentageOverlap(rectObject, rectOverlap);
                // if (percentOverlap > 0)
                // {
                //     std::cout << "Dinh " << std::endl;
                //     stOutputLitter_t output;
                //     output.riBox = objects[i].rect;
                //     m_previousMotion.copyTo(output.imgEvent);
                //     stLitters.push_back(output);
                // }
                // rectangle(m_previousMotion, objects[i].rect, cv::Scalar(0,255,0));   
                // rectangle(m_previousMotion, rectObject, cv::Scalar(255,255,0));

                double dDistance = cv::pointPolygonTest(m_ptRegion, cv::Point2f(objects[i].rect.x + objects[i].rect.width / 2, objects[i].rect.y + objects[i].rect.height), true);
                dDistance = abs(dDistance);
                // PRINT_LOG(TAG, "Khoang cach: %f", dDistance);
                if (dDistance < 400){
                stOutputLitter_t output;
                output.riBox = objects[i].rect;
                m_previousMotionList[index].copyTo(output.imgEvent);
                stLitters.push_back(output);
                }
            }                
        }  
    }
    
}
/*-------------------------------------------------------------------------------
Function: bool LT_bCheckInside(TrackingTrace& stTrack, std::vector<cv::Point> ptArea)
Purpose: 
Parameters: stTrack, ptArea
Return: 
Comments: 
Modified: 
    <Modified by>
    <Date>
    <Change>
-------------------------------------------------------------------------------*/
bool Litter::LT_bCheckInside(TrackingTrace& stTrack, std::vector<cv::Point> ptArea)
{
    if (ptArea.size() == 0)
    {
        return false;
    }

    cv::Point2f ptCenterBox1 = cv::Point2f(stTrack.m_rect.x, stTrack.m_rect.y);
    cv::Point2f ptCenterBox2 = cv::Point2f(stTrack.m_rect.x + stTrack.m_rect.width, stTrack.m_rect.y);

    if ((cv::pointPolygonTest(ptArea, ptCenterBox1, false) >= 0) || (cv::pointPolygonTest(ptArea, ptCenterBox2, false) >= 0))
    {
        return true;
    }
    return false;
}

/*-------------------------------------------------------------------------------
Function: bool LT_bCheckMove(TrackingTrace& stTrack)
Purpose: 
Parameters: stTrack
Return: 
Comments: 
Modified: 
    <Modified by>
    <Date>
    <Change>
-------------------------------------------------------------------------------*/
bool Litter::LT_bCheckMove(TrackingTrace& stTrack)
{
    int iMinSize = 2;
    if (stTrack.m_trace.size() < iMinSize)
    {
        return false;
    }

    cv::Point2f ptLastTrace = stTrack.m_trace[stTrack.m_trace.size() - 1];
    cv::Point2f ptPreTrace = stTrack.m_trace[stTrack.m_trace.size() - iMinSize];

    double dDistance = cv::norm(ptLastTrace - ptPreTrace);

    if (dDistance > THRESH_TRASH_MOVE)
    {
        return true;
    }

    return false;
}

/*-------------------------------------------------------------------------------
Function: void LT_vGetObject(stOutputLitter_t& stLitter, std::vector<ObjectTrace>& stObjects)
Purpose: 
Parameters: stLitter, stObjects
Return: 
Comments: 
Modified: 
    <Modified by>
    <Date>
    <Change>
-------------------------------------------------------------------------------*/
void Litter::LT_vGetObject(stOutputLitter_t& stLitter, std::vector<ObjectTrace>& stObjects)
{
    cv::Point2f ptCenterBox = cv::Point2f(stLitter.stTrash.riBox.x + (float)stLitter.stTrash.riBox.width,
                                            stLitter.stTrash.riBox.y + (float)stLitter.stTrash.riBox.height);
    
    if (stObjects.size() == 0)
    {
        stLitter.strType = "Unknown";
    }

    stLitter.strType = "Unknown";

    double dMaxDistance = -20;

    for (int i = 0; i < stObjects.size(); i++)
    {
        std::vector<cv::Point2f> ptAreas{stObjects[i].rect.tl(), 
                                            cv::Point2f(stObjects[i].rect.x + stObjects[i].rect.width, stObjects[i].rect.y), 
                                            stObjects[i].rect.br(), 
                                            cv::Point2f(stObjects[i].rect.x, stObjects[i].rect.y + stObjects[i].rect.height)};
        
        double dDistance = cv::pointPolygonTest(ptAreas, ptCenterBox, true);
        if (dDistance >= dMaxDistance)
        {
            stLitter.riBox = stObjects[i].rect;
            stLitter.strType = stObjects[i].label;

            dMaxDistance = dDistance;
        }
    }
}

std::vector<cv::Rect2f> Litter::getMotionContours(cv::Mat image, cv::Mat& previousImage) 
{
     #ifdef CHECK_TIME
        float start = static_cast<double>(cv::getTickCount());
    #endif
    cv::Mat gray, thresh, frameDelta;
    std::vector<std::vector<cv::Point>> cnts;

    cvtColor(image, gray, COLOR_BGR2GRAY);
    GaussianBlur(gray, gray, Size(5, 5), 0);
    if (previousImage.empty()){
        gray.copyTo(previousImage);
    }
    //compute difference between first frame and current frame
    absdiff(previousImage, gray, frameDelta);
    gray.copyTo(previousImage);
    threshold(frameDelta, thresh, 10, 255, THRESH_BINARY);
    erode(thresh, thresh, Mat(), Point(-1,-1), 1);
    dilate(thresh, thresh, Mat(), Point(-1,-1), 3);
    findContours(thresh, cnts, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    std::vector<cv::Rect2f> boxes;
    
    for(int i = 0; i< cnts.size(); i++) 
    {
        if (contourArea(cnts[i]) < 10000)
            continue;
        cv::Rect box = boundingRect(cnts[i]);
        // syslog(LOG_INFO, "TEST width: %d ||| height: %d ||| contourArea: %d", box.width, box.height, contourArea(cnts[i]));

        boxes.push_back(box);
    }
    resize(thresh, thresh, Size(1280, 640));
    imshow("motion", thresh);
    
    #ifdef CHECK_TIME
        float time = ((double)cv::getTickCount() - start) / cv::getTickFrequency();
        PRINT_LOG(TAG, "___TIME___MOTION: %f", time);
    #endif
    return boxes;
}

float Litter::_getPercentageOverlap(cv::Rect2f rectObject, cv::Rect2f rectMotion)
{
    int x1 = std::max(rectObject.x, rectMotion.x);
    int y1 = std::max(rectObject.y, rectMotion.y);
    int x2 = std::min(rectObject.x + rectObject.width, rectMotion.x + rectMotion.width);
    int y2 = std::min(rectObject.y + rectObject.height, rectMotion.y + rectMotion.height);
    int w = std::max(0, (x2 - x1 + 1));
    int h = std::max(0, (y2 - y1 + 1));
    float inter = w * h;
    float area1 = rectObject.width * rectObject.height;
    float area2 = rectMotion.width * rectMotion.height;
    float percentageOverlap = inter / (area1 + area2 - inter);
    return percentageOverlap;
}

bool Litter::_checkOverlapRect(cv::Rect2f rectObject, std::vector<cv::Rect2f> rectMotion)
{
    for (int i = 0; i < rectMotion.size(); i ++){
        cv::Rect rectOverlap = rectObject & rectMotion[i];
        float percentOverlap = _getPercentageOverlap(rectObject, rectOverlap);
        if (percentOverlap > 0)
        {
            return true;
        }
    }
    return false;
    
}

int Litter::countFeature(cv::Mat img, std::vector<cv::Point> region)
{
    cv::Mat gray;
    // GaussianBlur(img, img,Size(5,5), 5, 0);
    // img.convertTo(img, -1,2.5,0);
    
    cvtColor(img, gray, COLOR_BGR2GRAY);
    
    cv::Mat1b thresholded = gray > 150;
    // Get bbox of polygon
    cv::Rect bbox = cv::boundingRect(region);

     // Make a new (small) mask 
    cv::Mat1b mask(bbox.height, bbox.width, uchar(0));
    cv::fillPoly(mask, std::vector<std::vector<cv::Point>>{m_ptRegion}, cv::Scalar(255), 8, 0, -bbox.tl());
    // Get crop
    cv::Mat1b cropped = thresholded(bbox) & mask;
    cv::Mat1b eroded;

    std::vector<std::vector<cv::Point> > contours;
    cv::findContours( cropped, contours, 1, 1 );
    //Draw the contours
    cv::Mat contourImage(cropped.size(), CV_8UC3, cv::Scalar(0,0,0));
    cv::Scalar colors[3];
    colors[0] = cv::Scalar(255, 0, 0);
    colors[1] = cv::Scalar(0, 255, 0);
    colors[2] = cv::Scalar(0, 0, 255);
    for (size_t idx = 0; idx < contours.size(); idx++) {
        cv::drawContours(contourImage, contours, idx, colors[idx % 3]);
    }
    cv::imshow("contourImage", contourImage);

    // dilate(eroded, eroded, Mat(), Point(-1,-1), 4);
    cv::imshow("cropped", cropped);
    cv::waitKey(1);
    return (cv::countNonZero(cropped));
}

float Litter::getAverageFeature()
{
    float sumFeature = 0;
    for (int i = 0; i < m_whitePixels.size(); i++)
    {
        sumFeature += m_whitePixels[i];
    }
    float averageFeature = sumFeature / m_whitePixels.size();
    return averageFeature;
}
cv::Scalar hsv2scalar(int h, int s, int v) {
    int scalar_h = h/2;
    int scalar_s = 255*s/100;
    int scalar_v = 255*v/100;

    return cv::Scalar(scalar_h, scalar_s, scalar_v);
}
int Litter::getColor(cv::Mat &frame, std::vector<std::vector<cv::Point>> &contoursColor, std::vector<cv::Point> region_color)
{
    // std::vector<std::vector<cv::Point>> countoursColor;
    cv::Mat hsv;
	cv::cvtColor(frame, hsv, COLOR_BGR2HSV);

	// define range of red color in HSV
	cv::Scalar lower_red = hsv2scalar(0 , 4, 24);
	cv::Scalar upper_red = hsv2scalar(100, 15, 29);

    cv::Scalar lower_red1 = cv::Scalar(0, 100, 100);
    cv::Scalar upper_red1 = cv::Scalar(10, 255, 255);

	cv::Scalar lower_red2 = hsv2scalar(100, 15, 29);
    cv::Scalar upper_red2 = hsv2scalar(210, 40, 33);

	// Threshold the HSV image to get only red colors
	cv::Mat mask1, mask2;
	cv::inRange(hsv, lower_red, upper_red, mask1);
	cv::inRange(hsv, lower_red2, upper_red2, mask2);
    
    cv::Mat mask;
	cv::bitwise_or(mask1, mask2, mask);
    // cv::bitwise_and(mask1, frameSegmentation, frameSegmentation);

	// Bitwise - AND mask and original image
	cv::Mat res;
	cv::bitwise_and(frame, frame, res, mask);
    cv::Mat res_channels[3];
    cv::split(res, res_channels);

	const char* titles[] = { "Shadow mask 1", "Shadow mask 2", "Final mask", "Result" };
	const cv::Mat* images[] = { &mask1, &mask2, &mask, &res };

    cv::findContours(res_channels[2], contoursColor, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0,0));
    std::vector<std::vector<cv::Point>> contours_poly(contoursColor.size());

    // cv::Mat res_channels[3];
    // cv::split(res, res_channels);

	// cv::imshow("Final mask", frame);
    // cv::imshow("Result", res);
    // cv::imshow("Origin", frame);
    resize(mask1, mask1, Size(640, 320));
    resize(mask2, mask2, Size(640, 320));
    resize(mask, mask, Size(640, 320));
    resize(res, res, Size(1280, 640));
	for (int i = 0; i < 4; i++) 
		cv::imshow(titles[i], *(images[i]));
    
	cv::waitKey(1);
	return contoursColor.size();
}

int Litter::calcCoutour(cv::Mat image, cv::Mat previousImage, std::vector<cv::Point> region) 
{
     #ifdef CHECK_TIME
        float start = static_cast<double>(cv::getTickCount());
    #endif
    // cv::resize(image, image, cv::Size(416,416));
    // cv::resize(previousImage, previousImage, cv::Size(416,416));
    std::vector<cv::Rect> colors;
    std::vector<std::vector<cv::Point>> contoursColor1;
    // std::cout << "OKKKKKKKKKKKKKKKKKKKKKK" << std::endl;
    std::cout << "ContoursColor1: " << contoursColor1.size() << std::endl;
    vector<vector<cv::Point>> coutour;
    static cv::Mat thresh;
    coutour.push_back(region);
    cout << region.size() << endl;
    // coutour[0][0].x = coutour[0][0].x * 416/1920;
    // coutour[0][0].y = coutour[0][0].y * 416/1080;
    // coutour[0][1].x = coutour[0][1].x * 416/1920;
    // coutour[0][1].y = coutour[0][1].y * 416/1080;
    // coutour[0][2].x = coutour[0][2].x * 416/1920;
    // coutour[0][2].y = coutour[0][2].y * 416/1080;
    // coutour[0][3].x = coutour[0][3].x * 416/1920;
    // coutour[0][3].y = coutour[0][3].y * 416/1080;

    // cv::Mat hsv;
    cv::Mat black1(image.rows, image.cols, image.type(), cv::Scalar::all(0));
    cv::Mat black2(image.rows, image.cols, image.type(), cv::Scalar::all(0));
    cv::Mat mask(image.rows, image.cols, CV_8UC1, cv::Scalar(0));
    cv::drawContours(mask,coutour, 0, cv::Scalar(255), FILLED, 8 );
    // cv::imshow("mask",mask );
    image.copyTo(black1,mask);
    previousImage.copyTo(black2,mask);

    cv::Rect bbox = cv::boundingRect(region);
    cv::Mat cropImage = black1(bbox).clone();
    cv::Mat cropPreviousImage = black2(bbox).clone();

    cv::Mat frameDelta;
    std::vector<std::vector<cv::Point>> cnts;

    cvtColor(cropImage, cropImage, COLOR_BGR2GRAY);
    cvtColor(cropPreviousImage, cropPreviousImage, COLOR_BGR2GRAY);
    GaussianBlur(cropImage, cropImage, Size(7, 7), 0);
    GaussianBlur(cropPreviousImage, cropPreviousImage, Size(7, 7), 0);

    // cv::cvtColor(image, hsv, COLOR_BGR2HSV);

    // cv::Scalar lower_gray = hsv2scalar(10, 50, 70);
    // cv::Scalar upper_gray = hsv2scalar(90, 100, 120);

    // cv::Scalar lower_gray1 = hsv2scalar(20, 60, 80);
    // cv::Scalar upper_gray1 = hsv2scalar(100, 110, 140);

    // cv::Mat mask1, mask2;
    // cv::inRange(hsv, lower_gray, upper_gray, mask1);
    // cv::inRange(hsv, lower_gray1, upper_gray1, mask2);

    // cv::Mat mask3;
    // cv::bitwise_or(mask1, mask2, mask3);

    // cv::Mat res;
    // cv::bitwise_and(image, image, res, mask3);

    // std::vector<std::vector<cv::Point>> countoursColor;
    //     std::cout<< "2222222222222222222" << std::endl;
    // cv::findContours(res, cnts, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    // std::cout<< "3333333333333333" << std::endl;
    // std::cout<< "CountoursColor size 1: " << countoursColor.size() << std::endl; 
    
    //compute difference between first frame and current frame
    absdiff(cropPreviousImage, cropImage, frameDelta);
    // gray.copyTo(previousImage);
    imwrite("framedelta.jpg", frameDelta);
    threshold(frameDelta, thresh, 10, 255, THRESH_BINARY);
    imwrite("thresh_before.jpg", thresh);
    erode(thresh, thresh, Mat(), Point(-1,-1), 2);
    imwrite("thresh_after.jpg", thresh);
    resize(thresh, thresh, Size(640, 360));
    // findContours(thresh, cnts, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    imshow("blur", thresh);

    return (cv::countNonZero(thresh));
}
cv::Rect2f Litter::pad_rectangle(cv::Mat image, cv::Rect_<float> rect, int paddingPercent)
{
    cv::Rect2f rectScaled;
	float padding = (float)(paddingPercent) / 100;
	rectScaled.x = rect.x - (rect.width * padding) / 2;
	rectScaled.y = rect.y - (rect.height * padding / 2) / 2;
	rectScaled.width = rect.width + (rect.width * padding);
	rectScaled.height = rect.height + (rect.height * padding /2);

	if (rectScaled.x < 0)
		rectScaled.x = 0;
	if (rectScaled.y < 0)
		rectScaled.y = 0;
	if ((rectScaled.x + rectScaled.width) > image.cols)
		rectScaled.width = image.cols - rectScaled.x;
	if ((rectScaled.y + rectScaled.height) > image.rows)
		rectScaled.height = image.rows - rectScaled.y;
    return rectScaled;
}

// cv::Rect Litter::reSize(cv::Mat& image, cv::Mat& output) {
//     cv::Mat frameDelta, thresh;
//     cv::resize(image, output, cv::Size(640, 320));
//     cv::cvtColor(image, output, COLOR_BGR2GRAY);

//     if (m_previousMat.empty()) {
//         output.copyTo(m_previousMat);
//     }

//     absdiff(m_previousMat, output, frameDelta);
//     output.copyTo(m_previousMat);
//     threshold(frameDelta, thresh, 20, 255, THRESH_BINARY);

//     dilate(thresh, thresh, Mat(), Point(-1, -1), 2);

// }