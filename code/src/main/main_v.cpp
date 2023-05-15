#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>
#include <unistd.h>
#include <vector>

#include "VehicleTracking.hpp"

int main()
{
	cv::Mat frame;
	VehicleTracking* vehicleTracking = new VehicleTracking();

    settingVehicleTracking settings;
    settings.ObjectsTrack = {"otocon", "xemay"};
    // settings.ObjectsTrack = {}; // tracking all
	settings.OptLicense = true;
	settings.nation = Nation::VN;

	if(vehicleTracking->initVehicleTracking(settings) != STATUS_SUCCESS) {
		std::cout << "[] Init VehicleTracking failed!" << std::endl;
		return -1;
	}

	cv::VideoCapture cap;
	cap.open("/home/na/Desktop/BlowRedLight_t/videos/filesZip/868513.mp4");
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
		
        cv::resize(frame, frame, Size(1920, 1080));

		// processing
		vector<outputVehicleTracking> outData;
		vehicleTracking->updateVehicleTracking(frame, outData);
		for(auto out : outData)
		{
            cv::rectangle(frame, out.rect, Scalar(0, 255, 255), 2, LINE_AA);
            std::string text{std::to_string(out.track_id) + " - " + out.label};
			cv::putText(frame, text, cv::Point(out.rect.x, out.rect.y), FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 1);

			// cv::Rect r(out.rectPlate.x + out.rect.x, out.rectPlate.y + out.rect.y, out.rectPlate.width, out.rectPlate.height);
			// cv::rectangle(frame, r, Scalar(0, 255, 255), 2, LINE_AA);
			cv::putText(frame, out.typePlate, cv::Point(out.rect.x, out.rect.y + 25), FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 1);
			cv::putText(frame, out.license, cv::Point(out.rect.x, out.rect.y + 50), FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 1);
		}

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
