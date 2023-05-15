#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <unistd.h>
#include <vector>
#include "./AiEngines/traffic/Speed.hpp"
#include "./Decoder/Decoder.hpp"
#include "./Decoder/DecoderThread.hpp"
#include "./Decoder/FrameManager.hpp"

int main()
{
    std::cout << "Main " << std:: endl;
    cv::Mat frame ;

    SpeedSetting settings;
    // settings.regionSet = {{1300, 440}, {1050, 215}}; 
    settings.lineSet = {{1030,746 },{950,533 } };
    //{940,503}
    // settings.lineSet = {{1030,746 },{950,533 } ,{910,415}, {880,335}, {860,281}, {845,240}};//cam 14 
    // settings.lineSet = {{1094,656 },{1032,380 } };
    // settings.lineSet = {{1094,656 },{1032,380 } ,{1002,252}, {984,178}, {974,133}}; // cam6
    // settings.lineSet = {{3090,1680 },{2832,1442 } ,{2645,1260}, {2500,1132}, {2390,1022},{2302,947},{2222,880},{2162,825}, {2105,775}};
    // settings.lineSet = {{3090,1680 } ,{2645,1260},  {2390,1022},{2222,880},{2105,775}};
    
    // {1196,972 }, 
    settings.vehiclesSet = {{"otocon", "otokhach", "ototai", "xebus"}, {"xemay"}};
    settings.iDistancePerRegion = 4;
 
    Speed* calcSpeed = new Speed();
    calcSpeed->initSpeed();
    calcSpeed->settingSpeed(settings);
    
    // cv::imwrite("b.jpg" , frame);


    cv::VideoCapture cap;
	cap.open("/media/tund/Data/VideoCam/test_speed3.mp4");
	if(!cap.isOpened())
	{
		printf("open video error\n");
		return 0 ;
	}
    std::string filename = "speed7.avi";   
    double fps = 12.0; 
    int codec = VideoWriter::fourcc('M', 'J', 'P', 'G'); 
    int frame_width = cap.get(CAP_PROP_FRAME_WIDTH );
    int frame_height = cap.get(CAP_PROP_FRAME_HEIGHT);
    cv::VideoWriter writer(filename, codec, fps,cv::Size(1920,1080) );

    while(1)
	{
		cap >> frame;
		if (frame.empty()) 
            continue;
        for (int i = 0 ; i < settings.lineSet.size(); i++){
            cv::circle(frame, settings.lineSet[i], 2, cv::Scalar(0,0,255));
        }
        
        std::vector<SpeedOutput> outputs;
        calcSpeed->updateSpeedv1(frame, outputs);

        for (int i = 0 ; i< outputs.size(); i++){
            int speed = int(outputs[i].speed);
            cv::rectangle(frame, outputs[i].rect, cv::Scalar(0,255,0));
            if (speed != 0){
                std::string showSpeed = std::to_string(speed) + " km/h";
                cv::putText(frame,showSpeed, outputs[i].rect.tl(), FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0,0,255) );
                // cv::putText(frame,std::to_string(outputs[i].timeIn), outputs[i].rect.tl(), CV_FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(0,0,255) );
                // cv::putText(frame,std::to_string(outputs[i].position), outputs[i].rect.br(), CV_FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(255,0,0) );
                
            }
           
        }
         
        writer.write(frame);
        // cv::resize(frame, frame, cv::Size(frame.cols/2, frame.rows/2));
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

/*
    FrameManager* frameManager = new FrameManager();
    DecoderThread* decoder = new DecoderThread(frameManager);

    decoder->run();
    usleep(100000);
    while(1)
    {
        cv::Mat frame = frameManager->getFrame();
        cv::Mat temp = frame.clone();
        if(!temp.empty())
        {
            std::vector<SpeedOutput> outputs;
            calcSpeed->updateSpeed(temp, outputs);
            for (int i = 0 ; i < outputs.size(); i++){
                cv::putText(temp,std::to_string(outputs[i].speed), outputs[i].rect.tl(), CV_FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(0,0,255) );
                // cv::putText(temp,std::to_string(outputs[i].timeIn), outputs[i].rect.tl(), CV_FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(0,0,255) );
                cv::rectangle(temp, outputs[i].rect, cv::Scalar(0,255,0));
            }
            imshow("frame", temp);
            char key = cv::waitKey(1);
            if(key == 'q') break;
        }
        
        // sleep(1); 
    }
*/

    return 0;
    
}