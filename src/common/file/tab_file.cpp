#include "stdafx.h"
#include "file/tab_file.h"

CTabFile::CTabFile()
{
	m_pData = NULL;
	m_uSize = 0;
	m_nRowCount = 0;
	m_nColCount = 0;
	m_pOffsetTable = NULL;
	m_nSkipLine = 0;
	m_dwCRC = 0;
}

CTabFile::~CTabFile()
{
	uninit();
}

BOOL CTabFile::init(const TCHAR* pcszFileName, int32_t nSkipLine)
{
	int32_t nRetCode = 0;
	IFile* pFile = NULL;
	UINT32 uBOM = 0;
	int32_t nStringIndex = 0;
	int32_t nOffsetIndex = 0;
	int32_t nIndex = 0;
	BOOL bInQuote = FALSE;
	int32_t nTabCount = 0;
	uint16_t* pszData = NULL;
	char* pszTmp = NULL;

	m_nSkipLine = nSkipLine;
	m_nColCount = 1;
	m_nRowCount = 0;

	LOG_PROCESS_ERROR(pcszFileName);

	pFile = open_file(pcszFileName, "rb");
	LOG_PROCESS_ERROR(pFile);

	m_uSize = pFile->size();
	LOG_PROCESS_ERROR(m_uSize > 0);

	nRetCode = pFile->read(&uBOM, sizeof(uBOM));
	LOG_PROCESS_ERROR(nRetCode == sizeof(uBOM));

	if ((uBOM & 0xFFFF) == UTF16_BOM)
	{
		m_uSize -= 2;
		nRetCode = pFile->seek(2, SEEK_SET);
		LOG_PROCESS_ERROR(nRetCode);
	}
	else if ((uBOM & 0xFFFFFF) == UTF8_BOM)
	{
		m_uSize -= 3;
		nRetCode = pFile->seek(3, SEEK_SET);
		LOG_PROCESS_ERROR(nRetCode);
	}
	else	// utf8 default
	{
		nRetCode = pFile->seek(0, SEEK_SET);
		LOG_PROCESS_ERROR(nRetCode);
	}

	LOG_PROCESS_ERROR(m_pData == NULL);
	m_pData = new char[m_uSize];
	memset(m_pData, 0, m_uSize);
	LOG_PROCESS_ERROR(m_pData);

	nRetCode = pFile->read(m_pData, (UINT32)m_uSize);
	LOG_PROCESS_ERROR((uint32_t)nRetCode == m_uSize);

	close_file(pFile);
	pFile = NULL;

	// convert
	if ((uBOM & 0xFFFF) == UTF16_BOM)
	{
		pszTmp = new char[m_uSize * 3 / 2];
		memset(pszTmp, 0, m_uSize * 3 / 2);

		pszData = (uint16_t*)m_pData;
		m_pData = new char[m_uSize * 3 / 2];
		LOG_PROCESS_ERROR(m_pData);

		m_uSize = utf16_2_utf8(pszData, m_uSize / 2, pszTmp, m_uSize * 3 / 2);
		LOG_PROCESS_ERROR(m_uSize > 0);

		memcpy(m_pData, pszTmp, m_uSize);
		SAFE_DELETE_ARRAY(pszTmp);
	}

	m_dwCRC = crc32c_append(0, (const unsigned char*)m_pData, m_uSize);

	// first scan
	m_nColCount = 1;
	m_nRowCount = 0;
	while ((uint32_t)nIndex < m_uSize)
	{
		switch (m_pData[nIndex])
		{
		case '\t':
			if (m_nRowCount == 0 && !bInQuote)
				m_nColCount++;
			nTabCount++;
			break;
		case '\r':
			if (!bInQuote)
			{
				m_pData[nIndex] = 0;
			}
			break;
		case '\n':
			if (!bInQuote)
			{
				if (nIndex - 1 >= 0 && m_pData[nIndex - 1] == '\r')
				{
					nIndex++;
					continue;
				}
				m_nRowCount++;
			}
			break;
		case '\"':
			if (bInQuote)
			{
				if (m_pData[nIndex + 1] != '\"')
					bInQuote = false;
				else
					nIndex++;
			}
			else
				bInQuote = true;
			break;
		}

		nIndex++;
	}

	LOG_PROCESS_ERROR(m_pOffsetTable == NULL);
	m_pOffsetTable = new int32_t[m_nColCount * m_nRowCount];
	LOG_PROCESS_ERROR(m_pOffsetTable);

	// second scan
	nIndex = 0;

	bInQuote = false;
	while ((uint32_t)nIndex < m_uSize)
	{
		switch (m_pData[nIndex])
		{
		case '\t':
		case '\n':
			if (!bInQuote)
			{
				m_pData[nIndex] = 0;

				while ((nStringIndex < nIndex) &&
					(m_pData[nStringIndex] == 0 ||
						m_pData[nStringIndex] == '\r' ||
						m_pData[nStringIndex] == '\n'))
					nStringIndex++;

				LOG_PROCESS_ERROR(nOffsetIndex < m_nColCount * m_nRowCount);
				m_pOffsetTable[nOffsetIndex] = nStringIndex;
				nOffsetIndex++;

				nStringIndex = nIndex + 1;
			}
			break;
		case '\"':
			if (bInQuote)
			{
				if (m_pData[nIndex + 1] == '\"')
				{
					int nTempIndex = nIndex;
					while (m_pData[nTempIndex] != 0 && m_pData[nTempIndex + 1] != '\n' && m_pData[nTempIndex + 1] != '\t')
					{
						m_pData[nTempIndex] = m_pData[nTempIndex + 1];
						nTempIndex++;
					}
					m_pData[nTempIndex] = 0;
				}
				else
				{
					bInQuote = false;
					m_pData[nIndex] = 0;
				}
			}
			else
			{
				bInQuote = true;
				m_pData[nIndex] = 0;
			}

			break;
		}

		nIndex++;
	}

	SAFE_DELETE_ARRAY(pszData);

	return TRUE;
Exit0:
	if (pcszFileName)
		CRI("invalid file %s", pcszFileName);

	if (pFile)
		close_file(pFile);

	SAFE_DELETE_ARRAY(pszData);
	SAFE_DELETE_ARRAY(pszTmp);

	uninit();

	return FALSE;
}

BOOL CTabFile::uninit(void)
{
	SAFE_DELETE_ARRAY(m_pData);
	SAFE_DELETE_ARRAY(m_pOffsetTable);

	return true;
}

const char* CTabFile::get_data(int32_t nCol, int32_t nRow)
{
	LOG_PROCESS_ERROR(nCol >= 0 && nCol < m_nColCount);
	LOG_PROCESS_ERROR(nRow >= 0 && nRow < m_nRowCount);

	return m_pData + m_pOffsetTable[nRow * m_nColCount + nCol];
Exit0:
	return NULL;
}

BOOL CTabFile::is_empty_row(int32_t nRow)
{
	for (int32_t nCol = 0; nCol < m_nColCount ; nCol++)
	{
		if (m_pData[m_pOffsetTable[nRow * m_nColCount + nCol]] != 0)
			return FALSE;
	}

	return TRUE;
}
