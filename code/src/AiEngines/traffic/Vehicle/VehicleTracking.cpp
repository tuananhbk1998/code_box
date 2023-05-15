#include "VehicleTracking.hpp"

VehicleTracking::VehicleTracking()
{
    this->detector = new ObjectDetector();
    this->tracker = new ObjectTracking();
    this->plateDetect = new ObjectDetector();
    this->listTracked.clear();
}

VehicleTracking::~VehicleTracking()
{
    if(this->detector) delete this->detector;
    if(this->tracker) delete this->tracker;
    if(this->plateDetect) delete this->plateDetect;
    // if(this->ocrVN) delete this->ocrVN;
    // if(this->ocrUS) delete this->ocrUS;
    // if(this->ocrMalay) delete this->ocrMalay;
}

int VehicleTracking::initVehicleTracking(settingVehicleTracking settings)
{
    if(this->detector->initObjectDetector("ncnn", "traffic", "CPU") != STATUS_SUCCESS)
        return STATUS_FAILED;

    if(settings.ObjectsTrack.size() == 0) {
        this->settings.TrackAll = true;
    } else {
        this->settings.ObjectsTrack = settings.ObjectsTrack;
    }

    if(settings.OptLicense) {
        this->settings.OptLicense = settings.OptLicense;
        this->settings.nation = settings.nation;

        if(this->plateDetect->initObjectDetector("ncnn", "AnprDetect", "CPU") != STATUS_SUCCESS)
            return STATUS_FAILED;

        // if(this->settings.nation == Nation::VN) {
        //     std::cout << "OCR model: " << DIR_OCR_DET_VN << ", " << DIR_OCR_RECOG_VN << endl;

        // } else if(this->settings.nation == Nation::US) {
        //     std::cout << "OCR model: " << DIR_OCR_DET_US << ", " << DIR_OCR_RECOG_US << endl;

        // } else if(this->settings.nation == Nation::MALAY) {
        //     std::cout << "OCR model: " << DIR_OCR_DET_MALAY << ", " << DIR_OCR_RECOG_MALAY << endl;

        // }

        if(initPaddleOCR() != STATUS_SUCCESS) {
            std::cout << "initPaddleOCR() failed!" << std::endl;
            return STATUS_FAILED;
        }
        std::cout << "[SUCCESS] - initPaddleOCR() successfully" << std::endl;

    } else {
        if(this->plateDetect) delete this->plateDetect;
    }
    
    return STATUS_SUCCESS;
}

// int VehicleTracking::setVehicleTracking(settingVehicleTracking settings)
// {
//     if(settings.ObjectsTrack.size() == 0) {
//         this->settings.TrackAll = true;
//     } else {
//         this->settings.ObjectsTrack = settings.ObjectsTrack;
//     }
    
//     this->settings.OptLicense = settings.OptLicense;
//     if(this->settings.OptLicense) {
        
//     }

//     return STATUS_SUCCESS;
// }

int VehicleTracking::updateVehicleTracking(cv::Mat &frame, std::vector<outputVehicleTracking> &output)
{
    try {
        cv::Mat Frame;
        frame.copyTo(Frame);

        output.clear();
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
                // cv::rectangle(Frame, track.m_rect, Scalar(255, 255, 255), 1, LINE_AA);
                // std::string text{std::to_string(track.m_ID) + " - " + track.m_type};
			    // cv::putText(Frame, text, cv::Point(track.m_rect.x, track.m_rect.y), FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);

                if(this->settings.TrackAll) {
                    int theId = track.m_ID;
                    const auto p = find_if(this->listTracked.begin(), this->listTracked.end(), 
                                            [theId] (const outputVehicleTracking &a) {return (a.track_id == theId);});

                    if(p != this->listTracked.end()) {
                        int index = distance(this->listTracked.begin(), p);

                        this->listTracked[index].rect = track.m_rect;
                        this->listTracked[index].isOutOfFrame = false;
                    } else {
                        outputVehicleTracking obj;
                        obj.track_id = track.m_ID;
                        obj.label = track.m_type;
                        obj.rect = track.m_rect;
                        this->listTracked.push_back(obj);
                    }
                }
                else if(find(this->settings.ObjectsTrack.begin(), this->settings.ObjectsTrack.end(), track.m_type) != this->settings.ObjectsTrack.end())
                {
                    int theId = track.m_ID;
                    const auto p = find_if(this->listTracked.begin(), this->listTracked.end(), 
                                            [theId] (const outputVehicleTracking &a) {return (a.track_id == theId);});

                    if(p != this->listTracked.end()) {
                        int index = distance(this->listTracked.begin(), p);

                        this->listTracked[index].rect = track.m_rect;
                        this->listTracked[index].isOutOfFrame = false;

                        if(this->settings.OptLicense) {
                            if(!this->listTracked[index].havePlate || this->listTracked[index].license == "Unknown") {
                                this->listTracked[index].imgVehicle = Frame(this->listTracked[index].rect).clone();
                                std::vector<ObjectTrace> plate_detected;
                                if(this->plateDetect->executeObjectDetector(this->listTracked[index].imgVehicle, plate_detected, 0.5) != STATUS_SUCCESS)
                                {
                                    LOG_FAIL("Execute Anpr detector failed");
                                    return STATUS_FAILED;
                                }
                                if(plate_detected.size() == 1) {
                                    this->listTracked[index].imgPlate = getPlate(this->listTracked[index].imgVehicle, plate_detected[0]);
                                    if(isValidPlate(this->listTracked[index].imgPlate)) {
                                        this->listTracked[index].havePlate = true;
                                        this->listTracked[index].rectPlate = plate_detected[0].rect;
                                        this->listTracked[index].typePlate = plate_detected[0].label;
                                        std::cout << this->listTracked[index].typePlate << std::endl;
                                        // this->listTracked[index].license = runPaddleOCR(this->listTracked[index].imgPlate);

                                        // if(this->listTracked[index].typePlate == "VnRect" || this->listTracked[index].typePlate == "VN_rectangle") {
                                        //     this->listTracked[index].license = "pVnRect";
                                        // } else if(this->listTracked[index].typePlate == "VnSquare" || this->listTracked[index].typePlate == "VN_square") {
                                        //     this->listTracked[index].license = "pVnSquare";
                                        // } else if(this->listTracked[index].typePlate == "US") {
                                        //     this->listTracked[index].license = "pUS";
                                        // } else if(this->listTracked[index].typePlate == "Malay") {
                                        //     this->listTracked[index].license = "pMalay";
                                        // }

                                        // cv::Rect r(obj.rectPlate.x + obj.rect.x, obj.rectPlate.y + obj.rect.y, obj.rectPlate.width, obj.rectPlate.height);
                                        // cv::rectangle(Frame, r, Scalar(255, 255, 255), 1, LINE_AA);
                                        // cv::imwrite("../outputs/plate_" + std::to_string(this->listTracked[index].track_id) + ".jpg", this->listTracked[index].imgPlate);
                                    } else {
                                        this->listTracked[index].license = "Unknown";
                                        this->listTracked[index].scorePlate = 0.0;
                                    }
                                }
                            }
                        }

                    } else {
                        outputVehicleTracking obj;
                        obj.track_id = track.m_ID;
                        obj.label = track.m_type;
                        obj.rect = track.m_rect;

                        if(this->settings.OptLicense) {
                            obj.imgVehicle = Frame(obj.rect).clone();
                            std::vector<ObjectTrace> plate_detected;
                            if(this->plateDetect->executeObjectDetector(obj.imgVehicle, plate_detected, 0.5) != STATUS_SUCCESS)
                            {
                                LOG_FAIL("Execute Anpr detector failed");
                                return STATUS_FAILED;
                            }
                            if(plate_detected.size() == 1) {
                                obj.imgPlate = getPlate(obj.imgVehicle, plate_detected[0]);
                                if(isValidPlate(obj.imgPlate)) {
                                    obj.havePlate = true;
                                    obj.rectPlate = plate_detected[0].rect;
                                    obj.typePlate = plate_detected[0].label;
                                    std::cout << "obj.typePlate = " << obj.typePlate << std::endl;
                                    // obj.license = runPaddleOCR(obj.imgPlate);

                                    // if(obj.typePlate == "VnRect" || obj.typePlate == "VN_rectangle") {
                                    //     obj.license = "pVnRect";
                                    // } else if(obj.typePlate == "VnSquare" || obj.typePlate == "VN_square") {
                                    //     obj.license = "pVnSquare";
                                    // } else if(obj.typePlate == "US") {
                                    //     obj.license = "pUS";
                                    // } else if(obj.typePlate == "Malay") {
                                    //     obj.license = "pMalay";
                                    // }

                                    // cv::Rect r(obj.rectPlate.x + obj.rect.x, obj.rectPlate.y + obj.rect.y, obj.rectPlate.width, obj.rectPlate.height);
                                    // cv::rectangle(Frame, r, Scalar(255, 255, 255), 1, LINE_AA);
                                    // cv::imwrite("../outputs/plate_" + std::to_string(obj.track_id) + ".jpg", obj.imgPlate);
                                } else {
                                    obj.license = "Unknown";
                                    obj.scorePlate = 0.0;
                                }
                            }
                        }

                        this->listTracked.push_back(obj);
                    }
                }

            } else {
                const int theId = track.m_ID;
                const auto p = find_if(this->listTracked.begin(), this->listTracked.end(), 
                                        [theId] (const outputVehicleTracking &a) {return (a.track_id == theId);});

                if(p != this->listTracked.end()) {
                    int dist = distance(this->listTracked.begin(), p);
                    this->listTracked[dist].isOutOfFrame = true;
                }
            }
        }

        for(auto out : this->listTracked)
            if(!out.isOutOfFrame)
                output.push_back(out);

        frame = Frame;

    } catch(exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return STATUS_SUCCESS;
}

cv::Mat VehicleTracking::getPlate(cv::Mat& imgVehicle, ObjectTrace& plate)
{
    plate.rect.x -= plate.rect.width * 0.1;
    plate.rect.x = plate.rect.x >= 0 ? plate.rect.x : 0;

    plate.rect.y -= plate.rect.height * 0.1;
    plate.rect.y = plate.rect.y >= 0 ? plate.rect.y : 0;

    plate.rect.width += plate.rect.width * 0.3;
    plate.rect.width = (plate.rect.x + plate.rect.width) > imgVehicle.cols ? (imgVehicle.cols - plate.rect.x) : plate.rect.width;

    plate.rect.height += plate.rect.height * 0.3;
    plate.rect.height = (plate.rect.y + plate.rect.height) > imgVehicle.rows ? (imgVehicle.rows - plate.rect.y) : plate.rect.height;

    cv::Mat imgPlate = imgVehicle(plate.rect).clone();
    std::cout << imgPlate.size() << std::endl;
    return imgPlate;
}

bool VehicleTracking::isValidPlate(cv::Mat& imgPlate)
{
    if(imgPlate.rows * imgPlate.cols >= 100) 
        return true;
    return false;
}

int VehicleTracking::initPaddleOCR()
{
    PaddleOCR::OCRConfig config(CONFIG_FILE);
    config.PrintConfigInfo();
    this->m_det = new PaddleOCR::DBDetector(config.det_model_dir, config.use_gpu, config.gpu_id,
                config.gpu_mem, config.cpu_math_library_num_threads,
                config.use_mkldnn, config.max_side_len, config.det_db_thresh,
                config.det_db_box_thresh, config.det_db_unclip_ratio,
                config.use_polygon_score, config.visualize,
                config.use_tensorrt, config.use_fp16);
    
    this->m_cls = nullptr;
    if (config.use_angle_cls == true) {
        this->m_cls = new PaddleOCR::Classifier(config.cls_model_dir, config.use_gpu, config.gpu_id,
                            config.gpu_mem, config.cpu_math_library_num_threads,
                            config.use_mkldnn, config.cls_thresh,
                            config.use_tensorrt, config.use_fp16);
    }

    this->m_rec = new PaddleOCR::CRNNRecognizer(config.rec_model_dir, config.use_gpu, config.gpu_id,
                        config.gpu_mem, config.cpu_math_library_num_threads,
                        config.use_mkldnn, config.char_list_file,
                        config.use_tensorrt, config.use_fp16);

    return STATUS_SUCCESS;
}

std::string VehicleTracking::runPaddleOCR(cv::Mat& imgPlate)
{
    std::string text;
    std::vector<std::vector<std::vector<int>>> boxes;

    this->m_det->Run(imgPlate, boxes);

    text = this->m_rec->Run(boxes, imgPlate, this->m_cls);

    return "text";
}