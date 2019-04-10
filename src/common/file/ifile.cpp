#include "stdafx.h"
#include <sys/stat.h>

#if defined(_WIN32) | defined(_WIN64)
static HANDLE s_hFind = INVALID_HANDLE_VALUE;
static WIN32_FIND_DATA s_FindFileData;
#else
static 	DIR* s_pDir;
#endif	// WIN32 | WIN64

class IInternalFile : public IFile
{
public:
	virtual BOOL close(void) = 0;
};

class CNormalFile : public IInternalFile
{
public:
	CNormalFile();
	virtual ~CNormalFile();

	BOOL init(const char* pcszFileName, const char* pcszMode);
	BOOL uninit(void);

	virtual BOOL close(void);

	virtual uint32_t read(void* pData, uint32_t dwLen);
	uint32_t write(const void* pData, uint32_t dwLen);
	virtual BOOL  seek(int32_t nOffset, int32_t nOrigin);
	virtual BOOL eof(void);
	virtual uint32_t size(void);

private:
	FILE*   m_pFile;
};

CNormalFile::CNormalFile()
{
	m_pFile = NULL;
}

CNormalFile::~CNormalFile()
{
	uninit();
}

BOOL CNormalFile::init(const char* pcszFileName, const char* pcszMode)
{
	LOG_PROCESS_ERROR(m_pFile == NULL);

	m_pFile = fopen(pcszFileName, pcszMode);
	PROCESS_ERROR(m_pFile);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CNormalFile::uninit(void)
{
	if (m_pFile)
	{
		fclose(m_pFile);
	}
	m_pFile = NULL;

	return TRUE;
}

BOOL CNormalFile::close(void)
{
	uninit();
	delete this;

	return TRUE;
}

uint32_t CNormalFile::read(void* pData, uint32_t dwLen)
{
	return (uint32_t)fread(pData, 1, dwLen, m_pFile);
}

uint32_t CNormalFile::write(const void* pData, uint32_t dwLen)
{
	return (uint32_t)fwrite(pData, 1, dwLen, m_pFile);
}

BOOL CNormalFile::seek(int32_t nOffset, int32_t nOrigin)
{
	return fseek(m_pFile, nOffset, nOrigin) == 0;
}

BOOL CNormalFile::eof(void)
{
	if (m_pFile)
		return feof(m_pFile);
	return TRUE;
}

uint32_t CNormalFile::size(void)
{
	uint32_t dwResult = 0;
	int32_t nRetCode = 0;
	int32_t nCurrent = 0;

	LOG_PROCESS_ERROR(m_pFile);

	nCurrent = ftell(m_pFile);

	nRetCode = fseek(m_pFile, 0, SEEK_END);
	LOG_PROCESS_ERROR(nRetCode == 0);

	dwResult = ftell(m_pFile);

	nRetCode = fseek(m_pFile, nCurrent, SEEK_SET);
	LOG_PROCESS_ERROR(nRetCode == 0);

Exit0:
	return dwResult;
}

/************************************************************************/

IFile* open_file(const char* pcszFileName, const char* pcszMode)
{
	int32_t nRetCode = 0;
	IFile* pResult = NULL;

	pResult = new CNormalFile;
	LOG_PROCESS_ERROR(pResult);

	nRetCode = ((CNormalFile*)pResult)->init(pcszFileName, pcszMode);
	PROCESS_ERROR(nRetCode);

	return pResult;
Exit0:
	SAFE_DELETE(pResult);

	return NULL;
}

BOOL close_file(IFile* pFile)
{
	LOG_PROCESS_ERROR(pFile);

	((IInternalFile*)pFile)->close();

	return TRUE;
Exit0:
	return FALSE;
}

BOOL is_file_exist(const char* strFileName)
{
	FILE* fp = fopen(strFileName, "r");
	if (fp == NULL)
	{
		return FALSE;
	}

	fclose(fp);

	return TRUE;
}

uint32_t get_file_create_time(const char* pcszFileName)
{
	uint32_t dwResult = 0;
	int32_t nRetCode = 0;
	FILE* pFile = NULL;

	LOG_PROCESS_ERROR(pcszFileName);

	pFile = fopen(pcszFileName, "rb");
	if (pFile)
	{
		int fd;
		struct stat FileStat;

		fd = fileno(pFile);
		LOG_PROCESS_ERROR(fd != -1);

		nRetCode = fstat(fd, &FileStat);
		LOG_PROCESS_ERROR(nRetCode == 0);

		dwResult = (uint32_t)FileStat.st_ctime;
	}

Exit0:
	if (pFile)
		fclose(pFile);

	return dwResult;
}

uint32_t get_file_modify_time(const char* pcszFileName)
{
	uint32_t dwResult = 0;
	int32_t nRetCode = 0;
	FILE* pFile = NULL;

	LOG_PROCESS_ERROR(pcszFileName);

	pFile = fopen(pcszFileName, "rb");
	if (pFile)
	{
		int fd;
		struct stat FileStat;

		fd = fileno(pFile);
		LOG_PROCESS_ERROR(fd != -1);

		nRetCode = fstat(fd, &FileStat);
		LOG_PROCESS_ERROR(nRetCode == 0);

		dwResult = (uint32_t)FileStat.st_mtime;
	}

Exit0:
	if (pFile)
		fclose(pFile);

	return dwResult;
}

const char* find_first_file(const char * pcszDirectory, const char * pcszSuffix)
{
	const char* pResult = NULL;

#if defined(_WIN32) | defined(_WIN64)
	char szPattern[256];

	snprintf(szPattern, 256, "%s/*%s", pcszDirectory, pcszSuffix);
	s_hFind = FindFirstFile(szPattern, &s_FindFileData);
	PROCESS_ERROR(s_hFind != INVALID_HANDLE_VALUE);

	pResult = s_FindFileData.cFileName;
#else
	struct dirent* pDirEntry = NULL;

	s_pDir = opendir(pcszDirectory);
	LOG_PROCESS_ERROR(s_pDir);

	pDirEntry = readdir(s_pDir);
	while (pDirEntry)
	{
		char* pszSuffix = strstr(pDirEntry->d_name, pcszSuffix);
		if (pszSuffix || *pcszSuffix == '*')
		{
			pResult = pDirEntry->d_name;
			break;
		}
	}
#endif	// _WIN32 | WIN64

	return pResult;
Exit0:
	
	return NULL;
}

const char* find_next_file(const char * pcszDirectory, const char * pcszSuffix)
{
	const char* pResult = NULL;

#if defined(_WIN32) | defined(_WIN64)
	int32_t nRetCode = 0;

	nRetCode = FindNextFile(s_hFind, &s_FindFileData);
	PROCESS_ERROR(nRetCode);

	pResult = s_FindFileData.cFileName;
#else
	struct dirent* pDirEntry = NULL;

	LOG_PROCESS_ERROR(s_pDir);

	pDirEntry = readdir(s_pDir);
	while (pDirEntry)
	{
		char* pszSuffix = strstr(pDirEntry->d_name, pcszSuffix);
		if (pszSuffix || *pcszSuffix == '*')
		{
			pResult = pDirEntry->d_name;
			break;
		}
	}
#endif	// _WIN32 | WIN64

	return pResult;
Exit0:

	return NULL;
}

void close_find_file(void)
{
#if defined(_WIN32) | defined(_WIN64)
	int32_t nRetCode = 0;

	nRetCode = FindClose(s_hFind);
	LOG_PROCESS_ERROR(nRetCode);

	s_hFind = INVALID_HANDLE_VALUE;
Exit0:
#else
	closedir(s_pDir);
#endif	// _WIN32 | WIN64

	return;
}

BOOL create_dir(const char* szDirName)
{
	int32_t nRetCode = 0;
#if defined(_WIN32) | defined(_WIN64)
	nRetCode = _mkdir(szDirName);
	PROCESS_SUCCESS(nRetCode == 0);
#else
	nRetCode = mkdir(szDirName, S_IRWXU | S_IRGRP | S_IXGRP | S_IXOTH);
	PROCESS_SUCCESS(nRetCode == 0);
#endif	// _WIN32 | WIN64

	LOG_PROCESS_ERROR(errno == EEXIST);

Exit1:
	return TRUE;
Exit0:
	return FALSE;
}
