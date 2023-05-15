#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <unistd.h>
#include <vector>
#include "../major/Litter/Litter.hpp"

int main()
{
    Litter* litter = new Litter();
    stSettingLitter setting;
    setting.ptRegionSet = {{424,725}, {407,987},{667,1006},{677,705}, {424, 725}};
    // setting.ptRegionSet = {{407,455}, {407,546},{562,529},{562,438}, {407,455}};
    litter->LT_Set(setting);

    litter->LT_Init();
    cv::Mat frame;
    cv::VideoCapture cap("/home/anhmtc/Downloads/aiview_auto_recorder_20221202075331.mp4");
	if(!cap.isOpened())
	{
		printf("open video error\n");
		return 0 ;
	}
    std::string filename = "./trash1.avi";   
    double fps = 10.0; 
    int codec = VideoWriter::fourcc('M', 'J', 'P', 'G'); 
    int frame_width = cap.get(CAP_PROP_FRAME_WIDTH );
    int frame_height = cap.get(CAP_PROP_FRAME_HEIGHT);
    cv::VideoWriter writer(filename, codec, fps,cv::Size(1920,1080) );
    int countframe = 0;
    while(1)
	{
		cap >> frame;
       
        countframe++;
        if (countframe % 2 == 0)
        {
            if (frame.empty()) 
            continue;
            std::cout << "----------------frame: " <<countframe <<  std::endl;
            for (int i = 0 ; i < setting.ptRegionSet.size() -1; i++)
            {
                cv::line(frame, setting.ptRegionSet[i], setting.ptRegionSet[i+1], cv::Scalar(255,0,0));
            }
            std::vector<stOutputLitter_t> outputs;
            litter->LT_Exe3(frame,outputs);
            // std::cout << "object: " << outputs.size() << std::endl;
            if (outputs.size() != 0){
                for (int i = 0; i < outputs.size(); i++){
                    cv::rectangle(outputs[i].imgEvent, outputs[i].riBox, cv::Scalar(0,0,255));
                    imwrite("output5.jpg", outputs[i].imgEvent);
                }                
            }
            writer.write(frame);
            resize(frame, frame, Size(1280, 640));
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