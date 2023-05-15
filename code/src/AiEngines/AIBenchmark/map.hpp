#ifndef MAP_H
#define MAP_H

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <fstream>
#include <algorithm>
#include <limits>
#include <sstream>
#include <iomanip>
#include <math.h>
#include "../nlohmann/json.hpp"
#include "./../AiCore/AiTypeData.hpp"
using nlohmann::json;

class ACCURACY
{
    public:
        std::vector<double> p_result;
        std::vector<double> r_result;
        std::vector<double> AP_result;
        std::string label_r;
        // void writeToFile(std::string filename);
        double iou(ObjectTrace &rect1, ObjectTrace &rect2);
        double mAP(std::vector<std::vector<ObjectTrace>> pred_boxes, std::vector<ObjectTrace> true_boxes, 
                    double iou_threshold, std::vector<std::string> &labels);
        void calcMAP(json PredictBox, json GroundTruthBox,double iou_threshold, std::vector<std::string> &labels);
};

#endif // MAP_H
