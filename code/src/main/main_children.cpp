#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <unistd.h>
#include "../major/Children/Children.hpp"
using namespace std;
using namespace cv;

int main() {
    Children *children = new Children();
    cv::Mat frame;

    stSettingChild settings;
    settings.strObjectSet = {"person"};
    // settings.ptRegionSet = {{202, 47}, {202, 713}, {756, 713}, {756, 47}, {202, 47}};
    // settings.SetRegion_1 = {{0, 0}, {0, 262}, {1920, 262}, {1920, 0}, {0, 0}};
    // settings.SetRegion_2 = {{0, 262}, {0, 600}, {1920, 600}, {1920, 262}, {0, 262}};
    // settings.SetRegion_3 = {{0, 600}, {0, 1080}, {1920, 1080}, {1920, 600}, {0, 600}};

    // settings.SetRegion_1 = {{0, 0}, {384, 0}, {384, 262}, {0, 262}, {0, 0}};
    // settings.SetRegion_2 = {{0, 262}, {384, 262}, {384, 600}, {0, 600}, {0, 262}};
    // settings.SetRegion_3 = {{0, 600}, {384, 600}, {384, 1080}, {0, 1080}, {0, 600}};
    // settings.SetRegion_4 = {{384, 0}, {960, 0}, {960, 262}, {384, 262}, {384, 0}};
    // settings.SetRegion_5 = {{384, 262}, {960, 262}, {960, 600}, {384, 600}, {384, 262}};
    // settings.SetRegion_6 = {{384, 600}, {960, 600}, {960, 1080}, {384, 1080}, {384, 600}};
    // settings.SetRegion_7 = {{960, 0}, {1536, 0}, {1536, 262}, {960, 262}, {960, 0}};
    // settings.SetRegion_8 = {{960, 262}, {1536, 262}, {1536, 600}, {960, 600}, {960, 262}};
    // settings.SetRegion_9 = {{960, 600}, {1536, 600}, {1536, 1080}, {960, 1080}, {960, 600}};
    // settings.SetRegion_10 = {{1536, 0}, {1920, 0}, {1920, 262}, {1536, 262}, {1536, 0}};
    // settings.SetRegion_11 = {{1536, 262}, {1920, 262}, {1920, 600}, {1536, 600}, {1536, 262}};
    // settings.SetRegion_12 = {{1536, 600}, {1920, 600}, {1920, 1080}, {1536, 1080}, {1536, 600}};

    settings.SetRegion_1 = {{0, 0}, {274, 0}, {274, 262}, {0, 262}, {0, 0}};
    settings.SetRegion_2 = {{0, 262}, {274, 262}, {274, 600}, {0, 600}, {0, 262}};
    settings.SetRegion_3 = {{0, 600}, {274, 600}, {274, 1080}, {0, 1080}, {0, 600}};
    settings.SetRegion_4 = {{274, 0}, {548, 0}, {548, 262}, {274, 262}, {274, 0}};
    settings.SetRegion_5 = {{274, 262}, {548, 262}, {548, 600}, {274, 600}, {274, 262}};
    settings.SetRegion_6 = {{274, 600}, {548, 600}, {548, 1080}, {274, 1080}, {274, 600}};
    settings.SetRegion_7 = {{548, 0}, {822, 0}, {822, 262}, {548, 262}, {548, 0}};
    settings.SetRegion_8 = {{548, 262}, {822, 262}, {822, 600}, {548, 600}, {548, 262}};
    settings.SetRegion_9 = {{548, 600}, {822, 600}, {822, 1080}, {548, 1080}, {548, 600}};
    settings.SetRegion_10 = {{822, 0}, {1096, 0}, {1096, 262}, {822, 262}, {822, 0}};
    settings.SetRegion_11 = {{822, 262}, {1096, 262}, {1096, 600}, {822, 600}, {822, 262}};
    settings.SetRegion_12 = {{822, 600}, {1096, 600}, {1096, 1080}, {822, 1080}, {822, 600}};
    settings.SetRegion_13 = {{1096, 0}, {1370, 0}, {1370, 262}, {1096, 262}, {1096, 0}};
    settings.SetRegion_14 = {{1096, 262}, {1370, 262}, {1370, 600}, {1096, 600}, {1096, 262}};
    settings.SetRegion_15 = {{1096, 600}, {1370, 600}, {1370, 1080}, {1096, 1080}, {1096, 600}};
    settings.SetRegion_16 = {{1370, 0}, {1644, 0}, {1644, 262}, {1370, 262}, {1370, 0}};
    settings.SetRegion_17 = {{1370, 262}, {1644, 262}, {1644, 600}, {1370, 600}, {1370, 262}};
    settings.SetRegion_18 = {{1370, 600}, {1644, 600}, {1644, 1080}, {1370, 1080}, {1370, 600}};
    settings.SetRegion_19 = {{1644, 0}, {1920, 0}, {1920, 262}, {1644, 262}, {1644, 0}};
    settings.SetRegion_20 = {{1644, 262}, {1920, 262}, {1920, 600}, {1644, 600}, {1644, 262}};
    settings.SetRegion_21 = {{1644, 600}, {1920, 600}, {1920, 1080}, {1644, 1080}, {1644, 600}};

    settings.line1 = {{274, 0}, {274, 1080}};
    settings.line2 = {{548, 0}, {548, 1080}};
    settings.line3 = {{822, 0}, {822, 1080}};
    settings.line4 = {{1096, 0}, {1096, 1080}};
    settings.line5 = {{1370, 0}, {1370, 1080}};
    settings.line6 = {{1644, 0}, {1644, 1080}};
    settings.line7 = {{1918, 0}, {1918, 1080}};
    settings.line8 = {{0, 262}, {1920, 262}};
    settings.line9 = {{0, 600}, {1920, 600}};
    // settings.SetRegion_4 = {{0, 1040}, {1920, 1040}};
    // settings.SetRegion_2 = std::vector<cv::Point>{cv::Point(0, 262), cv::Point(0, 590), cv::Point(1920, 590), cv::Point(1920, 262), cv::Point(0, 262)};
    children->Child_Set(settings);
    cv::VideoCapture cap("/home/anhmtc/Downloads/test.mp4");
    if (!cap.isOpened()) {
        printf("open video error\n");
        return 0;
    }
    std::string namefile = "./child.avi";
    double fps = 6.0;
    int countframe = 0;
    children->Child_Init();
    cv::VideoWriter video(namefile, cv::VideoWriter::fourcc('M','J','P','G'), fps, cv::Size(1920,1080));
    while(1) {
        cap >> frame;
        countframe++;
        if (countframe % 6 == 0) {
            if (frame.empty())
                continue;
            std::cout << "Frame: " << countframe << std::endl;
            std::vector<OutputChild> outputs;
            children->Child_Execute(frame, outputs);
            // for (int i = 0 ; i < settings.SetRegion_1.size() - 1 ; i++) {
            //     cv::line(frame, settings.SetRegion_1[i], settings.SetRegion_1[i+1], cv::Scalar(255, 0, 0), 2);
            // }
            for (int j = 0 ; j < settings.line9.size() - 1 ; j++) {
                cv::line(frame, settings.line9[j], settings.line9[j+1], cv::Scalar(0, 0, 255), 2);
            }
            for (int k = 0 ; k < settings.line8.size() - 1 ; k++) {
                cv::line(frame, settings.line8[k], settings.line8[k+1], cv::Scalar(255, 255, 255), 2);
            }
            for (int h = 0 ; h < settings.line1.size() - 1 ; h++) {
                cv::line(frame, settings.line1[h], settings.line1[h+1], cv::Scalar(255, 0, 0), 2);
            }
            for (int g = 0 ; g < settings.line2.size() - 1 ; g++) {
                cv::line(frame, settings.line2[g], settings.line2[g+1], cv::Scalar(255, 0, 0), 2);
            }
            for (int m = 0 ; m < settings.line3.size() - 1 ; m++) {
                cv::line(frame, settings.line3[m], settings.line3[m+1], cv::Scalar(255, 0, 0), 2); 
            }
            for (int h = 0 ; h < settings.line4.size() - 1 ; h++) {
                cv::line(frame, settings.line4[h], settings.line4[h+1], cv::Scalar(255, 0, 0), 2);
            }
            for (int g = 0 ; g < settings.line5.size() - 1 ; g++) {
                cv::line(frame, settings.line5[g], settings.line5[g+1], cv::Scalar(255, 0, 0), 2);
            }
            for (int m = 0 ; m < settings.line6.size() - 1 ; m++) {
                cv::line(frame, settings.line6[m], settings.line6[m+1], cv::Scalar(255, 0, 0), 2); 
            }
            for (int m = 0 ; m < settings.line7.size() - 1 ; m++) {
                cv::line(frame, settings.line7[m], settings.line7[m+1], cv::Scalar(255, 0, 0), 2); 
            }
            video.write(frame);
            cv::resize(frame, frame, Size(1080, 640));
            cv::imshow("window", frame);
            char c= (char)cv::waitKey(25);
            if (c==27)
                break;

        } else {
            continue;
        }
    }
    cap.release();
    cv::destroyAllWindows();
    // video.release();
    return 0;
}
