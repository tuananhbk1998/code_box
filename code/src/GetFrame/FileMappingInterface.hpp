#pragma once
#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct {
    int m_nFile;
    int m_nSize;
    char* m_pData;
} MmapFile;

class FileMappingInterface
{
    private:
        /* data */
    public:
        FileMappingInterface() {};
        virtual ~FileMappingInterface() {};
        virtual std::pair<bool, MmapFile> CreateMapping() = 0; //tạo ánh xạ
        virtual int  UnMapping()   = 0; // hủy ánh xạ
        virtual std::pair<bool, MmapFile> Remapping() = 0; // tạo lại ánh xạ với nội dung mới.
};