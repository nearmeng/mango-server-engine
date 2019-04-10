/**
*
* @file     tdr_types.h
* @brief    DRʹ�õ���Ҫ���Ͷ���
*
* @author steve jackyai
* @version 1.0
* @date 2007-03-22
*
*
* Copyright (c)  2007, ��Ѷ�Ƽ����޹�˾���������з���
* All rights reserved.
*
*/



#ifndef TDR_TYPES_H
#define TDR_TYPES_H

#if defined(_WIN32) || defined(_WIN64)
    #if(_WIN32_WINNT >= 0x0400)
        #include <winsock2.h>
        #include <windows.h>
    #else /* _WIN32_WINNT */
        #include <winsock2.h>
        #include <windows.h>
        #include <Ws2tcpip.h>
    #endif /* _WIN32_WINNT */

    #include <tchar.h>
#elif defined(__MACH__) && defined(__APPLE__)
    #include <unistd.h>
    #include <machine/endian.h>
#else
    #include <unistd.h>
    #include <endian.h>
#endif

#include <stddef.h>
#include <time.h>
#include <sys/types.h>
#include "tdr/tdr_define.h"


#if defined(_WIN32) || defined(_WIN64)

    #if defined (LITTLEENDIAN) && LITTLEENDIAN >0
        #define TDR_OS_LITTLEENDIAN
        #if defined (TDR_OS_BIGENDIAN)
        #undef TDR_OS_BIGENDIAN
        #endif
    #else
        #define TDR_OS_BIGENDIAN
        #if defined (TOS_LITTLEENDIAN)
        #undef TDR_OS_LITTLEENDIAN
        #endif
    #endif

#else

    #if __BYTE_ORDER == __LITTLE_ENDIAN
        #define TDR_OS_LITTLEENDIAN

        #if defined (TDR_OS_BIGENDIAN)
        #undef TDR_OS_BIGENDIAN
        #endif
    #else
        #define TDR_OS_BIGENDIAN
        #if defined (TDR_OS_LITTLEENDIAN)
        #undef TDR_OS_LITTLEENDIAN
    #endif
#endif

#endif


/** @defgroup TDR_TYPE TDR_��Ҫ���Ͷ���
 *  @{
 */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */



#ifndef TDR_CUSTOM_C_TYPES
#define TDR_CUSTOM_C_TYPES


    #if !defined(_WIN32) && !defined(_WIN64)
        #include <stdint.h>
        #include <inttypes.h>

    #else /*#if !defined(_WIN32) && !defined(_WIN64)*/
        //#include <crtdefs.h>

        //The stdint declaras
        typedef signed char  int8_t;
        typedef signed short int16_t;
        typedef signed int   int32_t;
        typedef unsigned char  uint8_t;
        typedef unsigned short uint16_t;
        typedef unsigned int   uint32_t;
        #if _MSC_VER >= 1300
            typedef unsigned long long 	uint64_t;
            typedef long long 	int64_t;
        #else /* _MSC_VER */
            typedef unsigned __int64	uint64_t;
            typedef __int64	int64_t;
        #endif /* _MSC_VER */

    #endif /*#if !defined(_WIN32) && !defined(_WIN64)*/


	typedef int64_t tdr_longlong;
	typedef uint64_t tdr_ulonglong;
	typedef uint16_t tdr_wchar_t;  /**<Wchar������������*/
	typedef uint32_t tdr_date_t;	/**<data������������*/
	typedef uint32_t tdr_time_t;	/**<time������������*/
	typedef uint64_t tdr_datetime_t; /**<datetime������������*/
	typedef uint32_t tdr_ip_t;  /**<IPv4��������*/
#endif /*TDR_CUSTOM_C_TYPES*/



/** @name �����������͵��ڴ�洢�ռ��С
*  @{
*/
#define TDR_SIZEOF_INT8		1	/**<8bit�����洢�ֽ��� */
#define TDR_SIZEOF_INT16	2	/**<16bit�����洢�ֽ���*/
#define TDR_SIZEOF_INT32	4	/**<32bit�����洢�ֽ���*/
#define TDR_SIZEOF_INT64	8	/**<64bit�����洢�ֽ���*/
#define TDR_SIZEOF_BYTE		TDR_SIZEOF_INT8	/**<char�洢�ֽ��� */
#define TDR_SIZEOF_SHORT	TDR_SIZEOF_INT16	/**<short���������Ĵ洢�ֽ���*/
#define TDR_SIZEOF_INT	TDR_SIZEOF_INT32	/**<int���������Ĵ洢�ֽ���*/
#define TDR_SIZEOF_LONGLONG	TDR_SIZEOF_INT64	/**<longlong���ʹ洢���ֽ���*/
/**   @}*/

#if defined(_WIN64) || (__WORDSIZE == 64)
    //64λϵͳ
    #define TDR_PALTFROM_ALIGN_VALUE		8	/**<ƽ̨ȱʡ�ֽڶ���ֵ*/
    #define TDR_SIZEOF_POINTER 8         /**<ָ������洢���ֽ���*/
    #define  TDR_POINTER_TYPE_ALIGN 8	/*ָ���������Ͷ��뷽ʽ*/
#else
    //32λϵͳ
    #define TDR_PALTFROM_ALIGN_VALUE		4	/**<ƽ̨ȱʡ�ֽڶ���ֵ*/
    #define TDR_SIZEOF_POINTER 4         /**<ָ������洢���ֽ���*/
    #define  TDR_POINTER_TYPE_ALIGN 4	/*ָ���������Ͷ��뷽ʽ*/
#endif

#define TDR_SIZEOF_POINTER32 4         /**<ָ������洢���ֽ���*/
#define TDR_SIZEOF_POINTER64 8         /**<ָ������洢���ֽ���*/

#define  TDR_POINTER_TYPE_ALIGN32 4   /*ָ���������Ͷ��뷽ʽ*/
#define  TDR_POINTER_TYPE_ALIGN64 8   /*ָ���������Ͷ��뷽ʽ*/

#if (defined(_WIN32) || defined(_WIN64)) &&  _MSC_VER < 1300
#define TDR_OS_SWAP64(x) \
    ((((x) & (uint64_t)0xff00000000000000) >> 56)                                   \
    | (((x) & (uint64_t)0x00ff000000000000) >> 40)                                 \
    | (((x) & (uint64_t)0x0000ff0000000000) >> 24)                                 \
    | (((x) & (uint64_t)0x000000ff00000000) >> 8)                                  \
    | (((x) & (uint64_t)0x00000000ff000000) << 8)                                  \
    | (((x) & (uint64_t)0x0000000000ff0000) << 24)                                 \
    | (((x) & (uint64_t)0x000000000000ff00) << 40)                                 \
    | (((x) & (uint64_t)0x00000000000000ff) << 56))
#else
#define TDR_OS_SWAP64(x) \
    ((((x) & (uint64_t)0xff00000000000000LL) >> 56)                                   \
    | (((x) & (uint64_t)0x00ff000000000000LL) >> 40)                                 \
    | (((x) & (uint64_t)0x0000ff0000000000LL) >> 24)                                 \
    | (((x) & (uint64_t)0x000000ff00000000LL) >> 8)                                  \
    | (((x) & (uint64_t)0x00000000ff000000) << 8)                                  \
    | (((x) & (uint64_t)0x0000000000ff0000) << 24)                                 \
    | (((x) & (uint64_t)0x000000000000ff00) << 40)                                 \
    | (((x) & (uint64_t)0x00000000000000ff) << 56))

#endif


#define TDR_OS_SWAP32(x) \
    ((((x) & 0xff000000) >> 24)                                  \
    | (((x) & 0x00ff0000) >> 8)                                 \
    | (((x) & 0x0000ff00) << 8)                                 \
    | (((x) & 0x000000ff) << 24))

#define TDR_OS_SWAP16(x) \
    ((((x) & 0xff00) >> 8)                                  \
    | (((x) & 0x00ff) << 8))






#if (defined(_WIN32) || defined(_WIN64))  && (_MSC_VER < 1300)
typedef int intptr_t;
typedef int ptrdiff_t;
#endif

/** a offset-alike type   */
typedef intptr_t TDRPTR;

/** a index-alike type   */
typedef int TDRIDX;

/**a offset-alike type, describe the position in a meta: struct or union */
typedef ptrdiff_t TDROFF;

typedef int32_t TDRPTR32;
typedef int64_t TDRPTR64;

typedef int32_t TDROFF32;
typedef int64_t TDROFF64;



/**
 * This is the pointer type delcaration for TDR MetaLib.
 */
typedef struct tagTDRMetaLib	*LPTDRMETALIB;

typedef struct tagTDRMetaLib32    *LPTDRMETALIB32;

typedef struct tagTDRMetaLib64    *LPTDRMETALIB64;




/**
 * This is the type delcaration for TDR MetaLib param.
 */
typedef struct tagTDRLibParam	TDRLIBPARAM;

/**
 * This is the pointer type delcaration for TDR MetaLib param.
 */
typedef struct tagTDRLibParam	*LPTDRLIBPARAM;




/**
 * This is the pointer type delcaration for TDR Meta.
 */
typedef struct tagTDRMeta	*LPTDRMETA;
typedef struct tagTDRMeta32   *LPTDRMETA32;
typedef struct tagTDRMeta64   *LPTDRMETA64;



/**
 * This is the pointer type delcaration for TDR MetaEntry.
 */
typedef struct tagTDRMetaEntry	*LPTDRMETAENTRY;
typedef struct tagTDRMetaEntry32  *LPTDRMETAENTRY32;
typedef struct tagTDRMetaEntry64  *LPTDRMETAENTRY64;

/**
 * This is the pointer type delcaration for TDR MetaIndex.
 */
typedef struct tagTDRMetaIndex	*LPTDRMETAINDEX;
typedef struct tagTDRMetaIndex32  *LPTDRMETAINDEX32;
typedef struct tagTDRMetaIndex64  *LPTDRMETAINDEX64;


/**
 * This is the type delcaration for TDR TDRData.
 */
typedef struct tagTDRData	TDRDATA;
/**
 * This is the pointer type delcaration for TDR TDRData.
 */
typedef struct tagTDRData	*LPTDRDATA;


/**
 * This is the pointer type delcaration for TDR Macro.
 */
typedef struct tagTDRMacro		*LPTDRMACRO;
typedef struct tagTDRMacro32      *LPTDRMACRO32;
typedef struct tagTDRMacro64      *LPTDRMACRO64;
/**
 * This is the pointer type delcaration for TDR Macro.
 */
typedef struct tagTDRMacrosGroup		*LPTDRMACROSGROUP;
typedef struct tagTDRMacrosGroup32        *LPTDRMACROSGROUP32;
typedef struct tagTDRMacrosGroup64        *LPTDRMACROSGROUP64;


/**
 * This is the pointer type info for c/c++ base date type info.
 */
typedef struct tagTDRCTypeInfo	*LPTDRCTYPEINFO;

/**����c/c++ ͷ�ļ�����ṹ����
*/
typedef struct tagTDRHppRule TDRHPPRULE;
typedef struct tagTDRHppRule *LPTDRHPPRULE;

/** �������ݿ����ϵͳ��Ϣ�ṹ������
*/
typedef struct tagTDRDBMS TDRDBMS;
typedef struct tagTDRDBMS *LPTDRDBMS;


/** ����DB������������
*/
typedef void * TDRDBHANDLE;
typedef TDRDBHANDLE * LPTDRDBHANDLE;

/** ����DB������������
*/
typedef void * TDRDBRESULTHANDLE;
typedef TDRDBRESULTHANDLE * LPTDRDBRESULTHANDLE;

/** DB�������ݶ��������
*/
typedef  struct tagDBObject  TDRDBOBJECT;
typedef  struct tagDBObject  *LPTDRDBOBJECT;


/**��ʼ��Ԫ���ݿ�Ľṹ��
*/
struct tagTDRLibParam
{
	int iID;        /**< Ԫ�����ID*/

	int iTagSetVersion; /**< Ԫ��������XML��ǩ���İ汾*/

	size_t iSize;		/**< Ԫ���ݵ��ڴ��С��tdr_init_lib�����ݴ����ݷ���ռ�*/
	size_t iMacrosGroupSize;      /**< �Զ�������������ռ�Ŀռ��С*/
	size_t iMetaSize;      /**< �Զ�������������ռ�Ŀռ��С*/
	size_t iStrBufSize;    /**< �ַ�����������ռ�Ĵ�С*/

	int iMaxMacros;     /**< ��Ԫ���ݿ���������궨����*/
	int iMaxMetas;	/**< ��Ԫ���ݿ���������ṹ����*/

	int iMaxMacrosGroupNum; /*��������ɵĺ궨������Ŀ, Add at TDR build Version: 0x00000008*/

	int  iResFlag;     /**< Ԥ��λ����ʱʹ��1bit*/
	int  iVersion;     /**< Ԫ����İ汾*/

	char szName[TDR_NAME_LEN];  /**< Ԫ���ݿ�����*/

};

/**
* struct of TDR DRData
*/
struct tagTDRData
{
	char* pszBuff;          /**< ����DR���ݵĻ�����ָ��*/
	size_t iBuff;		/**< ����DR���ݵĻ�������С */
};



/**����c/c++ ͷ�ļ�����ṹ
*/
struct tagTDRHppRule
{
	int iRule;	/**<����ͷ�ļ��Ĺ���*/

	/*�û��Զ�������ǰ׺ iRule������TDR_HPPRULE_ADD_CUSTOM_PREFIX��־λʱ��������*/
	char szCustomNamePrefix[TDR_MAX_CUSTOM_NAME_PREFIX_LEN];
};

/**���ݿ����ϵͳDBMS��Ϣ�ṹ
*/
struct tagTDRDBMS
{
	int iMajorVersion;				/**<DBMS�����汾��*/
	int iMinVersion;				/**<DBMS�Ĵΰ汾��*/


	/**<��DBMS�������˽������ӵ���Ϣ����ͬDBMS�������ݿ��ܲ�ͬ������mysqlϵͳ�����ֶ�ָ��mysql��������������Ϣ*/
	int iReconnectOpt;	 /*�����Ϊ���Ӷ�ʧ����ִ��SQLʧ��ʱ�����ֶ�������ʾ�Ƿ��ؽ����Ӳ�����
						 ����ִ��SQL�����ֶ�ֵΪ����ֵʱ�ؽ����ӣ������ؽ�����*/
	char szDBMSName[TDR_NAME_LEN];   /**<DBMS�����ƣ��磺mysql,oracle,sqlserver��*/
	char szDBMSConnectionInfo[TDR_DBMS_CONNECTION_INFO_LEN];
    int  iDBMSPortNum;      /**<DBMS���ݿ�������˿ں�*/
	char szDBMSCurDatabaseName[TDR_DBMS_DATABASE_NAME_LEN]; /**<DBMS��ǰ���ݿ���*/
	char szDBMSUser[TDR_DBMS_USER_LEN];					/**<DBMS��ǰ���ݿ��û�����*/
	char szDBMSPassword[TDR_DBMS_PASSWORD_LEN];			/**<DBMS�û�����*/
	char szDBMSEngine[TDR_DBMS_TABLE_ENGINE_LEN];		/**<DBMS���ݿ�������,����mysql�����ǣ�ISAM,MyISAM, InnoDB*/
	char szDBMSCharset[TDR_DBMS_TABLE_CHARTSET_LEN];     /**<DBMS���ݿ����ַ���*/
	char szDBMSSock[TDR_DBMS_SOCK_FILE_LEN];		/*��dbmsʹ��UNIX sockͨ��ʱ��ʹ�õ��ļ�*/
};



struct tagDBObject
{
	LPTDRMETA pstMeta;    /**<���ݵ�Ԫ��������*/
	int iVersion;		/**<���ݵļ��ð汾*/
	size_t  iObjSize;     /**<�������ݵ��������Ĵ�С*/
	char *pszObj;     /**<�������ݵ��������׵�ַ*/
};

typedef enum tagTDRSortMethod TDRSORTMETHOD;
typedef enum tagTDRSortMethod *LPTDRSORTMETHOD;


/**
sizeinfo/sizetype����
*/
struct tagTDRSizeInfo
{
    TDROFF iNOff;   /**<���紫����Ϣƫ��*/
    TDROFF iHOff;    /**<���ش洢��Ϣƫ��*/
    size_t iUnitSize;
    int idxSizeType;	/*����sizeinfo �ü����ʹ������ʱʹ�����ͣ���TDR_BUILD version 5ʱ����*/
};
typedef struct tagTDRSizeInfo TDRSIZEINFO;
typedef struct tagTDRSizeInfo *LPTDRSIZEINFO;

/**
sizeinfo/sizetype����(32λ)
*/
#pragma pack(4)
struct tagTDRSizeInfo32
{
    TDROFF32 iNOff;   /**<���紫����Ϣƫ��*/
    TDROFF32 iHOff;    /**<���ش洢��Ϣƫ��*/
    unsigned int iUnitSize;
    int idxSizeType;    /*����sizeinfo �ü����ʹ������ʱʹ�����ͣ���TDR_BUILD version 5ʱ����*/
};
#pragma pack()
typedef struct tagTDRSizeInfo32 TDRSIZEINFO32;
typedef struct tagTDRSizeInfo32 *LPTDRSIZEINFO32;

/**
sizeinfo/sizetype����(64λ)
*/
struct tagTDRSizeInfo64
{
    TDROFF64 iNOff;   /**<���紫����Ϣƫ��*/
    TDROFF64 iHOff;    /**<���ش洢��Ϣƫ��*/
    uint64_t iUnitSize;
    int idxSizeType;    /*����sizeinfo �ü����ʹ������ʱʹ�����ͣ���TDR_BUILD version 5ʱ����*/
};
typedef struct tagTDRSizeInfo64 TDRSIZEINFO64;
typedef struct tagTDRSizeInfo64 *LPTDRSIZEINFO64;

/*�ص��������ڱ����ṹ��Աʱ������˳�ԱΪstruct/uion������ص��˺�������ȡ���ӳ�Ա������
@param[in] a_pHandle �˳�Ա�����Ĵ�����
@param[in] a_pstChildEntry ���ӳ�Ա��Ԫ�����������
@param[in] a_iTotal ���ӳ�Ա�����鳤��
@param[in] a_Index	����˳�Ա���ڽṹ��Ϊ���飬���������鵱ǰ����ֵ������ -1
@param[out] a_ppChildHandle ��ȡ���ӳ�Ա��������ָ��
@note �����ӳ�Ա��a_pstChildEntry�������tdr_get_entry_type_meta���Ի�ô��ӳ�Ա������
@return 0 �ɹ� !0 ʧ��
*/
typedef int (*PFN_ENTER_META)(IN void *a_pHandle, IN LPTDRMETAENTRY a_pstChildEntry, IN int a_iTotal, IN int a_Index,  OUT void ** a_ppHandle);


/*�ص��������ڱ����ṹ��Աʱ������˳�ԱΪ����������,��ص��˺�����
@param[in] a_pHandle �˳�Ա�����Ĵ�����
@param[in] a_pstEntry ���ӳ�Ա��Ԫ�����������
@param[in] a_iCount �˳�Ա�����鳤��
@param[in] a_pszHostStart �˳�Ա���ش洢��ʼ��ַ
@param[in] a_iSize �˳�Ա�洢�ռ���ó���
@return 0 �ɹ� !0 ʧ��
*/
typedef int (*PFN_VISIT_SIMPLE_ENTRY)(IN void *a_pHandle, IN LPTDRMETAENTRY a_pstEntry, IN int a_iCount,
									  IN char *a_pszHostStart, IN int a_iSize);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @}*/ // TDR_TYPE DR��Ҫ���Ͷ���
#endif /* TDR_TYPES_H */
