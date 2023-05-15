#include "map.hpp"


double ACCURACY::iou(ObjectTrace &obj1, ObjectTrace &obj2)
{
    int x1 = std::max(obj1.rect.x, obj2.rect.x);
    int y1 = std::max(obj1.rect.y, obj2.rect.y);
    int x2 = std::min(obj1.rect.x + obj1.rect.width, obj2.rect.x + obj2.rect.width);
    int y2 = std::min(obj1.rect.y + obj1.rect.height, obj2.rect.y + obj2.rect.height);
    int w = std::max(0, (x2 - x1 + 1));
    int h = std::max(0, (y2 - y1 + 1));
    double inter = w * h;
    double area1 = obj1.rect.width * obj1.rect.height;
    double area2 = obj2.rect.width * obj2.rect.height;
    double o = inter / (area1 + area2 - inter);
    return (o >= 0) ? o : 0;
}
bool compare(ObjectTrace &i, ObjectTrace &j)
{ return i.score > j.score; }

double ACCURACY::mAP(std::vector<std::vector<ObjectTrace>> pred_boxes, std::vector<ObjectTrace> true_boxes,
                         double iou_threshold, std::vector<std::string> &labels)
{
    std::vector<double> sum_AP;
    std::vector<double> AP_vector;
    std::vector<ObjectTrace> trueBoxesWithLabel;
    double P_sum;
    double R_sum;
    double TP = 0;
    double FP = 0;
    
    for (int c = 0; c < labels.size(); c++)
    {
        label_r = labels[c];
        std::cout << "-----------" << label_r << "-----------" << std::endl;
        trueBoxesWithLabel.clear();
        int ground_num = 0;
        for (int i=0 ; i< true_boxes.size() ; i++)
        {
            if (true_boxes[i].label == std::to_string(c))
            {
                trueBoxesWithLabel.push_back(true_boxes[i]);
                // ground_num ++;
            }
        }
        // if (ground_num == 0) continue;
        // std::cout<< "c = " << c << std::endl;
        std::vector<ObjectTrace> detections;
        std::vector<double> IoU;
        int dem = 0;
        for (int i = 0; i < pred_boxes.size(); i++)
        {
            for (int j = 0; j < pred_boxes[i].size(); j++)
            {
                if (pred_boxes.at(i).at(j).label == labels[c])
                {
                    detections.push_back(pred_boxes.at(i).at(j));
                }
            }
        }
        // std::cout<< "trueBoxesWithLabel = " << trueBoxesWithLabel.size() << std::endl;
        // std::cout<< "detections = " << detections.size() << std::endl;
        if (detections.size() > 0 && trueBoxesWithLabel.size() > 0)
        {
            if (detections.size() >= 2 && trueBoxesWithLabel.size() >= 2)
            {
                sort(detections.begin(), detections.end(), compare);
                TP = 0;
                FP = 0;
                std::vector<double> recall;
                std::vector<double> precision;
                int total_true_boxes = trueBoxesWithLabel.size();
                //std::cout << "\ntotal_trueboxes: " << total_true_boxes;
                
                //std::cout << "\n";
                for (int i = 0; i < detections.size(); i++)
                {
                    float iou_max = iou(detections[i], trueBoxesWithLabel[0]);
                    for (int j = 1; j < trueBoxesWithLabel.size(); j++)
                    {
                        iou_max = (iou_max > iou(detections[i], trueBoxesWithLabel[j])) ? iou_max : iou(detections[i], trueBoxesWithLabel[j]);
                    }
                    IoU.push_back(iou_max);
                    //std::cout << "IoU" << i + 1 << ": " << IoU[i] << " ";
                }
                std::cout<<"detected: " << IoU.size() << " / " << trueBoxesWithLabel.size() << std::endl;
                // for(int i=0;i<IoU.size();i++)
                // {
                //     std::cout<<"iou   "<<IoU[i]<<std::endl;
                // }
                for (int i = 0; i < detections.size(); i++)
                {
                    if (IoU[i] > iou_threshold)
                    {
                        TP += 1;
                    }
                    else
                    {
                        FP += 1;
                    }
                    //std::cout << "\nTP: " << TP << " FP: " << FP << std::endl;
                    P_sum = TP / (TP + FP);
                    
                    R_sum = TP / total_true_boxes;
                    R_sum = (R_sum > 1) ? 1 : R_sum;
                    
                    precision.push_back(P_sum);
                    p_result.push_back(P_sum);
                    recall.push_back(R_sum);
                    r_result.push_back(R_sum);
                }
                
                double AP = 0;
                // std::string file = "/home/manhtdg/Desktop/AIbench_API/AIBenchClone/output/bangAP/" + labels[c] + ".txt";
                // std::ofstream fileout(file.c_str());
                //     //fileout << "AUC: " << AP_result[AP_result.size() - 1] << "\n";
                //     fileout << "         "
                //             << "AP"
                //             << "\t              "
                //             << "Gia Tri"
                //             << "\n";
                for (int i = recall.size() - 1; i > 0; i--)
                {
                    if (precision[i] > precision[i - 1])
                    {
                        precision[i - 1] = precision[i];
                    }
                    AP += precision[i] * (recall[i] - recall[i - 1]);
                    //std::cout << "AP" << i << ": " << AP << std::endl;
    //for (int i = 0; i < p_result.size(); i++)
    //{
                    // fileout << "\t" << i << "\t            " << AP << "\n";
    //}
                }
                
                AP += precision[0] * recall[0];
                    // fileout << "\t" << 0 << "\t            " << AP << "\n";
                sum_AP.push_back(AP);
                AP_result.push_back(AP);
                std::cout  <<"AP: " << AP << std::endl;

            }
            else
            {
                if (iou(detections[0], true_boxes[0]) > iou_threshold)
                {
                    sum_AP.push_back(1 / true_boxes.size());
                    AP_result.push_back(1 / true_boxes.size());
                }
                else
                {
                    sum_AP.push_back(0);
                    AP_result.push_back(0);
                }
            }
            // writeToFile("../output/bang_precision_recall/" + labels[c] + ".txt");
        }
        std::cout << "Precision: " << P_sum << "\t";
        std::cout << "Recall: " << R_sum << std::endl;
        std::cout << "TP: " << TP << "\t";
        std::cout << "FP: " << FP << std::endl;
    }
    
    double sAP = 0, mAP;
    for (int i = 0; i < sum_AP.size(); i++)
    {
        sAP += sum_AP[i];
    }
    mAP = sAP / sum_AP.size();
    std::cout << "\nmAP = ";
    return mAP;
}

void ACCURACY::calcMAP(json PredictBox, json GroundTruthBox,double iou_threshold, std::vector<std::string> &labels)
{
    int count = 1;
    std::vector<double> listAP;
    for (int c = 0; c < labels.size(); c++)
    {
        double TP = 0;
        double FP = 0;
        double FN = 0;
        double totalTruthBox = 0;
        double Precision = 0;
        double Recall = 0;
        double AP = 0;
        label_r = labels[c];
        std::vector<ObjectTrace> listGBox;
        std::vector<ObjectTrace> listPBox;
        std::vector<double> listPrecision;
        std::vector<double> listRecall;
        std::cout << "-----------" << label_r << "---------------" <<std::endl;

        //get number of total ground truth box
        for (int i = 0; i < GroundTruthBox.size() ; i++){
            for (int j = 0 ; j < GroundTruthBox[i]["object"].size() ; j++){
                if (GroundTruthBox[i]["object"][j]["class"] == label_r){
                    totalTruthBox +=1;
                }
                
            }
        }
        
        // calculating precision and recall per image
        if (totalTruthBox != 0){
            for (int i = 0; i < PredictBox.size(); i ++){
  
                for (int j = 0; j < GroundTruthBox.size(); j++){
            
                    if (PredictBox[i]["name"] == GroundTruthBox[j]["name"]){
                        listGBox.clear();
                        listPBox.clear();
                        // std::cout << " =========================" <<std::endl;
                        // std::cout << "couple: " << count <<std::endl;
                    //    std::cout << "name: " << GroundTruthBox[j]["name"] <<std::endl;
                    count++;
                        double TP_temp = 0;
                        double FP_temp = 0;
                        double FN_temp = 0;

                        //adding all ground truth box with label in an image
                        for (int k = 0; k < GroundTruthBox[j]["object"].size(); k++){
                            
                            // totalTruthBox += 1;
                            std::string nameGroundTruth = GroundTruthBox[j]["object"][k]["class"];
                            if (nameGroundTruth == label_r){
                                ObjectTrace gBox;
                                gBox.rect.x = GroundTruthBox[j]["object"][k]["box"][0];
                                gBox.rect.y = GroundTruthBox[j]["object"][k]["box"][1];
                                gBox.rect.width = GroundTruthBox[j]["object"][k]["box"][2];
                                gBox.rect.height = GroundTruthBox[j]["object"][k]["box"][3];
                                listGBox.push_back(gBox);
                            }
                        }   
                        //adding all predict box with label in an image
                        for (int l = 0 ; l < PredictBox[i]["object"].size(); l++){
                            std::string namePredict = PredictBox[i]["object"][l]["class"]; 
                            if (namePredict == label_r){
                                ObjectTrace pBox;
                                pBox.rect.x = PredictBox[i]["object"][l]["box"][0];
                                pBox.rect.y = PredictBox[i]["object"][l]["box"][1];
                                pBox.rect.width = PredictBox[i]["object"][l]["box"][2];
                                pBox.rect.height = PredictBox[i]["object"][l]["box"][3];
                                listPBox.push_back(pBox);
                            } 
                        }
                        int predictCount = listPBox.size();
                        int groundTruthCount = listGBox.size();
                        for (int x = 0 ; x < listGBox.size() ; x++){
                            bool check = false;
                            for (int y = 0 ; y < listPBox.size() ; y++){
                                double calcIOU = iou(listGBox[x], listPBox[y]);
                                if (calcIOU > iou_threshold){
                                    if (check == false){
                                        TP_temp += 1;
                                        TP += 1;
                                        Precision = TP / (TP + FP);     listPrecision.push_back(Precision);
                                        Recall = TP / totalTruthBox;    listRecall.push_back(Recall);
                                        listPBox.erase(listPBox.begin() + y);
                                        check = true;
                                    }
                                    
                                }else {
                                    continue;
                                }
                            }
                        }
                        FP_temp = predictCount - TP_temp;
                        for (int y = 0 ; y < FP_temp ; y++){
                            FP += 1;
                            Precision = TP / (TP + FP);     listPrecision.push_back(Precision);
                            Recall = TP / totalTruthBox;    listRecall.push_back(Recall);
                        }
                        FN_temp = groundTruthCount - TP_temp;
                        // TP += TP_temp;
                        // FP += FP_temp;
                        FN += FN_temp;
                        // if ((TP_temp != 0) || (FP_temp != 0) ){
                        //     if (TP + FP == 0){
                        //         Precision = 0;     listPrecision.push_back(Precision);
                        //     }else{
                        //         Precision = TP / (TP + FP);     listPrecision.push_back(Precision);
                        //     }
                            
                        //     Recall = TP / totalTruthBox;    listRecall.push_back(Recall);
                        // }
                       

                        // std::cout << "Precision: "<< Precision << "\t" << "Recall: "<< Recall << std::endl;
                        // std::cout << "Recall: "<< Recall << std::endl;
                        // std::cout << "totalTruthBox: "<< totalTruthBox << std::endl;
                    } else{
                        continue;
                    }
                }
            }
            //calculating AP
            // AP += listPrecision[0] * listRecall[0];
            // listPrecision.insert(listPrecision.begin(), 0);
            // listPrecision.push_back(0);
            // listRecall.insert(listRecall.begin(),0);
            // listRecall.push_back(1);
            std::cout << "size list prec: " <<listPrecision.size() << std::endl;
            std::cout << "size list rec: " <<listRecall.size() << std::endl;
            // for (int i =0;i< listPrecision.size();i++){
            //     std::cout << listPrecision[i] << std::endl;
            // }
            // for (int i = listPrecision.size() - 2; i >-1 ; i--){
            //     if (listPrecision[i] < listPrecision[i+1]){
            //         listPrecision[i] = listPrecision[i+1];
            //     }
            // }
            
            // std::vector<double> i_list;
            // for (int i = 0; i < listRecall.size(); i++){
            //     if (listRecall[i] != listRecall[i+1]){
            //         i_list.push_back(i);
            //     }
            // }
            // for(int i= 0; i< i_list.size(); i++){
            
            // }
            // for (int i = 1; i < listRecall.size(); i++){
            //     AP += (listRecall[i] - listRecall[i-1] ) * listPrecision[i];
            // }
            // AP += listRecall[0]*listPrecision[0];

            // listAP.push_back(AP);
            std::cout << "TP: "<< TP << std::endl;
            std::cout << "FP: "<< FP << std::endl;
            std::cout << "FN: "<< FN << std::endl;
            std::cout << "totalTruthBox: "<< totalTruthBox << std::endl;
            std::cout << "Precision: "<< Precision << std::endl;
            std::cout << "Recall: "<< Recall << std::endl;
            // std::cout << "AP: " << AP << std::endl;
        }
        
    }
    //calculating mAP
    // double sumAP = 0;
    // double mAP = 0;
    // for (int i = 0 ; i< listAP.size(); i++){
    //     sumAP += listAP[i];
    // }
    // mAP = sumAP / listAP.size();
    // std::cout << "---------------" <<std::endl;
    // std::cout << "mAP: " << mAP << std::endl;
}
// void ACCURACY::writeToFile(std::string filename)
// {
//     std::ofstream fileout(filename.c_str());
//     fileout << "AUC: " << AP_result[AP_result.size() - 1] << "\n";
//     fileout << "     "
//             << "precison"
//             << "\t            "
//             << "recall"
//             << "\n";
//     for (int i = 0; i < p_result.size(); i++)
//     {
//         fileout << i + 1 << "\t" << p_result[i] << "\t            " << r_result[i] << "\n";
//     }
//     RGBABitmapImageReference *imageReference = CreateRGBABitmapImageReference();
//     DrawScatterPlot(imageReference, 600, 400, &r_result, &p_result);
//     std::vector<double> *pngdata = ConvertToPNG(imageReference->image);
//     WriteToFile(pngdata, "../output/so_do/" + label_r + "manh.png");
//     DeleteImage(imageReference->image);
//     r_result.clear();
//     p_result.clear();
// }


