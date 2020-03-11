#ifndef _STRING_EX_H_
#define _STRING_EX_H_

#include <string>
#include <vector>
#include "common/common_inc.h"

size_t strxcpy(char *dst, const char *src, size_t size);
size_t strxcat(char *dst, const char *src, size_t size);
size_t strxlen(const char *str, size_t maxlen);
size_t strxtolower(char *str, size_t maxlen);
void strfix(char* str);
void strfixplus(char* str);
void strfilter(const char* sourStr, char* destStr);
const char* col2str(int nCol);
void get_file_path(const char* pcszFileName, int32_t nFileNameLen, char* pszPath, int32_t nPathLen);
std::string UriEncode(const std::string & sSrc);
std::string UriDecode(const std::string & sSrc);
int base64_encode(const char * sourcedata, size_t srclength,char * base64);
//int base64_decode(const char *str, size_t length, char* desStr);
std::vector<std::string> strsep(const std::string &sStr, const std::string &sSep, bool withEmpty = false);


#if defined(WIN32) & !defined(snprintf)
	#define snprintf _snprintf_s
#endif	// WIN32

#endif  // _STRING_EX_H_
