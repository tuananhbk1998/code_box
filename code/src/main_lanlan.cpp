#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <unistd.h>
#include <vector>
#include "./AiEngines/traffic/Invade.hpp"
#include "nlohmann/json.hpp"

using namespace std;

int main()
{
    // setenv("OPENCV_FFMPEG_CAPTURE_OPTIONS", "rtsp_transport;tcp", 1);
    std::cout << "=======Main=====" << std::endl;
    cv::Mat frame;
    Invade *lanlan = new Invade();
    settingInvade settings;
    //rtsp15
    settings.regionSet =    {{{584,118 }, {1076,130 }, {796,1051 }, {36,780 }}, 
                        { {1101,132 }, {1625, 116}, {1842, 1049},{845,1043}}};



    settings.vehiclesSet = {{"car", "motor"}, {"car", "motor"}};
    settings.invadeSet = {};
    lanlan->initInvade();
    lanlan->setInvade(settings);

    
    

    cv::VideoCapture cap("/home/tund/Videos/vlc-record-2022-10-18-14h24m58s-aiview_auto_recorder_20221003020751.mp4-.mp4");
	// cap.open("rtsp://vmsqn4.quangninh.gov.vn:9447/86ef65b3-78c3-49a9-be8e-cc78633b18d1?token=eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiJab25lTWluZGVyIiwiaWF0IjoxNjUyODYzNTQyLCJleHAiOjE2Nzg3ODM1NDIsInVzZXIiOiJhZG1pbiIsInR5cGUiOiJhY2Nlc3MifQ.co1DKYb139kwLOdLYXLj5RUGrxdsvzrBgh3c9udIN-E");
	if(!cap.isOpened())
	{
		printf("open video error\n");
		return 0 ;
	}
    std::string filename = "invade.avi";   
    double fps = 12.0; 
    int codec = VideoWriter::fourcc('M', 'J', 'P', 'G'); 
    int frame_width = cap.get(CAP_PROP_FRAME_WIDTH );
    int frame_height = cap.get(CAP_PROP_FRAME_HEIGHT);
    cv::VideoWriter writer(filename, codec, fps,cv::Size(1920,1080) );

    int count = 1;
    int frame_count = 1;
    while(1)
	{
		cap >> frame;
		if (frame.empty()) 
            continue;
		
        frame_count++;
        if (frame_count % 1 == 0){
            std::cout << "-------frame: " << frame_count <<"-------------"<< std::endl;
            std::vector<outputInvade> outputs;
        
            lanlan->updateInvade2(frame, outputs);
            
            for (auto &out : outputs)
            {
                cv::rectangle(frame, out.rect, cv::Scalar(0,255,0));
                // if ((out.isEvent == true) && (out.typeError != ErrorCode_t::Undefine)){
                // if ((out.isEvent == true) && (out.typeError != ErrorCode_t::Undefine)){
                    rectangle(out.eventImage, out.eventRect,Scalar(0,255,255)); // vang   
                    putText(out.eventImage, out.eventPlate.license, cv::Point(out.eventRect.tl()), FONT_HERSHEY_COMPLEX, 0.8, Scalar(0,0,255), 2);                                  
                    std::string nameFile = "./out/" + std::to_string(count) + ".jpg";
                    cv::imwrite(nameFile,out.eventImage);
                    cv::imwrite("./out/hahaaa.jpg",out.eventPlate.imgPlate);
                    count ++;
                    std::cout << " nah anha" << std::endl;
                // }
                //  out.isEvent = false;
                // std::cout << out.isEvent << std::endl;
                
            }
            
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