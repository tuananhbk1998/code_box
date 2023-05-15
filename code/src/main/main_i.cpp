#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>
#include <unistd.h>
#include <vector>

#include "Invade.hpp"

int main()
{
	cv::Mat frame;
	Invade* invade = new Invade();
    invade->initInvade();

    settingInvade settings;
    settings.regionSet = {{{764, 132}, {1200, 132}, {1200, 638}, {774, 638}}, 
                          {{1210, 132}, {1700, 132}, {1700, 638}, {1210, 638}}, 
                          {{300, 132}, {760, 132}, {760, 638}, {300, 638}}};
    settings.labelSet = {"0"};
    settings.invadeSet = {{2, 1}};
    settings.vehiclesSet = {{"xemay"}, {"xemay"}, {""}};

	if(invade->setInvade(settings) != STATUS_SUCCESS) return -1;

	cv::VideoCapture cap;
	cap.open("/home/na/Desktop/BlowRedLight_t/videos/filesZip/868513.mp4");
	if(!cap.isOpened())
	{
		printf("open video error\n");
		return 0 ;
	}

    int n = 1;
	while(1)
	{
		cap >> frame;
		if (frame.empty()) 
            break;
		
        cv::resize(frame, frame, Size(1920, 1080));

		// processing
		vector<outputInvade> outData;
		invade->updateInvade(frame, outData);
		for(auto out : outData)
		{
            if(out.isEvent) {
                // cv::rectangle(frame, out.rect, Scalar(0, 255, 255), 2, LINE_AA);
                std::cout << "out.typeError = " << out.typeError << " , " << ErrorCode::InvadeLaneError << std::endl;
                if(out.typeError == ErrorCode::WrongLaneError) {
                    std::cout << "b" << std::endl;
                    cv::rectangle(frame, out.rect, Scalar(0, 255, 255), 2, LINE_AA);
                    std::string text{std::to_string(out.track_id) + " - " + out.label};
                    cv::putText(frame, text, cv::Point(out.rect.x, out.rect.y), FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 255), 1);
                }
                if(out.typeError == ErrorCode::InvadeLaneError) {
                    std::cout << "c" << std::endl;
                    cv::rectangle(frame, out.rect, Scalar(0, 0, 255), 2, LINE_AA);
                    std::string text{std::to_string(out.track_id) + " - " + out.label};
                    cv::putText(frame, text, cv::Point(out.rect.x, out.rect.y), FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 1);
                }
            }
		}
        cv::imwrite("../outputs/" + std::to_string(n) + ".jpg", frame);
        
        n++;

		// show result
		cv::resize(frame, frame, Size(1280, 720));
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
    
	return 0;
}
