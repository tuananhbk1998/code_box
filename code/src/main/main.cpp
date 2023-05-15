#include<iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include"../AiEngines/Intrustion/IntrustionManager.hpp"
#include"../AiCore/ObjectDetector.hpp"

int main()
{
    IntrustionManager* intrusion = new IntrustionManager();
	intrusion->initIntrustion();
    cv::Mat frame;
    cv::VideoCapture cap;
	cap.open("../videos/person5.mp4");
	if(!cap.isOpened())
	{
		printf("open video error\n");
		return 0 ;
	}
    int frame_width = (cap.get(CAP_PROP_FRAME_WIDTH )) /2;
    int frame_height = (cap.get(CAP_PROP_FRAME_HEIGHT)) /2;
        // create intrustion area
    std::vector<cv::Point> singlePoint;
    singlePoint.push_back(cv::Point(250,285));
    singlePoint.push_back(cv::Point(300,450));
    singlePoint.push_back(cv::Point(800,500));
    singlePoint.push_back(cv::Point(650,300));
    std::vector<std::vector<cv::Point>> areaIntrus;
    areaIntrus.push_back(singlePoint);
    vector<PersonTrace> objects;
    std::vector<PersonTrace> objInArea;
    int minObj = 2;
    bool b_first = false;
    bool b_return =  false;
    int num = 1;
    std::string saveFileName;
    double timeReturnStart = 0;
    double frequency = cv::getTickFrequency();
	while(1)
	{
        std::cout << "-------------------------" <<std::endl; 
		cap >> frame;
		if (frame.empty()) 
            break;
        cv::resize(frame, frame, cv::Size(frame_width,frame_height ));
        cv::line(frame,singlePoint[0], singlePoint[1],cv::Scalar(0,0,255), 2,8);
        cv::line(frame,singlePoint[1], singlePoint[2],cv::Scalar(0,0,255), 2,8);
        cv::line(frame,singlePoint[2], singlePoint[3],cv::Scalar(0,0,255), 2,8);
        cv::line(frame,singlePoint[3], singlePoint[0],cv::Scalar(0,0,255), 2,8);

        objects.clear();
        objInArea.clear();
        intrusion->run(frame, objects,singlePoint );
        for(auto v : objects)
        {
            
            rectangle(frame, v.rect, cv::Scalar(255, 255, 255), 2, 8);
            char text[100];
            sprintf(text, "%d", v.track_id);
            putText(frame, text, cv::Point(v.rect.x, v.rect.y), FONT_HERSHEY_COMPLEX, 1.0, cv::Scalar(255, 255, 255));
            std::string time_obj = std::to_string(v.duration);
            putText(frame, time_obj, cv::Point(v.rect.x + v.rect.width / 2, v.rect.y ), FONT_HERSHEY_COMPLEX, 1.0, cv::Scalar(255, 255, 255));
        }
        std::cout <<"Object inside couting: "<< objects.size() << std::endl;


        if (objects.size() >= minObj)
        {
            //canh bao lan dau
            for (auto o : objects) {
                if (o.isNew == true){
                    saveFileName = "../output/new_" + std::to_string(num) + ".jpg";
                    cv::imwrite(saveFileName, frame);
                    num++;
                }
            }
            //them cac doi tuong du thoi gian
            for (int i = 0; i < objects.size(); i++)
            {
                if (objects[i].duration > 2) {
                    objInArea.push_back(objects[i]);
                }
            }
            //canh bao lap lai
            if (b_first  == false){
                if (objInArea.size() >= minObj){
                    b_first = true;
                    timeReturnStart = cv::getTickCount();
                    for (int j = 0; j < objInArea.size(); j++)
                    {
                        cv::rectangle(frame, objects[j].rect, cv::Scalar(0,0,255), 2,8);
                    } 
                    saveFileName = "../output/return_" + std::to_string(num) + ".jpg";
                    cv::imwrite(saveFileName, frame);
                    num++;
                }
            } else {
                if (((cv::getTickCount() - timeReturnStart) / frequency) > 2) {
                    if (objInArea.size() >= minObj) {
                        for (int j = 0; j < objInArea.size(); j++)
                        {
                            cv::rectangle(frame, objects[j].rect, cv::Scalar(0,0,255), 2,8);
                        }
                        saveFileName = "../output/return_" + std::to_string(num) + ".jpg";
                        cv::imwrite(saveFileName, frame);
                        num++;
                        timeReturnStart = cv::getTickCount();
                    } else {
                        b_first = false;
                    }
                }
            }
            
        }
        else {
            b_first = false;
        }

        char key = cv::waitKey(1);
        imshow("Video", frame);
        if(key == 'q') 
        {
            printf("Quited\n");
            break;
        }
        if(key == 'p') 
        {
            printf("Paused\n");
            while(cv::waitKey(1) != 'p');
        }

    }



    return 0;
}

/*
int main()
{
    ObjectDetector* detector = new ObjectDetector();
    detector->initObjectDetector("snpe", "person", "CPU");
    std::vector<ObjectTrace> objects;
    cv::VideoCapture cap(0);
	if (!cap.isOpened()){
        std::cout <<"Error opening video stream or file" << std::endl;
        return -1;
    }
    while(1)
    {
        std::cout << " =======" << std::endl;
        cv::Mat frame;
        cap >> frame;
        objects.clear();
        detector->executeObjectDetector(frame, objects, 0.5);
        for (int i = 0; i< objects.size(); i++)
        {
             rectangle(frame, objects[i].rect, cv::Scalar(255, 255, 255), 2, 8);
             putText(frame, std::to_string(objects[i].score), cv::Point(objects[i].rect.x, objects[i].rect.y), FONT_HERSHEY_COMPLEX, 1.0, cv::Scalar(255, 255, 255));
        }
        
        cv::imshow("Video", frame);
        
        char c= (char)cv::waitKey(25);
        if (c==27)
            break;
    }





    return 0;
}
*/