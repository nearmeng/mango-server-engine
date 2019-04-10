/**
*
* @file     tdr_error.h
* @brief    TDR��������
*
* @author steve jackyai
* @version 1.0
* @date 2007-03-28
*
*
* Copyright (c)  2007, ��Ѷ�Ƽ����޹�˾���������з���
* All rights reserved.
*
*/


#ifndef TDR_ERROR_H
#define TDR_ERROR_H

#include "tdr/tdr_define.h"
#include "tdr/tdr_external.h"

/** @defgroup TDR_ERR TDR_������
* @{
*@note TDR������Ļ���˼��:
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

#ifndef TMODID_DR
#define TMODID_DR    0x01
#endif

#define TDR_ERR_MAKE(level, mod, errno)    (0x80000000 | ((level)<<24) | ((mod)<<16) | (errno))

#define TDR_ERR_LEVEL_DEBUG            0x01
#define TDR_ERR_LEVEL_WARN                0x02
#define TDR_ERR_LEVEL_ERROR            0x03
#define TDR_ERR_LEVEL_SEVERE            0x04

#define TDR_SUCCESS                0
#define TDR_ERR_ERROR                -1


#define TDR_ERR_MAKE_DEBUG(errno)        TDR_ERR_MAKE(TDR_ERR_LEVEL_DEBUG, TMODID_DR, errno)
#define TDR_ERR_MAKE_WARN(errno)         TDR_ERR_MAKE(TDR_ERR_LEVEL_WARN, TMODID_DR, errno)
#define TDR_ERR_MAKE_ERROR(errno)        TDR_ERR_MAKE(TDR_ERR_LEVEL_WARN, TMODID_DR, errno)
#define TDR_ERR_MAKE_SEVERE(errno)       TDR_ERR_MAKE(TDR_ERR_LEVEL_WARN, TMODID_DR, errno)


#define TDR_ERR_IS_ERROR(error)            ( (error) & 0x80000000 )


#define TDR_SUCCEESS_WARNING(warningno)         ((TMODID_DR<<16) | (warningno))

#define TDR_ERR_START_ERRNO        1024
#define TDR_ERR_GET_ERROR_CODE(err)       (((err) & 0xFFFF) - TDR_ERR_START_ERRNO)




/**
 *TDR������붨��
 */
typedef enum
{
    TDR_ERROR_NONE = 0,                    /**< û�д���*/
    TDR_ERROR_INVALID_CUTOFF_VERSION,        /**<���ð汾������ȡֵӦ����Ԫ���ݻ�׼�汾,����С��Ԫ���ݵĵ�ǰ�汾
                                              ���Ԫ�����а汾ָʾ��������ð汾���벻С�ڰ汾ָʾ���������С�汾*/
    TDR_ERR_NET_NO_NETBUFF_SPACE,            /**<���������Ϣ������ʣ��ռ䲻��*/
    TDR_ERROR_INVALID_REFER_VALUE,        /**<��Ԫ�ص�refer����ֵ����ȷ����ֵ����Ϊ�����ұ����count����ֵС*/
    TDR_ERROR_TOO_COMPLIEX_META,        /**<Ԫ��������������������Ƕ�ײ�γ���32��*/
    TDR_ERROR_NET_UNSUPPORTED_TYPE,            /**<��֧�ֵ�Ԫ��������*/
    TDR_ERROR_NET_INVALID_STRING_LEN,        /**<string���͵�Ԫ�����ַ������ȳ�����Ԥ����󳤶�*/
    TDR_ERROR_NO_HOSTBUFF_SPACE,            /**<���ش洢������ʣ��ռ䲻��*/
    TDR_ERROR_NET_INVALID_VERSIONINDICATOR,    /**<Ԫ�����汾ָʾ����ֵ����ȷ*/
    TDR_ERROR_NO_MEMORY,                   /**< ����洢�ռ�ʧ��*/
    TDR_ERROR_XML_PARSE,                /**< XML�ļ��������� */
    TDR_ERROR_NO_XML_ROOT,              /**< XMLԪ������û�и�Ԫ��*/
    TDR_ERROR_INVAILD_XML_ROOT,         /**< ��Ч��Ԫ���ݸ�Ԫ��*/
    TDR_ERROR_NAMESPACE_CONFLICT,       /**< Ԫ�����������ֿռ��ͻ�������ܽ���Ԫ��name����ֵ��ͬ����Ϣ�ӵ�ͬһ������*/
    TDR_ERROR_MACRO_NO_NAME_ATTR,             /**< �궨��Ԫ�ر���ָ��name����*/
    TDR_ERROR_INCLUDE_NO_FILE,          /**< includeδ����file����*/
    TDR_ERROR_NO_VERSION,       /**< û��ָ��version����*/
    TDR_ERROR_ROOT_ID_CONFLICT,         /**< ��Ԫ��ID��ͻ�������ܽ���Ԫ��ID����ֵ��ͬ����Ϣ�ӵ�ͬһ������*/
    TDR_ERROR_INVALID_TAGSET_VERSION,    /**< ��֧�ֵ�Ԫ��������XML��ǩ���汾*/
    TDR_ERROR_INVALID_METALIB_PARAM,    /**< Ԫ���ݿ��������ȷ*/
    TDR_ERROR_INVALID_METALIB_CHECKSUM, /**< Ԫ���ݿ�У��Ͳ���ȷ*/
    TDR_ERROR_TOO_MUCH_MACROS,          /**< ��ӵ�Ԫ�����������еĺ궨������Ԥ�����Ҫ�� */
    TDR_ERROR_MACRO_NO_VALUE,           /**< �궨��Ԫ��û��ֵ����*/
    TDR_ERROR_UNSUPPORTED_TYPE,           /**< ��֧�ֵ��������� */
    TDR_ERROR_METALIB_ROOT_NO_NAME,         /**< Ԫ�����������Ԫ�ر���ָ��name����*/
    TDR_ERROR_NO_SPACE_FOR_MATA,            /**< û���㹻�Ŀռ�洢�Զ�����������*/
    TDR_ERROR_NO_SPACE_FOR_STRBUF,          /**< �ַ����������ռ䲻��*/
    TDR_ERROR_META_NO_NAME,                 /**< union��stuctԪ�ر������name����*/
    TDR_ERROR_META_NAME_CONFLICT,           /**< ͬ���͵�union��stuctԪ�ز�����ͬ��*/
    TDR_ERROR_UNDEFINED_MACRO_NAME,         /**< �ú���û�ж���*/
    TDR_ERROR_META_ID_CONFLICT,          /**< ͬһ��Ԫ���²��ܳ���ID��ͬ����Ԫ��*/
    TDR_ERROR_ENTRY_NO_TYPE,             /**< entryԪ�ر������type�����Ҳ���Ϊ�մ�*/
    TDR_ERROR_ENTRY_INVALID_TYPE_VALUE,     /**< entry��type����ֵ��Ч*/
    TDR_ERROR_ENTRY_INVALID_IO_VALUE,       /**< entry��io����ֵ��Ч*/
    TDR_ERROR_ENTRY_INVALID_UNIQUE_VALUE,   /**< entry��unique���Բ���ȷ����ȷȡֵΪfalse,true*/
    TDR_ERROR_ENTRY_INVALID_NOTNULL_VALUE,  /**< entry��notnull���Բ���ȷ����ȷȡֵΪfalse,true*/
    TDR_ERROR_ENTRY_INVALID_SIZE_VALUE,  /**< entry��size����ֵ����ȷ*/
    TDR_ERROR_ENTRY_IVALID_SORTKEY_VALUE,    /**<entry��sortkey����ֵ����ȷ*/
    TDR_ERROR_ENTRY_INVALID_SELECT_VALUE,    /**< entry��select����ֵ����ȷ*/
    TDR_ERROR_ENTRY_INVALID_MAXID_VALUE,     /**< entry��maxid���Բ���ȷ*/
    TDR_ERROR_ENTRY_INVALID_MINID_VALUE,     /**< entry��minid���Բ���ȷ*/
    TDR_ERROR_ENTRY_INVALID_MAXMINID_VALUE,     /**< entry��minid��maxid����ֵ����ȷ*/
    TDR_ERROR_ENTRY_INVALID_COUNT_VALUE,     /**<entry��count����ֵ����ȷ*/
    TDR_ERROR_ENTRY_INVALID_ID_VALUE,        /**<entry��id����ֵ����ȷ*/
    TDR_ERROR_ENTRY_INVALID_DEFAULT_VALUE,  /**<entry��default����ֵ����ȷ*/
    TDR_ERROR_ENTRY_INVALID_SORTMETHOD_VALUE, /**<entry��sortmethod����ֵ����ȷ*/
    TDR_ERROR_ENTRY_INVALID_DATETIME_VALUE,        /**<entry��datetime����ֵ����ȷ*/
    TDR_ERROR_ENTRY_INVAILD_DATE_VALUE,            /**<entry��date����ֵ����ȷ*/
    TDR_ERROR_ENTRY_INVALID_TIME_VALUE,            /**<entry��time����ֵ����ȷ*/
    TDR_ERROR_ENTRY_INVALID_IP_VALUE,            /**<entry��ip����ֵ����ȷ*/
    TDR_ERROR_ENTRY_INVALID_EXTENDTOTABLE_VALUE,    /**<entry��extendtotable���Բ���ȷ*/
    TDR_ERROR_META_INVALID_SIZE_VALUE,      /**<structԪ�ص�size���Բ���ȷ*/
    TDR_ERROR_META_INVALID_ALIGN_VALUE,     /**<structԪ�ص�align����ֵ����ȷ*/
    TDR_ERROR_META_INVALID_VERSIONINDICATOR_VALUE,      /**<structԪ�ص�versionindicator���Բ���ȷ*/
    TDR_ERROR_META_INVALID_SIZETYPE_VALUE,      /**< Ԫ�ص�sizetype/sizeof����ֵ����ȷ*/
    TDR_ERROR_META_INVALID_SPLITTABLEFACTOR,    /**< structԪ�ص�splittablefactor����ֵ����ȷ*/
    TDR_ERROR_META_INVALID_PRIMARYKEY,            /**< structԪ�ص�primarykey����ֵ����ȷ*/
    TDR_ERROR_META_INVALID_INDEXCOLUMN,         /**< indexԪ�ص�column����ֵ����ȷ*/
    TDR_ERROR_META_INVALID_SPLITTABLEKEY,    /**< structԪ�ص�splittablekey����ֵ����ȷ*/
    TDR_ERROR_META_INVALID_SPLITTABLERULE,    /**< structԪ�ص�splittablerule����ֵ����ȷ*/
    TDR_ERROR_META_INVALID_STRICTINPUT,        /**<structԪ�ص�strictinput����ֵ����ȷ*/
    TDR_ERROR_META_INVALID_DEPENDONSTRUCT,        /**<structԪ�ص�dependonstruct����ֵ����ȷ*/
    TDR_ERROR_INVALID_PATH_VALUE,               /**<Ԫ�ص�path����ȷ��������ȷƥ��meta�е�Ԫ��*/
    TDR_ERROR_INVALID_OFFSET,                   /**<Ԫ�ص�ƫ��ֵ����*/
    TDR_ERROR_NO_SPACE_TO_WRITE,                /**<����Ϣд��������ʱ�ռ䲻��*/
    TDR_ERROR_META_NO_ENTRY,                    /**< �Զ�����������û�а����κ��ӳ�Ա*/
    TDR_ERROR_ENTRY_INVALID_REFER_VALUE,        /**<entryԪ�ص�refer����ֵ����ȷ*/
    TDR_ERROR_ENTRY_INVALID_SIZEINFO_VALUE,     /**<entryԪ�ص�sizeinfo����ֵ����ȷ*/
    TDR_ERROR_UNSPORTED_IOSTREAM,                /**<��֧�ֵ�IO��*/
    TDR_ERROR_FAILED_TO_WRITE_FILE,                /**<д�ļ�ʧ��*/
    TDR_ERROR_FAILED_OPEN_FILE_TO_WRITE,                    /**<���ļ�дʧ��*/
    TDR_ERROR_INVALID_METALIB_FILE,                /**<����Ԫ���ݿ�Ķ������ļ���Ч*/
    TDR_ERROR_FAILED_OPEN_FILE_TO_READ,            /**<���ļ���ʧ��*/
    TDR_ERROR_VARIABLE_ARRAY_NO_REFER,          /**<�ɱ��������ָ��refer����*/
    TDR_ERROR_VARIABLE_BEFOR_SIZEINFO,          /**<Ԫ������sizeinfo��Աǰ�ĳ�Ա�Ĵ洢�ռ�����ǹ̶���*/
    TDR_ERROR_FAILED_CONVERT_CHINESE_TO_UNICODE,    /**<�����ַ���ת����unicode�ַ���ʧ��*/
    TDR_ERROR_BREACH_KEY_RESTRICTION,            /**<entryԪ�ص�ֵ�������Լ��*/
    TDR_ERROR_DB_UNSUPPORTED_DBMS,                /**<��֧�ֵ����ݿ����ϵͳDBMS*/
    TDR_ERROR_DB_UNSUPPORTED_COMPOSITE_ARRAY,    /**<��֧��Ϊ�����������������Ա���ɽ������*/
    TDR_ERROR_DB_FAILD_TO_CONNECT_SERVER,       /**<�������ݿ������ʧ��*/
    TDR_ERROR_DB_UNSUPPORTED_OPER,                /**<��֧�ֵ����ݲ���*/
    TDR_ERROR_DB_NO_PRIMARYKEY,                    /**<�ü��ð汾�޷�������Ч��������Ϣ*/
    TDR_ERROR_DB_FAILED_TO_QUERY,                    /**<ִ�����ݿ�SQL���ʧ��*/
    TDR_ERROR_DB_FAILED_TO_CONNECT,                    /**<���ݿ����Ӵ���*/
    TDR_ERROR_DB_FAILED_TO_GET_QUERY_RESULT,        /**<ȡSQL��ѯ���ʧ��*/
    TDR_ERROR_DB_NO_RESULT_SET,            /**<SQL��ѯ�����Ϊ��*/
    TDR_ERROR_DB_NO_RECORD_IN_RESULTSET, /**<�������û�и�������ݼ�¼������˴���*/
    TDR_ERROR_DB_NO_EXPECTED_FIELD,            /**<��ǰ�������в�����ָ����������*/
    TDR_ERROR_DB_UNSUPPORTED_VARIABLE_META,    /**<��֧��Ϊ�洢�ռ䲻�̶��Ľṹ���ɽ������*/
    TDR_ERROR_BUILD_VERSION_CONFLICT,        /**<����Ԫ���ݿ��ļ���tdr���ߵĺ������ݹ����汾��tdr��ĺ������ݹ������汾��һ��*/
    TDR_ERROR_DIFF_METALIB_HASH,            /**<Ԫ���ݿ��ɢ��ֵ��������ɢ��ֵ��һ��*/
    TDR_ERROR_CONFICT_INDEX_NUM,            /**<�ṹ���Ա��ʵ����������Ԥ�ƵĲ�һ��*/
    TDR_ERROR_INVALID_VERSION,                    /**<��Ա��vesion����ֵ����ȷ*/
    TDR_ERROR_DB_NOSPLITTABLE_KEY,                /**<�������ڵ����ݿ���Ƿֱ�洢��,�������ݵ�Ԫ��������û��ָ���ֱ�ؼ���*/
    TDR_ERROR_MACROGSROUP_ATTRIBUTE,            /**<����macrosgroup����ʧ��*/
    TDR_ERROR_ENTRY_INVALID_BINDMACROSGROUP,    /**<entry��bindmacrosgroup����ֵ��Ч*/
    TDR_ERROR_VALUE_BEYOND_TYPE_FIELD,                /**<��Աȡֵ�Ѿ����������͵����ڷ�Χ*/
    TDR_ERROR_META_EXTENABLE_ARRAY,                    /**<�ڿ���չ�Ľṹ�������Ա�ж����˲�������ֵ�����*/
    TDR_ERROR_FAILED_TO_CALL_ENTER_META,        /**<�����ָ��������������ݳ�Աʱ�����ûص�ʧ��*/
    TDR_ERROR_FAILED_TO_CALL_VIST_ENTRY,        /**<�����ּ��������ͳ�Աʱ,���ûص�����ʧ��*/
    TDR_ERROR_INVALID_AUTOINCREMENT,            /**<��Ա��autoincrement������Ч��Ŀǰֻ�з����������������͵ĳ�Ա���ܶ��������*/
    TDR_ERROR_INVALID_CUSTOMATTR,                /**<��Ա��custom����ֵ��Ч,��ȷ������ֵ�ĳ��Ȳ�������󳤶�����*/
    TDR_ERROR_INVALID_UNIQUEENTRYNAMEVALUE,                /**<�ṹ���uniqueentryname����ֵ��Ч,�����Ե�����ֵֻ��Ϊtrue/false*/
    TDR_ERROR_NAME_CONFLICT_WHEN_EXTEND,                /**<���ṹ���Աչ���洢ʱ���������*/
    TDR_ERROR_INVALID_PARAM,                    /**<������Ч������ÿ�������Ƿ�����ӿڵ�ǰ������Լ��*/
    TDR_ERROR_NEED_ADD_PREFIX_META_UNSPPORT_ENTRY_INDEX,    /**<��չ���洢ʱ��Ҫ���ǰ׺�Ľṹ�岻֧�ֽ������г�Ա����ƫ��������*/
    TDR_ERROR_SET_DB_OPTIONS,     /**<����db����ʧ��*/
    TDR_ERROR_INVALID_VARIABLE_ARRAY_ENTRY,        /**<�ṹ���Ա�У�ֻ�������һ����Ա�ǿɱ��������͵ĳ�Ա*/
    TDR_ERROR_INVALID_VARIABLE_STRING_ENTRY,    /**<�ṹ���Ա�У�û�ж���size���Ե�string���ͳ�Ա(��洢�ռ��ǿɱ��)ֻ����Ϊ���һ����Ա���Ҳ���Ϊ���飬*/
    TDR_ERROR_NAME_LEN_BEYOND_UPLIMIT,          /**<���ִ�̫������������󳤶�����*/
    TDR_ERROR_PLATFORM_CONFLICT,               /**<����Ԫ���ݿ��ļ���ϵͳƽ̨������tdr���ϵͳƽ̨��һ�£���ע��32λƽ̨��64λƽ̨������*/
    TDR_ERROR_UNSOLVED_INCLUDE,                 /**<xml�ļ��д����޷�������include��ϵ*/
    TDR_ERROR_INVALID_INDICATOR_VALUE,      /**<�������ݰ��еİ汾ָʾ��ֵ����ȷ����ȡֵӦ����Ԫ���ݻ�׼�汾,����С��Ԫ���ݵĵ�ǰ�汾
                                              ���Ԫ�����а汾ָʾ������汾ָʾ����ֵ���벻С�ڰ汾ָʾ���������С�汾*/
    TDR_ERROR_ENTRY_INVALID_COMPRESS,        /**<entry��compress���Բ���ȷ*/
    TDR_ERROR_ENTRY_INVALID_COMPRESS_EXTENDTOTABLE,        /**<entry��compress��extendtotable��ͻ*/
    TDR_ERROR_ENTRY_COMPRESS_FAIL,        /**<����ѹ��ʧ��*/
    TDR_ERROR_ENTRY_UNCOMPRESS_FAIL,        /**<���ݽ�ѹ��ʧ��*/
    TDR_ERROR_JSON_FAILED_TO_CREATE_PARSER, /**<����JSON������ʧ��*/
    TDR_ERROR_JSON_SHORT_OF_INPUT, /**<������JSON����������*/
    TDR_ERROR_JSON_INVALIED_FORMAT, /**<JSON�������ĸ�ʽ����ȷ*/
    TDR_ERROR_SHORT_METALIB_BUF,  /**< ���뻺�����ĳ���̫С��δ����������metalib */
    TDR_ERROR_SIZEINFO_OVERFLOW,   /**<sizeinfo���õ��ֶζ�Ӧ���ڴ�ռ�̫С������sizeֵʱ���*/
    TDR_ERROR_INVALID_STRCOMPRESS_VALUE, /**<stringcompress���Ե�ֵ����ȷ���Ϸ���ֵֻ����false��true*/
    TDR_ERROR_INVALID_STRCOMPRESS_TYPE,  /**<ֻ��string��wstring���Ϳ��Զ���stringcompress����*/
    TDR_ERROR_INVALID_USESUBTABLE_VALUE, /**<usesubtable���Ե�ֵ����ȷ���Ϸ���ֵֻ����false��true*/
    TDR_ERROR_INVALID_USESUBTABLE_TYPE,  /**<ֻ��struct���͵�entry���Զ���usesubtable����, ������Ӧ��structֻ�ܰ����������͵�entry����ѡ����Ϊrefer��entry*/
    TDR_ERROR_UNMATCHED_PRINTFORMAT, /**< ��ȡ���ӻ��ı���Ϣ��PRINTFORMATֵ���ò���*/
    TDR_ERROR_UNMATCHED_META, /**< ��ȡ���ӻ��ı���Ϣ��Ԫ������Ϣ����*/
    TDR_ERROR_UNSUITABLE_SIZEINFO, /**< �����������ͳ�Աsizeinfo���Զ��岻����Լ��*/
    TDR_ERROR_INVALID_ENCODE_TYPE, /**<enabletlv���Ե�ֵ����ȷ���Ϸ���ֵֻ����false��true*/
    TDR_ERROR_INVALID_REQUIRED,    /**<required ���Ե�ֵ����ȷ���Ϸ���ֵֻ����false��true*/
    TDR_ERROR_ENTRY_NO_VALID_ID,   /**<TLV����뷽ʽ�£� ��Աentryδ����id����ֵ��idֵ�����߽�*/
    TDR_ERROR_ENTRY_STR_NO_SIZE_ATTR,  /**< entry string��size����ֵδ����*/
    TDR_ERROR_INVALID_ENCODE_TAG,       /**<TLV����뷽ʽ�£�tagֵ����ȷ*/
    TDR_ERROR_INVALID_REQUIREDNUM,  /**<TLV����뷽ʽ�£� �����required��Ա��Ŀ����ȷ*/
    TDR_ERROR_INVALID_ARRAY_NUM,    /**<invalid array number*/
    TDR_ERROR_COUNT,                     /**<������������־*/
} TDRERROR;


/**����ɹ��������������ʱ�ľ�����Ϣ
*/
typedef enum
{
    TDR_SUCWARN_NONE = 0,                    /**< û�д���*/
    TDR_SUCWARN_MACRO_NAME_CONFLICT,        /**<����ͬ���ĺ궨��*/
    TDR_SUCWARN_CNAME_BE_TRANCATED,         /**<cname����ֵ����Ԥ����󳤶ȣ����ض�*/
    TDR_SUCWARN_DESC_VALUE_BE_TRANCATED,    /**<desc����ֵ����Ԥ����󳤶ȣ����ض�*/
    TDR_SUCWARN_NO_ID,                      /**<Ԫ��û��ָ��id����*/
    TDR_SUCWARN_NO_NAME,                    /**<Ԫ��û��ָ��name����*/
    TDR_SUCWARN_NO_VERSION,                 /**<Ԫ��û��ָ��version����*/
    TDR_SUCWARN_TRUNCATE_DATE,                /**<��������ʱ���������ݽض�*/
}TDRSUCWARNINGNO;


/*������ϴ������*/
#define TDR_ERRIMPLE_MAKE_ERROR(err)    TDR_ERR_MAKE_ERROR(TDR_ERR_START_ERRNO + (err))
#define TDR_ERRIMPLE_FAILED_EXPACT_XML     TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_XML_PARSE)
#define TDR_ERRIMPLE_NO_XML_ROOT        TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_NO_XML_ROOT)
#define TDR_ERRIMPLE_INVALID_XML_ROOT   TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_INVAILD_XML_ROOT)
#define TDR_ERRIMPLE_NAMESPACE_CONFLICT TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_NAMESPACE_CONFLICT)
#define TDR_ERRIMPLE_MACRO_NO_NAME_ATTR       TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_MACRO_NO_NAME_ATTR)
#define TDR_ERRIMPLE_NO_ROOT_VERSION    TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_NO_ROOT_VERSION)
#define TDR_ERRIMPLE_ROOT_ID_CONFLICT   TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_ROOT_ID_CONFLICT)
#define TDR_ERRIMPLE_INVALID_TAGSET_VERSION TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_INVALID_TAGSET_VERSION)
#define TDR_ERRIMPLE_INVALID_METALIB_PARAM      TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_INVALID_METALIB_PARAM)
#define TDR_ERRIMPLE_INVALID_METALIB_CHECKSUM   TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_INVALID_METALIB_CHECKSUM)
#define TDR_ERRIMPLE_NO_MEMORY          TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_NO_MEMORY)
#define TDR_ERRIMPLE_TO_MUCH_MACROS     TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_TOO_MUCH_MACROS)
#define TDR_ERRIMPLE_MACRO_NO_VALUE     TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_MACRO_NO_VALUE)
#define TDR_ERRIMPLE_INCLUDE_NO_VALUE   TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_INCLUDE_NO_FILE)
/*#define TDR_ERRIMPLE_MACRO_NAME_CONFLICT    TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_MACRO_NAME_CONFLICT)*/
#define TDR_ERRIMPLE_UNSUPPORTED_TYPE       TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_UNSUPPORTED_TYPE)
#define TDR_ERRIMPLE_METALIB_ROOT_NO_NAME   TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_METALIB_ROOT_NO_NAME)
#define TDR_ERRIMPLE_NO_SPACE_FOR_MATA      TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_NO_SPACE_FOR_MATA)
#define TDR_ERRIMPLE_NO_SPACE_FOR_STRBUF    TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_NO_SPACE_FOR_STRBUF)
#define TDR_ERRIMPLE_META_NO_NAME           TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_META_NO_NAME)
#define TDR_ERRIMPLE_META_NAME_CONFLICT     TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_META_NAME_CONFLICT)
#define TDR_ERRIMPLE_UNDEFINED_MACRO_NAME   TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_UNDEFINED_MACRO_NAME)
#define TDR_ERRIMPLE_META_ID_CONFLICT       TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_META_ID_CONFLICT)
#define TDR_ERRIMPLE_ENTRY_NO_TYPE          TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_ENTRY_NO_TYPE)
#define TDR_ERRIMPLE_ENTRY_INVALID_TYPE_VALUE   TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_ENTRY_INVALID_TYPE_VALUE)
#define TDR_ERRIMPLE_ENTRY_INVALID_IO_VALUE     TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_ENTRY_INVALID_IO_VALUE)
#define TDR_ERRIMPLE_ENTRY_INVALID_UNIQUE_VALUE TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_ENTRY_INVALID_UNIQUE_VALUE)
#define TDR_ERRIMPLE_ENTRY_INVALID_NOTNULL_VALUE    TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_ENTRY_INVALID_NOTNULL_VALUE)
#define TDR_ERRIMPLE_ENTRY_INVALID_SIZE_VALUE       TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_ENTRY_INVALID_SIZE_VALUE)
#define TDR_ERRIMPLE_ENTRY_IVALID_SORTKEY_VALUE     TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_ENTRY_IVALID_SORTKEY_VALUE)
#define TDR_ERRIMPLE_ENTRY_INVALID_SELECT_VALUE     TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_ENTRY_INVALID_SELECT_VALUE)
#define TDR_ERRIMPLE_ENTRY_INVALID_MAXID_VALUE      TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_ENTRY_INVALID_MAXID_VALUE)
#define TDR_ERRIMPLE_ENTRY_INVALID_MINID_VALUE      TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_ENTRY_INVALID_MINID_VALUE)
#define TDR_ERRIMPLE_ENTRY_INVALID_MAXMINID_VALUE      TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_ENTRY_INVALID_MAXMINID_VALUE)
#define TDR_ERRIMPLE_ENTRY_INVALID_COUNT_VALUE          TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_ENTRY_INVALID_COUNT_VALUE)
#define TDR_ERRIMPLE_ENTRY_STR_NO_SIZE_ATTR          TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_ENTRY_STR_NO_SIZE_ATTR)
#define TDR_ERRIMPLE_META_INVALID_SIZE_VALUE            TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_META_INVALID_SIZE_VALUE)
#define TDR_ERRIMPLE_META_INVALID_ALIGN_VALUE           TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_META_INVALID_ALIGN_VALUE)
#define TDR_ERRIMPLE_META_INVALID_VERSIONINDICATOR_VALUE    TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_META_INVALID_VERSIONINDICATOR_VALUE)
#define TDR_ERRIMPLE_META_INVALID_SIZETYPE_VALUE            TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_META_INVALID_SIZETYPE_VALUE)
#define TDR_ERRIMPLE_INVALID_PATH_VALUE                 TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_INVALID_PATH_VALUE)
#define TDR_ERRIMPLE_INVALID_OFFSET                 TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_INVALID_OFFSET)

#define TDR_ERRIMPLET_INVALID_REQUIREDNUM           TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_INVALID_REQUIREDNUM)
#define TDR_ERRIMPLE_INVALID_ENCODE_TAG             TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_INVALID_ENCODE_TAG)
#define TDR_ERRIMPLE_META_NO_ENTRY                  TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_META_NO_ENTRY)
#define TDR_ERRIMPLE_ENTRY_INVALID_REFER_VALUE      TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_ENTRY_INVALID_REFER_VALUE)
#define TDR_ERRIMPLE_ENTRY_INVALID_SIZEINFO_VALUE   TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_ENTRY_INVALID_SIZEINFO_VALUE)
#define TDR_ERRIMPLE_UNSPORTED_IOSTREAM                TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_UNSPORTED_IOSTREAM)
#define TDR_ERRIMPLE_FAILED_TO_WRITE_FILE            TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_FAILED_TO_WRITE_FILE)
#define TDR_ERRIMPLE_FAILED_OPEN_FILE_TO_WRITE                TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_FAILED_OPEN_FILE_TO_WRITE)
#define TDR_ERRIMPLE_INVALID_METALIB_FILE            TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_INVALID_METALIB_FILE)
#define TDR_ERRIMPLE_FAILED_OPEN_FILE_TO_READ        TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_FAILED_OPEN_FILE_TO_READ)
#define TDR_ERRIMPLE_ENTRY_INVALID_ID_VALUE            TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_ENTRY_INVALID_ID_VALUE)
#define TDR_ERRIMPLE_ENTRY_INVALID_DEFAULT_VALUE       TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_ENTRY_INVALID_DEFAULT_VALUE)
#define TDR_ERRIMPLE_NET_INVALID_CUTOFF_VERSION        TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_INVALID_CUTOFF_VERSION);
#define TDR_ERRIMPLE_INVALID_PARAM        TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_INVALID_PARAM)
#define TDR_ERRIMPLE_UNSOLVED_INCLUDE    TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_UNSOLVED_INCLUDE)
#define TDR_ERRIMPLE_INVALID_ARRAY_NUM    TDR_ERRIMPLE_MAKE_ERROR(TDR_ERROR_INVALID_ARRAY_NUM)


/**
 * ���ݴ�������ȡ������Ϣ
 * @param[in] iErrorCode �������
 *
 * @return  ������Ϣ����ָ��
 */
TDR_API  char const* tdr_error_string(IN int iErrorCode);

/**
 * Ϊ�̳߳�ʼ�� tdr TSD/TLS ����
 * @return  0   �ɹ�
 * @return !0   ʧ��
 *
 * @note ����ýӿڷ��سɹ������߳��˳�ʱҪ���� tdr_fini_for_thread �ͷ���Դ
 */
TDR_API int tdr_init_for_thread(void);

/**
 * Ϊ�߳��ͷ� tdr TSD/TLS ����
 */
TDR_API void tdr_fini_for_thread(void);

/**
 * ȡ�� tdr �������ϸ��Ϣ
 *
 * @note ���ڶ��̻߳���:
 *       �����ڵ����� tdr_init_for_thread ֮��, ���ܵ��� tdr_get_error_detail,
 *       �߳��˳�ʱ��������� tdr_fini_for_thread �ͷ� tdr_init_for_thread �������Դ��
 *       TAPP ���� tapp_create_thread �ڲ����� tdr_init_for_thread/tdr_fini_for_thread,
 *       ʹ�� tapp_create_thread �������̣߳�����ֱ�ӵ��ú��� tdr_get_error_detail.
 */
TDR_API const char* tdr_get_error_detail(void);

/**
 * ȡ�� tdr �������ϸ��Ϣ����󳤶�
 * @note �ڶ��̻߳�����ʹ�øýӿ�ʱ��Ҫ�ȵ��� tdr_init_for_thread ��ʼ�� TSD/TLS ����
 */
TDR_API int tdr_max_error_detail(void);

/** @} */ // TDR_ERR TDR������

#ifdef __cplusplus
}
#endif




#endif /* TDR_ERROR_H */
