#include<iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "./AiCore/ObjectDetector.hpp"
#include "./AiEngines/Tracking/ObjTracking.hpp"
#include "./Decoder/Decoder.hpp"
#include "./Decoder/DecoderThread.hpp"
#include "./Decoder/FrameManager.hpp"
#include "./AiCore/MotionDetector.hpp"
#include "./AiEngines/Tracking/ObjTracking.hpp"
using namespace std;
using namespace cv;

std::string GetFileName(const std::string& s) {

    char sep = '/';

    size_t i = s.rfind(sep, s.length());
    std::string s1;
    if (i != std::string::npos) 
    {
        s1 = s.substr(i+1, s.length() - i);
    }

    sep = '.';
    i = s1.rfind(sep, s1.length());
    if (i != std::string::npos) 
    {
        return (s1.substr(0, i));
    }

    return("");
}
std::vector<cv::Rect2f> getMotionContours(cv::Mat image, cv::Mat& previousImage) 
{
     #ifdef CHECK_TIME
        float start = static_cast<double>(cv::getTickCount());
    #endif
    cv::Mat gray, thresh, frameDelta;
    std::vector<std::vector<cv::Point>> cnts;
    cv::resize(image, image, cv::Size(640, 360));

    cvtColor(image, gray, COLOR_BGR2GRAY);
    GaussianBlur(gray, gray, Size(5, 5), 0);
    if (previousImage.empty()){
        gray.copyTo(previousImage);
    }
    //compute difference between first frame and current frame
    absdiff(previousImage, gray, frameDelta);
    gray.copyTo(previousImage);
    threshold(frameDelta, thresh, 10, 255, THRESH_BINARY);
    
    dilate(thresh, thresh, Mat(), Point(-1,-1), 2);
    findContours(thresh, cnts, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    std::vector<cv::Rect2f> boxes;
    for(int i = 0; i< cnts.size(); i++) 
    {
        if (contourArea(cnts[i]) < 3000)
            continue;
        cv::Rect box = boundingRect(cnts[i]);
        // syslog(LOG_INFO, "TEST width: %d ||| height: %d ||| contourArea: %d", box.width, box.height, contourArea(cnts[i]));
        box.x = box.x * 1920/640;
        box.y = box.y * 1080/360;
        box.width = box.width * 1920/640;
        box.height = box.height * 1080/360;
        boxes.push_back(box);
    }
    cv::imshow("aaaa", frameDelta);
    #ifdef CHECK_TIME
        float time = ((double)cv::getTickCount() - start) / cv::getTickFrequency();
        PRINT_LOG(TAG, "___TIME___MOTION: %f", time);
    #endif
    return boxes;
}
float _getPercentageOverlap(cv::Rect2f rectObject, cv::Rect2f rectMotion)
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

bool _checkOverlapRect(cv::Rect2f rectObject, cv::Rect2f rectMotion)
{
    cv::Rect rectOverlap = rectObject & rectMotion;
    float percentOverlap = _getPercentageOverlap(rectObject, rectOverlap);

    if (percentOverlap >= 0.5)
    {
        return true;
    }
    else
    {
        return false;
    }
}


int main()
{
    ObjectDetector* detector = new ObjectDetector();
    ObjTracking tracking;
    tracking.init("snpe", "people", "CPU");
    
    // detector->initObjectDetector("snpe", "people", "CPU");
    // ObjTracking* obj = new ObjTracking();
    // obj->init();
    vector<ObjectTrace> objects;



 //=================================================================================
   
    // std::vector<cv::String>  fn;
    // cv::glob("/home/tund/Pictures/chongfake/*.jpg", fn, false);
    // size_t count = fn.size(); 
    // int number = 0;
    // for(int i=0 ; i< fn.size() ; i++ )
    // {
    //     objects.clear();
    //     cv::Mat img = imread(fn[i]);
        
    //     std::cout <<"image: " <<  fn[i] << std::endl;
    //     detector->executeObjectDetector(img, objects, 0.5);
    //     std::cout << objects.size() << std::endl;
    //     // std::cout << objects[0].rect.x <<std::endl;
    //     // std::cout << objects[0].rect.y <<std::endl;
    //     // std::cout << objects[0].rect.width <<std::endl;
    //     // std::cout << objects[0].rect.height <<std::endl;
    //     if (objects.size() != 0){
    //         for (int j = 0; j < objects.size(); j++){
    //             rectangle(img, objects[j].rect, Scalar(0,0,255));
    //             // cv::putText(img, std::to_string(objects[j].score),cv::Point(objects[j].rect.x, objects[j].rect.y),cv::FONT_ITALIC, 0.75, cv::Scalar(0,0,0), 1  );
    //             // std::string name_new_file = "/media/tund/Disk1/2.MyProject/tensorflow/ObjectDetection/out/"+ GetFileName(fn[i]) +std::to_string(j) + ".jpg";
    //             double x1 = ((objects[j].rect.x  - objects[j].rect.width * 0.2)  < 0) ? 0 : (objects[j].rect.x  - objects[j].rect.width * 0.2);
    //             double y1 = ((objects[j].rect.y  - objects[j].rect.height * 0.2) < 0) ? 0 : (objects[j].rect.y  - objects[j].rect.height * 0.2);
    //             double x2 = ((objects[j].rect.x  + objects[j].rect.width * 0.2 + objects[j].rect.width) > 1920) ? 1920 : (objects[j].rect.x  + objects[j].rect.width * 0.2 + objects[j].rect.width);
    //             double y2 = ((objects[j].rect.y  + objects[j].rect.height * 0.2 + objects[j].rect.height) > 1080) ? 1080 : (objects[j].rect.y  + objects[j].rect.height * 0.2 + objects[j].rect.height);
  
    //              cv::Mat crop = img(cv::Range(y1,y2), cv::Range(x1,x2));
    //         }
            
    //     }
    //     std::string name_new_file = "/media/tund/Disk1/2.MyProject/tensorflow/ObjectDetection/out/"+ GetFileName(fn[i]) + ".jpg";

    //      cv::imwrite(name_new_file, img);

    // }
    

//=============================================================================================
    VideoCapture cap("/media/tund/Data/VideoCam/vlc-record-2022-10-31-13h04m21s-aiview_auto_recorder_20221031035701.mp4-.mp4");
    if (!cap.isOpened()){
        std::cout <<"Error opening video stream or file" << std::endl;
        return -1;
    }

    std::string filename = "./motion5.avi";   
    double fps = 30.0; 
    int codec = VideoWriter::fourcc('M', 'J', 'P', 'G'); 
    int frame_width = cap.get(CAP_PROP_FRAME_WIDTH );
    int frame_height = cap.get(CAP_PROP_FRAME_HEIGHT);
    cv::VideoWriter writer(filename, codec, fps,cv::Size(1920,1080) );
    int count_frame = 0;
    static cv::Mat prvFrame;
    while(1){
        cv::Mat frame;
        cap >> frame;
        count_frame++;
        if (count_frame % 3 == 0){
            objects.clear();
            tracking.run(frame, objects);
           



            // motion.getMotionContours(frame);
            // objects.clear();
            // detector->executeObjectDetector(frame, objects, 0.5);
            
            // std::vector<cv::Rect2f> rectMotion = getMotionContours(frame, prvFrame);
            for (int i = 0; i < objects.size(); i++)
            {
                float areaObject = objects[i].rect.width * objects[i].rect.height;
                float rate = areaObject / (1920 * 1080);
                // std::cout << "area: " << areaObject << endl;

                cv::putText(frame, std::to_string(objects[i].distance), objects[i].rect.tl(), FONT_HERSHEY_COMPLEX, 0.8, cv::Scalar(0,0,255));
                cv::rectangle(frame, objects[i].rect, cv::Scalar(0,255,0));
            }
            // for (int i = 0; i < rectMotion.size(); i++)
            // {
            //     cv::rectangle(frame, rectMotion[i], cv::Scalar(255,255,0));
            // }
            // for (int i = 0 ; i < objects.size(); i++)
            // {
            //     for (int j = 0 ; j < rectMotion.size() ; j++)
            //     {
            //         cv::Rect2f rectOverlap = objects[i].rect & rectMotion[j];
            //         rectangle(frame, rectOverlap, Scalar(255,0,255));

            //         float overlap = _getPercentageOverlap(objects[i].rect, rectOverlap);
            //         cv::putText(frame, std::to_string(overlap), objects[i].rect.br(), FONT_HERSHEY_COMPLEX, 0.8, Scalar(0,0,255));
            //     }
            // }


#ifdef CROP_LICENSE_PLATE
            if (objects.size() != 0){
                 for (int j = 0; j < objects.size(); j++){
                    rectangle(frame, objects[j].rect, Scalar(0,0,255));
                    // cv::putText(frame, std::to_string(objects[j].track_id) + " " + objects[j].label ,cv::Point(objects[j].rect.x, objects[j].rect.y),cv::FONT_ITALIC, 0.75, cv::Scalar(0,0,0), 1  );
                    cv::putText(frame,std::to_string(objects[j].score),cv::Point(objects[j].rect.x, objects[j].rect.y),cv::FONT_ITALIC, 0.75, cv::Scalar(0,0,0), 1  );
                    // std::string text = std::to_string((int)objects[j].rect.width) + " x " + std::to_string((int)objects[j].rect.height);
                    // cv::putText(frame,text,cv::Point(objects[j].rect.x, objects[j].rect.y),cv::FONT_ITALIC, 0.75, cv::Scalar(0,255,0), 2  );
                    
                    // double x1 = ((objects[j].rect.x  - objects[j].rect.width * 0.2)  < 0) ? 0 : (objects[j].rect.x  - objects[j].rect.width * 0.2);
                    // double y1 = ((objects[j].rect.y  - objects[j].rect.height * 0.2) < 0) ? 0 : (objects[j].rect.y  - objects[j].rect.height * 0.2);
                    // double x2 = ((objects[j].rect.x  + objects[j].rect.width * 0.2 + objects[j].rect.width) > 1920) ? 1920 : (objects[j].rect.x  + objects[j].rect.width * 0.2 + objects[j].rect.width);
                    // double y2 = ((objects[j].rect.y  + objects[j].rect.height * 0.2 + objects[j].rect.height) > 1080) ? 1080 : (objects[j].rect.y  + objects[j].rect.height * 0.2 + objects[j].rect.height);
                    // cv::Mat crop = frame(cv::Range(y1,y2), cv::Range(x1,x2));
                    
                }

                // std::string fileout = "../out/" + std::to_string(count_frame) + ".jpg";
                // cv::imwrite(fileout, frame);
            }
#endif           
            writer.write(frame);
            cv::imshow("window", frame);
            char c= (char)cv::waitKey(25);
            if (c==27)
                break;
        }else{
            continue;
        }
        
    }
    // writer.release();
    cap.release();
    cv::destroyAllWindows(); 
//==============================================================================================================



   /*
    FrameManager* frameManager = new FrameManager();
    DecoderThread* decoder = new DecoderThread(frameManager);

    decoder->run();
    usleep(100000);
     while(1)
    {
        cv::Mat frame = frameManager->getFrame();
        if(!frame.empty())
        {
            imshow("frame", frame);
            char key = cv::waitKey(1);
            if(key == 'q') break;
        }
        
        sleep(1); 
    }
    */




    return 0;
}