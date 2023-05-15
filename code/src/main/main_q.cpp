#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>
#include <unistd.h>
#include <iostream>
#include <vector>

#include "BlowRedLight.hpp"

int main()
{
	cv::Mat frame;
	BlowRedLight* blowRedLight = new BlowRedLight();
    blowRedLight->initBlowRedLight();

    settingBlowRedLight settings;
    settings.allowedObjects = {"", ""};
    // settings.regionSet = {{{1201,35},{1252,33},{1252,0},{1200,0}},
    //                         {{1205,35},{1252,35},{1247,82},{1201,82}},
    //                         {{1201,84},{1247,82},{1242,134},{1195,134}},
    //                         {{30,531},{95,268}},
    //                         {{1061,465},{949,233}},
    //                         {{1049,600},{53,600}},
    //                         {{959,248},{97,258}},
    //                         // {{1070,600}, {400, 600}},
    //                         {{1100,720}, {1070,550}, {400, 550}, {400, 720}}
    //                      };

    // settings.regionSet = {{{1844, 16},{1866, 16},{1866, 26},{1844, 26}},
    //                     {{1844, 50},{1866, 50},{1866, 66},{1844, 66}},
    //                     {{1844, 92},{1866, 92},{1866, 106},{1844, 106}},
    //                     {{0, 0},{0, 0}},
    //                     {{0, 0},{0, 0}},
    //                     {{196, 754},{1918, 820}},
    //                     {{298, 542},{1700, 574}},
    //                     {{0,948}, {316,622}, {1918, 664}, {1918, 1078}, {0, 1078}}
    //                 };

    settings.regionSet = {{{1240, 192}, {1254, 192}, {1254, 201}, {1240, 201}},
                        {{1242, 212}, {1252, 212}, {1252, 219}, {1241, 219}},
                        {{1240, 229}, {1252, 231}, {1250, 237}, {1240, 237}},
                        {{369, 368}, {526, 300}},
                        {{0, 0}, {0, 0}},
                        {{373, 421}, {1192, 425}},
                        {{579, 314}, {1144, 331}},
                        {{765, 718}, {903, 399}, {1195, 399}, {1279, 549}, {1038, 719}},
                        {{390, 719}, {740, 399}, {903, 399}, {765, 718}},
                        {{43, 719}, {588, 400}, {740, 399}, {390, 719}},
                        {{1, 555}, {397, 392}, {588, 400}, {43, 719}},
                        {{350, 234}, {359, 234}, {360, 240}, {352, 241}},
                        {{352, 250}, {360, 249}, {360, 255}, {352, 241}},
                        {{353, 263}, {362, 264}, {362, 271}, {354, 271}}
                    };

    // settings.labelSet = {"1", "1", "1", "4", "5", "2", "3", "7"};
    settings.labelSet = {"1", "1", "1", "4", "5", "2", "3", "7", "7", "8", "10", "0", "0", "0"};
    settings.allowLeft = 0;
    settings.allowRight = 0;
    if(blowRedLight->setBlowRedLight(settings) != STATUS_SUCCESS) return -1;

	cv::VideoCapture cap;
	cap.open("../videos/1102.mp4");
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
		
        cv::resize(frame, frame, Size(1280, 720));

		// processing
		vector<outputBlowRedLight> outData;
		blowRedLight->updateBlowRedLight(frame, outData);
		for(auto out : outData)
		{
			// if(out.isBlowRedLight)
			// {
                // for(int i=0; i<out.images.size(); i++) {
                //     std::string outImg{"../outputs/out_" + std::to_string(n) + "_" + std::to_string(i) + ".jpg"};
                //     cv::imwrite(outImg, out.images[i]);
                // }
                    // std::string outImg{"../outputs/out_" + std::to_string(rand()) + ".jpg"};
                    // cv::imwrite(outImg, out.imgEvent);
				// cv::rectangle(frame, out.rect, cv::Scalar(0, 0, 255), 3, 8);
				// cv::putText(frame, "!!!", cv::Point(out.rect.x, out.rect.y), FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 2);
			// } else {
			// 	cv::rectangle(frame, out.rect, cv::Scalar(0, 255, 0), 3, 8);
			// }

            // if(!out.imgEvent.empty()) {
                std::string outImg{"../outputs/" + std::to_string(n) + ".jpg"};
                cv::imwrite(outImg, out.imgEvent);
            // }
            n += 1;
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
