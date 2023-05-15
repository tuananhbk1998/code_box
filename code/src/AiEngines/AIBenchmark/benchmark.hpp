#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <queue>
#include "map.hpp"
#include "../nlohmann/json.hpp"
#include"./../AiCore/ObjectDetector.hpp"


using nlohmann::json;
class Benchmark
{
    private:
        ObjectDetector* detector;
        // std::queue<std::string> m_InputImgFilePath;
        std::vector<std::string> m_InputImgFilePath;
        std::vector<std::string> m_InputVerifyData;
        std::vector<std::string> m_Labels;
        std::vector<cv::Mat> listImg;
    public:
        Benchmark();
         ~Benchmark() {};

        int Init( std::string& framework, std::string& objectTarget, 
                        std::string& classFile, std::string& imagesPath, std::string& groundTruthBox);
        int Run(float threshold = 0.5f);
        // void Finish() = 0;
    
};





#endif // BENCHMARK_H
