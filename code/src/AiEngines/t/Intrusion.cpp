/*
    Module: Intrusion
    Author: Le Manh Thang
    Created: Oct 04, 2021
*/


#include "Intrusion.hpp"

Intrusion::Intrusion()
{

}
// Intrusion::Intrusion(settingsIntrusion settings)
// {

//     this->m_detector = new ObjectDetector();
//     this->m_tracker = new ObjectTracking();
//     this->m_settings = settings;
    
//     // if(this->m_detector->initObjectDetector("ncnn", "traffic", "CPU") != STATUS_SUCCESS)
//     if(this->m_detector->initObjectDetector("snpe", "traffic", "DSP") != STATUS_SUCCESS)
//     {
//         LOG_FAIL("Init detector failed");            
//     }
// }

Intrusion::~Intrusion()
{
    if(this->m_detector != nullptr) delete this->m_detector;
    if(this->m_tracker != nullptr) delete this->m_tracker;
}

int Intrusion::init(settingsIntrusion settings)
{
    this->m_detector = new ObjectDetector();
    this->m_tracker = new ObjectTracking();
    this->m_settings = settings;
    if(this->m_detector->initObjectDetector("ncnn", "traffic", "CPU") != STATUS_SUCCESS)
    // if(this->m_detector->initObjectDetector("snpe", "people", "DSP") != STATUS_SUCCESS)
    // if(this->m_detector->initObjectDetector("snpe", "traffic", "DSP") != STATUS_SUCCESS)
    {
        LOG_FAIL("Init detector failed");
        return STATUS_FAILED;
    }
    return STATUS_SUCCESS;
}
int Intrusion::set(settingsIntrusion settings)
{
    this->m_settings = settings;
    return STATUS_SUCCESS;
}
int Intrusion::update(Mat& frame, vector<outDataIntrusion>& outData)
{
    try
    {
        Mat img;
        frame.copyTo(img);
        int widthFrame = img.cols;
        int heightFrame = img.rows;
        outData.clear();

        this->m_flIsNewOutSide = false;
        this->m_flIsNewAccessing = false;      
        this->m_flTypeEvent = -1;
        // draw region intrusion
        for( int i = 0; i < this->m_settings.arRegionsSet.size(); i++)
        {
            // std::cout << "Point: " << this->m_settings.arRegionsSet[i] << " " ;
            line( img, this->m_settings.arRegionsSet[i],
            this->m_settings.arRegionsSet[(i+1) % this->m_settings.arRegionsSet.size()], Scalar( 0, 0, 200), 2, LINE_AA);
        }
        std::vector<std::vector<cv::Point> > fillContAll;
        fillContAll.push_back(this->m_settings.arRegionsSet);
        cv::Mat layer = cv::Mat::zeros(img.size(), CV_8UC3);
        cv::fillPoly(layer, fillContAll, Scalar( 0, 0, 100));
        cv::addWeighted(img, 1, layer, 0.3, 0, img);

        
        // detect
        std::vector<ObjectTrace> detected;
		if(this->m_detector->executeObjectDetector(img, detected, THRES_DETECT_INTRUSION) != STATUS_SUCCESS)
        {
            LOG_FAIL("Execute Anpr detector failed");
            return STATUS_FAILED;
        }

        // std::cout << "SIZE: " << detected.size() << std::endl;
        // tracking
        vector<TrackingTrace> tracks;
        this->m_tracker->process(detected, tracks);

        
        //delete object which abandoned
        for(auto it = this->m_listTracked.begin(); it != this->m_listTracked.end();)
        {
            const int theId =  (*it).track_id;
            const auto p = find_if(tracks.begin(), tracks.end(), 
                                        [theId] ( const TrackingTrace& a ) { return (a.m_ID == theId);}); 
            if (p == tracks.end() && it != this->m_listTracked.end())
                it = this->m_listTracked.erase(it);                
            else 
                it++;
        }

        // check opposition
        for (auto &track: tracks) 
        {           
            if(!track.isOutOfFrame)
            {
                
                // if object is not in list of objects, then abort (skip)
                if(find(this->m_settings.arListObjects.begin(), this->m_settings.arListObjects.end(), track.m_type) 
                == this->m_settings.arListObjects.end())
                    continue;
                // std::cout << "ID: " << track.m_ID << endl;
                // // draw box
                // rectangle(img, track.m_rect, Scalar(255, 255, 255), 1, 8);
                // char text[100];
			    // sprintf(text,"%d:%s", (int)track.m_ID, track.m_type.c_str());
			    // cv::putText(img, text, cv::Point(track.m_rect.x, track.m_rect.y), FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1); 

                // find object in tracked list                 
                int theId = track.m_ID;
                const auto p = find_if(this->m_listTracked.begin(), this->m_listTracked.end(), [theId] (const outDataIntrusion& a) {return theId == a.track_id;});               
                // if found
                if(p != this->m_listTracked.end())
                {
                    int i = distance(this->m_listTracked.begin(), p);
                    this->m_listTracked[i].rect = track.m_rect;
                    // if object inside region
                    if(this->m_tracker->isInsideRegion(this->m_settings.arRegionsSet, 
                    cv::Point(track.m_rect.x + track.m_rect.width/2, 
                    track.m_rect.y + track.m_rect.height/2 )))
                    {
                        this->m_listTracked[i].rect = track.m_rect;
                        this->m_listTracked[i].cTimeCounter = getTickCount()/getTickFrequency() - this->m_listTracked[i].fTimeAccessed;
                    }
                    // check object has gone out of region 
                    else
                    {
                        this->m_listTracked.erase(p);
                        this->m_flIsNewOutSide = true;
                    }
                }
                //if not found => new object
                else
                {
                    // if object inside region
                    if(this->m_tracker->isInsideRegion(this->m_settings.arRegionsSet, 
                    cv::Point(track.m_rect.x + track.m_rect.width/2, 
                    track.m_rect.y + track.m_rect.height/2 )))
                    {
                        outDataIntrusion obj_in;
                        obj_in.track_id = track.m_ID;
                        obj_in.label = track.m_type;
                        obj_in.rect = track.m_rect;
                        obj_in.fTimeAccessed = getTickCount() / getTickFrequency();
                        this->m_listTracked.push_back(obj_in); // push to list in                    
                        this->m_flIsNewAccessing = true;
                    }                  
                }  
            }
            else
            {
                const int theId =  track.m_ID;
                const auto p = find_if(this->m_listTracked.begin(), this->m_listTracked.end(), 
                                        [theId] ( const outDataIntrusion& a ) { return (a.track_id == theId);});                         
                if (p != this->m_listTracked.end()) 
                {
                    int dist = distance(this->m_listTracked.begin(), p);
                    this->m_listTracked[dist].isOutOfFrame = true;
                }
            } 
        }

        // abort object out of frame 
        for(auto out : this->m_listTracked)
            if(!out.isOutOfFrame)
                outData.push_back(out);

        // check event
        if(outData.size() >= this->m_settings.cObjectCounter)
        {          
            this->m_cTimeCounter = getTickCount() / getTickFrequency() - this->m_fTimeStartCounter;

            if(this->m_cTimeCounter >= this->m_settings.cTimeOut)
            {
                if(this->m_flIsReadyEvent)
                {
                    this->m_flIsReadyEvent = false;
                    this->m_flFirstEvent = true;
                    this->m_flTypeEvent = 1;
                    return STATUS_SUCCESS;  
                }
                // // having new accessing
                else if(this->m_flIsNewAccessing)
                {              
                    this->m_flTypeEvent = 1;
                    return STATUS_SUCCESS;                   
                } 
                else if(this->m_flFirstEvent 
                && this->m_settings.cTimeRepeat > 0
                && (this->m_cTimeCounter - this->m_settings.cTimeOut) >= 1)
                {                
                    uint16_t cRepeatCounter = (this->m_cTimeCounter - this->m_settings.cTimeOut) / this->m_settings.cTimeRepeat;
                    if(this->m_cRepeatCounter < cRepeatCounter)
                    {
                        this->m_cRepeatCounter = cRepeatCounter;
                        if ((this->m_cTimeCounter - this->m_settings.cTimeOut) % this->m_settings.cTimeRepeat == 0)
                        {                                                      
                            this->m_flIsReadyEvent = false;
                            this->m_flTypeEvent = 0;
                            return STATUS_SUCCESS;             
                        } 
                    }          
                }                                 
            }                   
        }
        else
        {
            this->m_flIsReadyEvent = true;    
            this->m_flFirstEvent = false;  
            this->m_flTypeEvent = -1;  
            this->m_cRepeatCounter = 0;   
            this->m_fTimeStartCounter = getTickCount() / getTickFrequency();
            this->m_cTimeCounter = 0;           
        }

        frame = img;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    
    }
    return STATUS_SUCCESS;
}