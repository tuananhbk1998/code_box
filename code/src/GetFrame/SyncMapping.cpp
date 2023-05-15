#include "SyncMapping.hpp"
#include <string.h>
SyncMapping::SyncMapping()
{
    this->file.m_nFile = 0;
    this->file.m_nSize = 0;
    this->file.m_pData = NULL;
}

std::pair<bool, MmapFile> SyncMapping::CreateMapping()
{
    struct stat sb;
    if (((this->file.m_nFile = open(PATH_SYNC, O_RDWR)) == -1) || (fstat(this->file.m_nFile, &sb) == -1))
    {
        close(this->file.m_nFile);
        return std::make_pair(false, this->file);
    }
    this->file.m_nSize = sb.st_size;
    this->file.m_pData = (char*)mmap(NULL, this->file.m_nSize, PROT_READ | PROT_WRITE, MAP_SHARED, this->file.m_nFile, 0);
    if (this->file.m_pData == MAP_FAILED)
    {
        close(this->file.m_nFile);
        return std::make_pair(false, this->file);
    }
    return std::make_pair(true, this->file);
}
//return 0 if success, -1 if fail
int SyncMapping::UnMapping()
{
    if (this->file.m_pData != NULL && this->file.m_nSize != 0)
    {
        close(this->file.m_nFile);
        return munmap(this->file.m_pData, this->file.m_nSize);
    }
    else
        return -1;
}
std::pair<bool, MmapFile> SyncMapping::Remapping()
{
    if (this->UnMapping() != 0)
        return std::make_pair(false, this->file);
    return this->CreateMapping();
}