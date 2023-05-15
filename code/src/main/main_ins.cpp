#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>
#include <unistd.h>
#include <iostream>
#include <vector>

#include "Intrusion.hpp"

using namespace std;
using namespace cv;

int main()
{
	cv::Mat frame;

	Intrusion* intrusion = new Intrusion();
	intrusion->initIntrusion();

	settingIntrusion settings;
	settings.objectsTrack = std::vector<std::string>{"person"};
    settings.regionSet = std::vector<cv::Point>{cv::Point(704, 525), cv::Point(1755, 729), cv::Point(1522, 1009), cv::Point(584, 955), cv::Point(704, 525)};
	// settings.numberOfObjects = 2;
    // settings.warningTime = 0;
	settings.repeated_time = 10;

	intrusion->setIntrusion(settings);

	
	cv::VideoCapture cap;
	cap.open("/media/tund/Data/VideoCam/person5.mp4");
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

		for (int i = 0; i < settings.regionSet.size() - 1; i++){
			std::cout << settings.regionSet[i] << std::endl;
			cv::line(frame, settings.regionSet[i], settings.regionSet[i+1], cv::Scalar(255,0,255));
		}
		//processing
        
		vector<outputIntrusion> outData;

		intrusion->updateIntrusion(frame, outData);
		std::cout << outData.size() << std::endl;
		for(auto& out : outData){
			cv::putText(frame, std::to_string(out.track_id), cv::Point(out.rect.x, out.rect.y), FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 2);
		}
		
		std::cout << "intrusion->typeWarning = " << intrusion->typeWarning << " | -- " << n << std::endl;
		switch(intrusion->typeWarning) {
			case TypeWarning::No:
			{
				break;
			}
			case TypeWarning::First:
			{
				for(auto out : outData)
				{
					rectangle(frame, out.rect, Scalar(0, 0, 255), 2, 8);
					// cv::putText(frame, "F", cv::Point(out.rect.x, out.rect.y), FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 2);
					
				}
				cv::imwrite("./outputs/" + std::to_string(n) + "_F.jpg", frame);
				break;
			}
			case TypeWarning::Repeat:
			{
				for(auto out : outData)
				{
					rectangle(frame, out.rect, Scalar(0, 255, 255), 2, 8);
					// cv::putText(frame, "R", cv::Point(out.rect.x, out.rect.y), FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 255), 2);
				}
				cv::imwrite("./outputs/" + std::to_string(n) + "_R.jpg", frame);
				break;
			}
			default:
				break;
		}

		// cv::imwrite("../outputs/" + std::to_string(n) + ".jpg", frame);
		n ++;

		// show result
		resize(frame, frame, Size(1280, 720));


        imshow("Video", frame);
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
