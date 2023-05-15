#include "../AiCore/paddle/OCR.hpp"
#include "../AiEngines/Tracking/ObjTracking.hpp"
#include<iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <opencv2/freetype.hpp>


#include "../AiEngines/Anpr/AnprRecognizex86.hpp"
using namespace std;
std::string GetFileName(const std::string& s) {

    char sep = '/';

    size_t i = s.rfind(sep, s.length());
    std::string s1;
    if (i != std::string::npos) 
    {
        s1 = s.substr(i+1, s.length() - i);
    }

    sep = '.';
    i = s1.rfind(sep, s1.length());
    if (i != std::string::npos) 
    {
        return (s1.substr(0, i));
    }

    return("");
}
int main()
{
    setenv("OPENCV_FFMPEG_CAPTURE_OPTIONS", "rtsp_transport;tcp", 1);
    cout << "====Main=====" << endl;
    cv::Mat frame;
    // // cv::resize(img, img, cv::Size(img.cols * 0.75, img.rows * 0.75));
    // std::string character;
    // OCR ocr;
    // ocr.RunOCR(img, character);
    // cv::rectangle(img, plate[i].rect, cv::Scalar(0,255,0));
    // cv::putText(img, plate[i].license, cv::Point(plate[i].rect.x, plate[i].rect.y), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 2);
    // cout<< character <<endl;
    std::vector<PlateInfor> plate;
    AnprRecognizerx86 bienso;
    ObjTracking 

//     cv::VideoCapture cap("rtsp://vmsqn4.quangninh.gov.vn:9447/5d5e8ff5-1e8d-4dfa-9e7d-e142a51fc7f1?token=eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiJab25lTWluZGVyIiwiaWF0IjoxNjUyODYzNTQyLCJleHAiOjE2Nzg3ODM1NDIsInVzZXIiOiJhZG1pbiIsInR5cGUiOiJhY2Nlc3MifQ.co1DKYb139kwLOdLYXLj5RUGrxdsvzrBgh3c9udIN-E", CAP_FFMPEG);
// 	if(!cap.isOpened())
// 	{
// 		printf("open video error\n");
// 		return 0 ;
// 	}
//     int count = 1;
//    cv::Mat frame;
//     while(1)
// 	{
		// cap >> frame;
		// if (frame.empty()) 
        //     continue;
    std::vector<cv::String> fn;
    std::string nameFile;
    cv::Ptr<cv::freetype::FreeType2> ft2 = cv::freetype::createFreeType2();
    ft2->loadFontData( "../models/NotoSansCJKjp-Regular.otf", 0 );
    glob("/media/tund/Disk1/3.TestData/TestBienso/china100/*.jpg", fn, false);
    for(int i = 0 ; i <= fn.size() ; i++ )
    {
        frame = imread(fn[i]);
        
        nameFile =  GetFileName(fn[i]) + ".jpg";
        
        std::string outputFile = "/media/tund/Disk1/3.TestData/TestBienso/chinaout1/" + nameFile;
        std::cout << outputFile <<std::endl;
        bienso.recognize(frame, plate);


        for (int i = 0 ; i < plate.size(); i++)
        {
            cv::rectangle(frame, plate[i].rect, cv::Scalar(0,255,0));
            std::cout << plate[i].license << std::endl;
            // cv::putText(frame, plate[i].license, cv::Point(plate[i].rect.x, plate[i].rect.y), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 2);
            ft2->putText( frame, plate[i].license, cv::Point(plate[i].rect.x, plate[i].rect.y - 50), 40, cv::Scalar( 0, 0, 255 ), -1, cv::LINE_AA, false );
        }
         cv::imwrite(outputFile, frame);
    } 
        
        // std::string nameOutputFile = "/media/tund/New Volume/2.MyProject/AiCorex86_64/plate/15/" + to_string(count) + ".jpg";
        // if (plate.size() != 0) {
        //     cv::imwrite(nameOutputFile, frame);
        //     count ++;
        // }
       
        // cv::imshow("aa", frame);
        // cv::waitKey(0);
    // }


    return 0;
}