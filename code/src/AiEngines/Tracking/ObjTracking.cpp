#include "ObjTracking.hpp"


ObjTracking::ObjTracking()
{
    this->m_detector = new ObjectDetector();
    this->m_tracker = new ObjectTracking();

}

ObjTracking::~ObjTracking()
{
    if(this->m_detector) delete this->m_detector;
    if(this->m_tracker) delete this->m_tracker;
    this->m_listObject.clear();
}
int ObjTracking::init(std::string typeModel, std::string objectTarget, std::string typeRuntime)
{

    if(this->m_detector->initObjectDetector(typeModel, objectTarget, typeRuntime) != STATUS_SUCCESS)
        return STATUS_FAILED;

    return STATUS_SUCCESS;
}
// bool ObjTracking::isInside(TrackingTrace track, vector<cv::Point> area)
// {
//     if (area.size() == 0) return false;
//     cv::Point2f cBox = cv::Point2f(track.m_rect.x + float(track.m_rect.width/2), 
//                                     track.m_rect.y + float(track.m_rect.height*0.9));
//     if (pointPolygonTest(area, cBox, false) >= 0) {
//         return true;
//     }
//     return false;
// }
int ObjTracking::run(const cv::Mat& img, std::vector<ObjectTrace>& output)
{
    
    std::vector<ObjectTrace> objects;
    if(STATUS_SUCCESS != this->m_detector->executeObjectDetector(img,objects, 0.3))
        return STATUS_FAILED;
    else
    {
        // process tracking
        std::vector<TrackingTrace> tracks;
        this->m_tracker->process(objects, tracks);
        
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
                                        [theId] ( const ObjectTrace& a ) { return (a.track_id == theId);});
                if (p == this->m_listObject.end() )
                {
                    ObjectTrace obj;
                    obj.rect = track.m_rect;
                    obj.track_id = track.m_ID;
                    obj.label = track.m_type;
                    obj.position.push_back(cv::Point2f(track.m_rect.x + track.m_rect.width / 2, track.m_rect.y + track.m_rect.height /2));
                    // cout<< "id: " << track.m_ID ;
                    this->m_listObject.push_back(obj);
                }
                else{
                    int index = distance(this->m_listObject.begin(), p);
                    this->m_listObject[index].rect = track.m_rect;
                    this->m_listObject[index].isOutOfFrame = false;                   
                    if (this->m_listObject[index].position.size() > 10){
                        this->m_listObject[index].position.pop_front();
                        this->m_listObject[index].position.push_back(cv::Point2f(track.m_rect.x + track.m_rect.width / 2, track.m_rect.y + track.m_rect.height /2));
                    }else{
                        this->m_listObject[index].position.push_back(cv::Point2f(track.m_rect.x + track.m_rect.width / 2, track.m_rect.y + track.m_rect.height /2));
                    }
                    this->m_listObject[index].distance = calDistanceTrace(this->m_listObject[index]);
                        
                }
            }
            else
            {
                
                const int theId =  track.m_ID;
                // cout << "Out of frame, id: " << theId <<endl;
                const auto p = find_if(this->m_listObject.begin(), this->m_listObject.end(), 
                                        [theId] ( const ObjectTrace& a ) { return (a.track_id == theId);});                         
                if (p != this->m_listObject.end()) 
                {
                    int dist = distance(this->m_listObject.begin(), p);
                    this->m_listObject[dist].isOutOfFrame = true;
                }
                
            } 
        }

        for(auto p : this->m_listObject)
            if(p.isOutOfFrame != true) 
                output.push_back(p);    
                                     
    }
    
    return STATUS_SUCCESS;
}
double ObjTracking::calDistanceTrace(TrackingTrace track)
{
    int minSize = 3;
    float dis1, dis2;

    if(track.m_trace.size() < minSize) {
        dis1 = 0;
        dis2 = 0;
    } else {
        cv::Point2f lastTrace = track.m_trace[track.m_trace.size() - 1];
        cv::Point2f preTrace = track.m_trace[track.m_trace.size() - 3];
        cv::Point2f pre1Trace = track.m_trace[track.m_trace.size() - 2];

        dis1 = cv::norm(pre1Trace - preTrace);
        dis2 = cv::norm(lastTrace - pre1Trace);

    }
    return std::abs((dis1 + dis2)/2);
}
double ObjTracking::calDistanceTrace(ObjectTrace object)
{
    int minSize = 3;
    double dis1, dis2;

    if(object.position.size() < minSize) {
        dis1 = 0;
        dis2 = 0;
    } else {
        cv::Point2f lastTrace = object.position[object.position.size() -1];
        cv::Point2f preTrace = object.position[object.position.size() - 3];
        cv::Point2f pre1Trace = object.position[object.position.size() - 2];

        dis1 = cv::norm(pre1Trace - preTrace);
        dis2 = cv::norm(lastTrace - pre1Trace);

    }
    return std::abs((dis1 + dis2)/2);
}


