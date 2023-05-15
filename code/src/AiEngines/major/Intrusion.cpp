#include "Intrusion.hpp"

Intrusion::Intrusion()
{
    this->detector = new ObjectDetector();
    this->tracker = new ObjectTracking();
    this->listTracked.clear();
}

Intrusion::~Intrusion()
{
    if(this->detector) delete this->detector;
    if(this->tracker) delete this->tracker;
}

int Intrusion::initIntrusion()
{
    this->frequency = getTickFrequency();

    if(this->detector->initObjectDetector("snpe", "people", "CPU") != STATUS_SUCCESS)
        return STATUS_FAILED;

    return STATUS_SUCCESS;
}

int Intrusion::setIntrusion(settingIntrusion settings)
{
    this->m_objects = settings.objectsTrack;
    this->m_region = settings.regionSet;
    this->m_allowedAmount = settings.allowed_amount;
    this->m_allowedTime = settings.allowed_time;
    this->m_repeatedTime = settings.repeated_time;

    return STATUS_SUCCESS;
}

/*

int Intrusion::updateIntrusion(cv::Mat &frame, std::vector<outputIntrusion> &output)
{
    try {
        cv::Mat Frame;
        frame.copyTo(Frame);

        output.clear();

        for(int i=0; i<this->m_region.size(); i++) {
            line(Frame, this->m_region[i], this->m_region[(i + 1) % this->m_region.size()], Scalar(0, 255, 0), 1, cv::LINE_AA);
        }

        std::vector<ObjectTrace> detected;
        if(this->detector->executeObjectDetector(Frame, detected, THRES_DETECT_INTRUSION) != STATUS_SUCCESS)
        {
            LOG_FAIL("Execute detector failed");
            return STATUS_FAILED;
        }
        if(detected.size() == 0)
            return STATUS_FAILED;

        std::vector<TrackingTrace> tracks;
        this->tracker->process(detected, tracks);

        for(auto it = this->listTracked.begin(); it != this->listTracked.end();)
        {
            const int theId =  (*it).track_id;
            const auto p = find_if(tracks.begin(), tracks.end(), 
                                        [theId] ( const TrackingTrace& a ) { return (a.m_ID == theId);}); 
            if (p == tracks.end() && it != this->listTracked.end())
                it = this->listTracked.erase(it);
            else 
                it++;
        }

        for(auto &track : tracks) {
            if(!track.isOutOfFrame) {
                cv::rectangle(Frame, track.m_rect, Scalar(255, 255, 255), 1, LINE_AA);
                std::string text{std::to_string(track.m_ID) + " - " + track.m_type};
			    cv::putText(Frame, text, cv::Point(track.m_rect.x, track.m_rect.y), FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);

                if(this->m_objects.size() != 0) {
                    if(find(this->m_objects.begin(), this->m_objects.end(), track.m_type) != this->m_objects.end()) {
                        int theId = track.m_ID;
                        const auto p = find_if(this->listTracked.begin(), this->listTracked.end(), 
                                                [theId] (const outputIntrusion &a) {return (a.track_id == theId);});

                        if(p != this->listTracked.end()) {
                            int index = distance(this->listTracked.begin(), p);

                            this->listTracked[index].rect = track.m_rect;
                            this->listTracked[index].isOutOfFrame = false;

                            if(isInside(track, this->m_region)) {
                                this->listTracked[index].timeIn = (cv::getTickCount() - this->listTracked[index].timeAccessed) / this->frequency;
                            } else {
                                this->listTracked.erase(this->listTracked.begin() + index);
                                this->m_objectCounter -= 1;
                                this->hasFirstWarning = false;
                            }

                        } else {
                            if(isInside(track, this->m_region)) {
                                outputIntrusion obj;
                                obj.track_id = track.m_ID;
                                obj.label = track.m_type;
                                obj.rect = track.m_rect;
                                obj.timeAccessed = cv::getTickCount();
                                this->listTracked.push_back(obj);

                                this->m_objectCounter += 1;
                                this->hasFirstWarning = false;
                            }
                        }
                    }
                } else {
                    int theId = track.m_ID;
                    const auto p = find_if(this->listTracked.begin(), this->listTracked.end(), 
                                            [theId] (const outputIntrusion &a) {return (a.track_id == theId);});

                    if(p != this->listTracked.end()) {
                        int index = distance(this->listTracked.begin(), p);

                        this->listTracked[index].rect = track.m_rect;
                        this->listTracked[index].isOutOfFrame = false;

                        if(isInside(track, this->m_region)) {
                            this->listTracked[index].timeIn = (cv::getTickCount() - this->listTracked[index].timeAccessed) / this->frequency;
                        } else {
                            this->listTracked.erase(this->listTracked.begin() + index);
                            this->m_objectCounter -= 1;
                            this->hasFirstWarning = false;
                        }

                    } else {
                        if(isInside(track, this->m_region)) {
                            outputIntrusion obj;
                            obj.track_id = track.m_ID;
                            obj.label = track.m_type;
                            obj.rect = track.m_rect;
                            obj.timeAccessed = cv::getTickCount();
                            this->listTracked.push_back(obj);

                            this->m_objectCounter += 1;
                            this->hasFirstWarning = false;
                        }
                    }
                }

            } else {
                const int theId = track.m_ID;
                const auto p = find_if(this->listTracked.begin(), this->listTracked.end(), 
                                        [theId] (const outputIntrusion &a) {return (a.track_id == theId);});

                if(p != this->listTracked.end()) {
                    int dist = distance(this->listTracked.begin(), p);
                    this->listTracked[dist].isOutOfFrame = true;

                    this->listTracked.erase(this->listTracked.begin() + dist);

                    if(this->m_objects.size() != 0) {
                        if(find(this->m_objects.begin(), this->m_objects.end(), track.m_type) != this->m_objects.end()) {
                            this->m_objectCounter -= 1;
                            this->hasFirstWarning = false;
                        }
                    } else {
                        this->m_objectCounter -= 1;
                        this->hasFirstWarning = false;
                    }
                }
            }
        }

        if(this->listTracked.size() > 0) {
            std::sort(this->listTracked.begin(), this->listTracked.end(), [](struct outputIntrusion &a, struct outputIntrusion &b)
                                                                            {
                                                                                return a.timeIn < b.timeIn;
                                                                            });

            this->m_timeCounter = this->listTracked[0].timeIn;
        } 

        for(auto out : this->listTracked)
            // if(!out.isOutOfFrame) {
                output.push_back(out);
            // }

        std::cout << "this->m_objectCounter = " << this->m_objectCounter << std::endl;
        std::cout << "this->m_timeCounter = " << this->m_timeCounter << std::endl;

        if(this->m_objectCounter >= this->m_limitedNumber) {          
            switch(this->typeWarning) {
                case TypeWarning::First:
                {
                    double repeatCounter = (this->m_timeCounter - this->m_timeFirstWarning) / this->m_repeatTime;
                    std::cout << "repeatCounter = " << repeatCounter << " | " << this->m_repeatCounter << std::endl;
                    if((this->m_repeatTime > 0) && ((repeatCounter - this->m_repeatCounter) >= 1)) {
                        this->typeWarning = TypeWarning::Repeat;
                        this->m_repeatCounter += 1;
                    } else {
                        this->typeWarning = TypeWarning::None;
                    }
                    break;
                }
                case TypeWarning::Repeat:
                {
                    double repeatCounter = (this->m_timeCounter - this->m_timeFirstWarning) / this->m_repeatTime;
                    if((this->m_repeatTime > 0) && ((repeatCounter - this->m_repeatCounter) >= 1)) {
                        this->typeWarning = TypeWarning::Repeat;
                        this->m_repeatCounter += 1;
                    } else {
                        this->typeWarning = TypeWarning::None;
                    }
                    break;
                }
                case TypeWarning::None:
                {
                    if(this->hasFirstWarning) {
                        double repeatCounter = (this->m_timeCounter - this->m_timeFirstWarning) / this->m_repeatTime;
                        std::cout << "repeatCounter = " << repeatCounter << " | " << this->m_repeatCounter << std::endl;
                        if((this->m_repeatTime > 0) && ((repeatCounter - this->m_repeatCounter) >= 1)) {
                            this->typeWarning = TypeWarning::Repeat;
                            this->m_repeatCounter += 1;
                        } else {
                            this->typeWarning = TypeWarning::None;
                        }
                    } else {
                        if(this->m_timeCounter >= this->m_limitedTime) {
                            this->typeWarning = TypeWarning::First;
                            this->hasFirstWarning = true;
                            this->m_timeFirstWarning = this->m_timeCounter;
                            this->m_repeatCounter = 0;
                        } else {
                            this->typeWarning = TypeWarning::None;
                        }
                    }
                    break;
                }
                default:
                    break;
            }

        } else {
            this->typeWarning = TypeWarning::None;
            this->hasFirstWarning = false;
            this->m_timeFirstWarning = 0;
            this->m_repeatCounter = 0;
        }

        frame = Frame;

    } catch(exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return STATUS_SUCCESS;
}

*/
int Intrusion::updateIntrusion(cv::Mat &frame, std::vector<outputIntrusion> &outputs)
{
    try {
        cv::Mat Frame;
        frame.copyTo(Frame);
        int width_frame = Frame.cols;
        int height_frame = Frame.rows;

        outputs.clear();

        cv::Mat crop = Frame(cv::Rect2d(Frame.cols / 2, Frame.rows / 2, 100, 100));

        std::vector<ObjectTrace> detected;
        // if(this->detector->executeObjectDetector(Frame, detected, THRES_DETECT_INTRUSION_DAY) != STATUS_SUCCESS)
        // {
        //     LOG_FAIL("Execute detector failed");
        //     return STATUS_FAILED;
        // }
        if (isGrayscale(crop)){
            // PRINT_LOG(TAG, "--------------NIGHT-----------");
            if(this->detector->executeObjectDetector(Frame, detected, THRES_DETECT_INTRUSION_NIGHT) != STATUS_SUCCESS)
            {
                LOG_FAIL("Execute detector failed");
                return STATUS_FAILED;
            }
        }else{
            // PRINT_LOG(TAG, "--------------DAY-----------");
            if(this->detector->executeObjectDetector(Frame, detected, THRES_DETECT_INTRUSION_DAY) != STATUS_SUCCESS)
            {
                LOG_FAIL("Execute detector failed");
                return STATUS_FAILED;
            }
        }
        std::cout << "Size out detect: " << detected.size() << std::endl;
    

        std::vector<TrackingTrace> tracks;
        this->tracker->process(detected, tracks);

        for(auto it = this->listTracked.begin(); it != this->listTracked.end();)
        {
            const int theId =  (*it).track_id;
            const auto p = find_if(tracks.begin(), tracks.end(), 
                                        [theId] ( const TrackingTrace& a ) { return (a.m_ID == theId);}); 
            if (p == tracks.end() && it != this->listTracked.end())
                it = this->listTracked.erase(it);
            else 
                it++;
        }

        for(auto &track : tracks) {
            if(!track.isOutOfFrame) {
                // cv::rectangle(Frame, track.m_rect, Scalar(255, 255, 255), 1, LINE_AA);
                // std::string text{std::to_string(track.m_ID) + " - " + track.m_type};
			    // cv::putText(Frame, text, cv::Point(track.m_rect.x, track.m_rect.y), FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);

                if(this->m_objects.size() != 0) {
                    if(find(this->m_objects.begin(), this->m_objects.end(), track.m_type) != this->m_objects.end()) {
                        int theId = track.m_ID;
                        const auto p = find_if(this->listTracked.begin(), this->listTracked.end(), 
                                                [theId] (const outputIntrusion &a) {return (a.track_id == theId);});

                        if(p != this->listTracked.end()) {
                            int index = distance(this->listTracked.begin(), p);

                            this->listTracked[index].rect = track.m_rect;
                            this->listTracked[index].isOutOfFrame = false;

                            if(isInside(track, this->m_region)) {
                                this->listTracked[index].timeIn = (cv::getTickCount() - this->listTracked[index].timeAccessed) / this->frequency;
                            } else {
                                this->listTracked.erase(this->listTracked.begin() + index);
                            }
                            if (this->listTracked[index].isNew == true){
                                this->listTracked[index].isNew = false;
                            }

                        } else {
                            if(isInside(track, this->m_region) && (isMove(track)) && (isPerson(track, width_frame, height_frame))) {
                            // if(isInside(track, this->m_region) ) {
                                outputIntrusion obj;
                                obj.track_id = track.m_ID;
                                obj.label = track.m_type;
                                obj.rect = track.m_rect;
                                obj.timeAccessed = cv::getTickCount();
                                obj.isNew = true;
                                this->listTracked.push_back(obj);
                            }
                        }
                    }
                } else {
                    int theId = track.m_ID;
                    const auto p = find_if(this->listTracked.begin(), this->listTracked.end(), 
                                            [theId] (const outputIntrusion &a) {return (a.track_id == theId);});

                    if(p != this->listTracked.end()) {
                        int index = distance(this->listTracked.begin(), p);

                        this->listTracked[index].rect = track.m_rect;
                        this->listTracked[index].isOutOfFrame = false;

                        if(isInside(track, this->m_region)) {
                            this->listTracked[index].timeIn = (cv::getTickCount() - this->listTracked[index].timeAccessed) / this->frequency;
                        } else {
                            this->listTracked.erase(this->listTracked.begin() + index);
                        }
                        if (this->listTracked[index].isNew == true){
                            this->listTracked[index].isNew = false;
                        }

                    } else {
                        if(isInside(track, this->m_region) && (isMove(track)) && (isPerson(track, width_frame, height_frame))) {
                        // if(isInside(track, this->m_region) ) {
                            outputIntrusion obj;
                            obj.track_id = track.m_ID;
                            obj.label = track.m_type;
                            obj.rect = track.m_rect;
                            obj.timeAccessed = cv::getTickCount();
                            obj.isNew = true;
                            this->listTracked.push_back(obj);
                        }
                    }
                }

            } else {
                const int theId = track.m_ID;
                const auto p = find_if(this->listTracked.begin(), this->listTracked.end(), 
                                        [theId] (const outputIntrusion &a) {return (a.track_id == theId);});

                if(p != this->listTracked.end()) {
                    int dist = distance(this->listTracked.begin(), p);
                    this->listTracked[dist].isOutOfFrame = true;
                }
            }
        }

        uint32_t countOutFrame = 0;

        for(auto out : this->listTracked) {
            if(!out.isOutOfFrame) {
                outputs.push_back(out);
                if (out.isNew){
                    this->typeWarning = TypeWarning::No;
                }
            } else {
                countOutFrame += 1;
            }
            
        }
        if(outputs.size() < this->m_allowedAmount) {
            if(countOutFrame > 0) {
                this->isWarning = false;
                return STATUS_SUCCESS;   
            } else {
                this->typeWarning = TypeWarning::No;
                this->isWarning = false;
                return STATUS_SUCCESS;
            }
        }

        std::sort(outputs.begin(), outputs.end(), [] (outputIntrusion a, outputIntrusion b) {return a.timeIn < b.timeIn;});

        this->isWarning = false;

        switch(this->typeWarning) {
            case TypeWarning::No:
            {
                if(outputs[0].timeIn >= this->m_allowedTime) {
                    this->m_timeWarning = cv::getTickCount();
                    this->isWarning = true;
                    this->typeWarning = TypeWarning::First;
                }
                break;
            }
            case TypeWarning::First:
            {
                if(this->m_repeatedTime > 0) {
                    double timeCounter = (cv::getTickCount() - this->m_timeWarning) / this->frequency;
                    if(timeCounter >= this->m_repeatedTime) {
                        this->m_timeWarning = cv::getTickCount();
                        this->isWarning = true;
                        this->typeWarning = TypeWarning::Repeat;
                    }
                }
                break;
            }
            case TypeWarning::Repeat:
            {
                double timeCounter = (cv::getTickCount() - this->m_timeWarning) / this->frequency;
                if(timeCounter >= this->m_repeatedTime) {
                    this->m_timeWarning = cv::getTickCount();
                    this->isWarning = true;
                    this->typeWarning = TypeWarning::Repeat;
                }
                break;
            }
        }

        frame = Frame;

    } catch(exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return STATUS_SUCCESS;
}

// bool Intrusion::isInside(TrackingTrace track, std::vector<cv::Point> area)
// {
//     if (area.size() == 0) return false;
//     cv::Point2f cBox = cv::Point2f(track.m_rect.x + float(track.m_rect.width/2), 
//                                     track.m_rect.y + float(track.m_rect.height*0.9));
//     if (pointPolygonTest(area, cBox, false) >= 0) {
//         return true;
//     }
//     return false;
// }

bool Intrusion::isInside(TrackingTrace track, std::vector<cv::Point> area)
{
    if (area.size() == 0) return false;
    cv::Point2f cBox = cv::Point2f(track.m_rect.x + float(track.m_rect.width/2), 
                                    track.m_rect.y + float(track.m_rect.height));
    if (pointPolygonTest(area, cBox, false) >= 0) {
        return true;
    }
    return false;
}
bool Intrusion::isMove(TrackingTrace track)
{
    int minSize = 3;
    double dis1 = 0;
    double dis2 = 0;
    if(track.m_trace.size() < minSize)
        return false;

    cv::Point2f lastTrace = track.m_trace[track.m_trace.size() - 1];
    cv::Point2f preTrace = track.m_trace[track.m_trace.size() - 3];
    cv::Point2f pre1Trace = track.m_trace[track.m_trace.size() - 2];

    dis1 = cv::norm(pre1Trace - preTrace);
    dis2 = cv::norm(lastTrace - pre1Trace);

    double res = std::abs((dis1 + dis2)/2);
    if(res > THRES_MOVE) {
        return true;
    }
    return false;
}
bool Intrusion::isObject(TrackingTrace track, int width, int height)
{
    if((track.m_rect.width > track.m_rect.height) || (track.m_rect.height > (height * 0.7)) || (track.m_rect.br().y > (height - 20)))
        return false;

    return true;
}

bool Intrusion::isPerson(TrackingTrace track, int width, int height)
{
    if(track.m_rect.br().y > (height - 20))
        return false;

    float p_width = track.m_rect.width / width;
    // if((p_width < 0.03) || (p_width > 0.15))
    if (p_width < 0.03) 
        return false;

    float p_height = track.m_rect.height / height;
    // if((p_height < 0.1) || (p_height > 0.45))
    if (p_height < 0.1) 
        return false;

    float areaObject = track.m_rect.height * track.m_rect.width;
    if (areaObject > (0.2 * width * height))
        return false;

    float p_width_height = track.m_rect.width / track.m_rect.height;
    if(p_width_height > 1.2)
        return false;
    

    return true;
}

bool Intrusion::isGrayscale(cv::Mat crop)
{
    bool grayscale = true;
    for(int i = 0; i < crop.rows; i++)
    {
        for(int j = 0; j < crop.cols; j++)
        {
            int rgb[3] = {crop.at<cv::Vec3b>(i,j)[0],crop.at<cv::Vec3b>(i,j)[1],crop.at<cv::Vec3b>(i,j)[2]}; //b, g, r

            for (int id = 0 ; id < 3;  ++id){
                int min_index = id;
                for (int k = id + 1;  k < 3; ++k){
                    if (rgb[k] < rgb[min_index])
                        min_index = k;
                }
                if (min_index != id){
                    int temp = rgb[id];
                    rgb[id] = rgb[min_index];
                    rgb[min_index] = temp;
                }
                
            }
            
            if (!((rgb[0] >= rgb[1] - 10) && (rgb[0] <= rgb[1] + 10) && (rgb[2] >= rgb[1] - 10) && (rgb[2] <= rgb[1] + 10))){
                // cout << rgb[0] << " " << rgb[1] << " " << rgb[2] << endl;
                grayscale = false;
                break;
            }
        }
        if(!grayscale)
            break;
    }
    return grayscale;
}