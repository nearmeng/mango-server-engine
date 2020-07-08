#include "stdafx.h"
#include "string/string_ex.h"

#include <vector>

size_t strxcpy(char *dst, const char *src, size_t size)
{
    const char *d = dst;

    if (dst == NULL || src == NULL || size == 0)
        return 0;

    while (--size > 0)
    {
        if ((*dst++ = *src++) == 0)
            break;
    }

    if (size == 0)
    {
        *dst++ = 0;
    }

    return dst - d - 1;
}

size_t strxcat(char *dst, const char *src, size_t size)
{
    const char *d = dst;

    if (dst == NULL || src == NULL || size == 0)
        return 0;

    while (size && *dst)
    {
        size--;
        dst++;
    }

    while (--size > 0)
    {
        if ((*dst++ = *src++) == 0)
            break;
    }

    if (size == 0)
    {
        *dst++ = 0;
    }

    return dst - d - 1;
}

size_t strxlen(const char *str, size_t maxlen)
{
	size_t len = maxlen;
	while (*str && len)
	{
		str++;
		len--;
	}
	return maxlen - len;
}

size_t strxtolower(char *str, size_t maxlen)
{
	size_t len = maxlen;
	while (*str && len)
	{
		if (*str >= 65 && *str <= 90)
			*str = *str | 32;
		str++;
		len--;
	}
	return maxlen - len;
}

void strfix(char* str)
{
	while (*str)
	{
		if (*str == '%' || *str == '|')
			*str = '#';
		str++;
	}
}

void strfixplus(char* str)
{
	while (*str)
	{
		if (*str == '%' || *str == '|' || *str == '\n')
			*str = '#';
		str++;
	}
}
int32_t check_utf8_chinese(const char* str)
{
    //0000 ¨C 007F 0xxxxxxx
    //0080 ¨C 07FF 110xxxxx 10xxxxxx
    //0800 ¨C FFFF 1110xxxx 10xxxxxx 10xxxxxx
    if ((*str & 0xE0) == 0xE0)
    {
        return 3;
    }
    else if ((*str & 0xC0) == 0xC0)
    {
        return 2;
    }
    return 0;
}
bool is_alphabet(const char* str)
{
    //!41-5a 61-7a
    if (((*str) >= 0x41 && (*str) <= 0x5a) || ((*str) >= 0x61 && (*str) <= 0x7a))
        return true;
    return false;
}
bool is_number(const char* str)
{
    if (((*str) >= 0x30 && (*str) <= 0x39))
        return true;
    return false;
}
//! remain number,chinese,alphabet
void strfilter(const char* sourStr, char* destStr)
{
    int32_t nBye = 0;
    while (*sourStr)
    {
		nBye = check_utf8_chinese(sourStr);
        if (nBye) 
        {
            bool bCpy = nBye > 2 ? true : false;
            while (nBye)
            {
                if (bCpy) 
                {
                    *destStr++ = *sourStr++;
                }
                else
                    sourStr++;
                nBye--;
            }
            
        }else if (is_number(sourStr) || is_alphabet(sourStr))
        {
            *destStr++ = *sourStr++;
        }
        else 
        {
            sourStr++;
        }        
    } 
      
}

const char* col2str(int32_t nCol)
{
	static char szStr[4];

	if (nCol >= 26 * 26)
	{
		szStr[0] = 'E';
		szStr[1] = 'R';
		szStr[2] = 'R';
		szStr[3] = 0;
	}
	else if (nCol >= 26)
	{
		szStr[0] = 'A' + nCol / 26;
		szStr[1] = 'A' + nCol % 26;
		szStr[2] = 0;
	}
	else
	{
		szStr[0] = 'A' + nCol;
		szStr[1] = 0;
	}

    return szStr;
}

void get_file_path(const char* pcszFileName, int32_t nFileNameLen, char* pszPath, int32_t nPathLen)
{
	if (pcszFileName == NULL || pszPath == NULL || nFileNameLen <= 0 || nPathLen <= 0)
		return;
	const char* pszPointer = pcszFileName + strxlen(pcszFileName, nFileNameLen) - 1;
	while (pszPointer >= pcszFileName)
	{
		if (*pszPointer == '/' || *pszPointer == '\\')
			break;
		pszPointer--;
	}
	if (*pszPointer == '/' || *pszPointer == '\\')
	{
		int32_t nLen = pszPointer - pcszFileName + 1;
		nLen = nLen > nPathLen ? nPathLen : nLen;
		strxcpy(pszPath, pcszFileName, nLen);
	}
}

std::string UriEncode(const std::string & sSrc)
{
    static char SAFE[256] =
    {
        /*      0 1 2 3  4 5 6 7  8 9 A B  C D E F */
        /* 0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* 1 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* 2 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* 3 */ 1,1,1,1, 1,1,1,1, 1,1,0,0, 0,0,0,0,

        /* 4 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
        /* 5 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,
        /* 6 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
        /* 7 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,

        /* 8 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* 9 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* A */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* B */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,

        /* C */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* D */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* E */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* F */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
    };

    const char DEC2HEX[16 + 1] = "0123456789ABCDEF";
    const unsigned char * pSrc = (const unsigned char *)sSrc.c_str();
    const int SRC_LEN = sSrc.length();
    unsigned char * const pStart = new unsigned char[SRC_LEN * 3];
    unsigned char * pEnd = pStart;
    const unsigned char * const SRC_END = pSrc + SRC_LEN;

    for (; pSrc < SRC_END; ++pSrc)
    {
        if (SAFE[*pSrc])
            *pEnd++ = *pSrc;
        else
        {
            // escape this char
            *pEnd++ = '%';
            *pEnd++ = DEC2HEX[*pSrc >> 4];
            *pEnd++ = DEC2HEX[*pSrc & 0x0F];
        }
    }

    std::string sResult((char *)pStart, (char *)pEnd);
    delete[] pStart;
    return sResult;
}

std::string UriDecode(const std::string & sSrc)
{
    static const char HEX2DEC[256] =
    {
        /*       0  1  2  3   4  5  6  7   8  9  A  B   C  D  E  F */
        /* 0 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
        /* 1 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
        /* 2 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
        /* 3 */  0, 1, 2, 3,  4, 5, 6, 7,  8, 9,-1,-1, -1,-1,-1,-1,

        /* 4 */ -1,10,11,12, 13,14,15,-1, -1,-1,-1,-1, -1,-1,-1,-1,
        /* 5 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
        /* 6 */ -1,10,11,12, 13,14,15,-1, -1,-1,-1,-1, -1,-1,-1,-1,
        /* 7 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,

        /* 8 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
        /* 9 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
        /* A */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
        /* B */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,

        /* C */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
        /* D */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
        /* E */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
        /* F */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1
    };

    // Note from RFC1630: "Sequences which start with a percent
    // sign but are not followed by two hexadecimal characters
    // (0-9, A-F) are reserved for future extension"

    const unsigned char * pSrc = (const unsigned char *)sSrc.c_str();
    const int SRC_LEN = sSrc.length();
    const unsigned char * const SRC_END = pSrc + SRC_LEN;
    // last decodable '%' 
    const unsigned char * const SRC_LAST_DEC = SRC_END - 2;

    char * const pStart = new char[SRC_LEN];
    char * pEnd = pStart;

    while (pSrc < SRC_LAST_DEC)
    {
        if (*pSrc == '%')
        {
            char dec1, dec2;
            if (-1 != (dec1 = HEX2DEC[*(pSrc + 1)])
                && -1 != (dec2 = HEX2DEC[*(pSrc + 2)]))
            {
                *pEnd++ = (dec1 << 4) + dec2;
                pSrc += 3;
                continue;
            }
        }

        *pEnd++ = *pSrc++;
    }

    // the last 2- chars
    while (pSrc < SRC_END)
        *pEnd++ = *pSrc++;

    std::string sResult(pStart, pEnd);
    delete[] pStart;
    return sResult;
}


int base64_encode(const char *sourcedata, size_t srclength,char *base64)
{
	const char *base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	const char padding_char = '=';

    size_t i=0, j=0;
    unsigned char trans_index=0;    
    //const int datalength = strlen((const char*)sourcedata);

    for (; i < srclength; i += 3){
        trans_index = ((sourcedata[i] >> 2) & 0x3f);
        base64[j++] = base64char[(int)trans_index];
        trans_index = ((sourcedata[i] << 4) & 0x30);
        if (i + 1 < srclength){
            trans_index |= ((sourcedata[i + 1] >> 4) & 0x0f);
            base64[j++] = base64char[(int)trans_index];
        }else{
            base64[j++] = base64char[(int)trans_index];

            base64[j++] = padding_char;

            base64[j++] = padding_char;

            break;   
        }
        
        trans_index = ((sourcedata[i + 1] << 2) & 0x3c);
        if (i + 2 < srclength){ 
            trans_index |= ((sourcedata[i + 2] >> 6) & 0x03);
            base64[j++] = base64char[(int)trans_index];

            trans_index = sourcedata[i + 2] & 0x3f;
            base64[j++] = base64char[(int)trans_index];
        }
        else{
            base64[j++] = base64char[(int)trans_index];

            base64[j++] = padding_char;

            break;
        }
    }

    //base64[j] = '\0'; 
    return 0;
}

//int base64_decode(const char *str,size_t length,char* desStr) 
//{
//    //½âÂë±í
//    const char DecodeTable[] =
//    {
//        -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -1, -2, -2, -1, -2, -2,
//        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
//        -1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 62, -2, -2, -2, 63,
//        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -2, -2, -2, -2, -2, -2,
//        -2,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
//        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -2, -2, -2, -2, -2,
//        -2, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
//        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -2, -2, -2, -2, -2,
//        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
//        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
//        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
//        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
//        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
//        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
//        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
//        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2
//    };
//    const char base64_pad = '=';
//    int bin = 0, i = 0, pos = 0,j = 0;
//    const char *current = str;
//    char ch;
//    while( (ch = *current++) != '\0' && length-- > 0 )
//    {
//        if (ch == base64_pad) { 
//            if (*current != '=' && (i % 4) == 1) {
//                return NULL;
//            }
//
//            continue;
//        }
//        ch = DecodeTable[ch];
//        if (ch < 0 ) 
//            continue;
//
//        switch(i % 4)
//        {
//            case 0:
//                bin = ch << 2;
//                break;
//            case 1:
//                bin |= ch >> 4;
//                desStr[j++] = (char)bin;
//                bin = ( ch & 0x0f ) << 4;
//                break;
//            case 2:
//                bin |= ch >> 2;
//                desStr[j++] = (char)bin;
//                bin = ( ch & 0x03 ) << 6;
//                break;
//            case 3:
//                bin |= ch;
//                desStr[j++] = (char)bin;
//                break;
//        }
//        i++;
//    }
//    
//    return i > 0 ? j-1:0;
//}

std::vector<std::string> strsep(const std::string &sStr, const std::string &sSep, bool withEmpty)
{
    std::vector<std::string> vt;

    std::string::size_type pos = 0;
    std::string::size_type pos1 = 0;

    while (true)
    {
        std::string s;
        pos1 = sStr.find_first_of(sSep, pos);
        if (pos1 == std::string::npos)
        {
            if (pos + 1 <= sStr.length())
            {
                s = sStr.substr(pos);
            }
        }
        else if (pos1 == pos)
        {
            s = "";
        }
        else
        {
            s = sStr.substr(pos, pos1 - pos);
            pos = pos1;
        }

        if (withEmpty)
        {
            vt.push_back(s);
        }
        else
        {
            if (!s.empty())
            {
                vt.push_back(s);
            }
        }

        if (pos1 == std::string::npos)
        {
            break;
        }

        pos++;
    }

    return vt;
}

char* left_trim_space(char* s)
{
	if (s == NULL || *s== '\0')
		return s;

	char* p = s;
	while (*p != '\0' && *p == ' ') { p++; }
	return p;
}

char* right_trim_space(char* s)
{
	if (s == NULL || *s== '\0')
		return s;

	size_t len = strlen(s) - 1;

	while (len >= 0 && *(s + len) == ' ') { len--; }
	s[len + 1] = '\0';
	return s;
}

char* trim_space(char* s)
{
	char* p = s;
	
	p = left_trim_space(p);
	p = right_trim_space(p);

	return p;
}
