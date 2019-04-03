/**
*
* @file     tbus_error.h
* @brief    TBUS��������
*
* @author jackyai
* @version 1.0
* @date 2008-09-12
*
*
* Copyright (c)  2008, ��Ѷ�Ƽ����޹�˾���������з���
* All rights reserved.
*
*/


#ifndef TBUS_ERROR_H
#define TBUS_ERROR_H

#include "pal/pal.h"

/** @defgroup TBUS_ERR TBUS_������
* @{
*@note TBUS������Ļ���˼��:
*  - ʹ��һ���������洢���������Ϣ��������ֵ�ĺ�������:
*      -# <0 ��ʾ����������ֵΪ�������Ĵ���
*      -# 0 ��ʾ�ɹ�
*      -# >0 ��ʾ�ɹ�����������ĳЩ��������
*  - �������洢��4�ֽڵ������У������������:
*      -# ���󼶱�: 1���ֽڣ�����Ϊ��λ�ĵ�1���ֽ�
*      -# ģ��ID:  1�ֽڣ�����Ϊ��λ�ĵ�2���ֽ�
*      -# �����:  2�ֽڣ�0-1023�ű�ʾͨ�ô��󣬸�ģ�鶨������1024��ʼ
*
*/



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef TMODID_BUS
#define TMODID_BUS  0x01
#endif

#define TBUS_ERR_MAKE(level, mod, errno)	(0x80000000 | ((level)<<24) | ((mod)<<16) | (errno))

#define TBUS_SUCCESS				0
#define TBUS_OK				0
#define TBUS_ERROR				-1

#define TBUS_ERR_IS_ERROR(error)			( (error) & 0x80000000 )
#define TBUS_ERR_START_ERRNO		1024
#define TBUS_ERR_GET_ERROR_CODE(err)       (((err) & 0xFFFF) - TBUS_ERR_START_ERRNO)

#define TBUS_ERR_LEVEL_DEBUG			0x01
#define TBUS_ERR_LEVEL_WARN				0x02
#define TBUS_ERR_LEVEL_ERROR			0x03
#define TBUS_ERR_LEVEL_SEVERE			0x04

#define TBUS_ERR_MAKE_DEBUG(errno)		TBUS_ERR_MAKE(TBUS_ERR_LEVEL_DEBUG, TMODID_BUS, errno)
#define TBUS_ERR_MAKE_WARN(errno)		TBUS_ERR_MAKE(TBUS_ERR_LEVEL_WARN, TMODID_BUS, errno)
#define TBUS_ERR_MAKE_ERROR(errno)		TBUS_ERR_MAKE(TBUS_ERR_LEVEL_WARN, TMODID_BUS, errno + TBUS_ERR_START_ERRNO)
#define TBUS_ERR_MAKE_SEVERE(errno)		TBUS_ERR_MAKE(TBUS_ERR_LEVEL_WARN, TMODID_BUS, errno)






/**
*TBUS������붨��
*/
typedef enum
{
	TBUS_ERROR_NONE = 0,                    /**< û�д���*/
	TBUS_ERROR_NO_MEMORY,					/**<�����ڴ�ʧ��*/
	TBUS_ERROR_GET_HEAD_META,				/**<��ȡtbusͷ����Ԫ��������ʧ��*/
	TBUS_ERROR_INIT_LOGSYSTEM_FAILED,		/**< ��ʼ����־ϵͳʧ��*/
	TBUS_ERROR_PARSE_ADDR_TEMPLET,			/**< ����ͨ�ŵ�ַʮ���Ƶ�ֱ�ʾģ��ʧ��*/
	TBUS_ERROR_INVALID_ADDR_STRING,			/**<���ʮ����ͨ�ŵ�ַ������ȷ*/
	TBUS_ERROR_REGISTER_BUSSID,				/**<�����ϵͳע��ҵ��IDʧ��*/
	TBUS_ERROR_SHMGET_FAILED,				/**<ͨ��shmget���乲���ڴ�ʧ��*/
	TBUS_ERROR_SHMAT_FAILED,				/**<ͨ��shmat���ع����ڴ�ʧ��*/
	TBUS_ERROR_GEN_SHMKEY_FAILED,			/**<���ɹ����ڴ��keyֵʧ��*/
	TBUS_ERROR_CHECK_GCIMSHM_FAILED,		/**<ͨ�������ڴ�У��ʧ��*/
	TBUS_ERROR_INVALID_ARG,					/**<���ݸ��ӿڵĲ�������*/
	TBUS_ERROR_NOT_INITIALIZED,				/**<Tbusϵͳ��û�г�ʼ��*/
	TBUS_ERROR_HANDLE_FULL,				/**<�ѷ���tbus��������Ѿ��ﵽ�趨�������Ŀ�������ڷ���*/
	TBUS_ERROR_INVALID_HANDLE,			/**<tbus��������Ч*/
	TBUS_ERROR_CHANNEL_NUM_LIMITED,			/**<һ��tbus�������������ͨ�����Ѿ���������*/
	TBUS_ERROR_BIND_FAILED,				/**<ͨ��ָ����ַ�󶨲����κ�ͨ��*/
	TBUS_ERROR_NO_CHANNEL,				/**<tbus���û�а��κ�ͨ��ͨ��*/
	TBUS_ERROR_CHANNEL_ADDRESS_CONFLICT,	/**<ͨ�������ڴ��м�¼�ĵ�ַ��Ϣ��GCIM�м�¼�ĵ�ַ��Ϣ��һ��*/
	TBUS_ERROR_NO_PEER_ADDRESS_MATCHED,		/**<��tbus���������κ�ͨ���ĶԶ˵�ַ����ƥ��*/
	TBUS_ERROR_NO_CHANNEL_MATCHED,			/*û����ָ��Դ��ַ��Ŀ�ĵ�ַƥ���ͨ��*/
	TBUS_ERROR_PACK_HEAD,					/**<tbusͷ�����ʧ��*/
	TBUS_ERROR_TOO_BIG_HEAD,				/**<tbusͷ�������Ϣ���ȳ��������Ԥ������ */
	TBUS_ERROR_CHANNEL_IS_FULL,				/**<tbus������Ϣͨ�������������ٷ�������*/
	TBUS_ERROR_CHANNEL_IS_EMPTY,			/**<tbus��Ϣ����ͨ���ѿգ�û���κ���Ϣ�ɽ���*/
	TBUS_ERROR_CHANNEL_DATA_CONFUSE,		/**<ͨ�������ݳ��ȴ���ֻ�ж��������ݲ��ָܻ�����*/
	TBUS_ERROR_RECV_BUFFER_LIMITED,			/**<�������ݵĻ�����̫С�������Խ����������ݰ�*/
	TBUS_ERROR_UNPACK_HEAD,					/**<tbus��Ϣͷ�����ʧ��*/
	TBUS_ERROR_FORWARD_ROUTE_LIMITED,		/**<�����ת��·���������ƣ����ܼ���ת��*/
	TBUS_ERROR_INVALID_ROUTE,				/**<·����Ϣ��Ч*/
	TBUS_ERROR_INVALID_GCIM_CONF,			/**<GCIM�����ò���ȷ*/
	TBUS_ERROR_SHMCTL_FAILED,				/*shmctl���������ڴ�ʧ��*/
	TBUS_ERROR_RELAY_NUM_LIMITED,			/**<�������תͨ�����Ѿ����������Ŀ����*/
	TBUS_ERROR_FAILED_CHECK_HEADSUM,					/**<У�����ݰ�ͷ��ʧ��*/
	TBUS_ERROR_DELETE_USED_CHANNEL,					/**<����ͨ������ʹ�ã�����ɾ����ͨ���Ĺ����ڴ�*/
	TBUS_ERROR_UNSUPPORTED_OPTION,			/**<��֧�ֵ�option*/
	TBUS_ERROR_INVALID_OPTION_VALUE,		/**<ѡ��ֵ����ȷ*/
	TBUS_ERROR_NULL_CHANNEL_HEAD,		/*tbus channel����������ݵ�ͷ���ṹָ��ΪNULL*/
	TBUS_ERROR_INVALID_CHANNEL_MAGIC,	/*TBus channel�����ڴ������м�¼��Magicƥ�䲻��*/
	TBUS_ERROR_INVALID_ALIGN,			/*Tbus ����ͨ���е��ֽڶ������ò���ȷ*/
	TBUS_ERROR_CHANNEL_SIZE_NOTMATCH,	/*TBUS  ͨ�������С�빲���ڴ�ʵ�ʴ�С��һ��*/
	TBUS_ERROR_INVALID_CHANNEL_SIZE,	/*TBUS ����ͨ���Ĵ�С�� ָ�����õ�ֵ��һ��*/
	TBUS_ERROR_INVALID_CAHNNEL_QUEUE_PARAM, /*TBus ͨ�������еĲ�������ȷ*/
    TBUS_ERROR_UNEXPECTED_SPECIAL_ADDR, /**<��Ӧ��ʹ�������TBUS��ַ*/
    TBUS_ERROR_NO_MATCHED_PEER,         /**<û��ƥ��ĶԶ˽���*/
    TBUS_ERROR_NO_MORE_PEER,            /**<û�и����peer����*/
	TBUS_ERROR_ADDR_BUFFER_LIMITED,		/**<���յ�ַ�Ļ�����̫С�������Խ������е�peer��ַ*/
	TBUS_ERROR_CHECK_GRMSHM_FAILED,		/**<·�ɹ����ڴ�У��ʧ��*/
	TBUS_ERROR_CHECK_BAMSHM_FAILED,		/**<��ַӳ�乲���ڴ�У��ʧ��*/
	TBUS_ERROR_ADDR_BIND_NUM_LIMITED,	/**<һ��tbus������������İ󶨵�ַ���Ѿ���������*/
	TBUS_ERROR_INITED_IN_NORMAL_MODE,   /**<�Ѿ�����ͨģʽ��init�ɹ��������ٵ���tbus_automatic_init*/
	TBUS_ERROR_INITED_IN_AUTOMATIC_MODE,  /**<�Ѿ����Զ�ģʽ��init�ɹ��������ٵ���tbus_init/tbus_init_ex*/
	TBUS_ERROR_CONNECT_TBUSD_FAILED,    /**<����tbusdʧ��*/
	TBUS_ERROR_GET_MGR_META,            /**<��ȡtbusd mgr��metaʧ��*/
	TBUS_ERROR_TDR_FAIL,                /**<tdr����ʧ��*/
	TBUS_ERROR_TBUSD_OPER_FAIL,         /**<��tbusd�Ľ���ʧ��*/
	TBUS_ERROR_TBUSD_RESPONSE_ERROR,    /**<tbusd���س�����Ϣ*/
	TBUS_ERROR_DST_ADDR_INVALID,        /**<Ŀ���ַ�Ƿ�*/
	TBUS_ERROR_WRONG_MODE,              /**<�����õĺ��������ڵ�ǰģʽ��ʹ��*/
    TBUS_ERROR_CHANNEL_MATCH_TWO_ROUTES,/**<ͨ�����˶��ҵ���Ӧ��·��*/
    TBUS_ERROR_HAS_BINDED,              /**<BusHandleֻ�����һ����ַ���Ѿ��󶨹���*/
    TBUS_ERROR_TIMEOUT,                 /**<������ʱ*/
    TBUS_ERROR_ZK_OPER_FAILED,          /**<zookeeper����ʧ��*/
    TBUS_ERROR_BINDED_BY_OTHER,         /**<�ѱ��������̰�*/
    TBUS_ERROR_CANNOT_CALLED_AFTER_INITED, /**<������������init֮�󱻵��� */
	TBUS_ERROR_NOT_INITED_IN_AUTOMATIC_MODE, /**<tbusδ��ʼ��Ϊ�Զ���ģʽ */
	TBUS_ERROR_MSG_PROCESSOR_NOT_SET, /**<δע����Ϣ����ص����� */
	TBUS_ERROR_NOTIFY_SOCKETFD_RESET, /**<notify socket������Ҫ����*/
	TBUS_ERROR_GET_NOTIFY_META,            /**<��ȡtbusd notify��metaʧ��*/
	TBUS_ERROR_NOPASS_ZK_AUTH,          /**<û��ͨ��zkȨ��У��*/
	TBUS_ERROR_COUNT                    /**< ������������־*/
} TBUSERROR;


#define TBUS_ERR_ARG	TBUS_ERR_MAKE_ERROR(TBUS_ERROR_INVALID_ARG)
#define TBUS_ERR_INIT	TBUS_ERR_MAKE_ERROR(TBUS_ERROR_NOT_INITIALIZED);
#define TBUS_ERR_CHANNEL_FULL	TBUS_ERR_MAKE_ERROR(TBUS_ERROR_CHANNEL_IS_FULL)
#define	TBUS_ERR_CHANNEL_EMPTY	TBUS_ERR_MAKE_ERROR(TBUS_ERROR_CHANNEL_IS_EMPTY)
#define	TBUS_ERR_CHANNEL_CONFUSE TBUS_ERR_MAKE_ERROR(TBUS_ERROR_CHANNEL_DATA_CONFUSE)
#define TBUS_ERR_LIMITED_BUF  TBUS_ERR_MAKE_ERROR(TBUS_ERROR_ADDR_BUFFER_LIMITED)




/**
* ���ݴ�������ȡ������Ϣ
* @param[in] iErrorCode �������
*
* @return  ������Ϣ����ָ��
*/
TSF4G_API const char *tbus_error_string(IN int iErrorCode);

/** @} */ // TBUS_ERR TBUS_������

#ifdef __cplusplus
}
#endif




#endif /* TBUS_ERROR_H */
