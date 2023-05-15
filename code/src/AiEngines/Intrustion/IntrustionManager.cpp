
#include "IntrustionManager.hpp"




IntrustionManager::IntrustionManager()
{
    this->m_detector = new ObjectDetector();
    this->m_tracker = new ObjectTracking();

}

IntrustionManager::~IntrustionManager()
{
    if(this->m_detector) delete this->m_detector;
    if(this->m_tracker) delete this->m_tracker;
    this->m_listPerson.clear();
}
int IntrustionManager::initIntrustion()
{
    this->frequency = getTickFrequency();

    if(this->m_detector->initObjectDetector("snpe", "person", "CPU") != STATUS_SUCCESS)
        return STATUS_FAILED;

    return STATUS_SUCCESS;
}
bool IntrustionManager::isInside(TrackingTrace track, vector<cv::Point> area)
{
    if (area.size() == 0) return false;
    cv::Point2f cBox = cv::Point2f(track.m_rect.x + float(track.m_rect.width/2), 
                                    track.m_rect.y + float(track.m_rect.height*0.9));
    if (pointPolygonTest(area, cBox, false) >= 0) {
        return true;
    }
    return false;
}
int IntrustionManager::run(const cv::Mat& img, std::vector<PersonTrace>& output, vector<cv::Point> area)
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
        
        for(auto it = this->m_listPerson.begin(); it != this->m_listPerson.end();)
        {
            const int theId =  (*it).track_id;
            const auto p = find_if(tracks.begin(), tracks.end(), 
                                        [theId] ( const TrackingTrace& a ) { return (a.m_ID == theId);}); 
            
            if (p == tracks.end() && it != this->m_listPerson.end())
                it = this->m_listPerson.erase(it);                
            else 
                it++;
        }

        
        // process new track and update old track
        for(auto track : tracks)
        {           
            if(!track.isOutOfFrame)
            {
                if (isInside(track, area ))
                {
                     const int theId =  track.m_ID;
                    const auto p = find_if(this->m_listPerson.begin(), this->m_listPerson.end(), 
                                            [theId] ( const PersonTrace& a ) { return (a.track_id == theId);});
                    if (p == this->m_listPerson.end() )
                    {
                        PersonTrace person;
                        person.rect = track.m_rect;
                        person.track_id = track.m_ID;
                        // cout<< "id: " << track.m_ID ;
                        person.isNew = true;
                        std::cout << " New" << std::endl;
                        person.start = cv::getTickCount();
                        person.duration = (cv::getTickCount() - person.start) / this->frequency;
                        this->m_listPerson.push_back(person);
                    }
                    else{
                        int index = distance(this->m_listPerson.begin(), p);
                        this->m_listPerson[index].rect = track.m_rect;
                        this->m_listPerson[index].isOutOfFrame = false;
                        this->m_listPerson[index].isNew = false;
                        // cout<< "id: " << track.m_ID ;
                        this->m_listPerson[index].duration = (cv::getTickCount() - this->m_listPerson[index].start) / this->frequency;
                        std::cout << " Old" << std::endl;
                    }
                    // output.push_back(vehicle);
                }
               
            }
            else
            {
                
                const int theId =  track.m_ID;
                // cout << "Out of frame, id: " << theId <<endl;
                const auto p = find_if(this->m_listPerson.begin(), this->m_listPerson.end(), 
                                        [theId] ( const PersonTrace& a ) { return (a.track_id == theId);});                         
                if (p != this->m_listPerson.end()) 
                {
                    int dist = distance(this->m_listPerson.begin(), p);
                    this->m_listPerson[dist].isOutOfFrame = true;
                }
                
            } 
        }
                // cout << "size: " << this->m_listPerson.size() <<endl;
        //get list of output plates
        for(auto p : this->m_listPerson)
            if(p.isOutOfFrame != true) 
                output.push_back(p);    
                                     
    }
    
    return STATUS_SUCCESS;
}

