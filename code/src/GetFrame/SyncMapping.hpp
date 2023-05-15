#pragma once

#include "FileMappingInterface.hpp"

#ifdef ANDROID
    //#define PATH_SYNC           "/data/bkavai/HIEUPV/SyncImg" // 1 = not sync, 0 = sync
    #define PATH_SYNC           "/data/bkavai/SyncImg" // 1 = not sync, 0 = sync

#else
    #define PATH_SYNC           "SyncImg" // 1 = not sync, 0 = sync
#endif

class SyncMapping : public FileMappingInterface
{
    private:
        MmapFile file;
    public:
        SyncMapping();
        ~SyncMapping() {};
        std::pair<bool, MmapFile> CreateMapping() override;
        int UnMapping() override;
        std::pair<bool, MmapFile> Remapping() override;
};
