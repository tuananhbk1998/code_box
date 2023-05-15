#include "Children.hpp"
using namespace std;

Children::Children() {
    m_ObjectDetector = new ObjectDetector();
    m_ChildDetector = new ObjectDetector();
}

Children::~Children() {
    if (m_ChildDetector != nullptr) {
        delete m_ChildDetector;
    }

    if (m_ObjectDetector != nullptr) {
        delete m_ObjectDetector;
    }
}

int Children::Child_Init() {
    if (m_ObjectDetector->initObjectDetector("snpe", "anhmtc", "CPU")) {

        return -1;
    }

    return 0;
}

int Children::Child_Set(stSettingChild_t stSetting) {
    // this->m_Region = stSetting.ptRegionSet;
    this->Region_Check1 = stSetting.SetRegion_1;
    this->Region_Check2 = stSetting.SetRegion_2;
    this->Region_Check3 = stSetting.SetRegion_3;
    this->Region_Check4 = stSetting.SetRegion_4;
    this->Region_Check5 = stSetting.SetRegion_5;
    this->Region_Check6 = stSetting.SetRegion_6;
    this->Region_Check7 = stSetting.SetRegion_7;
    this->Region_Check8 = stSetting.SetRegion_8;
    this->Region_Check9 = stSetting.SetRegion_9;
    this->Region_Check10 = stSetting.SetRegion_10;
    this->Region_Check11 = stSetting.SetRegion_11;
    this->Region_Check12 = stSetting.SetRegion_12;
    this->Region_Check13 = stSetting.SetRegion_13;
    this->Region_Check14 = stSetting.SetRegion_14;
    this->Region_Check15 = stSetting.SetRegion_15;
    this->Region_Check16 = stSetting.SetRegion_16;
    this->Region_Check17 = stSetting.SetRegion_17;
    this->Region_Check18 = stSetting.SetRegion_18;
    this->Region_Check19 = stSetting.SetRegion_19;
    this->Region_Check20 = stSetting.SetRegion_20;
    this->Region_Check21 = stSetting.SetRegion_21;

    return 0;
}


bool Children::CheckInside_1(cv::Rect &contour, std::vector<cv::Point> Area_1) {
    if (Area_1.size() == 0) return false;
    cv::Point2f cBox_1 = cv::Point2f(contour.x + float(contour.width/2), 
                                    contour.y + float(contour.height));
    if (pointPolygonTest(Area_1, cBox_1, false) >= 0) {
        return true;
    }
    return false;
}

bool Children::CheckInside_2(cv::Rect &contour, std::vector<cv::Point> Area_2) {
    if (Area_2.size() == 0) return false;
    cv::Point2f cBox_2 = cv::Point2f(contour.x + float(contour.width/2),
                                            contour.y + float(contour.height));
    if (pointPolygonTest(Area_2, cBox_2, false) >= 0) {
        return true;
    }
    return false;
}

bool Children::CheckInside_3(cv::Rect &contour, std::vector<cv::Point> Area_3) {
    if (Area_3.size() == 0) return false;
    cv::Point2f cBox_3 = cv::Point2f(contour.x + float(contour.width/2),
                                            contour.y + float(contour.height));
    if (pointPolygonTest(Area_3, cBox_3, false) >= 0) {
        return true;
    }
    return false;
}



int Children::Child_Execute(cv::Mat &Image, std::vector<OutputChild> &Child) {
    // getMotion(Image, preImage, contours);
    // for (int j = 0 ; j < contours.size() ; j++) {
    //     cv::Rect box = contours[j];
    //     for (auto contour1 : contours) {
    //         if (CheckMotionInside1(contour1, Region_CheckMotion1) && ( (box.x > 202 && box.x + box.width < 756) && (box.y > 47 && (box.y < 300 && (box.y + box.height) > 300) )) ) {
    //             std::vector<ObjectTrace> objects;
    //             m_ObjectDetector->executeObjectDetector(Image, objects, 0.5);
    //             for (auto object : objects) {
    //                 if (CheckInside(object, m_Region)) {
    //                     std::cout << "Objects size: " << objects.size() << std::endl;
    //                     cv::rectangle(Image, object.rect, cv::Scalar(0, 255, 255), 2);
    //                     std::cout << "Adult" << std::endl;
    //                 }
    //             }
    //         } else if (CheckMotionInside1(contour1, Region_CheckMotion1) && ( (box.x > 202 && box.x + box.width < 756) && (box.y > 300) )) {
    //             std::vector<ObjectTrace> objects2;
    //             m_ObjectDetector->executeObjectDetector(Image, objects2, 0.5);
    //             for (int k = 0 ; k < objects2.size(); k++) {
    //                 cv::Rect2f box_children = objects2[k].rect;
    //                 if ( (box_children.x > 202 && box_children.x + box_children.width < 756) && (box_children.y > 300) ) {
    //                     std::cout << "Children" << std::endl;
    //                     cv::rectangle(Image, box_children, cv::Scalar(255, 0, 0), 2);
    //                 }
    //             }
    //             // for (auto object2 : objects2) {
    //             //     if (CheckInside(object2, m_Region)) {
    //             //         std::cout << "Objects size: " << objects2.size() << std::endl;
    //             //         cv::rectangle(Image, object2.rect, cv::Scalar(255, 0, 0), 2);
    //             //         std::cout << "Children" << std::endl;
    //             //     }
    //             // }
    //         }
    //     }
    // }


/*

    std::vector<ObjectTrace> objects;
    m_ObjectDetector->executeObjectDetector(Image, objects, 0.7);
    std::cout << "Object size: " << objects.size() << std::endl;
    for (int j = 0 ; j < objects.size() ; j++) {
        cv::Rect box = objects[j].rect;
        if ( CheckInside_1(box, this->Region_Check1) ) {
            double res1 = cv::norm( (box.y + box.height) - box.y );
            cv::Point a(0, 0);
            cv::Point b(0, 262);
            double res2 = cv::norm( b.y - a.y );
            if ( res1 > 0.7*res2 && (box.y > 262) ) {
                std::cout << "Adult" << std::endl;
                cv::putText(Image, "ADULT", cv::Point(box.x, box.y), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 255), 2, false);
                cv::rectangle(Image, box, cv::Scalar(0, 0, 255), 2);
            } else if ( (res1 <= 0.45*res2) && ((box.x > 0 && box.x + box.width < 1920) && (box.y > 0 && box.y + box.height < 262))) {
                std::cout << "Children" << std::endl;
                cv::putText(Image, "CHILDREN", cv::Point(box.x, box.y), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 255, 0), 2, false);
                cv::rectangle(Image, box, cv::Scalar(0, 255, 0), 2);
            }
        } else if (CheckInside_2(box, this->Region_Check2) ) {
            double res3 = cv::norm( (box.y + box.height) - box.y );
            cv::Point c(0, 262);
            cv::Point d(0, 600);
            double res4 = cv::norm( d.y - c.y );
            if ( res3 > 0.9*res4 && ((box.y < 262) || (box.y > 600)) ) {
                std::cout << "Adult" << std::endl;
                cv::putText(Image, "ADULT", cv::Point(box.x, box.y), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 255), 2, false);
                cv::rectangle(Image, box, cv::Scalar(0, 0, 255), 2);
            } else if ( res3 <= 0.8*res4 && ( (box.x > 0 && box.x + box.width < 1920) && (box.y > 262 && box.y + box.height < 600) )) {
                std::cout << "Children" << std::endl;
                cv::putText(Image, "CHILDREN", cv::Point(box.x, box.y), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 255, 0), 2, false);
                cv::rectangle(Image, box, cv::Scalar(0, 255, 0), 2);
            }
        } else if (CheckInside_3(box, this->Region_Check3)) {
            double res5 = cv::norm( (box.y + box.height) - box.y );
            cv::Point e(0, 600);
            cv::Point f(0, 1080);
            double res6 = cv::norm( f.y - e.y );
            if ( res5 > 0.7*res6 && (box.y < 600) ) {
                std::cout << "Adult" << std::endl;
                cv::putText(Image, "ADULT", cv::Point(box.x, box.y), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 255), 2, false);
                cv::rectangle(Image, box, cv::Scalar(0, 0, 255), 2);
            } else if ( res5 <= 0.6*res6 && ( (box.x > 0 && box.x + box.width < 1920) && (box.y > 600 && box.y + box.height < 1080) ) ) {
                std::cout << "Children" << std::endl;
                cv::putText(Image, "CHILDREN", cv::Point(box.x, box.y), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 255, 0), 2, false);
                cv::rectangle(Image, box, cv::Scalar(0, 255, 0), 2);
            }
        }
    }
    */
            
     std::vector<ObjectTrace> objects;
    m_ObjectDetector->executeObjectDetector(Image, objects, 0.7);
    std::cout << "Object size: " << objects.size() << std::endl;
    for (int j = 0 ; j < objects.size() ; j++) {
        cv::Rect box = objects[j].rect;
        if ( CheckInside_1(box, this->Region_Check1) || CheckInside_1(box, this->Region_Check4) || CheckInside_1(box, this->Region_Check7) || CheckInside_1(box, this->Region_Check10) || CheckInside_1(box, this->Region_Check13) || CheckInside_1(box, this->Region_Check16) || CheckInside_1(box, this->Region_Check19)) {
            double res1 = cv::norm( (box.y + box.height) - box.y );
            cv::Point a(0, 0);
            cv::Point b(0, 262);
            double res2 = cv::norm( b.y - a.y );
            if ( (res1 > 0.7*res2 && (box.y > 262)) ) {
                std::cout << "Adult" << std::endl;
                cv::putText(Image, "ADULT", cv::Point(box.x, box.y), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 255), 2, false);
                cv::rectangle(Image, box, cv::Scalar(0, 0, 255), 2);
            } else if ( (res1 <= 0.45*res2) && ((box.x > 0 && box.x + box.width < 274) || (box.x > 274 && box.x + box.width < 548) || (box.x > 548 && box.x + box.width < 822) || (box.x > 822 && box.x + box.width < 1096) || (box.x > 1096 && box.x + box.width < 1370) || (box.x > 1370 && box.x + box.width < 1644) || (box.x > 1644 && box.x + box.width < 1920)) && (box.y > 0 && box.y + box.height < 262) ) {
                std::cout << "Children" << std::endl;
                cv::putText(Image, "CHILDREN", cv::Point(box.x, box.y), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 255, 0), 2, false);
                cv::rectangle(Image, box, cv::Scalar(0, 255, 0), 2);
            }
        } else if (CheckInside_2(box, this->Region_Check2) || CheckInside_2(box, this->Region_Check5) || CheckInside_2(box, this->Region_Check8) || CheckInside_2(box, this->Region_Check11) || CheckInside_2(box, this->Region_Check14) || CheckInside_2(box, this->Region_Check17) || CheckInside_2(box, this->Region_Check20)) {
            double res3 = cv::norm( (box.y + box.height) - box.y );
            cv::Point c(0, 262);
            cv::Point d(0, 600);
            double res4 = cv::norm( d.y - c.y );
            if ( res3 > 0.9*res4 && ((box.y < 262) || (box.y > 600)) ) {
                std::cout << "Adult" << std::endl;
                cv::putText(Image, "ADULT", cv::Point(box.x, box.y), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 255), 2, false);
                cv::rectangle(Image, box, cv::Scalar(0, 0, 255), 2);
            } else if ( res3 <= 0.8*res4 && ((box.x > 0 && box.x + box.width < 274) || (box.x > 274 && box.x + box.width < 548) || (box.x > 548 && box.x + box.width < 822) || (box.x > 822 && box.x + box.width < 1096) || (box.x > 1096 && box.x + box.width < 1370) || (box.x > 1370 && box.x + box.width < 1644) || (box.x > 1644 && box.x + box.width < 1920)) && (box.y > 262 && box.y + box.height < 600)) {
                std::cout << "Children" << std::endl;
                cv::putText(Image, "CHILDREN", cv::Point(box.x, box.y), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 255, 0), 2, false);
                cv::rectangle(Image, box, cv::Scalar(0, 255, 0), 2);
            }
        } else if (CheckInside_3(box, this->Region_Check3) || CheckInside_3(box, this->Region_Check6) || CheckInside_3(box, this->Region_Check9) || CheckInside_3(box, this->Region_Check12) || CheckInside_3(box, this->Region_Check15) || CheckInside_3(box, this->Region_Check18) || CheckInside_3(box, this->Region_Check21)) {
            double res5 = cv::norm( (box.y + box.height) - box.y );
            cv::Point e(0, 600);
            cv::Point f(0, 1080);
            double res6 = cv::norm( f.y - e.y );
            if ( res5 > 0.7*res6 && (box.y < 600) ) {
                std::cout << "Adult" << std::endl;
                cv::putText(Image, "ADULT", cv::Point(box.x, box.y), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 255), 2, false);
                cv::rectangle(Image, box, cv::Scalar(0, 0, 255), 2);
            } else if ( res5 <= 0.6*res6 && ((box.x > 0 && box.x + box.width < 274) || (box.x > 274 && box.x + box.width < 548) || (box.x > 548 && box.x + box.width < 822) || (box.x > 822 && box.x + box.width < 1096) || (box.x > 1096 && box.x + box.width < 1370) || (box.x > 1370 && box.x + box.width < 1644) || (box.x > 1644 && box.x + box.width < 1920)) && (box.y > 600 && box.y + box.height < 1080) ) {
                std::cout << "Children" << std::endl;
                cv::putText(Image, "CHILDREN", cv::Point(box.x, box.y), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 255, 0), 2, false);
                cv::rectangle(Image, box, cv::Scalar(0, 255, 0), 2);
            }
        }
    }   
    

    // for (int j = 0 ; j < objects.size() ; j++) {
    //     cv::Rect2f box = objects[j].rect;
    //     if ( (box.x > 202 && box.x + box.width < 756) && ((box.y > 311 && box.y < 420 ) && box.y + box.height < 500) ) {
    //         std::cout << "Children" << std::endl;
    //         cv::putText(Image, "CHILDREN", cv::Point(box.x, box.y), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 255, 0), 2, false);
    //         cv::rectangle(Image, box, cv::Scalar(0, 255, 0), 2);
    //     } 
    //     else if ( (box.x > 202 && box.x + box.width < 756) && ((47 < box.y && box.y < 285) && (400 < box.y + box.height && box.y + box.height < 700)) ) {
            // std::cout << "Adult" << std::endl;
            // cv::putText(Image, "ADULT", cv::Point(box.x, box.y), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 255), 2, false);
            // cv::rectangle(Image, box, cv::Scalar(0, 0, 255), 2);
    //     }

    // }

    // for (int k = 0 ; objects.size() ; k++) {
    //     cv::Rect2f box_K = objects[k].rect;
    //     if ( () )
    // }
    
    // std::vector<ObjectTrace> objects;
    // m_ObjectDetector->executeObjectDetector(Image, objects, 0.5);
    // for (int j = 0 ; j < objects.size() ; j++) {
    //     cv::Rect2f box_adult = objects[j].rect;
    //     if ( (box_adult.x > 202 && box_adult.x + box_adult.width < 756) && ((47 < box_adult.y < 300) && (box_adult.y + box_adult.width > 300)) ) {
    //         std::cout << "Adult" << std::endl;
    //         cv::rectangle(Image, box_adult, cv::Scalar(0, 255, 0), 2);
    //     }

    // }

    // std::vector<ObjectTrace> objects2;
    // m_ObjectDetector->executeObjectDetector(Image, objects2, 0.5);
    // for (int k = 0 ; k < objects2.size(); k++) {
    //     cv::Rect2f box_children = objects2[k].rect;
    //     if ( (box_children.x > 202 && box_children.x + box_children.width < 756) && (box_children.y > 300) ) {
    //         std::cout << "Children" << std::endl;
    //         cv::rectangle(Image, box_children, cv::Scalar(255, 0, 0), 2);
    //     }
    // } 

    return 0;
}

// int Children::Child_Execute_3(cv::Mat &Image, std::vector<OutputChild> &Child) {
//     std::vector<ObjectTrace> objects;
//     m_ObjectDetector->executeObjectDetector(Image, objects, 0.7);
//     std::cout << "Object size: " << objects.size() << std::endl;
//     for (int j = 0 ; j < objects.size() ; j++) {
//         cv::Rect box = objects[j].rect;
//         if (CheckInside_3(box, this->Region_Check3)) {
//             cv::rectangle(Image, box, cv::Scalar(255, 0, 0), 2);
//         }
//     }
// }

