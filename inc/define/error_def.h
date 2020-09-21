#ifndef _ERROR_DEF_H_
#define _ERROR_DEF_H_

enum ERROR_DEF
{
	errUnknown = -1,
	errSuccess = 0,

    errConnBegin            =   1,
    errConnAlreadyExist,
    errConnTimeout,
    errConnEnd              =   100,

    errLoginBegin           =   101,
    errLoginNotInWhiteList,
    errLoginUserBanned,
    errLoginAlreadyLogin,
    errLoginRepeated,
    errLogoutNormal,
    errLoginEnd             =   200,

	errTotal
};

#endif
