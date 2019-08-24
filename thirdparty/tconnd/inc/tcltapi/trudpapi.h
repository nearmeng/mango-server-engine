/**
 * @file trudpapi.h
 * @brief pack/unpack the data for reliable udp protocol.
 *
 * @author jiaweixu
 * @version 1.0
 * @date 2011-05-19
 *
 * Copyright (c) 2011 Tencent. All Rights Reserved.
 */

#ifndef TRUDPAPI_H
#define TRUDPAPI_H

#include "pal/pal.h"
#include "tdr/tdr.h"
#include "tcltapi/trudp_packet.h"
#include "tcltapi/trudp_protocol.h"

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef INOUT
#define INOUT
#endif


struct tagRudpApiHandle;
typedef struct tagRudpApiHandle TRUDPAPIHANDLE;
typedef struct tagRudpApiHandle *HTRUDPAPI;


#define TRUDP_LOG_PATTERN_LEN              1024

typedef enum
{
    TRUDPAPI_ERR_NONE = 0,
    TRUDPAPI_ERR_PACKETHTON_FAIL =1,
    TRUDPAPI_ERR_PACKETNTOH_FAIL =2,
    TRUDPAPI_ERR_BODYHTON_FAIL =3,
    TRUDPAPI_ERR_BODYNTOH_FAIL =4,
    TRUDPAPI_ERR_INVALID_URL =5,
    TRUDPAPI_ERR_SEND_FAIL =6,
    TRUDPAPI_ERR_RECV_FAIL =7,
    TRUDPAPI_ERR_PEER_CLOSE=8,
    TRUDPAPI_ERR_INVALID_CMD=9,
    TRUDPAPI_ERR_CONNECT_TIMEOUT=10,
    TRUDPAPI_ERR_NOT_ACTIVE=11,
    TRUDPAPI_ERR_INSUFFICIENT_BUFF=12,
    TRUDPAPI_ERR_RETRANSMISSION_TIMEOUT=13,
    TRUDPAPI_ERR_INVALID_INPUTARG=14,
    TRUDPAPI_ERR_SET_OPT=15,
    TRUDPAPI_ERR_COUNT
}TRUDPAPI_ERR;


/*connection options*/
typedef struct tagTRUDPConnOpt
{

    uint32_t dwRetryTimes; /*Max. re-connection times, which is greater than or equal to 0*/
    uint32_t dwConnTimeout;  /*connection timeout(unit: ms), which is greater than or equal to 0*/
    uint32_t dwUserId;   /*user identify, for example QQ number,if not used,set to 0*/
}TRUDPCONNOPT, *LPTRUDPCONNOPT;

/*reliable udp option*/
typedef enum
{
    TRUDP_RCVBUF = 0,                         /*udp recv cache buffer */
    TRUDP_SOCKET,                               /*udp socket*/
    TRUDP_SOCKET_WBUF,                    /*udp socket send buff*/
    TRUDP_SOCKET_RBUF,                     /*udp socket recv buff*/
    TRUDP_LOG_ARGS,                          /*handle log args*/
    TRUDP_RCV_QUEUE,                          /*handle recv queue,only for read*/
}TRUDPOPT;

/*default value of rudp socket option*/
#define TRUDP_DEFAULT_RCVBUF                              (5 * 30)      /*unit: packets*/
#define TRUDP_DEFAULT_SOCKET_WBUFF                  1024*1024
#define TRUDP_DEFAULT_SOCKET_RBUFF                   1024*1024



/*log priority*/
typedef enum tagTRUDPLogPriority
{
    TRUDP_LOG_FATAL = 0,
    TRUDP_LOG_ALERT = 100,
    TRUDP_LOG_CRIT = 200,
    TRUDP_LOG_ERROR = 300,
    TRUDP_LOG_WARN = 400,
    TRUDP_LOG_NOTICE = 500,
    TRUDP_LOG_INFO = 600,
    TRUDP_LOG_DEBUG = 700,
    TRUDP_LOG_TRACE = 800,
    TRUDP_LOG_NOTSET = 900,
    TRUDP_LOG_UNKNOWN = 1000,
    TRUDP_LOG_NULL = -1,
}TRUDPLOGPRIOR;



typedef struct tagTRUDPLogArgs
{
    TRUDPLOGPRIOR enLogLevel;
    /*specify the log path, for example ./log/trudpapi_%Y-%m-%d.log. see strftime about conversion specifications*/
    char szLogPattern[TRUDP_LOG_PATTERN_LEN];
}TRUDPLOGARGS, *LPTRUDPLOGARGS;


typedef struct tagTRUDPRecvQueue
{
    int *piQueueIdx;            /*INOUT,IN:Recv Queue index,-1 for head;Out:Next idx,-1 for null*/
    char *pszDataBuff;	       /*IN,data buffer*/
    int *piDataLen;              /*INOUT,IN:recv buffer len;OUT:data len,0 for null */
} TRUDPRCVQUE,*LPTRUDPRCVQUE;


#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Create socket handler.
     * @param a_phRudp[IN] The socket handler for reliable upd.
     * @retval Returns 0 for success, negative number for failure.
     *          -1: invalid input argument.
     *          -2: Wsastartup fail, only for windows.
     *          -3: out of memory.
     *          -4: init default opt failed.
     */
    TSF4G_API int trudpapi_create(IN HTRUDPAPI *a_phRudp);

    /**
     * @brief Free resource of the socket handler.
     * @param a_phRudp[IN] The socket handler for reliable upd.

*/

    TSF4G_API void trudpapi_free(IN HTRUDPAPI *a_phRudp);


    /**
     * @brief Connect the peer.
     * @param a_phRudp[IN] The socket handler
     * @param a_pszUrl[IN] The peer address to connect:eg:udp://10.6.221.106:8062
     * @param a_pstConnOpt[IN] The connection option.
     * @retval Returns 0 for success, negative number for failure.use trudpapi_geterrno get errornum
     *  ERRNO = TRUDPAPI_ERR_INVALID_INPUTARG invalid arguments
     *  ERRNO = TRUDPAPI_ERR_INVALID_URL  url invalid
     *  ERRNO = TRUDPAPI_ERR_CONNECT_TIMEOUT  connect timeout
     *  ERRNO = TRUDPAPI_ERR_PEER_CLOSE  server reset connection

*/
    TSF4G_API int trudpapi_connect(IN HTRUDPAPI a_phRudp, IN const char *a_pszUrl, IN LPTRUDPCONNOPT a_pstConnOpt);


    /**
     * @brief Send the data.
     * @param a_phRudp[IN] The socket handler for reliable upd.
     * @param a_pszbuf[IN] The received buffer.
     * @param a_iBufflen[IN] The length of send buffer.no greater than TRUDP_MAX_DATAGRAM_LEN
     * @param a_pitimeout[IN] data sent timeout value, the unit is millisecond.generally set to zero..
     * @param a_flags[IN] It must be zero, which is reserved for extendibility.
     * @retval Returns 0 for success, nagtive number for failure.use trudpapi_geterrno get errornum
     *  ERRNO = TRUDPAPI_ERR_INVALID_INPUTARG invalid arguments
     *  ERRNO = TRUDPAPI_ERR_SEND_FAIL    network send fail
     *  ERRNO = TRUDPAPI_ERR_NOT_ACTIVE  connection closed
     */
    TSF4G_API int trudpapi_send(IN HTRUDPAPI a_phRudp, IN const char* a_pszBuff, INOUT int a_iBufflen, IN int a_iTimeout, IN int a_iFlags);

    /**
     * @brief Send the ACK packet.
     * @param a_phRudp[IN] The socket handler for reliable upd.
     * @param timeout[IN] The timeout period, the unit is millisecond.
     * @retval Returns 0 for success, nagtive number for failure.
     * @retval Returns 0 for success, nagtive number for failure.use trudpapi_geterrno get errornum
     *  ERRNO = TRUDPAPI_ERR_SEND_FAIL    network send fail
     *  ERRNO = TRUDPAPI_ERR_NOT_ACTIVE  connection closed
     */
    TSF4G_API int trudpapi_send_ack(IN HTRUDPAPI a_phRudp, IN int a_iTimeout);

    /**
     * @brief Receive the data.
     * @param a_phRudp[IN] The socket handler for reliable upd.
     * @param buf[IN] The received buffer.
     * @param len[INOUT] The length of the received buffer[IN].The length of package[OUT]
     * @param timeout[IN] The timeout period, the unit is millisecond.
     * @param flags[IN] It must be zero, which is reserved for extendibility.
     * @retval Returns 0: don't return any packet; 1: return a packet; -1: recv error,use trudpapi_geterrno get errornum
     *  ERRNO = TRUDPAPI_ERR_INVALID_INPUTARG invalid arguments
     *  ERRNO = TRUDPAPI_ERR_NOT_ACTIVE  connection closed
     *  ERRNO = TRUDPAPI_ERR_INSUFFICIENT_BUFF  recv buffer insufficient
     *  ERRNO =TRUDPAPI_ERR_RETRANSMISSION_TIMEOUT retransmission timeout
     *  ERRNO = TRUDPAPI_ERR_RECV_FAIL recv error
     *  ERRNO = TRUDPAPI_ERR_PEER_CLOSE  server reset connection
     */
    TSF4G_API int trudpapi_recv(IN HTRUDPAPI a_phRudp, IN char* a_pszBuff, IN int *a_piBuffLen, IN int a_iTimeout, IN int a_iFlags);

    /**
     * @brief Get the value of the socket option for reliable udp.
     * @param a_phRudp[IN] The socket handler for reliable upd.
     * @param optname[IN] The option name.refer enum TRUDPOPT
     * @param optval[IN] The value of socket option.
     * @param optlen[INOUT] The length of socket option.
     * @retval Returns 0  for success; -1 for fail
     */
    TSF4G_API int trudpapi_getopt(IN HTRUDPAPI a_phRudp, IN TRUDPOPT a_eOptname, IN void *optval, INOUT int *a_piOptlen);

    /**
     * @brief Set the value of the socket option for reliable udp.
     * @param a_phRudp[IN] The socket handler for reliable upd.
     * @param optname[IN] The option name.
     * @param optval[IN] The value of socket option.
     * @param optlen[IN] The length of socket option.
     * @retval Returns 0  for success; -1 for fail
     */
    TSF4G_API int trudpapi_setopt(IN HTRUDPAPI a_phRudp, IN TRUDPOPT a_eOptname, IN void *a_pOptval, IN int a_iOptlen);


    /**
     * @brief get errorcode for HTRUDPAPI
     * @retval Returns errorcode
     */
    TSF4G_API int trudpapi_geterrno(IN HTRUDPAPI a_phRudp);

#ifdef __cplusplus
}
#endif

#endif
