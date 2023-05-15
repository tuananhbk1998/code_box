#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <unistd.h>
#include <vector>
#include "lanlan.hpp"

void phuongTrinhDuongThang(cv::Point a, cv::Point b,vector<float> heSo)
{

}

int main()
{
    cv::Mat frame;
    Lanlan *lanlan = new Lanlan();
    settingLanlan settings;
    settings.regionSet = {{1300, 440}, {1050, 215}}; 
    settings.vehiclesSet = {{"otocon", "otokhach", "ototai", "xebus"}, {"xemay"}};

    lanlan->initLanlan();
    lanlan->setLanlan(settings);

    


    cv::VideoCapture cap;
	cap.open("../videos/lanlan.mp4");
	if(!cap.isOpened())
	{
		printf("open video error\n");
		return 0 ;
	}

    while(1)
	{
		cap >> frame;
		if (frame.empty()) 
            break;
		cv::resize(frame, frame, cv::Size(1920,1080));
        // cv::resize(frame, frame, Size(1920, 1080));
        std::vector<OutputLanlan> outputs;
        lanlan->updateLanlan(frame, outputs);
        // cv::resize(frame, frame, Size(1280, 720));
        // std::cout << outputs.size()<<std::endl;
        for (int i = 0 ; i< outputs.size(); i++){
            cv::circle(frame, cv::Point(( outputs[i].rect.x+ outputs[i].rect.width/2),(outputs[i].rect.y+ outputs[i].rect.height/2)), 2, cv::Scalar(255,255,255),1);
        }
         
     
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