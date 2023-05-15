#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <unistd.h>
#include <vector>
#include "./AiEngines/traffic/Oppose.hpp"
#include "nlohmann/json.hpp"

using namespace std;
// 

int main()
{
    // setenv("OPENCV_FFMPEG_CAPTURE_OPTIONS", "rtsp_transport;tcp", 1);
    std::cout << "=======Main=====" << std::endl;
    cv::Mat frame;
    Oppose *nguocchieu = new Oppose();
    stSettingOppose_t settings;
    //rtsp15
    settings.ptRegionSet =    {{907,304 }, {1919,310 }, {1919,1008 }, {501,1021 },{907,304 }};



    settings.strVehicleSet = {"car", "motor","bicycle","truck","van", "bus"};
    // settings.invadeSet = {};
    nguocchieu->OP_Init();
    nguocchieu->OP_Set(settings);

    
    

    cv::VideoCapture cap("/media/tund/HDD/Data/TestData/Traffic/Invade/13.mp4");
	// cap.open("rtsp://vmsqn4.quangninh.gov.vn:9447/86ef65b3-78c3-49a9-be8e-cc78633b18d1?token=eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiJab25lTWluZGVyIiwiaWF0IjoxNjUyODYzNTQyLCJleHAiOjE2Nzg3ODM1NDIsInVzZXIiOiJhZG1pbiIsInR5cGUiOiJhY2Nlc3MifQ.co1DKYb139kwLOdLYXLj5RUGrxdsvzrBgh3c9udIN-E");
	std::string filename = "./2-tracking.avi";   
    double fps = 10.0; 
    int codec = VideoWriter::fourcc('M', 'J', 'P', 'G'); 
    int frame_width = cap.get(CAP_PROP_FRAME_WIDTH );
    int frame_height = cap.get(CAP_PROP_FRAME_HEIGHT);
    cv::VideoWriter writer(filename, codec, fps,cv::Size(1920,1080) );
    
    if(!cap.isOpened())
	{
		printf("open video error\n");
		return 0 ;
	}
    int count = 1;
    int frame_count = 1;
    while(1)
	{
		cap >> frame;
		if (frame.empty()) 
            continue;
		
        frame_count++;
        if (frame_count % 3 == 0){
            for (int i = 0; i < settings.ptRegionSet.size() - 1; i++){
                cv::line(frame, settings.ptRegionSet[i], settings.ptRegionSet[i+1], cv::Scalar(255,0,0));
            }
            std::vector<stOutputOppose_t> outputs;
        
            nguocchieu->OP_Exe(frame, outputs);
            
            for (auto &out : outputs)
            {
                    
                    cv::rectangle(frame, out.riBox, CV_RGB(0,255,0),2);
                    std::string nameFile = "/media/tund/HDD/Data/TestData/Traffic/Invade/13/" + std::to_string(count) + ".jpg";
                    cv::imwrite(nameFile,frame );
                    count ++;
                
                //  out.isEvent = false;
                // std::cout << out.isEvent << std::endl;
                
            }
            
            // saveData(outputs);
            
            // for(auto v : outputs)
            // {
            //     json j;
            //     j["label"] = v.label;
            //     j["id"]  = v.track_id;
            //     j["violation"] = v.violation;
            //     j["box"] = {v.rect.x, v.rect.y, v.rect.width, v.rect.height};
            // }
            // std::cout << j.dump() << std::endl;
            writer.write(frame);
            cv::imshow("Video", frame);
            char key = waitKey(1);
            if(key == 'q') 
            {
                printf("Quited\n");
                break;
            }
            if(key == 'p') 
            {
                printf("Paused\n");
                while(waitKey(1) != 'p');
            }
        }
        
    }

    return 0;
}