#include"Speed.hpp"




Speed::Speed()
{
    // this->soLuongXe = 0;
    this->m_Detector = new ObjectDetector();
    this->m_Tracker = new ObjectTracking();
    this->m_listObject.clear();
}

Speed::~Speed()
{
    if(this->m_Detector) delete this->m_Detector;
    if(this->m_Tracker) delete this->m_Tracker;
}

int Speed::initSpeed()
{
    this->frequency = getTickFrequency();

    if(this->m_Detector->initObjectDetector("snpe", "traffic", "CPU") != STATUS_SUCCESS)
        return STATUS_FAILED;
    return STATUS_SUCCESS;
}

int Speed::settingSpeed(SpeedSetting st_Settings)
{
    makeRegions(st_Settings);
    this->m_Vehicles = st_Settings.vehiclesSet;
    this->m_DistancePerRegion = st_Settings.iDistancePerRegion;
    // this->m_Rules = st_Settings.invadeSet;
    return STATUS_SUCCESS;
}
void Speed::makeRegions(SpeedSetting settings)
{
    
    for (int i = 0; i < (settings.lineSet.size() - 1) ; i++)
    {
        Region_t region;
        cv::Point2f bl(0,settings.lineSet[i].y);      
        cv::Point2f tl(0,settings.lineSet[i+1].y);    
        cv::Point2f tr(3840, settings.lineSet[i+1].y);
        cv::Point2f br(3840, settings.lineSet[i].y);

        
        region.position.push_back(bl);
        region.position.push_back(tl);
        region.position.push_back(tr);
        region.position.push_back(br);
        region.position.push_back(bl);
     
        region.distance = cv::norm(settings.lineSet[i]- settings.lineSet[i+1]);
    
        m_Region.push_back(region);
    }
}
//Version 1

int Speed::updateSpeedv1(cv::Mat& img, std::vector<SpeedOutput>& outputs)
{
   
    for (int i = 0; i < m_Region.size(); i++)
    {
        for (int j = 0; j < m_Region[i].position.size() -1; j++ )
            cv::line(img, m_Region[i].position[j], m_Region[i].position[j+1], cv::Scalar(0,255,255));
    }
    // for (int i = 0; i < m_Region.size(); i++)
    // {
    //     std::cout << "distance[" << i << "]: " << m_Region[i].distance << std::endl;
    // }
    std::vector<ObjectTrace> objects;
    if(STATUS_SUCCESS != this->m_Detector->executeObjectDetector(img,objects, 0.3))
        return STATUS_FAILED;
    else
    {
        // process tracking
        std::vector<TrackingTrace> tracks;
        this->m_Tracker->process(objects, tracks);
        
        // if(this->m_settings.violation.oppose.isUsed)
        //     this->m_violation->process(img, this->m_tracker, tracks);
        //delete object which is abandoned
        
        for(auto it = this->m_listObject.begin(); it != this->m_listObject.end();)
        {
            const int theId =  (*it).track_id;
            const auto p = find_if(tracks.begin(), tracks.end(), 
                                        [theId] ( const TrackingTrace& a ) { return (a.m_ID == theId);}); 
            
            if (p == tracks.end() && it != this->m_listObject.end())
                it = this->m_listObject.erase(it);                
            else 
                it++;
        }

        
        // process new track and update old track
        for(auto track : tracks)
        {           
            if(!track.isOutOfFrame)
            {
                
                const int theId =  track.m_ID;
                const auto p = find_if(this->m_listObject.begin(), this->m_listObject.end(), 
                                        [theId] ( const SpeedOutput& a ) { return (a.track_id == theId);});
                if (p == this->m_listObject.end() )
                {
                    SpeedOutput obj;
                    obj.rect = track.m_rect;
                    obj.track_id = track.m_ID;
                    obj.label = track.m_type;
                    obj.timeAccessed = cv::getTickCount(); 
                    obj.trackTrace.push_back(cv::Point2f(track.m_rect.x + (float)(track.m_rect.width / 2),
                                                        track.m_rect.y + (float)(track.m_rect.height)));
                    this->m_listObject.push_back(obj);
                    
                }
                else{
                    int index = distance(this->m_listObject.begin(), p);
                    this->m_listObject[index].rect = track.m_rect;
                    this->m_listObject[index].isOutOfFrame = false;
                    this->m_listObject[index].timeIn = (cv::getTickCount() - this->m_listObject[index].timeAccessed) / this->frequency;
                    this->m_listObject[index].trackTrace.push_back(cv::Point2f(track.m_rect.x + (float)(track.m_rect.width / 2),
                                                                    track.m_rect.y + (float)(track.m_rect.height)));
                    // cv::Point2f lastTrace = track.m_trace[track.m_trace.size() - 1];
                    // cv::Point2f preTrace = track.m_trace[track.m_trace.size() - 2];
                     cv::Point2f lastTrace = this->m_listObject[index].trackTrace[this->m_listObject[index].trackTrace.size()-1];
                     cv::Point2f preTrace = this->m_listObject[index].trackTrace[this->m_listObject[index].trackTrace.size()  - 2];
                    std::cout << lastTrace.x << " , " << lastTrace.y << std::endl;
                    std::cout << preTrace.x << " , " << preTrace.y << std::endl;
                    std::cout << this->m_listObject[index].trackTrace.size() << std::endl;
                    cv::line(img, lastTrace, preTrace, cv::Scalar(0,0,255));
                    for (int i = 0 ; i < m_Region.size() ; i++)
                    {
                        if (isInside(preTrace, m_Region[i].position) && isInside(lastTrace, m_Region[i].position))
                        {
                            if (this->m_listObject[index].position != i){
                                double temp_speed = calcSpeed(this->m_listObject[index], m_Region[i], 2/12.f);
                                this->m_listObject[index].vTempSpeed.push_back(temp_speed);
                          
                                this->m_listObject[index].timeAccessed = cv::getTickCount();
                                this->m_listObject[index].position = i;
                            }
                            
                        }else if (this->m_listObject[index].position == 0){
                            this->m_listObject[index].position = 1;
                            for (int j = 0 ; j < this->m_listObject[index].vTempSpeed.size(); j++){
                                this->m_listObject[index].speed += this->m_listObject[index].vTempSpeed[j];
                            }
                            this->m_listObject[index].speed = this->m_listObject[index].speed / this->m_listObject[index].vTempSpeed.size();
                            std::cout << this->m_listObject[index].speed  << std::endl;
                        }
                    }

                }
            }
            else
            {
                
                const int theId =  track.m_ID;
                // cout << "Out of frame, id: " << theId <<endl;
                const auto p = find_if(this->m_listObject.begin(), this->m_listObject.end(), 
                                        [theId] ( const SpeedOutput& a ) { return (a.track_id == theId);});                         
                if (p != this->m_listObject.end()) 
                {
                    int dist = distance(this->m_listObject.begin(), p);
                    this->m_listObject[dist].isOutOfFrame = true;
                }
                
            } 
        }

        for(auto p : this->m_listObject)
            if(p.isOutOfFrame != true) 
                outputs.push_back(p);    
                                     
    }
}
/*
int Speed::updateSpeedv2(cv::Mat& img, std::vector<SpeedOutput>& outputs)
{
   
    for (int i = 0; i < m_Region.size(); i++)
    {
        for (int j = 0; j < m_Region[i].position.size() -1; j++ )
            cv::line(img, m_Region[i].position[j], m_Region[i].position[j+1], cv::Scalar(0,255,255));
    }

    std::vector<ObjectTrace> objects;
    if(STATUS_SUCCESS != this->m_Detector->executeObjectDetector(img,objects, 0.3))
        return STATUS_FAILED;
    else
    {
        // process tracking
        std::vector<TrackingTrace> tracks;
        this->m_Tracker->process(objects, tracks);
        
        // if(this->m_settings.violation.oppose.isUsed)
        //     this->m_violation->process(img, this->m_tracker, tracks);
        //delete object which is abandoned
        
        for(auto it = this->m_listObject.begin(); it != this->m_listObject.end();)
        {
            const int theId =  (*it).track_id;
            const auto p = find_if(tracks.begin(), tracks.end(), 
                                        [theId] ( const TrackingTrace& a ) { return (a.m_ID == theId);}); 
            
            if (p == tracks.end() && it != this->m_listObject.end())
                it = this->m_listObject.erase(it);                
            else 
                it++;
        }

        
        // process new track and update old track
        for(auto track : tracks)
        {           
            if(!track.isOutOfFrame)
            {
                
                const int theId =  track.m_ID;
                const auto p = find_if(this->m_listObject.begin(), this->m_listObject.end(), 
                                        [theId] ( const SpeedOutput& a ) { return (a.track_id == theId);});
                if (p == this->m_listObject.end() )
                {
                   for (int i = 0 ; i < m_Region.size() ; i++)
                    {
                        if (isInside(cv::Point2f(track.m_rect.x + track.m_rect.width/2, track.m_rect.y + track.m_rect.height), m_Region[i].position))
                        { 
                            SpeedOutput obj;
                            obj.rect = track.m_rect;
                            obj.track_id = track.m_ID;
                            obj.label = track.m_type;
                            obj.timeAccessed = cv::getTickCount(); 
                            obj.position = i;
                            this->m_listObject.push_back(obj);
                        }
     
                    }
                }
                else{
                    int index = distance(this->m_listObject.begin(), p);
                    this->m_listObject[index].rect = track.m_rect;
                    this->m_listObject[index].isOutOfFrame = false;

                     cv::Point2f bottomPoint = cv::Point2f((this->m_listObject[index].rect.x + this->m_listObject[index].rect.width /2), 
                                                (this->m_listObject[index].rect.y + this->m_listObject[index].rect.height) );
                    for (int i = 0 ; i < m_Region.size() ; i++)
                    {
                        if (isInside(bottomPoint, m_Region[i].position))
                        {
                            if (this->m_listObject[index].position != i){
                                this->m_listObject[index].speed =  calcSpeed(track, m_Region[i], this->m_listObject[index].timeIn);
                                this->m_listObject[index].timeIn = 0;
                                // this->m_listObject[index].timeAccessed = cv::getTickCount();
                                this->m_listObject[index].position = i;
                            }
                            else {
                                this->m_listObject[index].timeIn += 1/15.f;
                            }
                            
                        } 
                    }

                }
            }
            else
            {
                
                const int theId =  track.m_ID;
                // cout << "Out of frame, id: " << theId <<endl;
                const auto p = find_if(this->m_listObject.begin(), this->m_listObject.end(), 
                                        [theId] ( const SpeedOutput& a ) { return (a.track_id == theId);});                         
                if (p != this->m_listObject.end()) 
                {
                    int dist = distance(this->m_listObject.begin(), p);
                    this->m_listObject[dist].isOutOfFrame = true;
                }
                
            } 
        }

        for(auto p : this->m_listObject)
            if(p.isOutOfFrame != true) 
                outputs.push_back(p);    
                                     
    }
}
int Speed::updateSpeedv3(cv::Mat& img, std::vector<SpeedOutput>& outputs)
{
//    std::cout<< m_Region[0].distance << std::endl;
    // for (int i = 0; i < m_Region.size(); i++)
    // {
    //     for (int j = 0; j < m_Region[i].position.size() -1; j++ )
    //         cv::line(img, m_Region[i].position[j], m_Region[i].position[j+1], cv::Scalar(0,255,255));
    // }
    // for (int i = 0; i < m_Region.size(); i++)
    // {
    //     std::cout << "distance[" << i << "]: " << m_Region[i].distance << std::endl;
    // }
    std::vector<ObjectTrace> objects;
    if(STATUS_SUCCESS != this->m_Detector->executeObjectDetector(img,objects, 0.3))
        return STATUS_FAILED;
    else
    {
        // process tracking
        std::vector<TrackingTrace> tracks;
        this->m_Tracker->process(objects, tracks);
        
        // if(this->m_settings.violation.oppose.isUsed)
        //     this->m_violation->process(img, this->m_tracker, tracks);
        //delete object which is abandoned
        
        for(auto it = this->m_listObject.begin(); it != this->m_listObject.end();)
        {
            const int theId =  (*it).track_id;
            const auto p = find_if(tracks.begin(), tracks.end(), 
                                        [theId] ( const TrackingTrace& a ) { return (a.m_ID == theId);}); 
            
            if (p == tracks.end() && it != this->m_listObject.end())
                it = this->m_listObject.erase(it);                
            else 
                it++;
        }

        
        // process new track and update old track
        for(auto track : tracks)
        {           
            if(!track.isOutOfFrame)
            {
                
                const int theId =  track.m_ID;
                const auto p = find_if(this->m_listObject.begin(), this->m_listObject.end(), 
                                        [theId] ( const SpeedOutput& a ) { return (a.track_id == theId);});
                if (p == this->m_listObject.end() )
                {
                    SpeedOutput obj;
                    obj.rect = track.m_rect;
                    obj.track_id = track.m_ID;
                    obj.label = track.m_type;
                    obj.timeAccessed = cv::getTickCount(); 
                 
                    this->m_listObject.push_back(obj);
                   
                }
                else{
                    int index = distance(this->m_listObject.begin(), p);
                    this->m_listObject[index].rect = track.m_rect;
                    this->m_listObject[index].isOutOfFrame = false;

                    cv::Point2f lastTrace = track.m_trace[track.m_trace.size() - 1];
                    cv::Point2f preTrace = track.m_trace[track.m_trace.size() - 2];
            
                    cv::line(img, lastTrace, preTrace, cv::Scalar(0,0,255));
                   
                            
                    this->m_listObject[index].speed =  calcSpeed(track, m_Region[1], 1/12.f);
 
                }
            }
            else
            {
                
                const int theId =  track.m_ID;
                // cout << "Out of frame, id: " << theId <<endl;
                const auto p = find_if(this->m_listObject.begin(), this->m_listObject.end(), 
                                        [theId] ( const SpeedOutput& a ) { return (a.track_id == theId);});                         
                if (p != this->m_listObject.end()) 
                {
                    int dist = distance(this->m_listObject.begin(), p);
                    this->m_listObject[dist].isOutOfFrame = true;
                }
                
            } 
        }

        for(auto p : this->m_listObject)
            if(p.isOutOfFrame != true) 
                outputs.push_back(p);    
                                     
    }
}
int Speed::updateSpeedv4(cv::Mat& img, std::vector<SpeedOutput>& outputs)
{
   
    for (int i = 0; i < m_Region.size(); i++)
    {
        for (int j = 0; j < m_Region[i].position.size() -1; j++ )
            cv::line(img, m_Region[i].position[j], m_Region[i].position[j+1], cv::Scalar(0,255,255));
    }
    // for (int i = 0; i < m_Region.size(); i++)
    // {
    //     std::cout << "distance[" << i << "]: " << m_Region[i].distance << std::endl;
    // }
    std::vector<ObjectTrace> objects;
    if(STATUS_SUCCESS != this->m_Detector->executeObjectDetector(img,objects, 0.3))
        return STATUS_FAILED;
    else
    {
        // process tracking
        std::vector<TrackingTrace> tracks;
        this->m_Tracker->process(objects, tracks);
        
        // if(this->m_settings.violation.oppose.isUsed)
        //     this->m_violation->process(img, this->m_tracker, tracks);
        //delete object which is abandoned
        
        for(auto it = this->m_listObject.begin(); it != this->m_listObject.end();)
        {
            const int theId =  (*it).track_id;
            const auto p = find_if(tracks.begin(), tracks.end(), 
                                        [theId] ( const TrackingTrace& a ) { return (a.m_ID == theId);}); 
            
            if (p == tracks.end() && it != this->m_listObject.end())
                it = this->m_listObject.erase(it);                
            else 
                it++;
        }

        
        // process new track and update old track
        for(auto track : tracks)
        {           
            if(!track.isOutOfFrame)
            {
                
                const int theId =  track.m_ID;
                const auto p = find_if(this->m_listObject.begin(), this->m_listObject.end(), 
                                        [theId] ( const SpeedOutput& a ) { return (a.track_id == theId);});
                if (p == this->m_listObject.end() )
                {
                    SpeedOutput obj;
                    obj.rect = track.m_rect;
                    obj.track_id = track.m_ID;
                    obj.label = track.m_type;
                    obj.timeAccessed = cv::getTickCount(); 

                    this->m_listObject.push_back(obj);
                    
                }
                else{
                    int index = distance(this->m_listObject.begin(), p);
                    this->m_listObject[index].rect = track.m_rect;
                    this->m_listObject[index].isOutOfFrame = false;
                   

                    cv::Point2f lastTrace = track.m_trace[track.m_trace.size() - 1];
                    cv::Point2f preTrace = track.m_trace[track.m_trace.size() - 2];
          
                    cv::line(img, lastTrace, preTrace, cv::Scalar(0,0,255));
                    for (int i = 0 ; i < m_Region.size() ; i++)
                    {
                        if (isInside(preTrace, m_Region[i].position) && isInside(lastTrace, m_Region[i].position))
                        {
                           
                            this->m_listObject[index].speed = calcSpeed(track, m_Region[i], 1/12.f);
                   
                            this->m_listObject[index].position = i;
                           
                            
                        }
                    }

                }
            }
            else
            {
                
                const int theId =  track.m_ID;
                // cout << "Out of frame, id: " << theId <<endl;
                const auto p = find_if(this->m_listObject.begin(), this->m_listObject.end(), 
                                        [theId] ( const SpeedOutput& a ) { return (a.track_id == theId);});                         
                if (p != this->m_listObject.end()) 
                {
                    int dist = distance(this->m_listObject.begin(), p);
                    this->m_listObject[dist].isOutOfFrame = true;
                }
                
            } 
        }

        for(auto p : this->m_listObject)
            if(p.isOutOfFrame != true) 
                outputs.push_back(p);    
                                     
    }
}
*/
bool Speed::isInside(cv::Point2f point, std::vector<cv::Point2f> area)
{
    if (area.size() == 0) return false;
    // cv::Point2f cBox = cv::Point2f(track.m_rect.x + float(track.m_rect.width/2), 
    //                                 track.m_rect.y + float(track.m_rect.height));
    if (pointPolygonTest(area, point, false) >= 0) {
        return true;
    }
    return false;
}
double Speed::calcSpeed(SpeedOutput object, Region_t stRegion, double dTimeMove)
{
    if(object.trackTrace.size() < 2)
        return 0;
    // cv::Point2f lastTrace = track.m_trace[track.m_trace.size() - 1];
    // cv::Point2f preTrace = track.m_trace[track.m_trace.size() - 3];

    cv::Point2f lastTrace = object.trackTrace[object.trackTrace.size() -1 ];
    cv::Point2f preTrace = object.trackTrace[object.trackTrace.size() -2 ];

    double res = cv::norm(lastTrace - preTrace);
    double speed = (((res / stRegion.distance)* m_DistancePerRegion) / dTimeMove) * 3.6;
    // double speed = (4 / dTimeMove) * 3.6;
    // double speed = ((res * 20 ) / 5/30.f) * 3.6;
    return speed;
}