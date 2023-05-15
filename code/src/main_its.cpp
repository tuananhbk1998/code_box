#include<iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "./AiCore/ObjectDetector.hpp"
#include "./AiEngines/Tracking/ObjTracking.hpp"
#include "./Decoder/Decoder.hpp"
#include "./Decoder/DecoderThread.hpp"
#include "./Decoder/FrameManager.hpp"
#include "./AiCore/MotionDetector.hpp"
#include "./AiEngines/Tracking/ObjTracking.hpp"
#include "./AiEngines/Anpr/ITS.hpp"
using namespace std;
using namespace cv;

int main()
{
    ITS its;
    its.init(Nations::VN);
    VideoCapture cap("/home/tund/Videos/aiview_record_20221110082645.mp4");
    if (!cap.isOpened()){
        std::cout <<"Error opening video stream or file" << std::endl;
        return -1;
    }

    std::string filename = "./motion5.avi";   
    double fps = 30.0; 
    int codec = VideoWriter::fourcc('M', 'J', 'P', 'G'); 
    int frame_width = cap.get(CAP_PROP_FRAME_WIDTH );
    int frame_height = cap.get(CAP_PROP_FRAME_HEIGHT);
    cv::VideoWriter writer(filename, codec, fps,cv::Size(1920,1080) );
    int count_frame = 0;
    static cv::Mat prvFrame;
    while(1){
        cv::Mat frame;
        cap >> frame;
        count_frame++;
        if (count_frame % 3 == 0){
            std::cout << " ------------------- frame: " << count_frame << "--------------" << std::endl;
            int count = 1;
            std::vector<ITS_Output> outputs;
            its.Execute2(frame, frame, outputs);
            std::cout << "Output: " << outputs.size() <<std::endl;
            putText(frame, std::to_string(count_frame), cv::Point2d(100,100), FONT_HERSHEY_COMPLEX, 0.8, cv::Scalar(0,0,255), 2);
            if (outputs.size() != 0){
                for (auto& out : outputs){
                    switch (out.typeOutput)
                    {
                    case TypeOutput::Both:
                        rectangle(frame, out.vehicle.rectVehicle, cv::Scalar(255,255,0)); //xanh ngoc
                        rectangle(frame, out.plate.rect, cv::Scalar(255,255,0));
                        putText(frame, out.plate.license, out.vehicle.rectVehicle.tl(), FONT_HERSHEY_COMPLEX, 0.8, cv::Scalar(0,0,255), 2);
                        break;
                    case TypeOutput::OnlyPlate:
                        rectangle(frame, out.plate.rect, cv::Scalar(0,255,255)); //vang
                        putText(frame, out.plate.license, out.plate.rect.tl(), FONT_HERSHEY_COMPLEX, 0.8, cv::Scalar(0,0,255), 2);
                    case TypeOutput::OnlyVehicle:
                        rectangle(frame, out.vehicle.rectVehicle, cv::Scalar(0,255,0)); // xanh la
                    default:
                        break;
                    }
                    std::string nameFile = std::to_string(count_frame) +  "_" + std::to_string(count) +   ".jpg";
                    imwrite(nameFile, frame);
                    count++;
                }
                
            }
            writer.write(frame);
            cv::imshow("window", frame);
            char c= (char)cv::waitKey(25);
            if (c==27)
                break;
        }else{
            continue;
        }
        
    }
    writer.release();
    cap.release();
    cv::destroyAllWindows(); 

    return 0;
}