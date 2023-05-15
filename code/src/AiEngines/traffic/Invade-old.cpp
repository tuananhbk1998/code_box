#include "Invade.hpp"

Invade::Invade()
{
    this->detector = new ObjectDetector();
    this->tracker = new ObjectTracking();
    this->listTracked.clear();
}

Invade::~Invade()
{
    if(this->detector) delete this->detector;
    if(this->tracker) delete this->tracker;
}

int Invade::initInvade()
{
    if(this->detector->initObjectDetector("ncnn", "traffic", "CPU") != STATUS_SUCCESS)
    // if(this->detector->initObjectDetector("snpe", "traffic", "CPU") != STATUS_SUCCESS)
        return STATUS_FAILED;

    return STATUS_SUCCESS;
}

int Invade::setInvade(settingInvade settings)
{
    this->settings = settings;

    this->regions = settings.regionSet;
    this->labels = settings.labelSet;
    this->rules = settings.invadeSet;
    this->vehicles = settings.vehiclesSet;

    return STATUS_SUCCESS;
}

int Invade::updateInvade(cv::Mat& frame, std::vector<outputInvade>& output)
{
    try {
        cv::Mat Frame;
        frame.copyTo(Frame);
        int widthFrame = frame.cols;
        int heightFrame = frame.rows;

        output.clear();

        for(int i=0; i<this->regions.size(); i++) {
            line(Frame, this->regions[i][0], this->regions[i][1], Scalar(0, 255, 0), 1, cv::LINE_AA);
            line(Frame, this->regions[i][1], this->regions[i][2], Scalar(0, 255, 0), 1, cv::LINE_AA);
            line(Frame, this->regions[i][2], this->regions[i][3], Scalar(0, 255, 0), 1, cv::LINE_AA);
            line(Frame, this->regions[i][3], this->regions[i][0], Scalar(0, 255, 0), 1, cv::LINE_AA);
        }

        std::vector<ObjectTrace> detected;
		if(this->detector->executeObjectDetector(Frame, detected, 0.5) != STATUS_SUCCESS)
        {
            LOG_FAIL("Execute Anpr detector failed");
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
			    cv::putText(Frame, std::to_string(track.m_ID), cv::Point(track.m_rect.x, track.m_rect.y), FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 0), 1);

                int theId = track.m_ID;
                const auto p = find_if(this->listTracked.begin(), this->listTracked.end(), 
                                    [theId] (const outputInvade &a) {return (a.track_id == theId);});
                if(p != this->listTracked.end()) {
                    int index = distance(this->listTracked.begin(), p);

                    this->listTracked[index].rect = track.m_rect;
                    this->listTracked[index].isOutOfFrame = false;

                    if(!this->listTracked[index].isEvent) {
                        int id = this->listTracked[index].idLane - 1;
                        if(find(this->vehicles[id].begin(), this->vehicles[id].end(), track.m_type) != this->vehicles[id].end()) {
                            if(!checkLane(track, this->regions[id])) {
                                for(int i=0; i<this->regions.size(); i++) {
                                    if(i == id) {
                                        continue;
                                    } else {
                                        if(checkLane(track, this->regions[i])) {
                                            std::vector<int> invade_current = {id + 1, i + 1};
                                            bool true_rules = false;
                                            for(int j=0; j<this->rules.size(); j++) {
                                                if(invade_current == this->rules[j]) {
                                                    true_rules = true;
                                                    // std::cout << true_rules << std::endl;
                                                }
                                            }
                                            if(!true_rules) {
                                                this->listTracked[index].typeError = ErrorCode::InvadeLaneError;
                                                this->listTracked[index].isEvent = true;
                                                output.push_back(this->listTracked[index]);
                                            }
                                        }
                                    }
                                }
                            }
                        } else if(this->vehicles[id][0] == "") {
                            if(!checkLane(track, this->regions[id])) {
                                for(int i=0; i<this->regions.size(); i++) {
                                    if(i == id) {
                                        continue;
                                    } else {
                                        if(checkLane(track, this->regions[i])) {
                                            std::vector<int> invade_current = {id + 1, i + 1};
                                            bool true_rules = false;
                                            for(int j=0; j<this->rules.size(); j++) {
                                                if(invade_current == this->rules[j]) {
                                                    true_rules = true;
                                                    // std::cout << true_rules << std::endl;
                                                }
                                            }
                                            if(!true_rules) {
                                                this->listTracked[index].typeError = ErrorCode::InvadeLaneError;
                                                this->listTracked[index].isEvent = true;
                                                output.push_back(this->listTracked[index]);
                                            }
                                        }
                                    }
                                }
                            }
                        } else {
                            this->listTracked[index].typeError = ErrorCode::WrongLaneError;
                            this->listTracked[index].isEvent = true;
                            output.push_back(this->listTracked[index]);
                        }
                    }


                } else {
                    for(int id=0; id<this->regions.size(); id++) {
                        if(checkLane(track, this->regions[id])) {
                            outputInvade obj;
                            obj.track_id = track.m_ID;
                            obj.label = track.m_type;
                            obj.rect = track.m_rect;
                            obj.idLane = id + 1;
                            obj.isEvent = false;
                            this->listTracked.push_back(obj);
                        }
                    }
                }

                // int theId = track.m_ID;
                // const auto p = find_if(this->listTracked.begin(), this->listTracked.end(), 
                //                         [theId] (const outputInvade &a) {return (a.track_id == theId);});
                // if(p != this->listTracked.end()) {
                //     int index = distance(this->listTracked.begin(), p);

                //     this->listTracked[index].rect = track.m_rect;
                //     this->listTracked[index].isOutOfFrame = false;

                    // for(int id = 0; id < this->regions.size(); id++) {
                        // int lineType = std::stoi(this->labels[id]);
                        // cv::Point p1 = {this->regions[id][0].x, this->regions[id][0].y};
                        // cv::Point q1 = {this->regions[id][1].x, this->regions[id][1].y}; 

                        // int x = this->listTracked[index].rect.x;
                        // int y = this->listTracked[index].rect.y;
                        // int w = this->listTracked[index].rect.width;
                        // int h = this->listTracked[index].rect.height;

                        // cv::Point pt = {x, y};     cv::Point qt = {x + w, y};
                        // cv::Point pb = {x, y + h}; cv::Point qb = {x + w, y + h};
                        // cv::Point pl = {x, y};     cv::Point ql = {x, y + h};
                        // cv::Point pr = {x + w, y}; cv::Point qr = {x + w, y + h};   

                        // if((doIntersect(p1, q1, pt, qt) && doIntersect(p1, q1, pb, qb)) || (doIntersect(p1, q1, pl, ql) && doIntersect(p1, q1, pr, qr)))
                        // {
                        //     int UP = 0, DOWN = 0, LEFT = 0, RIGHT = 0;
                        //     checkTraces(track.m_trace, UP, DOWN, LEFT, RIGHT);
                        //     switch(lineType)
                        //     {
                        //         case 0:
                        //             output.push_back(this->listTracked[index]);
                        //             break;
                        //         case 4:
                        //             if(UP > 0)
                        //                 output.push_back(this->listTracked[index]);
                        //             break;
                        //         case 3:
                        //             if(DOWN > 0)
                        //                 output.push_back(this->listTracked[index]);
                        //             break;
                        //         case 2:
                        //             if(LEFT > 0)
                        //                 output.push_back(this->listTracked[index]);
                        //             break;
                        //         case 1:
                        //             if(RIGHT > 0)
                        //                 output.push_back(this->listTracked[index]);
                        //             break;
                        //         default:
                        //             break;
                        //     }
                        // }
                    // }

                // } else {
                //     for(int id=0; id<this->regions.size(); id++) {
                //         if(checkLane(track, this->regions[id])) {
                //             outputInvade obj;
                //             obj.track_id = track.m_ID;
                //             obj.label = track.m_type;
                //             obj.rect = track.m_rect;
                //             this->listTracked.push_back(obj);
                //         }
                //     }
                // }
            } else {
                const int theId = track.m_ID;
                const auto p = find_if(this->listTracked.begin(), this->listTracked.end(), 
                                        [theId] (const outputInvade &a) {return (a.track_id == theId);});
                if(p != this->listTracked.end()) {
                    int dist = distance(this->listTracked.begin(), p);
                    this->listTracked[dist].isOutOfFrame = true;
                }
            }
        }

        frame = Frame;

    } catch(exception& e) {
        std::cerr << e.what() << '\n';
    }

    return STATUS_SUCCESS;
}

bool Invade::checkLane(TrackingTrace track, std::vector<cv::Point> area)
{
    if (area.size() == 0) return false;
    cv::Point2f cBox = cv::Point2f(track.m_rect.x + float(track.m_rect.width/2), 
                                    track.m_rect.y + float(track.m_rect.height/2));
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