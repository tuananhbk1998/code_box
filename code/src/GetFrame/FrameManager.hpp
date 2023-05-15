// #pragma once
// #include <opencv2/opencv.hpp>
// #include "BuffImgMapping.hpp"
// #include "SyncMapping.hpp"
// #define W_FHD                   1920
// #define H_FHD                   1080
// #define SIZE_ORI_FRAME_FULLHD   3133440
// #define W_4K                    3840
// #define H_4K                    2160
// #define SIZE_ORI_FRAME_4K       12533760
// #define W_HD                    1280    
// #define H_HD                    720
// class FrameManager
// {
//     private:
//         FileMappingInterface* pSyncMapping  = nullptr;
//         FileMappingInterface* pBuffImg      = nullptr;
//         MmapFile              sSyncMapping;
//         MmapFile              sBuffImgMapping;
//         bool                  bReset;
//         size_t                lSizeFrame;
//         inline bool           checkExist(const char* path);
//         size_t                getSizeFrameCurrent();

//     public:
//         FrameManager(/* args */);
//         ~FrameManager();
//         bool getFrames(cv::Mat& img_fhd, cv::Mat& img_4k);
// };