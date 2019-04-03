#ifndef _TAB_FILE_H_
#define _TAB_FILE_H_

class CTabFile
{
public:
	CTabFile();
	virtual ~CTabFile();

	BOOL init(const char* pcszFileName, int32_t nSkipLine);
	BOOL uninit(void);

	inline int32_t get_row_count(void);
	inline int32_t get_col_count(void);
	inline int32_t get_skip_line(void);

	const char* get_data(int32_t nCol, int32_t nRow);
	
	inline uint32_t get_crc(void);
	inline uint32_t get_file_size(void);
	BOOL is_empty_row(int32_t nRow);

private:
	char*			m_pData;
	size_t			m_uSize;
	int32_t         m_nSkipLine;
	int32_t			m_nRowCount;
	int32_t			m_nColCount;
	int32_t*		m_pOffsetTable;
	uint32_t		m_dwCRC;
};


inline int32_t CTabFile::get_row_count(void)
{
	return m_nRowCount;
}

inline int32_t CTabFile::get_col_count(void)
{
	return m_nColCount;
}
inline int32_t CTabFile::get_skip_line(void)
{
	return m_nSkipLine;
}

inline uint32_t CTabFile::get_crc(void)
{
	return m_dwCRC;
}

inline uint32_t CTabFile::get_file_size(void)
{
	return (uint32_t)m_uSize;
}

#endif // _TAB_FILE_H_
