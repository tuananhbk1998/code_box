#include "Invade.hpp"

Invade::Invade()
{
    this->m_Detector = new ObjectDetector();
    this->m_Tracker = new ObjectTracking();
    this->m_License = new AnprRecognizerx86();
    this->m_ListTracked.clear();
}

Invade::~Invade()
{
    if(this->m_Detector) delete this->m_Detector;
    if(this->m_Tracker) delete this->m_Tracker;
    if(this->m_Tracker) delete this->m_License;
}

int Invade::initInvade()
{
    if(this->m_Detector->initObjectDetector("snpe", "traffic", "CPU") != STATUS_SUCCESS)
    // if(this->detector->initObjectDetector("snpe", "traffic", "CPU") != STATUS_SUCCESS)
        return STATUS_FAILED;
    return STATUS_SUCCESS;

}
void Invade::makeRelativePosition(std::vector<RegionInvade_t>& regions)
{
    float centerRegion1 = (regions[0].zone[0].x + regions[0].zone[2].x) / 2;
    float centerRegion2 = (regions[1].zone[0].x + regions[1].zone[2].x) / 2;
    std::cout << "centerRegion1: " << centerRegion1 << std::endl;
    std::cout << "centerRegion2: " << centerRegion2 << std::endl;

    if (centerRegion1 < centerRegion2){
        regions[0].relativePosition = "left";
        regions[1].relativePosition = "right";
    }else{
        regions[1].relativePosition = "left";
        regions[0].relativePosition = "right";
    }
}
int Invade::setInvade(settingInvade settings)
{
    // this->m_Settings = settings;
        

    this->m_Labels = settings.labelSet;
    this->m_Rules = settings.invadeSet;
    this->m_Vehicles = settings.vehiclesSet;
    
    for (int i = 0; i < settings.regionSet.size() ; i++)
    {
         RegionInvade_t region;
         region.zone = settings.regionSet[i];
         this->m_Regions.push_back(region);

    }
    makeRelativePosition(this->m_Regions);

    return STATUS_SUCCESS;
}

//================Ver1================================================
int Invade::updateInvade1(cv::Mat& frame, std::vector<outputInvade>& output)
{
   try
    {
        cv::Mat Frame;
        frame.copyTo(Frame);
        output.clear();

        for(auto i=0; i<this->m_Regions.size(); i++)
        {
            // if(i == 0)
            //     cv::putText(Frame, this->text_lane[i], cv::Point(this->regions[i][0].x, this->regions[i][1].y), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 0), 2);
            // else if (i == 1)
            //     cv::putText(Frame, this->text_lane[i], cv::Point(this->regions[i][0].x, this->regions[i][1].y), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 0), 2);

            for(auto j=0; j<this->m_Regions[i].zone.size(); j++)
                cv::line(Frame, this->m_Regions[i].zone[j], this->m_Regions[i].zone[(j+1) % this->m_Regions[i].zone.size()], cv::Scalar(0, 255, 255), 2, cv::LINE_AA);
        }
        
        std::vector<ObjectTrace> detected;
        if(this->m_Detector->executeObjectDetector(Frame, detected, 0.5) != STATUS_SUCCESS)
        {
            LOG_FAIL("Execute Anpr detector failed");
            return STATUS_FAILED;
        }

        if(detected.size() == 0)
            return STATUS_FAILED;

        std::vector<TrackingTrace> tracks;
        this->m_Tracker->process(detected, tracks);

        for(auto it = this->m_ListTracked.begin(); it != this->m_ListTracked.end();)
        {
            const int theId = (*it).track_id;
            const auto p = find_if(tracks.begin(), tracks.end(), 
                                    [theId](const TrackingTrace &a) {return (a.m_ID == theId);});
            if(p == tracks.end() && it != m_ListTracked.end())
                it = m_ListTracked.erase(it);
            else
                it++;
        }
 
        for(auto &track : tracks)
        {
            if(!track.isOutOfFrame)
            {
                if(track.m_trace.size() > 1)
                {
                    
                    // cv::arrowedLine(Frame, track.m_trace[track.m_trace.size() - 2], track.m_trace[track.m_trace.size() - 1], cv::Scalar(0, 0, 255), 2, cv::LINE_AA);

                    const int theId = track.m_ID;
                    const auto p = std::find_if(this->m_ListTracked.begin(), this->m_ListTracked.end(),
                                            [theId](const outputInvade &a) {return (a.track_id == theId);});
                    auto it = this->m_ListTracked.begin();
                    if(p != this->m_ListTracked.end())
                    {
                        int index = std::distance(this->m_ListTracked.begin(), p);
                        this->m_ListTracked[index].rect = track.m_rect;
                        this->m_ListTracked[index].isOutOfFrame = false;
                        this->m_ListTracked[index].isEvent = false;
                        int tempPosition;
                        if (checkLane(track, this->m_Regions[0].zone)){
                            tempPosition = 0;
                        }else if (checkLane(track, this->m_Regions[1].zone)){
                            tempPosition = 1;
                        } else tempPosition = 2;
                        // std::cout << vehicles.size() << std::endl;
                        
                        /* check loi lan lan */
                        if (m_Rules.size() == 0){
                            if (!(this->m_ListTracked[index].typeError == InvadeLaneError || this->m_ListTracked[index].typeError == BothError))
                            {
                                if (this->m_ListTracked[index].idLane != 2){                               
                                    if ((this->m_ListTracked[index].idLane != tempPosition) && (tempPosition != 2)) {
                                        this->m_ListTracked[index].isEvent = true;
                                        if (this->m_ListTracked[index].typeError == WrongLaneError){
                                            this->m_ListTracked[index].typeError = BothError; //bi 2 loi
                                        }else {
                                            this->m_ListTracked[index].typeError = InvadeLaneError; // loi lan lan
                                        }
                                    }
                                }
                            }
                        }
                        else if (m_Rules.size() == 1){
                            if (!(this->m_ListTracked[index].typeError == InvadeLaneError || this->m_ListTracked[index].typeError == BothError))
                            {
                                if (this->m_ListTracked[index].idLane == m_Rules[0][1]){
                                    if ((tempPosition == m_Rules[0][0]) && (tempPosition != 2)){
                                        this->m_ListTracked[index].isEvent = true;
                                        if (this->m_ListTracked[index].typeError == WrongLaneError){
                                            this->m_ListTracked[index].typeError = BothError; //bi 2 loi
                                        }else {
                                            this->m_ListTracked[index].typeError = InvadeLaneError; // loi lan lan
                                        }
                                    }
                                }
                            }
                        } else{
                            // do nothing
                        }
                        
                        this->m_ListTracked[index].idLane = tempPosition;
                        
                        /* check loi sai lan */
                        
                        if (this->m_ListTracked[index].idLane != 2){
                            if(std::find(m_Vehicles[tempPosition].begin(), m_Vehicles[tempPosition].end(), track.m_type) == m_Vehicles[tempPosition].end()){       
                                if (!(this->m_ListTracked[index].typeError == WrongLaneError || this->m_ListTracked[index].typeError == BothError))
                                {
                                    this->m_ListTracked[index].isEvent = true;
                                    if (this->m_ListTracked[index].typeError == InvadeLaneError){
                                        this->m_ListTracked[index].typeError = BothError;    //2 loi
                                    } else{
                                        this->m_ListTracked[index].typeError = WrongLaneError;   //loi sai lan
                                    }
                                }               
                            } 
                            else  {
                                if (this->m_ListTracked[index].typeError == BothError){
                                    this->m_ListTracked[index].typeError = InvadeLaneError;
                                } else if (this->m_ListTracked[index].typeError == WrongLaneError){
                                    this->m_ListTracked[index].typeError = Undefine;
                                }
                            }  
                        }
                        std::string text(this->m_ListTracked[index].label +  " - " + std::to_string(this->m_ListTracked[index].track_id));
                        cv::putText(Frame, text, cv::Point(track.m_rect.x, track.m_rect.y), 1, 2, cv::Scalar(255, 255, 255), 1);
                        cv::circle(Frame, cv::Point(this->m_ListTracked[index].rect.x + this->m_ListTracked[index].rect.width/2, this->m_ListTracked[index].rect.y + this->m_ListTracked[index].rect.height * 0.9), 2, cv::Scalar(0,255,0));
                    } 
                    else 
                    {
                        for (int i = 0; i < m_Regions.size(); i++){
                            if (checkLane(track, this->m_Regions[i].zone)){
                                outputInvade obj;
                                obj.track_id = track.m_ID;
                                obj.label = track.m_type;
                                obj.rect = track.m_rect;
                                obj.idLane = i;
                                obj.typeError = Undefine;
                                this->m_ListTracked.push_back(obj);
                            }
                        }
                            
                    }
                } 
            }
            else 
            {
                const int theId = track.m_ID;
                const auto p = find_if(m_ListTracked.begin(), m_ListTracked.end(), 
                                        [theId] (const outputInvade &a) {return (a.track_id == theId);});

                if(p != m_ListTracked.end()) {
                    int dist = distance(m_ListTracked.begin(), p);
                    m_ListTracked[dist].isOutOfFrame = true;
                }
            }   
        }
        for(auto out : m_ListTracked) {
            if(!out.isOutOfFrame) {
                output.push_back(out);
            }
        }
        frame = Frame;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return STATUS_SUCCESS;
}


//V =================Ver 2: bo phan sai lan ================================
int Invade::updateInvade2(cv::Mat& frame, std::vector<outputInvade>& output)
{
   try
    {
        cv::Mat Frame;
        frame.copyTo(Frame);
        output.clear();

        // for(auto i=0; i<this->m_Regions.size(); i++)
        // {
        //     // if(i == 0)
        //     //     cv::putText(Frame, this->text_lane[i], cv::Point(this->regions[i][0].x, this->regions[i][1].y), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 0), 2);
        //     // else if (i == 1)
        //     //     cv::putText(Frame, this->text_lane[i], cv::Point(this->regions[i][0].x, this->regions[i][1].y), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 0), 2);

        //     for(auto j=0; j<this->m_Regions[i].zone.size(); j++)
        //         cv::line(Frame, this->m_Regions[i].zone[j], this->m_Regions[i].zone[(j+1) % this->m_Regions[i].zone.size()], cv::Scalar(0, 255, 255), 2, cv::LINE_AA);
        // }
        
        std::vector<ObjectTrace> detected;
        if(this->m_Detector->executeObjectDetector(Frame, detected, 0.5) != STATUS_SUCCESS)
        {
            LOG_FAIL("Execute Anpr detector failed");
            return STATUS_FAILED;
        }
        for(auto it = this->m_ListTracked.begin(); it != this->m_ListTracked.end();)
        {
            if ((*it).isEvent == true)
                it = m_ListTracked.erase(it);
            else 
                it++;
        }
        std::vector<ObjectTrace> plateObj;
        if (isExistErrorObj(m_ListTracked))
        {
            m_License->detect_traffic(Frame, plateObj);
            std::cout << "plate: " << plateObj.size() << std::endl;
        }
        // if(detected.size() == 0)
        //     return STATUS_FAILED;

        std::vector<TrackingTrace> tracks;
        this->m_Tracker->process(detected, tracks);
        
        for(auto it = this->m_ListTracked.begin(); it != this->m_ListTracked.end();)
        {
            // if ((*it).isEvent == true)
            //     it = m_ListTracked.erase(it);
            const int theId = (*it).track_id;
            const auto p = find_if(tracks.begin(), tracks.end(), 
                                    [theId](const TrackingTrace &a) {return (a.m_ID == theId);});
            if(p == tracks.end() && it != m_ListTracked.end()){
                if ((*it).typeError == InvadeLaneError){
                    (*it).isEvent = true;
                    int index;
                    (*it).eventPlate.license = getLegitLicense((*it).listLicensePlate, index);
                    (*it).eventPlate.imgPlate = (*it).listImgPlate[index];
                    it++;
                }else{
                    it = m_ListTracked.erase(it);
                }
                
            }               
            else
                it++;      
        }
 
        for(auto &track : tracks)
        {
            if(!track.isOutOfFrame)
            {
                if(track.m_trace.size() > 1)
                {
                    
                    // cv::arrowedLine(Frame, track.m_trace[track.m_trace.size() - 2], track.m_trace[track.m_trace.size() - 1], cv::Scalar(0, 0, 255), 2, cv::LINE_AA);

                    const int theId = track.m_ID;
                    const auto p = std::find_if(m_ListTracked.begin(), m_ListTracked.end(),
                                            [theId](const outputInvade &a) {return (a.track_id == theId);});
                    auto it = m_ListTracked.begin();
                    if(p != m_ListTracked.end())
                    {
                        int index = std::distance(m_ListTracked.begin(), p);
                        m_ListTracked[index].rect = track.m_rect;
                        m_ListTracked[index].isOutOfFrame = false;
                        m_ListTracked[index].isEvent = false;
                    
                        std::cout << "Object ID: "<< m_ListTracked[index].typeError << ": " <<m_ListTracked[index].idLane << std::endl;
                        // std::cout << m_Regions[m_ListTracked[index].idLane].relativePosition << std::endl;
                        //-----check vi tri hien phuong tien hien tai
                        int tempPosition;  
                        if (m_ListTracked[index].typeLane == "left")  
                        {
                            cv::circle(Frame, cv::Point(m_ListTracked[index].rect.x + m_ListTracked[index].rect.width * 0.9, m_ListTracked[index].rect.y + m_ListTracked[index].rect.height * 0.9), 2, cv::Scalar(255,0,255));
                            if (checkLaneLeft(track, m_Regions[0].zone)){
                                tempPosition = 0;
                            }else if (checkLaneLeft(track, m_Regions[1].zone)){
                                tempPosition = 1;
                            } else tempPosition = 2;

                        }
                        else if (m_ListTracked[index].typeLane == "right") 
                        {
                            cv::circle(Frame, cv::Point(m_ListTracked[index].rect.x + m_ListTracked[index].rect.width * 0.1, m_ListTracked[index].rect.y + m_ListTracked[index].rect.height * 0.9), 2, cv::Scalar(0,255,255));
                            if (checkLaneRight(track, m_Regions[0].zone)){
                                tempPosition = 0;
                            }else if (checkLaneRight(track, m_Regions[1].zone)){
                                tempPosition = 1;
                            } else tempPosition = 2;
                        } else{
                            // do nothing
                        }       

                        std::cout << "tempPosition: "<< tempPosition << std::endl;
                        if (tempPosition != 2){
                            std::cout << "tempPosition: "<< m_Regions[tempPosition].relativePosition << std::endl;

                        }

                        // ------- check loi lan lan
                        if (m_Rules.size() == 0){
                            if (!(m_ListTracked[index].typeError == InvadeLaneError))
                            {
                                if (m_ListTracked[index].idLane != 2){                               
                                    if ((m_ListTracked[index].idLane != tempPosition) && (tempPosition != 2)) {
                                        m_ListTracked[index].typeError = InvadeLaneError; // loi lan lan  
                                        m_ListTracked[index].eventImage = Frame;
                                        m_ListTracked[index].eventRect = m_ListTracked[index].rect;       
                                        // cv::rectangle(Frame, m_ListTracked[index].rect , cv::Scalar(0,255,255));                       
                                    }
                                }
                            }
                        }
                        else if (m_Rules.size() == 1){
                            if (!(m_ListTracked[index].typeError == InvadeLaneError))
                            {
                                if (m_ListTracked[index].idLane == m_Rules[0][1]){
                                    if ((tempPosition == m_Rules[0][0]) && (tempPosition != 2)){
                                        m_ListTracked[index].typeError = InvadeLaneError; // loi lan lan
                                        m_ListTracked[index].eventImage = Frame;
                                        m_ListTracked[index].eventRect = m_ListTracked[index].rect;  
                                    }
                                }
                            }
                        } else{
                            // do nothing
                        }

                        //doc bien so
                        if( (m_ListTracked[index].typeError == InvadeLaneError) ){
                            std::vector<cv::Point2f> ptAreas{cv::Point2f(m_ListTracked[index].rect.x, m_ListTracked[index].rect.y ), 
                                        cv::Point2f(m_ListTracked[index].rect.x + m_ListTracked[index].rect.width, m_ListTracked[index].rect.y ),
                                        cv::Point2f(m_ListTracked[index].rect.br()), 
                                        cv::Point2f(m_ListTracked[index].rect.x, m_ListTracked[index].rect.height)};
                            for (int i = 0 ; i < plateObj.size() ; i++){ 
                                if(isInside(plateObj[i], ptAreas))
                                {
                                    PlateInfor tempPlate;
                                    m_ListTracked[index].eventPlate.rect = plateObj[i].rect;
                                    // cv::rectangle(Frame, m_ListTracked[index].eventPlate.rect, cv::Scalar(0,255,0));
                                    m_License->recognize_traffic(Frame,Frame, plateObj[i], tempPlate); //sau ham nay rect cua plate se thay doi
                                    cv::putText(Frame, tempPlate.license , cv::Point(m_ListTracked[index].eventPlate.rect.x, m_ListTracked[index].eventPlate.rect.y -5), FONT_HERSHEY_COMPLEX, 0.8, cv::Scalar(0,0,255),2);
                                    if ((tempPlate.license != "") && (tempPlate.license != "Unknown")){
                                        m_ListTracked[index].listLicensePlate.push_back(tempPlate.license);
                                        m_ListTracked[index].listImgPlate.push_back(Frame(plateObj[i].rect).clone());
                                    }
                                    // }
                                }
                            }

                        }

                        std::string text(m_ListTracked[index].idLane +  " - " + std::to_string(m_ListTracked[index].track_id));
                        cv::putText(Frame, text, cv::Point(track.m_rect.x, track.m_rect.y), 1, 2, cv::Scalar(255, 255, 255), 1);
                        cv::circle(Frame, cv::Point(m_ListTracked[index].rect.x + m_ListTracked[index].rect.width * 0.5, m_ListTracked[index].rect.y + m_ListTracked[index].rect.height * 0.9), 2, cv::Scalar(0,255,0));
                        
                        
                    } 
                    else 
                    {
                        for (int i = 0; i < m_Regions.size(); i++){
                        cv::circle(Frame, cv::Point2f(track.m_rect.x + float(track.m_rect.width * 0.5), 
                                    track.m_rect.y + float(track.m_rect.height * 0.9)), 2, cv::Scalar(0,255,0));

                            if (checkLane(track, m_Regions[i].zone)){
                                outputInvade obj;
                                obj.track_id = track.m_ID;
                                obj.label = track.m_type;
                                obj.rect = track.m_rect;
                                obj.idLane = i;
                                std::cout << m_Regions[i].relativePosition << std::endl;
                                obj.typeLane = m_Regions[i].relativePosition;
                                obj.typeError = Undefine;
                                m_ListTracked.push_back(obj);
                            }
                        }
                            
                    }
                } 
            }
            else 
            {
                const int theId = track.m_ID;
                const auto p = find_if(m_ListTracked.begin(), m_ListTracked.end(), 
                                        [theId] (const outputInvade &a) {return (a.track_id == theId);});

                if(p != m_ListTracked.end()) {
                    int dist = distance(m_ListTracked.begin(), p);
                    m_ListTracked[dist].isOutOfFrame = true;
                }
            }   
        }

        frame = Frame;
        for (auto out : m_ListTracked)
        {
           
            if (out.isEvent)
            {
                output.push_back(out);
            }
        
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return STATUS_SUCCESS;
}
bool Invade::checkLane(TrackingTrace track, std::vector<cv::Point2f> area)
{
    if (area.size() == 0) return false;
    cv::Point2f cBox = cv::Point2f(track.m_rect.x + float(track.m_rect.width * 0.5), 
                                    track.m_rect.y + float(track.m_rect.height * 0.9));
    if (pointPolygonTest(area, cBox, false) >= 0) {
        return true;
    }
    return false;
}
bool Invade::checkLaneLeft(TrackingTrace track, std::vector<cv::Point2f> area)
{
    if (area.size() == 0) return false;
    cv::Point2f cBox = cv::Point2f(track.m_rect.x + float(track.m_rect.width * 0.9), 
                                    track.m_rect.y + float(track.m_rect.height * 0.9));
    if (pointPolygonTest(area, cBox, false) >= 0) {
        return true;
    }
    return false;
}

bool Invade::checkLaneRight(TrackingTrace track, std::vector<cv::Point2f> area)
{
    if (area.size() == 0) return false;
    cv::Point2f cBox = cv::Point2f(track.m_rect.x + float(track.m_rect.width * 0.1), 
                                    track.m_rect.y + float(track.m_rect.height * 0.9));
    if (pointPolygonTest(area, cBox, false) >= 0) {
        return true;
    }
    return false;
}

bool Invade::onSegment(cv::Point p, cv::Point q, cv::Point r)
{
    if(q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) && 
        q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y))
    {
        return true;
    }

    return false;
}

int Invade::orientation(cv::Point p, cv::Point q, cv::Point r)
{
    int val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
    if(val == 0)
        return 0;
    
    return (val > 0) ? 1 : 2;
}

bool Invade::doIntersect(cv::Point p1, cv::Point q1, cv::Point p2, cv::Point q2)
{
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);

    if(o1 != o2 && o3 != o4)
        return true;
    if(o1 == 0 && onSegment(p1, p2, q1))
        return true;
    if(o2 == 0 && onSegment(p1, q2, q1))
        return true;
    if(o3 == 0 && onSegment(p2, p1, q2))
        return true;
    if(o4 == 0 && onSegment(p2, q1, q2))
        return true;
    
    return false;
}

void Invade::checkTraces(Trace trace, int& UP, int& DOWN, int& LEFT, int& RIGHT)
{
    constexpr size_t minSize = 3;
    if(trace.size() > minSize) {
        const TrajectoryPoint &pt1 = trace.at(trace.size() - 1);
        const TrajectoryPoint &pt2 = trace.at(trace.size() - minSize);

        if(pt1.m_prediction.y > pt2.m_prediction.y) {
            DOWN = 1;
        }
        if(pt2.m_prediction.y > pt1.m_prediction.y) {
            UP = 1;
        }
        if(pt1.m_prediction.x > pt2.m_prediction.x) {
            RIGHT = 1;
        }
        if(pt2.m_prediction.x > pt1.m_prediction.x) {
            LEFT = 1;
        }
    }
}

bool Invade::isExistErrorObj(std::vector<outputInvade> listTrack)
{
    for (int i = 0; i < listTrack.size(); i++)
    {
        if ((listTrack[i].typeError == InvadeLaneError) )
            return true;    
    }
    return false;
}

bool Invade::isInside(ObjectTrace object, std::vector<cv::Point2f> area)
{
    if (area.size() == 0) return false;
    cv::Point2f cBox = cv::Point2f(object.rect.x + float(object.rect.width/2), 
                                    object.rect.y + float(object.rect.height/2));
    if (pointPolygonTest(area, cBox, false) >= 0) {
        return true;
    }
    return false;
}
std::string Invade::getLegitLicense(std::vector<std::string> listLicensePlate, int &index)
{
    int vDemSo[50];
    for (int i =0 ; i < listLicensePlate.size(); i++)
    {
        int count = 1;
        vDemSo[i] = count;
        for (int j = i + 1; j < listLicensePlate.size() - 1; j++){
            if (listLicensePlate[i] == listLicensePlate[j]){
                vDemSo[i] += 1;
                listLicensePlate.erase(listLicensePlate.begin() + j);
                j--;
            }
        }
        
    }    
    int max =0;
    for (int i = 0 ; i < listLicensePlate.size() ; i++)
    {
        cout << listLicensePlate[i] << endl;
        if (vDemSo[i] > max){
            max = vDemSo[i];
            index = i;
        }
           
    }
    return listLicensePlate[index];
}