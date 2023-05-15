#include "BlowRedLight.hpp"

BlowRedLight::BlowRedLight()
{
    this->detector = new ObjectDetector();
    this->tracker = new ObjectTracking();
    this->listTracked.clear();
}

BlowRedLight::~BlowRedLight()
{
    if(this->detector) delete this->detector;
    if(this->tracker) delete this->tracker;
}

int BlowRedLight::initBlowRedLight()
{
    if(this->detector->initObjectDetector("ncnn", "traffic", "CPU") != STATUS_SUCCESS)
    // if(this->detector->initObjectDetector("snpe", "traffic", "CPU") != STATUS_SUCCESS)
        return STATUS_FAILED;
    return STATUS_SUCCESS;
}

int BlowRedLight::setBlowRedLight(settingBlowRedLight settings)
{
    this->settings = settings;

    this->leftLight.clear();
    this->standardLight.clear();
    // this->isBeginLine = false;
    // this->isEndLine = false;
    this->isLeftLine = false;
    this->isRightLine = false;
    if((this->settings.regionSet.size() != 0) && (this->settings.labelSet.size() == this->settings.regionSet.size())) {
        for(int index=0; index<settings.regionSet.size(); index++) {
            if(this->settings.labelSet[index] == "0") {
                this->leftLight.push_back(this->settings.regionSet[index]);
            }
            else if(this->settings.labelSet[index] == "1") {
                this->standardLight.push_back(this->settings.regionSet[index]);
            }
            else if(this->settings.labelSet[index] == "2") {
                // this->isBeginLine = true;
                this->beginLine = this->settings.regionSet[index];
            }
            else if(this->settings.labelSet[index] == "3") {
                // this->isEndLine = true;
                this->endLine = this->settings.regionSet[index];
            }
            else if(this->settings.labelSet[index] == "4") {
                // this->isLeftLine = true;
                this->leftLine = this->settings.regionSet[index];
            }
            else if(this->settings.labelSet[index] == "5") {
                this->isRightLine = true;
                this->rightLine = this->settings.regionSet[index];
            }
            else if(this->settings.labelSet[index] == "6") {
                this->fullLane = this->settings.regionSet[index];
            }
            else if(this->settings.labelSet[index] == "7") {
                this->straightLane = this->settings.regionSet[index];
            }
            else if(this->settings.labelSet[index] == "8") {
                this->straight_leftLane = this->settings.regionSet[index];
            }
            else if(this->settings.labelSet[index] == "9") {
                this->straight_rightLane = this->settings.regionSet[index];
            }
            else if(this->settings.labelSet[index] == "10") {
                this->leftLane = this->settings.regionSet[index];
            }
            else if(this->settings.labelSet[index] == "11") {
                this->rightLane = this->settings.regionSet[index];
            }
        }
    } else {
        return STATUS_FAILED;
    }
    this->allow2TurnLeft = this->settings.allowLeft;
    this->allow2TurnRight = this->settings.allowRight;

    return STATUS_SUCCESS;
}

int BlowRedLight::updateBlowRedLight(cv::Mat &frame, std::vector<outputBlowRedLight> &output)
{
    try {
        cv::Mat Frame;
        frame.copyTo(Frame);
        int widthFrame = frame.cols;
        int heightFrame = frame.rows;

        output.clear();

        if((this->standardLight.size() == 0) || (this->beginLine.size() == 0) || (this->endLine.size() == 0) 
            || (this->leftLine.size() == 0) || (this->rightLine.size() == 0))
        {
            return STATUS_FAILED;
        }

        // this->standardSignal = getCurrentLightState(frame, this->standardLight);
        LightState signal = getCurrentLightState(frame, this->standardLight);
        switch(signal) {
            case LightState::RED:
                this->standardSignal = LightState::RED;
                break;
            case LightState::YELLOW:
                this->standardSignal = LightState::YELLOW;
                break;
            case LightState::GREEN:
                this->standardSignal = LightState::GREEN;
                break;
            default:
                break;
        } 
        // this->standardSignal = LightState::RED;
        // switch(this->standardSignal) {
        //     case 0:
        //         {
        //             cv::putText(frame, "R", this->standardLight[0][0], FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 1, 2);
        //             break;
        //         }
        //     case 1:
        //         {
        //             cv::putText(frame, "Y", this->standardLight[1][3], FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 255), 1, 2);
        //             break;
        //         }
        //     case 2:
        //         {
        //             cv::putText(frame, "G", this->standardLight[2][3], FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 0), 1, 2);
        //             break;
        //         }
        //     case 3:
        //         {
        //             cv::putText(frame, "OFF", this->standardLight[0][0], FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 1, 2);
        //             break;
        //         }
        //     default:
        //         {
        //             cv::putText(frame, "N/A", this->standardLight[0][3], FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1, 2);
        //             break;
        //         }
        // }
        line(frame, this->beginLine[0], this->beginLine[1], Scalar(0, 255, 0), 1, cv::LINE_AA);
        line(frame, this->endLine[0], this->endLine[1], Scalar(0, 0, 255), 1, cv::LINE_AA);
        line(frame, this->leftLine[0], this->leftLine[1], Scalar(0, 255, 0), 1, cv::LINE_AA);
        // line(frame, this->rightLine[0], this->rightLine[1], Scalar(0, 255, 0), 1, cv::LINE_AA);

        cv::Point2f beginLinePtB((float) this->beginLine[0].x/widthFrame, (float) this->beginLine[0].y/heightFrame);
        cv::Point2f beginLinePtE((float) this->beginLine[1].x/widthFrame, (float) this->beginLine[1].y/heightFrame);

        cv::Point2f endLinePtB((float) this->endLine[0].x/widthFrame, (float) this->endLine[0].y/heightFrame);
        cv::Point2f endLinePtE((float) this->endLine[1].x/widthFrame, (float) this->endLine[1].y/heightFrame);

        cv::Point2f leftLinePtB((float) this->leftLine[0].x/widthFrame, (float) this->leftLine[0].y/heightFrame);
        cv::Point2f leftLinePtE((float) this->leftLine[1].x/widthFrame, (float) this->leftLine[1].y/heightFrame);

        cv::Point2f rightLinePtB((float) this->rightLine[0].x/widthFrame, (float) this->rightLine[0].y/heightFrame);
        cv::Point2f rightLinePtE((float) this->rightLine[1].x/widthFrame, (float) this->rightLine[1].y/heightFrame);

        // line(frame, this->straightLane[0], this->straightLane[1], Scalar(255, 0, 0), 1, cv::LINE_AA);
        // line(frame, this->straightLane[1], this->straightLane[2], Scalar(255, 0, 0), 1, cv::LINE_AA);
        // line(frame, this->straightLane[2], this->straightLane[3], Scalar(255, 0, 0), 1, cv::LINE_AA);

        for(int i=0; i<this->straightLane.size(); i++) {
            line(Frame, this->straightLane[i], this->straightLane[(i + 1) % this->straightLane.size()], Scalar(0, 255, 0), 1, cv::LINE_AA);
        }

        // this->leftSignal = getCurrentLightState(frame, this->leftLight);
        LightState l_signal = getCurrentLightState(frame, this->standardLight);
        switch(l_signal) {
            case LightState::RED:
                this->leftSignal = LightState::RED;
                break;
            case LightState::YELLOW:
                this->leftSignal = LightState::YELLOW;
                break;
            case LightState::GREEN:
                this->leftSignal = LightState::GREEN;
                break;
            default:
                break;
        } 
        // this->leftSignal = LightState::UNDEFINED;

        // std::string status = getStatus(this->standardSignal, this->leftSignal);

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
                cv::rectangle(frame, track.m_rect, Scalar(255, 255, 255), 1, LINE_AA);
			    cv::putText(frame, std::to_string(track.m_ID), cv::Point(track.m_rect.x, track.m_rect.y), FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 0), 1);

                if(find(this->settings.allowedObjects.begin(), this->settings.allowedObjects.end(), track.m_type) != this->settings.allowedObjects.end())
                    continue;

                int theId = track.m_ID;
                const auto p = find_if(this->listTracked.begin(), this->listTracked.end(), 
                                        [theId] (const outputBlowRedLight &a) {return (a.track_id == theId);});
                if(p != this->listTracked.end()) {
                    int index = distance(this->listTracked.begin(), p);

                    this->listTracked[index].rect = track.m_rect;
                    this->listTracked[index].isOutOfFrame = false;

                    if(!this->tracker->checkIntersection(track, static_cast<float>(widthFrame), static_cast<float>(heightFrame), 
                                                            RoadLine(endLinePtB, endLinePtE, 0)))
                    {
                        if(this->standardSignal == LightState::RED || this->leftSignal == RED) {
                            cv::Mat outImg = Frame.clone();
                            cv::rectangle(outImg, this->listTracked[index].rect, cv::Scalar(0, 255, 255), 2, 8);
                            this->listTracked[index].imgEvent = outImg;
                        }
                    }

                    if(!this->listTracked[index].straight) {
                        if(this->tracker->checkIntersection(track, static_cast<float>(widthFrame), static_cast<float>(heightFrame), 
                                                            RoadLine(endLinePtB, endLinePtE, 0)))
                        {
                            if(this->standardSignal == LightState::RED || this->leftSignal == RED) {
                                this->listTracked[index].isBlowRedLight = true;
                                this->listTracked[index].typeError = TypeError::RedLight;
                                cv::rectangle(frame,  this->listTracked[index].rect, Scalar(0, 0, 255), 2, LINE_AA);
                            } else {
                                this->listTracked[index].isOverTheLine = true;
                                this->listTracked[index].typeError = TypeError::OverTheLine;
                                cv::rectangle(frame,  this->listTracked[index].rect, Scalar(0, 255, 255), 2, LINE_AA);
                            }
                            output.push_back(this->listTracked[index]);
                            this->listTracked.erase(this->listTracked.begin() + index);
                        }
                    }

                    if(!this->listTracked[index].left) {
                        if(this->tracker->checkIntersection(track, static_cast<float>(widthFrame), static_cast<float>(heightFrame), 
                                                            RoadLine(leftLinePtB, leftLinePtE, 0)))
                        {
                            if(this->standardSignal == LightState::RED || this->leftSignal == RED) {
                                this->listTracked[index].isBlowRedLight = true;
                                this->listTracked[index].typeError = TypeError::RedLight;
                                cv::rectangle(frame,  this->listTracked[index].rect, Scalar(0, 0, 255), 2, LINE_AA);
                            } else {
                                this->listTracked[index].isOverTheLine = true;
                                this->listTracked[index].typeError = TypeError::OverTheLine;
                                cv::rectangle(frame,  this->listTracked[index].rect, Scalar(0, 255, 255), 2, LINE_AA);
                            }
                            output.push_back(this->listTracked[index]);
                            this->listTracked.erase(this->listTracked.begin() + index);
                        }
                    }

                    if(!this->listTracked[index].right) {
                        if(this->tracker->checkIntersection(track, static_cast<float>(widthFrame), static_cast<float>(heightFrame), 
                                                            RoadLine(rightLinePtB, rightLinePtE, 0)))
                        {
                            if(this->standardSignal == LightState::RED || this->leftSignal == RED) {
                                this->listTracked[index].isBlowRedLight = true;
                                this->listTracked[index].typeError = TypeError::RedLight;
                                cv::rectangle(frame,  this->listTracked[index].rect, Scalar(0, 0, 255), 2, LINE_AA);
                            } else {
                                this->listTracked[index].isOverTheLine = true;
                                this->listTracked[index].typeError = TypeError::OverTheLine;
                                cv::rectangle(frame,  this->listTracked[index].rect, Scalar(0, 255, 255), 2, LINE_AA);
                            }
                            output.push_back(this->listTracked[index]);
                            this->listTracked.erase(this->listTracked.begin() + index);
                        }
                    }
                } else {
                    outputBlowRedLight obj;
                    obj.track_id = track.m_ID;
                    obj.label = track.m_type;
                    obj.rect = track.m_rect;

                    if(this->tracker->checkIntersection(track, static_cast<float>(widthFrame), static_cast<float>(heightFrame),
                                                        RoadLine(beginLinePtB, beginLinePtE, 0)))
                    {
                        if(this->standardSignal == LightState::RED || 
                            checkLane(track, this->leftLane) || 
                            checkLane(track, this->rightLane)) obj.straight = false;

                        if(this->leftSignal == LightState::RED || 
                            (!allow2turn(this->allow2TurnLeft, track.m_type) && this->standardSignal == LightState::RED) || 
                            checkLane(track, this->straightLane) || 
                            checkLane(track, this->straight_rightLane) || 
                            checkLane(track, this->rightLane)) obj.left = false;

                        if(this->leftSignal == LightState::RED || 
                            (!allow2turn(this->allow2TurnRight, track.m_type) && this->standardSignal == LightState::RED) || 
                            checkLane(track, this->straightLane) || 
                            checkLane(track, this->straight_leftLane) || 
                            checkLane(track, this->leftLane)) obj.right = false;

                        if(this->standardSignal == LightState::RED || this->leftSignal == LightState::RED) {
                            cv::Mat outImg = Frame.clone();
                            cv::rectangle(outImg, obj.rect, cv::Scalar(0, 255, 255), 2, 8);
                            obj.imgEvent = outImg;
                        }

                        // if(checkLane(track, this->fullLane) || checkLane(track, this->straightLane) || checkLane(track, this->straight_leftLane) || 
                        //     checkLane(track, this->straight_rightLane) || checkLane(track, this->leftLane) || checkLane(track, this->rightLane))
                            this->listTracked.push_back(obj);
                    }
                }
            } else {
                const int theId = track.m_ID;
                const auto p = find_if(this->listTracked.begin(), this->listTracked.end(), 
                                        [theId] (const outputBlowRedLight &a) {return (a.track_id == theId);});
                if(p != this->listTracked.end()) {
                    int dist = distance(this->listTracked.begin(), p);
                    this->listTracked[dist].isOutOfFrame = true;
                }
            }
        }

        Frame = frame;

    } catch(exception& e) {
        std::cerr << e.what() << '\n';
    }
    return STATUS_SUCCESS;
}

bool BlowRedLight::checkLane(TrackingTrace track, std::vector<cv::Point> area)
{
    if (area.size() == 0) return false;
    cv::Point2f cBox = cv::Point2f(track.m_rect.x + float(track.m_rect.width/2), 
                                    track.m_rect.y + float(track.m_rect.height/2));
    if (pointPolygonTest(area, cBox, false) >= 0) {
        return true;
    }
    return false;
}

bool BlowRedLight::allow2turn(int allowTurn, std::string type)
{
    switch (allowTurn)
    {
    case 0:
        return false;
        break;
    case 1:
        return true;
        break;
    case 2:
        if (type == "xemay" || type == "xedap") {
            return true;
        }
        else {
            return false;
        }
        break;
    default:
        return false;
        break;
    }
}

inline float isBrighting(const Mat &input, Rect light) {
    Rect small;
    small.x = light.x + light.width/4;
    small.y = light.y + light.height/4;
    small.width = light.width - light.width/2;
    small.height = light.height - light.height/2;
    Mat roiImg = input(small);
    if (roiImg.empty()) return -1;
    Mat grey;
    cvtColor(roiImg, grey, COLOR_BGR2GRAY);
    return mean(grey)[0];
}

LightState BlowRedLight::getCurrentLightState(const cv::Mat &input, std::vector<std::vector<cv::Point>> light)
{
    if (light.size() == 0) return LightState::UNDEFINED;

    Rect red, yellow, green;
    float a,b,c;
    if (light.size() == 3) {
        red = boundingRect(light[0]);
        yellow = boundingRect(light[1]);
        green = boundingRect(light[2]);
        a = isBrighting(input, red);
        b = isBrighting(input, yellow);
        c = isBrighting(input, green);
    }
    if (light.size() == 2) {
        red = boundingRect(light[0]);
        green = boundingRect(light[1]);
        a = isBrighting(input, red);
        b = isBrighting(input, yellow);
        c = -1;
    }
    if (light.size() == 1) {
        red = boundingRect(light[0]);
        a = isBrighting(input, red);
        b = c = -1;
    }

    if (a < 100 && b < 100 && c < 100) return LightState::OFF;
    if (a > b && a > c) return LightState::RED;
    if (b > a && b > c) return LightState::YELLOW;
    if (c > a && c > b) return LightState::GREEN;
    return LightState::UNDEFINED;
}

std::string BlowRedLight::getStatus(LightState standardSignal, LightState leftSignal)
{
    std::string a, b;
    switch (standardSignal) {
        case LightState::GREEN:
            a = "G";
            break;
        case LightState::YELLOW:
            a = "Y";
            break;
        case LightState::RED:
            a = "R";
            break;
        case LightState::OFF:
        case LightState::UNDEFINED:
            a = "U";
            break;
        default:
            break;
    }
    switch (leftSignal) {
        case LightState::GREEN:
            b = "G";
            break;
        case LightState::YELLOW:
            b = "Y";
            break;
        case LightState::RED:
            b = "R";
            break;
        case LightState::OFF:
        case LightState::UNDEFINED:
            b = "U";
            break;
        default:
            break;
    }
    return a + "," + b;
}
