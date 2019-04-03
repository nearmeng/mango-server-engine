#ifndef _IFILE_H_
#define _IFILE_H_

class IFile
{
public:
	virtual uint32_t read(void* pData, uint32_t dwLen) = 0;
	virtual BOOL  seek(int32_t nOffset, int32_t nOrigin) = 0;
	virtual BOOL eof(void) = 0;
	virtual uint32_t size(void) = 0;
};

IFile* open_file(const char* pcszFileName, const char* pcszMode);
BOOL close_file(IFile* pFile);

BOOL is_file_exist(const char* pcszFileName);
uint32_t get_file_create_time(const char* pcszFileName);
uint32_t get_file_modify_time(const char* pcszFileName);

const char* find_first_file(const char* pcszDirectory, const char* pcszSuffix);
const char* find_next_file(const char* pcszDirectory, const char* pcszSuffix);
void close_find_file(void);

BOOL create_dir(const char* szDirName);

#endif	// _IFILE_H_
