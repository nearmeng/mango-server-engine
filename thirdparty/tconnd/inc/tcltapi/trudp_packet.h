/**
 * @file trudp_packet.h
 * @brief pack/unpack the data for reliable udp protocol.
 *
 * @author jiaweixu
 * @version 1.0
 * @date 2011-05-19
 *
 * Copyright (c) 2011 Tencent. All Rights Reserved.
 */

#ifndef TRUDP_PACKET_H
#define TRUDP_PACKET_H

#include "pal/pal.h"
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

#define HAS_OPTION_FLAG(flag, opt) ((flag) & (opt))
#define SET_OPTION_FLAG(flag, opt) ((flag) |= (opt))
#define CLR_OPTION_FLAG(flag, opt) ((flag) &= (~opt))

#define HAS_ACK_FLAG(flag) HAS_OPTION_FLAG(flag, TRUDP_ACK_FLAG)
#define SET_ACK_FLAG(flag) SET_OPTION_FLAG(flag, TRUDP_ACK_FLAG)
#define CLR_ACK_FLAG(flag) CLR_OPTION_FLAG(flag,TRUDP_ACK_FLAG)

#define HAS_SESSION_FLAG(flag) HAS_OPTION_FLAG(flag,TRUDP_SESSION_FLAG)
#define SET_SESSION_FLAG(flag) SET_OPTION_FLAG(flag, TRUDP_SESSION_FLAG)
#define CLR_SESSION_FLAG(flag) CLR_OPTION_FLAG(flag,TRUDP_SESSION_FLAG)

#define HAS_RELIABLE_FLAG(flag) HAS_OPTION_FLAG(flag, TRUDP_RELIABLE_FLAG)
#define SET_RELIABLE_FLAG(flag) SET_OPTION_FLAG(flag, TRUDP_RELIABLE_FLAG)
#define CLR_RELIABLE_FLAG(flag) CLR_OPTION_FLAG(flag, TRUDP_RELIABLE_FLAG)

#define RUDP_INIT_HEAD(packet, cmd)                                        \
    do{                                                                        \
        (packet).stHead.bMagic = TRUDP_MAGIC;                                  \
        (packet).stHead.bVersion = TDR_METALIB_TRUDP_VERSION;              \
        (packet).stHead.bCmd = (cmd);                                       \
        (packet).stHead.bOptionFlag = 0;                                         \
    }while(0)


#define PACK_INTEGER_TYPE_ENTRY(pszNetStart, pszNetEnd, type, integer) \
    do{                                                                    \
        if (pszNetEnd-pszNetStart < sizeof(type))                          \
        {                                                                  \
            return -2;                                                     \
        }                                                                  \
        switch (sizeof(type))                                              \
        {                                                                  \
            case 4:                                                        \
                                                                           *(type*)pszNetStart = (type)htonl((integer));              \
            pszNetStart += 4;                                          \
            break;                                                     \
            case 2:                                                        \
                                                                           *(type*)pszNetStart = (type)htons((integer));              \
            pszNetStart += 2;                                          \
            break;                                                     \
            case 1:                                                        \
                                                                           *(type*)pszNetStart++ = (integer);                          \
            break;                                                     \
            default:                                                       \
                                                                           break;                                                     \
        }                                                                  \
    }while(0)

#define UNPACK_INTEGER_TYPE_ENTRY(integer, type, pszNetStart, pszNetEnd) \
    do{                                                                    \
        if (pszNetEnd-pszNetStart < sizeof(type))                          \
        {                                                                  \
            return -2;                                                     \
        }                                                                  \
        switch (sizeof(type))                                              \
        {                                                                  \
            case 4:                                                        \
                                                                           integer = (type)ntohl(*(type*)pszNetStart);                \
            pszNetStart += 4;                                          \
            break;                                                     \
            case 2:                                                        \
                                                                           integer = (type)ntohs(*(type*)pszNetStart);                \
            pszNetStart += 2;                                          \
            break;                                                     \
            case 1:                                                        \
                                                                           integer = *(type*)pszNetStart++;                           \
            break;                                                     \
            default:                                                       \
                                                                           break;                                                     \
        }                                                                  \
    }while(0)



#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Pack the host packet for reliable udp protocol.
     * @param lpPacket[IN] The host packet.
     * @param pszBuff[OUT] The net-packet buffer.
     * @param piLen[INOUT] IN: The length of the buffer; OUT: the size of the net packet.
     * @retval Returns 0 for success, negative number for failure.
     *          -1: invalid arguments
     *          -2: network buffer insufficient for encode
     */
    TSF4G_API int trudpapi_hton(IN LPTRUDPFRAME pstPacket, OUT char *pszBuff, INOUT int *piLen);

    /**
     * @brief Unpack the network packet for reliable udp protocol.
     * @param pszBuff[IN] The network packet.
     * @param iSize[IN] The size of the network packet.
     * @param lpPacket[OUT] The host packet for reliable udp.
     * @retval Returns 0 for success, negative number for failure.
     *          -1: invalid arguments
     *          -2: network datalen insufficient for decode
     */
    TSF4G_API int trudpapi_ntoh(IN const char *pszBuff, IN int iSize, OUT LPTRUDPFRAME pstPacket);

    /**
     * @brief Pack packet cmd body
     * @param pstCmdBody[IN] cmd body
     * @param iCmdId[IN] cmd id
     * @param pszBuff[OUT] The net-packet buffer.
     * @param piLen[INOUT] IN: The length of the buffer; OUT: the size of the net packet.
     * @retval Returns 0 for success, negative number for failure.
     *          -1: invalid arguments
     *          -2: network buffer insufficient for encode
     *          -3: invalid cmdId
     */
    TSF4G_API int trudpapi_cmdbody_hton(IN TRUDPCMDBODY *pstCmdBody,IN int iCmdId, OUT char *pszBuff, INOUT int *piLen);


    /**
     * @brief unpack packet cmd body
     * @param pszBuff[IN] The network packet.
     * @param iSize[IN] The size of the network packet.
     * @param iCmdId[IN] cmd id
     * @param pstCmdBody[OUT] packet cmd body for reliable udp.
     * @retval Returns 0 for success, negative number for failure.
     *          -1: invalid arguments
     *          -2: network datalen insufficient for decode
     *          -3: invalid cmdId
     */
    TSF4G_API int trudpapi_cmdbody_ntoh(IN const char *pszBuff, IN int iSize,IN int iCmdId, OUT TRUDPCMDBODY *pstCmdBody);



#ifdef __cplusplus
}
#endif

#endif

