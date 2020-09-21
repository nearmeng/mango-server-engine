#ifndef _CONN_DEF_H_
#define _CONN_DEF_H_

#define MAX_MESSAGE_ID			(65535)

enum CONN_EVENT_TYPE
{
    cetInvalid,

    cetStart,
    cetStop,
    cetTimeout,

    cetTotal
};

#endif