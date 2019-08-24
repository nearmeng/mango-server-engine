
/************************************************************
 *  File Module:    TSock5Def.h
 *  File Description: 头文件,RFC1928创建
 *  File History:
 *  <author>  <time>            <version >   <desc>
 *  Hardway    2010/10/27         1.0           create this moudle
 ***********************************************************/
/****************Sock5验证流冲(RFC1928,RFC1929)*************************
 *
 *init request:client->server
 *                    +----+----------+----------+
 *                  |VER | NMETHODS | METHODS  |
 *                   +----+----------+----------+
 *                   | 1  |    1     | 1 to 255 |
 *                  +----+----------+----------+
 *The VER field is set to X'05' for this version of the protocol.  The
 *NMETHODS field contains the number of method identifier octets that
 * appear in the METHODS field.



 *init reponse:server->client
 *                         +----+--------+
 *                         |VER | METHOD |
 *                         +----+--------+
 *                         | 1  |   1    |
 *The server selects from one of the methods given in METHODS, and
 *sends a METHOD selection message:
 *If the selected METHOD is X'FF', none of the methods listed by the
 *client are acceptable, and the client MUST close the connection.
 *   The values currently defined for METHOD are:
 *
 *          o  X'00' NO AUTHENTICATION REQUIRED
 *          o  X'01' GSSAPI
 *          o  X'02' USERNAME/PASSWORD
 *          o  X'03' to X'7F' IANA ASSIGNED
 *          o  X'80' to X'FE' RESERVED FOR PRIVATE METHODS
 *          o  X'FF' NO ACCEPTABLE METHODS

 *subsequent user/passed request:client->server
 * +----+------+----------+------+----------+
 *           |VER | ULEN |  UNAME   | PLEN |  PASSWD  |
 *           +----+------+----------+------+----------+
 *           | 1  |  1   | 1 to 255 |  1   | 1 to 255 |
 *           +----+------+----------+------+----------+
 *
 *   The VER field contains the current version of the subnegotiation,
 *   which is X'01'. The ULEN field contains the length of the UNAME field
 *   that follows. The UNAME field contains the username as known to the
 *   source operating system. The PLEN field contains the length of the
 *   PASSWD field that follows. The PASSWD field contains the password
 *   association with the given UNAME.


 *subsequent user/passed reponse:server->client
 * The server verifies the supplied UNAME and PASSWD, and sends the
 *   following response:
 *
 *                        +----+--------+
 *                        |VER | STATUS |
 *                        +----+--------+
 *                        | 1  |   1    |
 *                        +----+--------+
 *
 *   A STATUS field of X'00' indicates success. If the server returns a
 *   `failure' (STATUS value other than X'00') status, it MUST close the
 *   connection.


 *subsequent request:client->server
 *     +----+-----+-------+------+----------+----------+
 *        |VER | CMD |  RSV  | ATYP | DST.ADDR | DST.PORT |
 *        +----+-----+-------+------+----------+----------+
 *        | 1  |  1  | X'00' |  1   | Variable |    2     |
 *        +----+-----+-------+------+----------+----------+
 *
 *     Where:
 *
*         o  VER    protocol version: X'05'
*         o  CMD
*            o  CONNECT X'01'
*            o  BIND X'02'
*            o  UDP ASSOCIATE X'03'
*         o  RSV    RESERVED
*          o  ATYP   address type of following address
*             o  IP V4 address: X'01'
*             o  DOMAINNAME: X'03'
*             o  IP V6 address: X'04'
*          o  DST.ADDR       desired destination address
*          o  DST.PORT desired destination port in network octet
*             order

*subsequent request:server->client
* +----+-----+-------+------+----------+----------+
*        |VER | REP |  RSV  | ATYP | BND.ADDR | BND.PORT |
*       +----+-----+-------+------+----------+----------+
*        | 1  |  1  | X'00' |  1   | Variable |    2     |
*        +----+-----+-------+------+----------+----------+
*
*     Where:
*
*         o  VER    protocol version: X'05'
*          o  REP    Reply field:
*             o  X'00' succeeded
*             o  X'01' general SOCKS server failure
*             o  X'02' connection not allowed by ruleset
*             o  X'03' Network unreachable
*             o  X'04' Host unreachable
*             o  X'05' Connection refused
*             o  X'06' TTL expired
*             o  X'07' Command not supported
*             o  X'08' Address type not supported
*             o  X'09' to X'FF' unassigned
*          o  RSV    RESERVED
*          o  ATYP   address type of following address
*
*             o  IP V4 address: X'01'
*             o  DOMAINNAME: X'03'
*             o  IP V6 address: X'04'
*          o  BND.ADDR       server bound address
*         o  BND.PORT       server bound port in network octet order
*
*   Fields marked RESERVED (RSV) must be set to X'00'.
*
***********************************************************/

#ifndef _TSOCK5DEF_H
#define _TSOCK5DEF_H

typedef enum
{
    TSOCK5_AUTH_NONE                      =0x00,   // 代理服务器不需要验证
    TSOCK5_AUTH_GSSAPI          =0x01, // GSSAPI
    TSOCK5_AUTH_USERPWD                =0x02,      //user/password
    TSOCK5_AUTH_IANA_ASSIGNED    =0x03,
    TSOCK5_AUTH_PRIVATE_METHOD  =0xFE,
    TSOCK5_AUTH_NOACCEPTABLE      =0xFF,   // 代理服务器验证方法不支持
} TSOCK5_AUTH;

typedef enum
{
    TSOCK5_CMD_CONNECT                =   0x01,
    TSOCK5_CMD_BIND                       =   0x02,
    TSOCK5_CMD_UDP_ASSOCIATE      =   0x03,
    TSOCK5_CMD_COUNT
} TSOCK5_CMD;

typedef enum
{
    TSOCK5_ATYP_IPV4            = 0x01,
    TSOCK5_ATYP_DOMAIN        = 0x03,
    TSOCK5_ATYP_IPV6            = 0x04,
} TSOCK5_ATYP;

typedef enum
{
    TSOCK5_REP_SUCCESSED                       =0x00,
    TSOCK5_REP_GENERAL_FAIL                =0x01,
    TSOCK5_REP_CONN_NOTALLOW           =0x02,
    TSOCK5_REP_NETWORK_UNREACHABLE         =0x03,
    TSOCK5_REP_HOST_UNREACHABLE            =0x04,
    TSOCK5_REP_CONNECTION_REFUSED      =0x05,
    TSOCK5_REP_TTL_EXPIRED                 =0x06,
    TSOCK5_REP_CMD_NOT_SUPPORTED       =0x07,
    TSOCK5_REP_ADDRESS_TYPE_NOT_SUPPORTED  =0x08,
    TSOCK5_REP_COUNT
} TSOCK5_REP;


#define  TSOCK5_VER                     0x05
#define  TSOCK5_RSV                     0x00
#define  TSOCK5_AUTH_USERPWD_VER         0x01
#define  TSOCK5_AUTH_SUCCESS                  0x00








#endif
