#include "PersonCount.hpp"

PersonCount::PersonCount()
{
    this->m_detector = new ObjectDetector();
    m_edgeZone.clear();
    m_valueArea.clear();
}
PersonCount::~PersonCount()
{}

int PersonCount::Setting(int frameWidth, int frameHeight)
{
    std::vector<cv::Point2f> topFrame = {cv::Point2f(0,0),cv::Point2f(frameWidth,0),
                                         cv::Point2f(frameWidth,frameHeight * PERCENT_EDGE), cv::Point2f(0,frameHeight * PERCENT_EDGE)};
    std::vector<cv::Point2f> leftFrame = {cv::Point2f(0,0),cv::Point2f(frameWidth * PERCENT_EDGE, 0),
                                         cv::Point2f(frameWidth * PERCENT_EDGE,frameHeight), cv::Point2f(0,frameHeight)};
    std::vector<cv::Point2f> bottomFrame = {cv::Point2f(0,frameHeight * (1 - PERCENT_EDGE)),cv::Point2f(frameWidth,frameHeight * (1 - PERCENT_EDGE)),
                                         cv::Point2f(frameWidth,frameHeight), cv::Point2f(0,frameHeight)};
    std::vector<cv::Point2f> rightFrame = {cv::Point2f(frameWidth * (1 - PERCENT_EDGE),0),cv::Point2f(frameWidth,0),
                                         cv::Point2f(frameWidth,frameHeight), cv::Point2f(frameWidth * (1 - PERCENT_EDGE),frameHeight)};
    m_edgeZone.push_back(topFrame);
    m_edgeZone.push_back(leftFrame); 
    m_edgeZone.push_back(bottomFrame);
    m_edgeZone.push_back(rightFrame);                                                                                                                   
    return 0;
}

int PersonCount::InitAI()
{
    if (m_detector->initObjectDetector("ncnn", "PersonCount") != STATUS_SUCCESS)
    {
        return -1;
    }
    return 0;
}

int PersonCount::Execute(cv::Mat& frame, std::vector<PersonCountOutput>& outputs) 
{
    try
    {
        cv::putText(frame, std::to_string(getStandardArea()), cv::Point(350, 50), FONT_HERSHEY_COMPLEX, 3, cv::Scalar(0,0,255));
        std::vector<ObjectTrace> detected;
        if(this->m_detector->executeObjectDetector(frame, detected, 0.5) != STATUS_SUCCESS)
        {
            LOG_FAIL("Execute Anpr detector failed");
            return STATUS_FAILED;
        }
        float fTempAreaAllObject = 0;
        float fAvarageArea = 0;
        int correct = 0;
        if (m_valueArea.size() == 0){
            for (int i = 0; i < detected.size() ; i++)
            {
                if (isOnEdge(detected[i], this->m_edgeZone) == false){
                    PersonCountOutput object;
                    object.rect = detected[i].rect;
                    object.score = detected[i].score;
                    object.type = "standard"; 
                    float area = detected[i].rect.width * detected[i].rect.height;
                    object.area = area;
                    fTempAreaAllObject += area;
                    correct ++;
                    outputs.push_back(object);
                }
            }
            if (fTempAreaAllObject != 0){
                fAvarageArea = fTempAreaAllObject / correct;
                m_valueArea.push_back(fAvarageArea);
            }
        }
            
        else{
            for (int i = 0; i < detected.size() ; i++)
            {
                if (isOnEdge(detected[i], this->m_edgeZone) == false){
                    float area = detected[i].rect.width * detected[i].rect.height;
                   
                    if (area >= getStandardArea() * PERCENT_AREA)
                    {
                        PersonCountOutput object;
                        object.rect = detected[i].rect;
                        object.score = detected[i].score;
                        object.type = "standard";
                        object.area = area;
                        outputs.push_back(object);
                        fTempAreaAllObject += area;
                        std::cout << "correct" << std::endl;
                        correct ++;
                    }else{
                        PersonCountOutput object;
                        object.rect = detected[i].rect;
                        object.score = detected[i].score;
                        object.area = area;
                        object.type = "substandard";
                        outputs.push_back(object);
                    }
                }
            }
            if (fTempAreaAllObject != 0){
                fAvarageArea = fTempAreaAllObject / correct;
                if (m_valueArea.size() == 20){
                    m_valueArea.erase(m_valueArea.begin());
                    m_valueArea.push_back(fAvarageArea);      
                } else if (m_valueArea.size() < 20){
                    m_valueArea.push_back(fAvarageArea);  
                }
        
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return STATUS_SUCCESS;
}

bool PersonCount::isOnEdge(ObjectTrace object, std::vector<std::vector<cv::Point2f>> edgeZone)
{
    for (int i = 0; i < edgeZone.size(); i++)
    {
        if (isInside(object,edgeZone[i]))
            return true;
    }
    return false;
}

bool PersonCount::isInside(ObjectTrace object, std::vector<cv::Point2f> area)
{
    if (area.size() == 0) return false;
    cv::Point2f topLeft = object.rect.tl();
    cv::Point2f bottomRight = object.rect.br();
    if ((pointPolygonTest(area, topLeft, false) >= 0) || (pointPolygonTest(area, bottomRight, false) >= 0)){
        return true;
    }
    return false;
}

float PersonCount::getStandardArea()
{
    float countArea = 0;
    std::cout << "so luong: "<<m_valueArea.size() <<std::endl;
    for (int i = 0; i < this->m_valueArea.size(); i++)
    {
        countArea += m_valueArea[i];
    }
    std::cout << "ket qua: " << (countArea / m_valueArea.size()) <<std::endl;
    return (countArea / m_valueArea.size());
}