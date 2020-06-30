#ifndef _ERROR_DEF_H_
#define _ERROR_DEF_H_

enum ERROR_DEF
{
	errUnknown = -1,
	errSuccess = 0,

    errConnBegin            =   1,
    errConnAlreadyExist     =   2,
    errConnEnd              =   100,

	errTotal
};

#endif
