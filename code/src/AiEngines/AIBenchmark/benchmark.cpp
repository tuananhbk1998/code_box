#include"benchmark.hpp"
std::string GetFileName(std::string& s) {

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
Benchmark::Benchmark(){
    this->detector = new ObjectDetector();
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
int Benchmark::Init( std::string& framework, std::string& objectTarget, 
                        std::string& classFile, std::string& imagesPath, std::string& groundTruthBox)
{
    this->detector->initObjectDetector(framework,objectTarget, "CPU" );
    // TODO: Load image
    std::fstream imgFilePath(imagesPath);
    if (imgFilePath.is_open())
    {
        std::string imgPath;
        while (std::getline(imgFilePath, imgPath))
        {
            this->m_InputImgFilePath.push_back(imgPath);
        }
    }
    imgFilePath.close();
    // TODO: Load verify data
    std::fstream verifyFilePath(groundTruthBox);
    if (verifyFilePath.is_open())
    {
        std::string verifyData;
        while (std::getline(verifyFilePath, verifyData))
        {
            this->m_InputVerifyData.push_back(verifyData);
        }
    }
    verifyFilePath.close();
    // TODO: Load label file
    std::fstream labelFile(classFile);
    if (labelFile.is_open())
    {
        std::string label;
        while (std::getline(labelFile, label))
        {
            this->m_Labels.push_back(label);
        }
    }
    labelFile.close();

    return STATUS_SUCCESS;
}

int Benchmark::Run(float threshold)
{

    std::vector<std::vector<ObjectTrace>> totalDetectionObjects;
    std::vector<ObjectTrace> result;
    int i=1;
    ObjectTrace s;
    std::vector<ObjectTrace> ground;
    std::vector<ObjectTrace> truthBox;

    std::vector<std::string> lines;
    std::vector<std::string> element;
    int dem = 0;
    /*
    while (!this->m_InputImgFilePath.empty())
    {
        ground.clear();
        std::cout<<"Image: " << i <<'\r' <<std::flush;
        cv::Mat img = cv::imread(this->m_InputImgFilePath.front());

        this->detector->executeObjectDetector(img, result, threshold);
        totalDetectionObjects.push_back(result);
         
        std::fstream labelFilePath(this->m_InputVerifyData[i-1].c_str());
        if (labelFilePath.is_open())
        {
            line.clear();
            std::string objTruth;
            while (std::getline(labelFilePath, objTruth))
            {
                line.push_back(objTruth);
            }
            
        }
        for(int j = 0 ; j< line.size(); j ++)
        {
            element.clear();
            element = split(line[j], ' ');
            s.label = element[0];
            s.rect.x = stof(element[1]);
            s.rect.y = stof(element[2]);
            s.rect.width = stof(element[3]);
            s.rect.height = stof(element[4]);

            ground.push_back(s);
        }
 
        for (int j = 0; j < ground.size(); j++)
        {
            ground[j].rect.x = (ground[j].rect.x * img.cols) - (img.cols * ground[j].rect.width) / 2;
            ground[j].rect.y = (ground[j].rect.y * img.rows) - (img.rows * ground[j].rect.height) / 2;
            ground[j].rect.width = ground[j].rect.width * img.cols;
            ground[j].rect.height = ground[j].rect.height * img.rows;
            truthBox.push_back(ground[j]);
        }

        m_InputImgFilePath.pop();
        i++;
    }
    */
   json jPredictBox;
   json jGroundTruthBox;
    int count = 1;
   std::cout << m_InputImgFilePath.size() << std::endl;

   for (int i = 0; i< this->m_InputImgFilePath.size(); i++)
   {
        ground.clear();
        result.clear();
        std::cout<<"Image: " << i <<'\r' <<std::flush;
        std::string namePredict =  GetFileName(this->m_InputImgFilePath[i]);

        for (int k = 0 ; k < this->m_InputVerifyData.size(); k++){
            std::string nameGroundTruthBox =  GetFileName(this->m_InputVerifyData[k]);
            if (namePredict == nameGroundTruthBox)
            {
                std::string imgFile = "/media/tund/HDD/Data/TestData/ANPR/VN/images/pythonmap/detect/" + namePredict + ".txt";
                std::ofstream imgOutFile(imgFile.c_str());
                cv::Mat img = cv::imread(this->m_InputImgFilePath[i]);
                this->detector->executeObjectDetector(img, result, threshold);
                for (int x = 0; x < result.size() ; x++){
                //     // cv::rectangle(img, result[i].rect, cv::Scalar(0,255,0));
                //     // cv::putText(img, result[i].label, result[i].rect.tl(), CV_FONT_HERSHEY_COMPLEX, 1, cv::Scalar(255,255,255));
                    imgOutFile << result[x].label << " " << result[x].score << " "<< result[x].rect.x << " " << result[x].rect.y << " " << result[x].rect.x + result[x].rect.width << " " << result[x].rect.y + result[x].rect.height << std::endl;
                }
                imgOutFile.close();
                // std::string name = "/media/tund/HDD/Data/TestData/XamNhap/Images/out/" + std::to_string(count) + ".jpg";
                // cv::imwrite(name, img);
                count++;
                json jImage;
                jImage["name"]= namePredict;
                // std::cout << " result.size():  "<< result.size() << std::endl;
                for (int j = 0; j< result.size() ; j++)
                {
                    json jPObject;

                    jPObject["box"] = {result[j].rect.x, result[j].rect.y, result[j].rect.width, result[j].rect.height };
                    jPObject["class"] = result[j].label;
                    jImage["object"].push_back(jPObject);
                }
                jPredictBox.push_back(jImage);
                

                //process ground truth box

                json jGround;
                jGround["name"]= nameGroundTruthBox;
                std::fstream labelFilePath(this->m_InputVerifyData[k].c_str());
                if (labelFilePath.is_open())
                {
                    lines.clear();
                    std::string objTruth;
                    while (std::getline(labelFilePath, objTruth))
                    {
                        lines.push_back(objTruth);
                    }
                }
                std::string grtruthh = "/media/tund/HDD/Data/TestData/ANPR/VN/images/pythonmap/ground/" + namePredict + ".txt";
                std::ofstream groundOutFile(grtruthh.c_str());
                for(int l = 0 ; l< lines.size(); l ++)
                {
                    json jGObject;
                    element.clear();
                    element = split(lines[l], ' ');
                    for (int index = 0; index < m_Labels.size(); index ++){
                        s.label = m_Labels[stoi(element[0])] ;
                    }
                    float x = stof(element[1]);
                    float y = stof(element[2]);
                    float w = stof(element[3]);
                    float h = stof(element[4]);

                    s.rect.x = (x * img.cols) - (w * img.cols ) / 2;
                    s.rect.y = (y * img.rows) - (h * img.rows) / 2;
                    s.rect.width = w * img.cols;
                    s.rect.height = h * img.rows;
                    
                    groundOutFile << s.label << " " << s.rect.x << " " << s.rect.y << " " << s.rect.x + s.rect.width << " " <<s.rect.y + s.rect.height << std::endl;

                    jGObject["box"] = {s.rect.x, s.rect.y, s.rect.width, s.rect.height};
                    jGObject["class"] = s.label;
                    jGround["object"].push_back(jGObject);
                }
                groundOutFile.close();
                jGroundTruthBox.push_back(jGround);
         
            }
            else {
                continue;
            }
        }
   }
 
    ACCURACY result1;
    result1.calcMAP(jPredictBox,jGroundTruthBox, 0.2, m_Labels);

    //////////////////////// EndmAP /////////////////////////
    return STATUS_SUCCESS;
}

