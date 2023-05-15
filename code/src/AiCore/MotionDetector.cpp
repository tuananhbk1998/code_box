/*
* To compile: g++ opencv_test2.cpp -o opencv_test2 $(pkg-config --cflags --libs opencv)
*/
#include <MotionDetector.hpp>

MotionDetector::MotionDetector()
{
}
MotionDetector::~MotionDetector()
{
    ;
}

int MotionDetector::isMotionDetected(Mat& frame, Rect& out)
{
    int fl_motion = 0;
    Mat gray,frameDelta, thresh, curFrame;
    vector<vector<Point> > cnts;
    vector<int> ArrayX;
    vector<int> ArrayY;
    vector<int> ArrayXmax;
    vector<int> ArrayYmax;
    Rect box;
    ArrayY.clear();
    ArrayX.clear();
    ArrayYmax.clear();
    ArrayXmax.clear();
    cnts.clear();
    if(frame.empty()){
        cout << "Frame is empty" << endl;
        return -1;
    }
    frame.copyTo(curFrame);
    if(!this->lastFrame.empty())
    {
        
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        GaussianBlur(gray, gray, Size(21, 21), 0);
        
        absdiff(this->lastFrame, gray, frameDelta);
        threshold(frameDelta, thresh, 27, 255, THRESH_BINARY);

        dilate(thresh, thresh, Mat(), Point(-1,-1), 2);
        findContours(thresh, cnts, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        
        if(cnts.size() > 0)
        {
            for(int i = 0; i < cnts.size(); i++)
            {
                // if(contourArea(cnts[i]) < THRES_MOTION) continue;
                if(contourArea(cnts[i]) < AREA_DEFAULT) continue;
                box = boundingRect(cnts[i]);  // box chua (x,y,width,height)
                if(box.x >= 0 && box.y >= 0 && 
                    (box.x + box.width) <= frame.cols && 
                    (box.y + box.height <= frame.rows))
                    {
                        ArrayX.push_back(box.x);
                        ArrayY.push_back(box.y);
                        ArrayXmax.push_back(box.x + box.width);
                        ArrayYmax.push_back(box.y + box.height);
                    }
                    
            }
            if(ArrayXmax.size() > 0 && ArrayYmax.size() > 0 && ArrayX.size() > 0 && ArrayY.size() > 0)
            {
                int x_max = *max_element(ArrayXmax.begin(), ArrayXmax.end());
                int y_max = *max_element(ArrayYmax.begin(), ArrayYmax.end());
                int x_min = *min_element(ArrayX.begin(), ArrayX.end());
                int y_min = *min_element(ArrayY.begin(), ArrayY.end());

                int x = x_min;
                int y = y_min;
                int w = x_max - x_min;
                int h = y_max - y_min;
                Rect rect = Rect(x, y, w, h); 
                out = rect;
                fl_motion = 1;
            }
        }
    }
    cvtColor(curFrame, this->lastFrame, COLOR_BGR2GRAY);
    GaussianBlur(this->lastFrame, lastFrame, Size(21, 21), 0);
    return fl_motion;
}

std::vector<cv::Rect> MotionDetector::getMotionContours(cv::Mat image) 
{
     #ifdef CHECK_TIME
        float start = static_cast<double>(cv::getTickCount());
    #endif
    cv::Mat gray, thresh, frameDelta;
    std::vector<std::vector<cv::Point>> cnts;
    cv::resize(image, image, cv::Size(640, 360));

    cvtColor(image, gray, COLOR_BGR2GRAY);
    GaussianBlur(gray, gray, Size(5, 5), 0);
    if (this->previousImage.empty()){
        gray.copyTo(this->previousImage);
    }
    //compute difference between first frame and current frame
    absdiff(this->previousImage, gray, frameDelta);
    gray.copyTo(this->previousImage);
    threshold(frameDelta, thresh, 27, 255, THRESH_BINARY);
    
    dilate(thresh, thresh, Mat(), Point(-1,-1), 2);
    findContours(thresh, cnts, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    std::vector<cv::Rect> boxes;
    for(int i = 0; i< cnts.size(); i++) 
    {
        if (contourArea(cnts[i]) < AREA_DEFAULT)
            continue;
        cv::Rect box = boundingRect(cnts[i]);
        // syslog(LOG_INFO, "TEST width: %d ||| height: %d ||| contourArea: %d", box.width, box.height, contourArea(cnts[i]));
        // box.x = box.x * 1920/640;
        // box.y = box.y * 1080/360;
        // box.width = box.width * 1920/640;
        // box.height = box.height * 1080/360;
        boxes.push_back(box);
    }
    
    #ifdef CHECK_TIME
        float time = ((double)cv::getTickCount() - start) / cv::getTickFrequency();
        PRINT_LOG(TAG, "___TIME___MOTION: %f", time);
    #endif
    return boxes;
}