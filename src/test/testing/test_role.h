#ifndef _TEST_ROLE_H_
#define _TEST_ROLE_H_

//tolua_begin

class CTestRole
{
public:
	BOOL init()
	{
		m_nRoleID = 1;
		m_nLevel = 1;
		m_nAge = 2;
		strncpy(m_szRoleName, "test", sizeof(m_szRoleName));
		return TRUE;
	};

	char* get_name() { return m_szRoleName;  };
	int32_t get_id() { return m_nRoleID;  };
	int32_t get_level() { return m_nLevel; };
	int32_t get_age() { return m_nAge; }

	void set_level(int32_t nLevel) { m_nLevel = nLevel;  }
	void set_age(int32_t nAge) { m_nAge = nAge;  }

// tolua_end
private:
	char	m_szRoleName[32];
	int32_t m_nRoleID;
	int32_t m_nLevel;
	int32_t m_nAge;
};	//tolua_export

#endif
