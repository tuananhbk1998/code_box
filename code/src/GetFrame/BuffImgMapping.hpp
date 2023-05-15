#pragma once

#include "FileMappingInterface.hpp"

#ifdef ANDROID
    //#define PATH_SHARED         "/data/bkavai/HIEUPV/BuffImg"
    #define PATH_SHARED         "/data/bkavai/BuffImg"

#else
    #define PATH_SHARED         "BuffImg"
#endif

class BuffImgMapping : public FileMappingInterface
{
    private:
        MmapFile file;
    public:
        BuffImgMapping() ;
        ~BuffImgMapping() {};
        std::pair<bool, MmapFile> CreateMapping() override;
        int UnMapping() override;
        std::pair<bool, MmapFile> Remapping() override;
};
