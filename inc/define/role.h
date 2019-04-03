#ifndef  _ROLE_H_
#define  _ROLE_H_

#include "common/common_inc.h"

class CRole 
{
public:
	CRole() {};
	~CRole() {};

	inline const char* get_name() { return m_szName; };
	inline void set_name(const char* pcszName) { strxcpy(m_szName, pcszName, sizeof(m_szName)); };

	inline int32_t get_age(void) { return m_nAge; };
	inline void set_age(int32_t nAge) { m_nAge = nAge; };

private:
	char m_szName[128];
	int32_t m_nAge;
};

#endif   /* ----- #ifndef _ROLE_H_  ----- */

