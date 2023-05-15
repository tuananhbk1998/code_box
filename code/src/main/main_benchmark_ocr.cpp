#include <iostream>
#include"../AiCore/paddle/OCR.hpp"
#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <map>
using namespace std;
using namespace cv;

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

std::vector<std::string> split(const std::string &str, const char delimiter)
{
    std::vector<std::string> ret;
    std::string cur_str("");
    for(unsigned int i=0; i < str.length(); i++) {
        if(str[i] == delimiter) {
            if(cur_str == "")
                continue;
            
            ret.push_back(cur_str);
            cur_str = "";
        } else {
            cur_str.push_back(str[i]);
        }
    }
    if(cur_str != "")
        ret.push_back(cur_str);
    if(ret.size() == 0)
        ret.push_back(str);
    return (ret);
}

// std::map<std::string, std::string> LoadConfig(const std::string &config_path) {
//   auto config = ReadDict(config_path);
  
//   std::map<std::string, std::string> dict;
//   for (int i = 0; i < config.size(); i++) {
//     // pass for empty line or comment
//     if (config[i].size() <= 1 || config[i][0] == '#') {
//       continue;
//     }

//     std::vector<std::string> res = split(config[i], " ");
//     dict[res[0]] = res[1];
//   }
//   return dict;
// }
int main()
{ 
    OCR* ocr = new OCR();
    std::vector<string> line;
    std::vector<string> element;
    std::fstream labelFile("/media/tund/HDD/Data/TestData/OCR/US/out.txt");
    if (labelFile.is_open())
    {
        std::string label;
        while (std::getline(labelFile, label))
        {
            line.push_back(label);
        }
    }
    int exact = 0;
    for(int i = 0 ; i< line.size(); i ++)
    {
        // cout << "------------------------" << endl;
        element.clear();
        cout <<  i << "\r" << std::flush;
        element = split(line[i], '\t');
        std::string nameImg = "/media/tund/HDD/Data/TestData/OCR/US/" + element[0];
        cv::Mat image = cv::imread(nameImg);
   
        std::string license;
        if (!image.empty()){
             ocr->RunOCR(image, license);
        }
        
        if (license == element[1]){
            exact += 1;
        }
        else{
            // cv::putText(image, license, cv::Point2d(0,image.rows /2), FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(0,0,255),1.2);
            //  std::string ouputName = "/data/tund/ocr/" + std::to_string(i) + ".jpg";
            // cv::imwrite(ouputName, image);
        }
       
   
    }
    
    std::cout << "Exact = " << exact<<std::endl;
    std::cout << "Total = " << line.size()<<std::endl;
    float accuracy = (float)exact / (float)line.size();
    std::cout << "Accuracy = " << accuracy * 100 << "%" << std::endl;
    

    return 0;
}