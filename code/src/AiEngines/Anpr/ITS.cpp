#include "ITS.hpp"

ITS::ITS()
{
    this->m_PlateDetect = new AnprDetector();
    this->ocr = OCR();
    this->m_VehicleDetector = new ObjectDetector();
    this->m_PlateTracker = new ObjectTracking();
    this->m_VehicleTracker = new ObjectTracking();
    this->m_listVehicles.clear();
    this->m_listPlateTracks.clear();
    this->m_listOutputs.clear();
}
ITS::~ITS()
{

}

int ITS::init(Nations nation)
{
    this->m_PlateDetect->init(nation);
    this->m_VehicleDetector->initObjectDetector("snpe", "traffic", "CPU");

    return STATUS_SUCCESS;
}
/*
int ITS::Execute(cv::Mat& mFHDImg, cv::Mat mUHDImg, std::vector<ITS_Output>& outputs)
{
    int iCols = mFHDImg.cols;
    int iRows = mFHDImg.rows;
    int iCols4K = mUHDImg.cols;
    int iRows4K = mUHDImg.rows;

    //detect plate
    std::vector<ObjectTrace> plates;
    this->m_PlateDetect->detect(mFHDImg, plates);

    //tracking plate
    std::vector<TrackingTrace> stTrackPlates;
    this->m_PlateTracker->process_anpr_VN(plates, stTrackPlates, 3.0, 10.0, 5.0, 7.0);
    // this->m_PlateTracker->process(plates, stTrackPlates);

    //detect vehicle
    std::vector<ObjectTrace> vehicles;
    this->m_VehicleDetector->executeObjectDetector(mFHDImg, vehicles, 0.3);
    
    //tracking vehicle
    std::vector<TrackingTrace> stTrackVehicles;
    m_VehicleTracker->process(vehicles, stTrackVehicles);

    // delete vehicle
    for (auto it = this->m_listVehicles.begin(); it != this->m_listVehicles.end(); )
    {
        const int iID = (*it).iTrackID;
        const auto p = find_if(stTrackVehicles.begin(), stTrackVehicles.end(), 
                                [iID] (const TrackingTrace& a) { return (a.m_ID == iID); });
        if (p == stTrackVehicles.end() && it != this->m_listVehicles.end())
        {          
            it = this->m_listVehicles.erase(it);
        }
        else
        {
            it ++;
        }
    }

    // update vehicle
    for (auto stTrackVehicle : stTrackVehicles)
    {
        if (stTrackVehicle.isOutOfFrame == false)
        {
            const int iID = stTrackVehicle.m_ID;
            const auto p = find_if(this->m_listVehicles.begin(), this->m_listVehicles.end(),
                                    [iID] (const VehicleInfor& a) { return (a.iTrackID == iID); });
            if (p != this->m_listVehicles.end())
            {
                int iIndex = distance(this->m_listVehicles.begin(), p);
                this->m_listVehicles[iIndex].rectVehicle = stTrackVehicle.m_rect;
                this->m_listVehicles[iIndex].bOutOfFrame = false;
                if ((this->m_listVehicles[iIndex].isMatch == 0) && (this->m_listVehicles[iIndex].countMatch <= MAX_COUNT_MATCH)) {
                    this->m_listVehicles[iIndex].countMatch += 1;
                }else if (this->m_listVehicles[iIndex].countMatch == MAX_COUNT_MATCH +1){ //cho vao list output
                    this->m_listVehicles[iIndex].countMatch += 1;
                    ITS_Output out;
                    out.vehicle = this->m_listVehicles[iIndex];
                    out.imgEvent = mFHDImg;
                    out.isEvent = true;
                    out.typeOutput = TypeOutput::OnlyVehicle;
                    cout << "new its vehicle" <<endl;
                    this->m_listOutputs.push_back(out);
                }
                
            }
            else
            {
                VehicleInfor stVehicle;
                stVehicle.iTrackID = stTrackVehicle.m_ID;
                stVehicle.rectVehicle = stTrackVehicle.m_rect;
                stVehicle.typeVehicle = stTrackVehicle.m_type;
                this->m_listVehicles.push_back(stVehicle);
            }
        }
        else
        {
            const int iID = stTrackVehicle.m_ID;
            const auto p = find_if(this->m_listVehicles.begin(), this->m_listVehicles.end(),
                                    [iID] (const VehicleInfor& a) { return (a.iTrackID == iID); });
            if (p != this->m_listVehicles.end())
            {
                int iIndex = distance(this->m_listVehicles.begin(), p);
                this->m_listVehicles[iIndex].bOutOfFrame = true;
            }
        }
    }
    std::cout << "List Vehicle: " << m_listVehicles.size() << std::endl;
    
     // delete plate
    for (auto it = this->m_listPlateTracks.begin(); it != this->m_listPlateTracks.end(); )
    {
        const int iID = (*it).track_id;
        const auto p = find_if(stTrackPlates.begin(), stTrackPlates.end(), 
                                [iID] (const TrackingTrace& a) { return (a.m_ID == iID); });
        if (p == stTrackPlates.end() && it != this->m_listPlateTracks.end())
        {          
            it = this->m_listPlateTracks.erase(it);
        }
        else
        {
            it ++;
        }
    }
   
    
     // update plate
    for (auto stTrackPlate : stTrackPlates)
    {

        if (stTrackPlate.isOutOfFrame == false)
        {
            const int iID = stTrackPlate.m_ID;
            const auto p = find_if(this->m_listPlateTracks.begin(), this->m_listPlateTracks.end(), 
                                    [iID] (const PlateInfor& a) { return (a.track_id == iID); });
            if (p != this->m_listPlateTracks.end())
            {
                int iIndex = distance(this->m_listPlateTracks.begin(), p);
                this->m_listPlateTracks[iIndex].rect = stTrackPlate.m_rect;
                this->m_listPlateTracks[iIndex].bOutOfFrame = false;
                // if ((this->m_listPlateTracks[iIndex].license == "Unknown" || this->m_listPlateTracks[iIndex].isDefectPlate)
                //     && this->m_listPlateTracks[iIndex].countUnknown <= MAX_COUNT_UNKNOWN) //bien so loi ma it lan thi cho doc lai
                // {
                if ((this->m_listPlateTracks[iIndex].license == "Unknown" )
                    && this->m_listPlateTracks[iIndex].countUnknown <= MAX_COUNT_UNKNOWN) //bien so loi ma it lan thi cho doc lai
                {
                    cv::Rect2f rfBox4K = getRect4K(stTrackPlate.m_rect, iCols4K, iRows4K);
                    cv::Mat mCropPlate4K = mUHDImg(rfBox4K).clone();
                    cv::Mat mCropPlate = mFHDImg(stTrackPlate.m_rect).clone();
                    float fConfidence = 0;
         
                    // cout << "size mat: " << sizeInBytes << endl;
                    if (checkPlate(this->m_listPlateTracks[iIndex], iCols, iRows))
                    {
                        this->m_listPlateTracks[iIndex].isNewEvent = true;
                        if (stTrackPlate.m_type == "VnRect")
                        {
                            this->ocr.RunOCR(mCropPlate4K, this->m_listPlateTracks[iIndex].license);
                        }
                        else if (stTrackPlate.m_type == "VnSquare")
                        {
                            this->ocr.RunOCR(mCropPlate4K, this->m_listPlateTracks[iIndex].license);
                        }
                        else if (stTrackPlate.m_type == "US")
                        {
                            this->ocr.RunOCR(mCropPlate4K, this->m_listPlateTracks[iIndex].license);
                        }
                        else if (stTrackPlate.m_type == "Malay")
                        {
                            this->ocr.RunOCR(mCropPlate4K, this->m_listPlateTracks[iIndex].license);
                        }

                        if (this->m_listPlateTracks[iIndex].license == "Unknown")
                        {
                            this->m_listPlateTracks[iIndex].countUnknown ++;
                            this->m_listPlateTracks[iIndex].isNewEvent = false;
                        }
                        else if (this->m_listPlateTracks[iIndex].license.size() < 7 || this->m_listPlateTracks[iIndex].license.size() > 10)
                        {
                            this->m_listPlateTracks[iIndex].license = "Unknown";
                            this->m_listPlateTracks[iIndex].countUnknown ++;
                            this->m_listPlateTracks[iIndex].isNewEvent = false;
                        }                       
                    }
                    else
                    {
                        this->m_listPlateTracks[iIndex].license = "Unknown";
                        this->m_listPlateTracks[iIndex].countUnknown ++;
                    }
                }
                else if (this->m_listPlateTracks[iIndex].countUnknown == (MAX_COUNT_UNKNOWN + 1))
                {
                    this->m_listPlateTracks[iIndex].countUnknown ++;
                    // this->m_listPlateTracks[iIndex].isDefectPlate = false;
                    this->m_listPlateTracks[iIndex].isNewEvent = true;
                    if (this->m_listPlateTracks[iIndex].isMatch == 0){ //cho nhung bien so khong duoc match ma doc qua len sk
                        ITS_Output out;
                        out.plate = this->m_listPlateTracks[iIndex];
                        out.isEvent = true;
                        out.imgEvent = mFHDImg;
                        out.typeOutput = TypeOutput::OnlyPlate;
                        cout << "new its only plate" <<endl;
                        this->m_listOutputs.push_back(out);
                    }
                }
                else
                {                  
                    this->m_listPlateTracks[iIndex].isNewEvent = false;
                }              
            }
            else //doi tuong moi
            {
                cv::Rect2f rfBox4K = getRect4K(stTrackPlate.m_rect, iCols4K, iRows4K);
                cv::Mat mCropPlate4K = mUHDImg(rfBox4K).clone();
                cv::Mat mCropPlate = mFHDImg(stTrackPlate.m_rect).clone();
                float fConfidence = 0;
                PlateInfor stPlate;
                stPlate.track_id = stTrackPlate.m_ID;
                stPlate.imgPlate = mCropPlate;
                stPlate.rect = stTrackPlate.m_rect;
                stPlate.typePlate = stTrackPlate.m_type;
                stPlate.bOutOfFrame = false;
                stPlate.isNewEvent = true; //cu bat true len da, khong dung thi tat sau
                if (checkPlate(stPlate, iCols, iRows))
                {
                    // stPlate.isNewEvent = true;
                    if (stTrackPlate.m_type == "VnRect")
                    {
                        this->ocr.RunOCR(mCropPlate4K, stPlate.license);
                    }
                    else if (stTrackPlate.m_type == "VnSquare")
                    {
                        this->ocr.RunOCR(mCropPlate4K, stPlate.license);
                    }
                    else if (stTrackPlate.m_type == "US")
                    {
                        this->ocr.RunOCR(mCropPlate4K, stPlate.license);
                    }
                    else if (stTrackPlate.m_type == "Malay")
                    {
                        this->ocr.RunOCR(mCropPlate4K, stPlate.license);
                    }

                    // if (isnan(fConfidence) == false)
                    // {
                    //     stPlate.score = fConfidence;
                    // }
                    // else
                    // {
                    //     stPlate.score = 0;
                    // }

                    if (stPlate.license.size() < 7 || stPlate.license.size() > 10) //ki tu khong dung dinh dang
                    {
                        // stPlate.isDefectPlate = true;
                        stPlate.license = "Unknown";
                    }
                }
                else //bien khong dung kich thuoc
                {
                    stPlate.license = "Unknown";
                    stPlate.score = 0;
                }

                // if (stPlate.license == "Unknown" || stPlate.isDefectPlate == true){//thang nao ko doc duoc thi cho ve false
                if (stPlate.license == "Unknown"){
                    stPlate.isNewEvent == false;
                    stPlate.countUnknown += 1;
                }

                this->m_listPlateTracks.push_back(stPlate);
            }
        }
        else
        {
            const int iID = stTrackPlate.m_ID;
            const auto p = find_if(this->m_listPlateTracks.begin(), this->m_listPlateTracks.end(), 
                                    [iID] (const PlateInfor& a) { return (a.track_id == iID); });
            if (p != this->m_listPlateTracks.end())
            {
                int iIndex = distance(this->m_listPlateTracks.begin(), p);
                this->m_listPlateTracks[iIndex].bOutOfFrame = true;
            }
        }
    }
    std::cout << "List Plate: " << this->m_listPlateTracks.size() << std::endl;


    
    for (auto& a : m_listVehicles)
    {
        if (!a.bOutOfFrame)
        cv::rectangle(mFHDImg, a.rectVehicle, cv::Scalar(0,255,0));
        cv::putText(mFHDImg, std::to_string(a.matchID), a.rectVehicle.tl(), FONT_HERSHEY_COMPLEX, 0.8, cv::Scalar(0,0,255), 2);

    }
    for (auto& a : this->m_listPlateTracks)
    {
        if (!a.bOutOfFrame){
            cv::rectangle(mFHDImg, a.rect, cv::Scalar(0,255,0));
            cv::putText(mFHDImg, a.license, a.rect.tl(), FONT_HERSHEY_COMPLEX, 0.8, cv::Scalar(0,0,255), 2);
        }
        
    }

    
    // for (int i = 0 ;i < m_listPlateTracks.size(); i++){
        
    //     if (m_listPlateTracks[i].isMatch == true){
    //         std::cout << "true " <<endl;
    //         cout << m_listPlateTracks[i].isMatch <<endl;
    //     }else if (m_listPlateTracks[i].isMatch == false){
    //         cout << "false" << endl;
    //         cout << m_listPlateTracks[i].isMatch <<endl;
    //     }else {
    //         cout << "chiu" << endl;
    //         cout << m_listPlateTracks[i].isMatch <<endl;
    //     }
    //     // std::cout << "Status plate: " << m_listPlateTracks[i].isMatch <<endl;
    // }
    
    //match vehicle voi plate
    for (int i = 0; i < m_listVehicles.size()  ; i ++)
    {   
        cout << "Xe thu: " << m_listVehicles[i].iTrackID << ", " << m_listVehicles[i].isMatch << endl;
        if (m_listVehicles[i].isMatch == 0){
            std::vector<cv::Point2f> ptAreas{cv::Point2f(m_listVehicles[i].rectVehicle.x, (float)m_listVehicles[i].rectVehicle.y + m_listVehicles[i].rectVehicle.height / 2), 
                                            cv::Point2f(m_listVehicles[i].rectVehicle.x + m_listVehicles[i].rectVehicle.width, m_listVehicles[i].rectVehicle.y + m_listVehicles[i].rectVehicle.height / 2),
                                            cv::Point2f(m_listVehicles[i].rectVehicle.br()), 
                                            cv::Point2f(m_listVehicles[i].rectVehicle.x, m_listVehicles[i].rectVehicle.height)}; //vung nua duoi cua phuong tien
            for (int j = 0; j < this->m_listPlateTracks.size(); j++){ 
                cout << "out if: " << this->m_listPlateTracks[j].track_id << ", " << this->m_listPlateTracks[j].isMatch << ", ";         
                if (this->m_listPlateTracks[j].isMatch == 0){
                    cout << "in if: " << this->m_listPlateTracks[j].track_id << ", " << this->m_listPlateTracks[j].isMatch << ", ";
                    cv::Point2f ptCenterBox = cv::Point2f(this->m_listPlateTracks[j].rect.x + (float)this->m_listPlateTracks[j].rect.width / 2, this->m_listPlateTracks[j].rect.y + (float)this->m_listPlateTracks[j].rect.height / 2); // diem giua cua bien so
                    if (pointPolygonTest(ptAreas, ptCenterBox, false)){
                        m_listVehicles[i].isMatch = 1;
                        this->m_listPlateTracks[j].isMatch = 1;
                        int ID = rand() % (10000 - 1 + 1) + 1; //random trong khoang 1 - 10000
                        m_listVehicles[i].matchID = ID;
                        this->m_listPlateTracks[j].matchID = ID;

                        ITS_Output out;
                        mFHDImg.copyTo(out.imgEvent);
                        out.plate = this->m_listPlateTracks[j];
                        out.vehicle = m_listVehicles[i];
                        out.matchID = ID;
                        out.typeOutput = TypeOutput::Both;
                        if (out.plate.isNewEvent == true){
                            out.isEvent = true;
                            cout << "new matched event" <<endl;
                        }else {
                            out.isEvent = false;
                            cout << "new matched not event" <<endl;
                        }                      
                        this->m_listOutputs.push_back(out);
                        // break;
                    }
                }    
            }
        }    
    }
    std::cout << "List its: " << m_listOutputs.size() << std::endl;

    //Cap nhat bien so cho nhung cap da match
    for (auto& its : this->m_listOutputs)
    {
        if (its.isEvent == false){
            int ID = its.matchID;
            const auto p = find_if(this->m_listPlateTracks.begin(), this->m_listPlateTracks.end(), 
                                        [ID] (const PlateInfor& a) { return (a.matchID == ID); });
            if (p != this->m_listPlateTracks.end())
            {
                int iIndex = distance(this->m_listPlateTracks.begin(), p);
                its.plate.license = this->m_listPlateTracks[iIndex].license;
                its.plate.imgPlate = this->m_listPlateTracks[iIndex].imgPlate;
                its.isEvent = true;
            }
            
        }
    }
    //push output
    for(auto it = this->m_listOutputs.begin(); it != this->m_listOutputs.end();)
    {
        if ((*it).isEvent == true){
            outputs.push_back((*it));
            it = m_listOutputs.erase(it);
        }
        else 
            it++;
    }
    
     for (auto& a : m_listVehicles)
    {
        if (!a.bOutOfFrame)
        // cv::rectangle(mFHDImg, a.rectVehicle, cv::Scalar(0,255,0));
        cv::putText(mFHDImg, std::to_string(a.matchID), cv::Point2d(a.rectVehicle.x, a.rectVehicle.y + 30), FONT_HERSHEY_COMPLEX, 0.8, cv::Scalar(0,0,255), 2);
        cv::putText(mFHDImg, std::to_string(a.isMatch), cv::Point2d(a.rectVehicle.x + a.rectVehicle.width - 30, a.rectVehicle.y ), FONT_HERSHEY_COMPLEX, 0.8, cv::Scalar(0,0,255), 2);

    }
    for (auto& a : this->m_listPlateTracks)
    {
        if (!a.bOutOfFrame){
            // cv::rectangle(mFHDImg, a.rect, cv::Scalar(0,255,0));
            cv::putText(mFHDImg, std::to_string(a.matchID), cv::Point2d(a.rect.x, a.rect.y + 30), FONT_HERSHEY_COMPLEX, 0.8, cv::Scalar(0,0,255), 2);
            cv::putText(mFHDImg, std::to_string(a.isMatch), cv::Point2d(a.rect.x, a.rect.y + 60), FONT_HERSHEY_COMPLEX, 0.8, cv::Scalar(0,0,255), 2);
        }
        
    }
    

  
    return STATUS_SUCCESS;
}
*/


int ITS::Execute2(cv::Mat& mFHDImg, cv::Mat mUHDImg, std::vector<ITS_Output>& outputs)
{
    int iCols = mFHDImg.cols;
    int iRows = mFHDImg.rows;
    int iCols4K = mUHDImg.cols;
    int iRows4K = mUHDImg.rows;

    //detect plate
    std::vector<ObjectTrace> plates;
    this->m_PlateDetect->detect(mFHDImg, plates);

    //tracking plate
    std::vector<TrackingTrace> stTrackPlates;
    this->m_PlateTracker->process_anpr_VN(plates, stTrackPlates, 3.0, 10.0, 5.0, 7.0);
    // this->m_PlateTracker->process(plates, stTrackPlates);

    //detect vehicle
    std::vector<ObjectTrace> vehicles;
    this->m_VehicleDetector->executeObjectDetector(mFHDImg, vehicles, 0.3);
    
    //tracking vehicle
    std::vector<TrackingTrace> stTrackVehicles;
    m_VehicleTracker->process(vehicles, stTrackVehicles);

    // delete vehicle
    for (auto it = this->m_listVehicles.begin(); it != this->m_listVehicles.end(); )
    {
        const int iID = (*it).iTrackID;
        const auto p = find_if(stTrackVehicles.begin(), stTrackVehicles.end(), 
                                [iID] (const TrackingTrace& a) { return (a.m_ID == iID); });
        if (p == stTrackVehicles.end() && it != this->m_listVehicles.end())
        {          
            it = this->m_listVehicles.erase(it);
        }
        else
        {
            it ++;
        }
    }

    // update vehicle
    for (auto stTrackVehicle : stTrackVehicles)
    {
        if (stTrackVehicle.isOutOfFrame == false)
        {
            const int iID = stTrackVehicle.m_ID;
            const auto p = find_if(this->m_listVehicles.begin(), this->m_listVehicles.end(),
                                    [iID] (const VehicleInfor& a) { return (a.iTrackID == iID); });
            if (p != this->m_listVehicles.end())
            {
                int iIndex = distance(this->m_listVehicles.begin(), p);
                this->m_listVehicles[iIndex].rectVehicle = stTrackVehicle.m_rect;
                this->m_listVehicles[iIndex].bOutOfFrame = false;
                if ((this->m_listVehicles[iIndex].isMatch == false) && (this->m_listVehicles[iIndex].countMatch <= MAX_COUNT_MATCH)) {
                    this->m_listVehicles[iIndex].countMatch += 1;
                }else if (this->m_listVehicles[iIndex].countMatch == MAX_COUNT_MATCH +1){ //cho vao list output
                    this->m_listVehicles[iIndex].countMatch += 1;
                    ITS_Output out;
                    out.vehicle = this->m_listVehicles[iIndex];
                    out.typeOutput = TypeOutput::OnlyVehicle;
                    cout << "new its vehicle" <<endl;
                    outputs.push_back(out);
                }
                
            }
            else
            {
                VehicleInfor stVehicle;
                stVehicle.iTrackID = stTrackVehicle.m_ID;
                stVehicle.rectVehicle = stTrackVehicle.m_rect;
                stVehicle.typeVehicle = stTrackVehicle.m_type;
                this->m_listVehicles.push_back(stVehicle);
            }
        }
        else
        {
            const int iID = stTrackVehicle.m_ID;
            const auto p = find_if(this->m_listVehicles.begin(), this->m_listVehicles.end(),
                                    [iID] (const VehicleInfor& a) { return (a.iTrackID == iID); });
            if (p != this->m_listVehicles.end())
            {
                int iIndex = distance(this->m_listVehicles.begin(), p);
                this->m_listVehicles[iIndex].bOutOfFrame = true;
            }
        }
    }

    // for (auto stTrackVehicle : stTrackVehicles)
    // {
    //     if (!stTrackVehicle.isOutOfFrame){
    //         rectangle(mFHDImg, stTrackVehicle.m_rect, Scalar(255,255,255));
    //         putText(mFHDImg, to_string(stTrackVehicle.m_ID), Point(stTrackVehicle.m_rect.x, stTrackVehicle.m_rect.y + 30), FONT_HERSHEY_COMPLEX, 0.8, Scalar(0,255,0), 2);
    //     }
    // }
    
    for (auto stTrackVehicle : m_listVehicles)
    {
        if (!stTrackVehicle.bOutOfFrame){
            rectangle(mFHDImg, stTrackVehicle.rectVehicle, Scalar(255,255,255));
            putText(mFHDImg, to_string(stTrackVehicle.iTrackID), Point(stTrackVehicle.rectVehicle.x, stTrackVehicle.rectVehicle.y + 30), FONT_HERSHEY_COMPLEX, 0.8, Scalar(0,255,0), 2);
            std::vector<cv::Point2f> ptAreas{cv::Point2f(stTrackVehicle.rectVehicle.x, (float)stTrackVehicle.rectVehicle.y + stTrackVehicle.rectVehicle.height / 2), 
                                            cv::Point2f(stTrackVehicle.rectVehicle.x + stTrackVehicle.rectVehicle.width, stTrackVehicle.rectVehicle.y + stTrackVehicle.rectVehicle.height / 2),
                                            cv::Point2f(stTrackVehicle.rectVehicle.br()), 
                                            cv::Point2f(stTrackVehicle.rectVehicle.x, stTrackVehicle.rectVehicle.y + stTrackVehicle.rectVehicle.height),
                                            cv::Point2f(stTrackVehicle.rectVehicle.x, (float)stTrackVehicle.rectVehicle.y + stTrackVehicle.rectVehicle.height / 2)
                                            };
            // for (int j = 0 ; j < ptAreas.size() - 1; j++){
            //     line(mFHDImg, ptAreas[j], ptAreas[j+1], Scalar(235,124,63),2);
            // }
            
           
        }
    }
    
    // delete plate
    for (auto it = this->m_listPlateTracks.begin(); it != this->m_listPlateTracks.end(); )
    {
        const int iID = (*it).track_id;
        const auto p = find_if(stTrackPlates.begin(), stTrackPlates.end(), 
                                [iID] (const TrackingTrace& a) { return (a.m_ID == iID); });
        if (p == stTrackPlates.end() && it != this->m_listPlateTracks.end())
        {          
            it = this->m_listPlateTracks.erase(it);
        }
        else
        {
            it ++;
        }
    }
   
    
     // update plate
    for (auto stTrackPlate : stTrackPlates)
    {

        if (stTrackPlate.isOutOfFrame == false)
        {
            const int iID = stTrackPlate.m_ID;
            const auto p = find_if(this->m_listPlateTracks.begin(), this->m_listPlateTracks.end(), 
                                    [iID] (const PlateInfor& a) { return (a.track_id == iID); });
            if (p != this->m_listPlateTracks.end())
            {
                int iIndex = distance(this->m_listPlateTracks.begin(), p);
                this->m_listPlateTracks[iIndex].rect = stTrackPlate.m_rect;
                this->m_listPlateTracks[iIndex].bOutOfFrame = false;
                // if ((this->m_listPlateTracks[iIndex].license == "Unknown" || this->m_listPlateTracks[iIndex].isDefectPlate)
                //     && this->m_listPlateTracks[iIndex].countUnknown <= MAX_COUNT_UNKNOWN) //bien so loi ma it lan thi cho doc lai
                // {
                if (!(this->m_listPlateTracks[iIndex].bRecognized )
                    && this->m_listPlateTracks[iIndex].countUnknown < MAX_COUNT_UNKNOWN) //bien so loi ma it lan thi cho doc lai
                {
                    cv::Rect2f rfBox4K = getRect4K(stTrackPlate.m_rect, iCols4K, iRows4K);
                    cv::Mat mCropPlate4K = mUHDImg(rfBox4K).clone();
                    cv::Mat mCropPlate = mFHDImg(stTrackPlate.m_rect).clone();
                    float fConfidence = 0;
         
                    // cout << "size mat: " << sizeInBytes << endl;
                    if (checkPlate(this->m_listPlateTracks[iIndex], iCols, iRows))
                    {
                        this->m_listPlateTracks[iIndex].bRecognized = true;
                        if (stTrackPlate.m_type == "VnRect")
                        {
                            this->ocr.RunOCR(mCropPlate4K, this->m_listPlateTracks[iIndex].license);
                        }
                        else if (stTrackPlate.m_type == "VnSquare")
                        {
                            this->ocr.RunOCR(mCropPlate4K, this->m_listPlateTracks[iIndex].license);
                        }
                        else if (stTrackPlate.m_type == "US")
                        {
                            this->ocr.RunOCR(mCropPlate4K, this->m_listPlateTracks[iIndex].license);
                        }
                        else if (stTrackPlate.m_type == "Malay")
                        {
                            this->ocr.RunOCR(mCropPlate4K, this->m_listPlateTracks[iIndex].license);
                        }
                        if (this->m_listPlateTracks[iIndex].license.size() < 7 || this->m_listPlateTracks[iIndex].license.size() > 10)
                        {
                            this->m_listPlateTracks[iIndex].license = "Unknown";
                        }      

                    }
                    else {
                        this->m_listPlateTracks[iIndex].license = "Unknown";
                    }

                    if (this->m_listPlateTracks[iIndex].license == "Unknown"){
                        this->m_listPlateTracks[iIndex].countUnknown ++;
                        this->m_listPlateTracks[iIndex].bRecognized = false;
                    }   
                    if (this->m_listPlateTracks[iIndex].bRecognized == true)
                    {
                        cv::Point2f ptCenterBox = cv::Point2f(this->m_listPlateTracks[iIndex].rect.x + (float)this->m_listPlateTracks[iIndex].rect.width / 2, this->m_listPlateTracks[iIndex].rect.y + (float)this->m_listPlateTracks[iIndex].rect.height / 2);
                        circle(mFHDImg, ptCenterBox, 2, Scalar(0,0,255),2);
                        if (!MatchingVehicle(this->m_listPlateTracks[iIndex], this->m_listVehicles, outputs)){
                            this->m_listPlateTracks[iIndex].countUnknown += 1;
                        }else {
                             cout << "Matching 4" << endl;
                        }
                    }
                }
                else if (this->m_listPlateTracks[iIndex].countUnknown == MAX_COUNT_UNKNOWN )
                {
                    this->m_listPlateTracks[iIndex].countUnknown ++;
                    cv::Point2f ptCenterBox = cv::Point2f(this->m_listPlateTracks[iIndex].rect.x + (float)this->m_listPlateTracks[iIndex].rect.width / 2, this->m_listPlateTracks[iIndex].rect.y + (float)this->m_listPlateTracks[iIndex].rect.height / 2);
                    circle(mFHDImg, ptCenterBox, 2, Scalar(0,0,255),2);
                    if (!MatchingVehicle(this->m_listPlateTracks[iIndex], this->m_listVehicles, outputs)){
                        ITS_Output out;
                        out.typeOutput = TypeOutput::OnlyPlate;
                        out.plate = this->m_listPlateTracks[iIndex];
                        cout << "only plate" << endl;
                        outputs.push_back(out);
                    }else {
                         cout << "Matching 3" << endl;
                    }
                }
                else if ((this->m_listPlateTracks[iIndex].countUnknown < MAX_COUNT_UNKNOWN) && (this->m_listPlateTracks[iIndex].bRecognized))
                {    
                    cv::Point2f ptCenterBox = cv::Point2f(this->m_listPlateTracks[iIndex].rect.x + (float)this->m_listPlateTracks[iIndex].rect.width / 2, this->m_listPlateTracks[iIndex].rect.y + (float)this->m_listPlateTracks[iIndex].rect.height / 2);
                    circle(mFHDImg, ptCenterBox, 2, Scalar(0,0,255),2);              
                    if (!MatchingVehicle(this->m_listPlateTracks[iIndex], this->m_listVehicles, outputs)){
                        this->m_listPlateTracks[iIndex].countUnknown += 1;
                    }else{
                        this->m_listPlateTracks[iIndex].countUnknown = MAX_COUNT_UNKNOWN + 2;
                        cout << "Matching 2" << endl;
                    }
                }
            }
            else //doi tuong bien so moi
            {
                cv::Rect2f rfBox4K = getRect4K(stTrackPlate.m_rect, iCols4K, iRows4K);
                cv::Mat mCropPlate4K = mUHDImg(rfBox4K).clone();
                cv::Mat mCropPlate = mFHDImg(stTrackPlate.m_rect).clone();
                float fConfidence = 0;
                PlateInfor stPlate;
                stPlate.track_id = stTrackPlate.m_ID;
                stPlate.imgPlate = mCropPlate;
                stPlate.rect = stTrackPlate.m_rect;
                stPlate.typePlate = stTrackPlate.m_type;
                stPlate.bOutOfFrame = false;
                
                if (checkPlate(stPlate, iCols, iRows))
                {
                    stPlate.bRecognized = true; //cu bat true len da, khong dung thi tat sau
                    if (stTrackPlate.m_type == "VnRect")
                    {
                        this->ocr.RunOCR(mCropPlate4K, stPlate.license);
                    }
                    else if (stTrackPlate.m_type == "VnSquare")
                    {
                        this->ocr.RunOCR(mCropPlate4K, stPlate.license);
                    }
                    else if (stTrackPlate.m_type == "US")
                    {
                        this->ocr.RunOCR(mCropPlate4K, stPlate.license);
                    }
                    else if (stTrackPlate.m_type == "Malay")
                    {
                        this->ocr.RunOCR(mCropPlate4K, stPlate.license);
                    }

                    // if (isnan(fConfidence) == false)
                    // {
                    //     stPlate.score = fConfidence;
                    // }
                    // else
                    // {
                    //     stPlate.score = 0;
                    // }

                    if (stPlate.license.size() < 7 || stPlate.license.size() > 10) //ki tu khong dung dinh dang
                    {
                        stPlate.license = "Unknown";
                    }
                }
                else //bien khong dung kich thuoc
                {
                    stPlate.license = "Unknown";
                    stPlate.score = 0;
                }

                // if (stPlate.license == "Unknown" || stPlate.isDefectPlate == true){//thang nao ko doc duoc thi cho ve false
                if (stPlate.license == "Unknown"){
                    stPlate.bRecognized = false;
                    stPlate.countUnknown += 1;
                }
                if (stPlate.bRecognized == true)
                {
                    cv::Point2f ptCenterBox = cv::Point2f(stPlate.rect.x + (float)stPlate.rect.width / 2, stPlate.rect.y + (float)stPlate.rect.height / 2);
                    circle(mFHDImg, ptCenterBox, 2, Scalar(0,0,255),2);
                    if (!MatchingVehicle(stPlate, this->m_listVehicles, outputs)){
                        stPlate.countUnknown += 1;
                    }else {
                         cout << "Matching 1" << endl;
                         stPlate.countUnknown = MAX_COUNT_UNKNOWN +2;
                    }
                }
                this->m_listPlateTracks.push_back(stPlate);
            }
        }
        else
        {
            const int iID = stTrackPlate.m_ID;
            const auto p = find_if(this->m_listPlateTracks.begin(), this->m_listPlateTracks.end(), 
                                    [iID] (const PlateInfor& a) { return (a.track_id == iID); });
            if (p != this->m_listPlateTracks.end())
            {
                int iIndex = distance(this->m_listPlateTracks.begin(), p);
                this->m_listPlateTracks[iIndex].bOutOfFrame = true;
            }
        }
    }
    for (auto& plate : this->m_listPlateTracks)
    {
        if (!plate.bOutOfFrame){
            cout << static_cast<int> (plate.countUnknown) << endl;
            rectangle(mFHDImg, plate.rect, Scalar(255,255,255));
            putText(mFHDImg, to_string(plate.track_id), Point(plate.rect.x, plate.rect.y + 30), FONT_HERSHEY_COMPLEX, 0.8, Scalar(0,255,0), 2);
        }
    }
    return 0;
}



bool ITS::MatchingVehicle(PlateInfor& stPlate, std::vector<VehicleInfor>& stVehicleTracks, std::vector<ITS_Output>& outputs)
{
    cv::Point2f ptCenterBox = cv::Point2f(stPlate.rect.x + (float)stPlate.rect.width / 2, stPlate.rect.y + (float)stPlate.rect.height / 2);
    double dMaxDistance = 0;
    cv::Rect2i riBox;
    std::string strType;
    for (auto& stVehicle : stVehicleTracks)
    {
        cout << "Plate: " << stPlate.track_id << endl;
        cout << "vehicle id: " << stVehicle.iTrackID << ", " << stVehicle.isMatch << endl;
        if (stVehicle.bOutOfFrame == false && stVehicle.isMatch == false)
        {
            std::vector<cv::Point2f> ptAreas{cv::Point2f(stVehicle.rectVehicle.x, (float)stVehicle.rectVehicle.y + stVehicle.rectVehicle.height * 0.25), 
                                            cv::Point2f(stVehicle.rectVehicle.x + stVehicle.rectVehicle.width, stVehicle.rectVehicle.y + stVehicle.rectVehicle.height * 0.25),
                                            cv::Point2f(stVehicle.rectVehicle.br()), 
                                            cv::Point2f(stVehicle.rectVehicle.x, stVehicle.rectVehicle.y + stVehicle.rectVehicle.height)};
            double dDistance = cv::pointPolygonTest(ptAreas, ptCenterBox, true);
            cout << "True or False: " << dDistance << endl;
            // if (pointPolygonTest(ptAreas, ptCenterBox, false))
            if (dDistance > 0)
            {
                stVehicle.isMatch = true;
                stVehicle.countMatch = MAX_COUNT_UNKNOWN + 2;
                ITS_Output out;
                out.vehicle = stVehicle;
                out.plate = stPlate;
                out.typeOutput = TypeOutput::Both;
                outputs.push_back(out);
                return true;
            }
        }
    }
    return false;
}


bool ITS::checkPlate(PlateInfor& plate, int cols, int rows)
{
    if(plate.rect.area() < 1500) return false;

    if((plate.rect.tl().x > 5) && (plate.rect.tl().y > 5) && (plate.rect.br().x < (cols - 5)) && (plate.rect.br().y < (rows - 5))) {
        return true;
    } else {
        return false;
    }
}
cv::Rect2f ITS::getRect4K(cv::Rect2f rect_fhd, int cols_4K, int rows_4K)
{
    if(cols_4K == WIDTH_4K && rows_4K == HEIGHT_4K) {
        cv::Rect2f rect_4K;
        rect_4K.x = rect_fhd.x * SCALE_FHD_4K;
        rect_4K.y = rect_fhd.y * SCALE_FHD_4K;
        rect_4K.width = rect_fhd.width * SCALE_FHD_4K;
        rect_4K.height = rect_fhd.height * SCALE_FHD_4K;
        return rect_4K;
    } else {
        return rect_fhd;
    }
}
