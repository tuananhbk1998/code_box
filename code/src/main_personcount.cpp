#include<iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "./AiEngines/Tracking/ObjTracking.hpp"
#include "./Decoder/Decoder.hpp"
#include "./Decoder/DecoderThread.hpp"
#include "./Decoder/FrameManager.hpp"
#include "./AiEngines/major/PersonCount.hpp"
using namespace std;
using namespace cv;

int main()
{
    PersonCount* personcount = new PersonCount();
    personcount->InitAI();
    
    VideoCapture cap("/home/tund/Videos/vlc-record-2022-10-05-15h05m30s-aiview_auto_recorder_20221005074150.mp4-.mp4");
    if (!cap.isOpened()){
        std::cout <<"Error opening video stream or file" << std::endl;
        return -1;
    }

    std::string filename = "./motion4.avi";   
    double fps = 10.0; 
    int codec = VideoWriter::fourcc('M', 'J', 'P', 'G'); 
    int frame_width = cap.get(CAP_PROP_FRAME_WIDTH );
    int frame_height = cap.get(CAP_PROP_FRAME_HEIGHT);
    personcount->Setting(frame_width,frame_height);
    cv::VideoWriter writer(filename, codec, fps,cv::Size(1920,1080) );
    int count_frame = 0;
    static cv::Mat prvFrame;
    while(1){
        cv::Mat frame;
        cap >> frame;
        count_frame++;
        if (count_frame % 10 == 0)
        {
            
            std::vector<PersonCountOutput> outputs;
            personcount->Execute(frame, outputs);
            for (int i = 0; i < outputs.size() ; i ++)
            {
                if (outputs[i].type == "standard"){
                    cv::rectangle(frame, outputs[i].rect, cv::Scalar(0,255,0));
                    cv::putText(frame, std::to_string(outputs[i].area), outputs[i].rect.tl(), FONT_HERSHEY_COMPLEX, 0.8, cv::Scalar(0,0,255));
                } else if (outputs[i].type == "substandard"){
                    cv::rectangle(frame, outputs[i].rect, cv::Scalar(0,0,255));
                    cv::putText(frame, std::to_string(outputs[i].area), outputs[i].rect.tl(), FONT_HERSHEY_COMPLEX, 0.8, cv::Scalar(0,0,255));
                }
            }

            writer.write(frame);
            cv::imshow("window", frame);
            char c= (char)cv::waitKey(25);
            if (c==27)
                break;
        }
        else{
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