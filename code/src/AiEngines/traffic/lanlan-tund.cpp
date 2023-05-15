// #include "lanlan-tund.hpp"

// HeSo phuongTrinhDuongThang(cv::Point a, cv::Point b)
// {
//     HeSo heSo;
//     cv::Point vtpt;
//     vtpt.x = b.x - a.x; vtpt.y = b.y -a.y;
//     cv::Point vtcp;
//     vtcp.x = - vtpt.y;
//     vtcp.y = vtpt.x;
//     heSo.heSoX = vtcp.x;
//     heSo.heSoY = vtcp.y;
//     heSo.heSoTuDo = - (heSo.heSoX * a.x + heSo.heSoY * a.y );
//     return heSo;
// }
// bool checkPhia(cv::Point a, cv::Point b, HeSo ptdt)
// {
//     float ketQua = (ptdt.heSoX * a.x + ptdt.heSoY * a.y + ptdt.heSoTuDo) * (ptdt.heSoX * b.x + ptdt.heSoY * b.y + ptdt.heSoTuDo);
//     if (ketQua >= 0)
//         return true;
//     else 
//         return false;
// }
// cv::Point diemGiaoNhau(HeSo ptdt1, HeSo ptdt2)
// {
//     cv::Point diemCat;
//     diemCat.x = (ptdt1.heSoY * ptdt2.heSoTuDo - ptdt2.heSoY * ptdt1.heSoTuDo) / (ptdt2.heSoY * ptdt1.heSoX - ptdt1.heSoY * ptdt2.heSoX);
    
//     diemCat.y = (- ptdt1.heSoTuDo - ptdt1.heSoX * diemCat.x) / ptdt1.heSoY;
//     return diemCat; 
// }
// vector<cv::Point> makeArea(cv::Mat frame, cv::Point point1, cv::Point point2)
// {
//     HeSo ptdt1, top, bottom, left, right;
//     cv::Point topLeft(0,0);
//     cv::Point topRight(frame.cols,0);
//     cv::Point bottomLeft(0,frame.rows);
//     cv::Point bottomRight(frame.cols,frame.rows);
//     cv::line(frame, point1, point2, cv::Scalar(0,0,255));

//     ptdt1 = phuongTrinhDuongThang(point1, point2);
//     top = phuongTrinhDuongThang(topLeft, topRight);
//     bottom = phuongTrinhDuongThang(bottomLeft, bottomRight);
//     left = phuongTrinhDuongThang(topLeft, bottomLeft);
//     right = phuongTrinhDuongThang(topRight, bottomRight);

//     cv::Point giao;
//     vector<cv::Point> listPoint;
//     if (!checkPhia(topLeft, topRight, ptdt1)){
//         giao = diemGiaoNhau(ptdt1, top);
//         listPoint.push_back(giao);
//         if (!checkPhia(topLeft,bottomLeft, ptdt1)){
//             giao = diemGiaoNhau(ptdt1, left);
//             listPoint.push_back(giao);
//             listPoint.push_back(topLeft);
//             giao = diemGiaoNhau(ptdt1, top);
//             listPoint.push_back(giao);
//         } else{
//             if (!checkPhia(topLeft, bottomRight, ptdt1)){
//                 giao = diemGiaoNhau(ptdt1, bottom);
//                 listPoint.push_back(topLeft);
//                 listPoint.push_back(bottomLeft);
//                 listPoint.push_back(giao);
//                 giao = diemGiaoNhau(ptdt1, top);
//                 listPoint.push_back(giao);   
//             } else {
//                 giao = diemGiaoNhau(ptdt1, right);
//                 listPoint.push_back(topLeft);
//                 listPoint.push_back(bottomLeft);
//                 listPoint.push_back(bottomRight); 
//                 listPoint.push_back(giao); 
//                 giao = diemGiaoNhau(ptdt1, top);
//                 listPoint.push_back(giao);                
//             }
//         }
//     } else{
//         if (!checkPhia(topLeft, bottomLeft, ptdt1)){
//             giao = diemGiaoNhau(ptdt1, left);
//             listPoint.push_back(giao);
//             if (!checkPhia(topLeft,bottomRight, ptdt1)){
//                 giao = diemGiaoNhau(ptdt1, right);
//                 listPoint.push_back(giao);
//                 listPoint.push_back(topRight);
//                 listPoint.push_back(topLeft);
//                 giao = diemGiaoNhau(ptdt1, left);
//                 listPoint.push_back(giao);
//             } else {
//                 giao = diemGiaoNhau(ptdt1, bottom);
//                 listPoint.push_back(bottomLeft);
//                 listPoint.push_back(giao);
//                 giao = diemGiaoNhau(ptdt1, left);
//                 listPoint.push_back(giao);              
//             }
//         } else {
//             listPoint.push_back(topLeft);
//             listPoint.push_back(bottomLeft);
//             giao= diemGiaoNhau(ptdt1, bottom);
//             listPoint.push_back(giao);
//             giao = diemGiaoNhau(ptdt1, right);
//             listPoint.push_back(giao);             
//             listPoint.push_back(topRight);  
//             listPoint.push_back(topLeft);    
//         }
//     }
//     return listPoint;
// }

// Lanlan::Lanlan()
// {
//     this->detector = new ObjectDetector();
//     this->tracker = new ObjectTracking();
//     this->mListTracked.clear();
// }

// Lanlan::~Lanlan()
// {
//     if(this->detector) delete this->detector;
//     if(this->tracker) delete this->tracker;
// }

// int Lanlan::initLanlan()
// {
//     if(this->detector->initObjectDetector("ncnn", "traffic", "CPU") != STATUS_SUCCESS)
//         return STATUS_FAILED;
//     return STATUS_SUCCESS;
// }

// int Lanlan::setLanlan(settingLanlan settings)
// {
//     this->m_region = settings.regionSet;
//     this->m_vehicles = settings.vehiclesSet;
//     return STATUS_SUCCESS;
// }

// int Lanlan::updateLanlan(cv::Mat& frame, std::vector<OutputLanlan>& outputs)
// {
//     try
//     {
//         cv::Mat Frame;
//         frame.copyTo(Frame);
//         outputs.clear();

//         std::vector<cv::Point> listPoint = makeArea(frame, this->m_region[0], this->m_region[1]);



//         cv::line(Frame,this->m_region[0], this->m_region[1], cv::Scalar(0,255,255));


//         std::vector<ObjectTrace> detected;
//         if(this->detector->executeObjectDetector(Frame, detected, 0.5) != STATUS_SUCCESS)
//         {
//             LOG_FAIL("Execute Anpr detector failed");
//             return STATUS_FAILED;
//         }

//         if(detected.size() == 0)
//             return STATUS_FAILED;

//         std::vector<TrackingTrace> tracks;
//         this->tracker->process(detected, tracks);

//         for(auto it = this->mListTracked.begin(); it != this->mListTracked.end();)
//         {
//             const int theId = (*it).track_id;
//             const auto p = find_if(tracks.begin(), tracks.end(), 
//                                     [theId](const TrackingTrace &a) {return (a.m_ID == theId);});
//             if(p == tracks.end() && it != mListTracked.end())
//                 it = mListTracked.erase(it);
//             else
//                 it++;
//         }

//         for(auto &track : tracks)
//         {
            
//             if(!track.isOutOfFrame)
//             {
//                 // cv::circle(frame, cv::Point((track.m_rect.x + track.m_rect.width/2),(track.m_rect.y + track.m_rect.height /2)), 1, cv::Scalar(255,0,0),1);
//                 const int theId = track.m_ID;
//                 const auto p = std::find_if(this->mListTracked.begin(), this->mListTracked.end(),
//                                         [theId](const OutputLanlan &a) {return (a.track_id == theId);});

//                 if(p != this->mListTracked.end())
//                 {
//                     int index = std::distance(this->mListTracked.begin(), p);
//                     this->mListTracked[index].rect = track.m_rect;
//                     this->mListTracked[index].isOutOfFrame = false;
//                     this->mListTracked[index].track_id = track.m_ID;
//                     // std::string ketqua = std::to_string(checkLane(track, listPoint));
         
//                     // cv::putText(Frame,ketqua, cv::Point2d(this->mListTracked[index].rect.x, this->mListTracked[index].rect.y),FONT_HERSHEY_COMPLEX,1, cv::Scalar(255,255,255));
//                     if (checkLane(track, listPoint) != this->mListTracked[index].position){
//                         // this->mListTracked[index].isEvent = true;                     
//                         cv::rectangle(Frame, this->mListTracked[index].rect, cv::Scalar(0,0,255), 1, 8);                           
//                     }
//                     else {      

//                     }

//                 }               
//                 else {
//                     OutputLanlan obj;
//                     obj.track_id = track.m_ID;
//                     obj.label = track.m_type;
//                     obj.rect = track.m_rect;
//                     obj.position = checkLane(track, listPoint);
  
//                     this->mListTracked.push_back(obj);
//                 }

//             } 
//             else 
//             {
//                 const int theId = track.m_ID;
//                 const auto p = find_if(mListTracked.begin(), mListTracked.end(), 
//                                         [theId] (const OutputLanlan &a) {return (a.track_id == theId);});

//                 if(p != mListTracked.end()) {
//                     int dist = distance(mListTracked.begin(), p);
//                     mListTracked[dist].isOutOfFrame = true;
//                 }
//             }
//         }

//         for(auto out : mListTracked) {
//             if(!out.isOutOfFrame) {
//                 outputs.push_back(out);
//             }
//         }

//         frame = Frame;
//     }   
//     catch(const std::exception& e)
//     {
//         std::cerr << e.what() << '\n';
//     }
//     return STATUS_SUCCESS;
// }

// bool Lanlan::checkLane(TrackingTrace track, std::vector<cv::Point> area)
// {
//     if(area.size() == 0)
//         return false;
//     cv::Point2f cBox = cv::Point2f(track.m_rect.x + float(track.m_rect.width/2),
//                                    track.m_rect.y + float(track.m_rect.height));
//     if(cv::pointPolygonTest(area, cBox, false) >= 0)
//         return true;
//     return false;
// }