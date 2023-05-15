// #include <regex>
// #include "FrameManager.hpp"
// // #include "Config.hpp"

// #define INDEX_STATUS            1
// #ifndef PRINT_LOG
//     #ifdef DEBUG
//         #define PRINT_LOG(_TAG, ...) do{__android_log_print(ANDROID_LOG_INFO, _TAG,__VA_ARGS__);} while(0)
//     #else
//         #define PRINT_LOG(_TAG, ...) do {} while (0)
//     #endif
// #endif
// FrameManager::FrameManager()
// {
//     this->bReset = true;
//     this->lSizeFrame = 0;
//     this->pBuffImg = new BuffImgMapping();
//     this->pSyncMapping = new SyncMapping();

//     std::string cmd1 = "rm " + std::string(PATH_SHARED);
//     system(cmd1.c_str());

//     std::string cmd2 = "rm " + std::string(PATH_SYNC);
//     system(cmd2.c_str());
// }

// FrameManager::~FrameManager()
// {
//     if (this->pBuffImg != nullptr)
//         delete this->pBuffImg;
//     if (this->pSyncMapping != nullptr)
//         delete this->pSyncMapping;
// }
// bool FrameManager::checkExist(const char* path)
// {
//     struct stat buffer;
//     return ((stat(path, &buffer)) == 0);
// }
// size_t FrameManager::getSizeFrameCurrent()
// {
//     std::string info_sync(this->sSyncMapping.m_pData);
//     if (std::regex_match(info_sync, std::regex("\\*[0-9]{3,12}")))
//     {
//         char _count[2] = {info_sync[2], info_sync[3]};
//         int count = std::stoi((char*)_count); // số lượng chữ số biểu thị cho kích thước frame
//         // check số lượng số lượng chữ số từ vị trí 4 đến hết của info_sync có bằng count không ? 
//         std::string check(info_sync.substr(4));
//         if (check.length() < count)
//             return 0;
//         return std::stoll(check);
//     }
//     else
//         return 0;
// }
// bool FrameManager::getFrames(cv::Mat& img_fhd, cv::Mat& img_4k)
// {
//     bool status = true; // trạng thái trả về của hàm
//     // Trường hợp 1 trong các file chưa được tạo ra từ dịch vụ libbkav_Ai.so => Trả về
//     if (!this->checkExist(PATH_SYNC) || !this->checkExist(PATH_SHARED))
//     {
//         PRINT_LOG(TAG, "Wait for image frame data mapping");
//         usleep(500000);
//         this->bReset = true; // khi tồn tại 2 file, đồng bộ lại toàn bộ dữ liệu.
//         return false;
//     }
//     if (this->bReset) // nếu cờ reset true => đồng bộ lại toàn bộ
//     {
//         // reset syncFile
//         this->pSyncMapping->UnMapping();
//         auto mSync = this->pSyncMapping->CreateMapping();
//         if (mSync.first)
//             sSyncMapping = mSync.second;
//         else
//         {
//             PRINT_LOG(TAG, "CreateMapping Sync Fail");
//             this->bReset = true;
//             return false;
//         }
//         // reset buffImg
//         this->pBuffImg->UnMapping();
//         auto mBuffImg = this->pBuffImg->CreateMapping();
//         if (mBuffImg.first)
//             sBuffImgMapping = mBuffImg.second;
//         else
//         {
//             PRINT_LOG(TAG, "CreateMapping BuffImg Fail");
//             this->bReset = true;
//             return false;
//         }
//         // lấy kích ảnh (FHD, 4K) từ file SyncImg
//         this->lSizeFrame = this->getSizeFrameCurrent();
//         if (this->lSizeFrame == 0)
//         {
//             PRINT_LOG(TAG, "Get size Frame Current Fail");
//             this->bReset = true;
//             return false;
//         }
//         this->bReset = false;
//     }
//     // (*) CODE XỬ LÝ LOGIC
//     char check = sSyncMapping.m_pData[INDEX_STATUS];
//     switch (check)
//     {
//         case '1': // Lấy ảnh từ bộ nhớ được ánh xạ
//         {
//             if (this->lSizeFrame == sBuffImgMapping.m_nSize) // Nếu size khác nhau nghĩa là mới khởi tạo hoặc đổi VAM
//             {
//                 try
//                 {
//                     switch (this->lSizeFrame)
//                     {
//                         case SIZE_ORI_FRAME_FULLHD:
//                         {
//                             cv::Mat mat_src = cv::Mat(1088*1.5, 1920, CV_8UC1, sBuffImgMapping.m_pData);
//                             cv::Mat mat_dst(1088, 1920, CV_8UC3);
//                             cv::cvtColor(mat_src, mat_dst, cv::COLOR_YUV2BGR_NV12);
//                             cv::Mat ROI(mat_dst, cv::Rect(0, 0, 1920, 1080));
//                             img_fhd = img_4k = ROI;
//                             break;
//                         }
//                         case SIZE_ORI_FRAME_4K:
//                         {
//                             cv::Mat mat_src = cv::Mat(2176*1.5, 3840, CV_8UC1, sBuffImgMapping.m_pData);
//                             cv::Mat mat_dst(2176, 3840, CV_8UC3);
//                             cv::cvtColor(mat_src, mat_dst, cv::COLOR_YUV2BGR_NV12);
//                             cv::Mat ROI(mat_dst, cv::Rect(0, 0, 3840, 2160));
//                             cv::resize(ROI, img_fhd, cv::Size(W_FHD, H_FHD));
//                             img_4k = ROI;
//                             break;
//                         }
//                         default:
//                         {
//                             PRINT_LOG(TAG, "error decode frame: size frame invalid");
//                             sleep(1);
//                             status = false;
//                             break;
//                         }
//                     }
//                 }
//                 catch(const std::exception& e)
//                 {
//                     PRINT_LOG(TAG, "error decode frame");
//                     sleep(1);
//                     status = false;
//                 }
                
//             }
//             else
//             {
//                 // nếu size hiện tại và size ảnh get được về khác nhau thì mapping lại buffimg
//                 this->bReset = true;
//             }
//             sSyncMapping.m_pData[INDEX_STATUS] = '4';
//             break;
//         }
//         case '3':
//         {
//             this->bReset = true;
//             break;
//         }
//         default:
//             break;
//     }
//     // request
//     if (check == '4' || check == '3')
//     {
//         sSyncMapping.m_pData[INDEX_STATUS] = '0';
//     }
//     if (img_fhd.empty())
//         return false;
//     return status;
// }