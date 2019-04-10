
#ifndef TMNG_MIB_H
#define TMNG_MIB_H

#include "pal/pal.h"
#include "pal/ttypes.h"
#include "tmng/tmetabase.h"


#define TMIB_VERSION        0x01

#define TMIB_IPC_TYPE       '1'
#define DR_NAME_LEN         32
#define DEFAULT_MIB_DATA_SIZE       1024*1024

struct tagMIBDataHead
{
    int32_t iPeriods;               /* report periods  (minutes) */
    time_t  tReportTime;            /* report timestamp */
    time_t  tUpdateTime;            /* update timestamp */
};

typedef struct tagMIBDataHead    TMIBHEAD;
typedef struct tagMIBDataHead    *LPTMIBHEAD;

#define     MIB_HEAD_SIZE       128


struct tagTMIBEntry
{
    char    szLib[DR_NAME_LEN];
    char    szMeta[DR_NAME_LEN];
    char    szDomain[DR_NAME_LEN];
    char    szName[DR_NAME_LEN];
    int32_t iProcID;
    int32_t iVersion;       
    int32_t iOff;                   /* iOff must be page aligned. */
    int32_t iSize;                  /* Use mprotect to do protect. */
    int32_t bExtern;                /* if  use external memory */
    HANDLE  iExShm;
    int32_t iExSize;
    int32_t bDelete;                /* this entry is deleted. */
    int32_t iPeriods;               /* report periods  (minutes) */
    time_t  tReportTime;            /* report timestamp */
    time_t  tUpdateTime;            /* update timestamp */
    int32_t bSystem;                /* this entry is added by system. */    
    int32_t iReserv1;
    int32_t iReserv2;
};

typedef struct tagTMIBEntry     TMIBENTRY;
typedef struct tagTMIBEntry    *LPTMIBENTRY;

struct tagTMIBData
{    
    char        szLib[DR_NAME_LEN];     /*! input/output. */
    char        szMeta[DR_NAME_LEN];    /*! input/output. */
    char        szDomain[DR_NAME_LEN];  /*! input. */
    char        szName[DR_NAME_LEN];    /*! input. */
    int32_t     iProcID;                /* input. */
    int32_t     iVersion;               
    int32_t     iSize;                  /* input/output. */
    int32_t     bExtern;                /* input/output. */
    int32_t     iPeriods;      
    LPTMIBHEAD  pstHead;
    char       *pszData;                /* output. */
    int32_t     iEntry;                 /* output, internal used. */
};

typedef struct tagTMIBData  TMIBDATA;
typedef struct tagTMIBData *LPTMIBDATA;

/*!
 * \brief �����ڴ�����Ϣ���ݿ�ͷ��Ϣ
 * \struct TMIBINFO
 *
 */
struct tagTMIBInfo
{
    int32_t   iVersion;     /*!< �汾�� */
    int32_t   iSize;        /*!< ���� Mib���ݿ��ڹ����ڴ������ռ�õĳߴ� */
    int32_t   iMaxDatas;    /*!< ���������ɵ� \struct TMIBENTRY ���� */
    int32_t   iCurDatas;    /*!< ��ǰ�Ѿ����ɵ� \struct TMIBENTRY ���� */
    int32_t   iExternSize;  /*!< ��չ�ߴ� */
    int32_t   iHeadSize;    /*!< ͷ�ߴ磬Ҳ����ģ�Ӧ�ö���д���� */
    int32_t   iCheckSum;    /*!< У��� */
    TMIBENTRY entries[1];
};

typedef struct tagTMIBInfo  TMIBINFO;
typedef struct tagTMIBInfo *LPTMIBINFO;

/*!
 * \brief �û����������Ϣ���ݿ�ʱ�ľ���ṹ
 * \struct TMIBDESC
 *
 */
struct tagTMIBDesc
{
    HANDLE    iLock;        /*!< ��Ϣ���ݿ������ */
    HANDLE    iShm;         /*!< ��Ϣ���ݿ⹲���ڴ���ʱ�ʶ */
    int32_t   bCreate;      /*!< �Ƿ�����Ϣ���ݿ⽨���� */
    TMIBINFO *pstInfo;      /*!< ��Ϣ���ݿ�ָ�� */
};

typedef struct tagTMIBDesc  TMIBDESC;
typedef struct tagTMIBDesc *LPMIBDESC;
typedef struct tagTMIBDesc *HTMIB;

#ifdef __cplusplus
extern "C"
{
#endif

/*!
 *  \fn TSF4G_API int32_t tmib_create(OUT HTMIB      *a_phMib,
 *                                    IN  const char *a_pszKey,
 *                                    IN  int32_t     a_iDatas,
 *                                    IN  int32_t     a_iSize);
 *  \brief              �����������һ����Ϣ���ݿ���ʾ��
 *                      ������ݿⲻ���ڣ����Ƚ�����
 *  \param a_phMib      ����Ϊ�գ����ָ��ָ���λ�û���д���ؾ��
 *  \param a_pszKey     ����ı�ʶ�ַ������ڲ������������͹����ڴ�
 *  \param a_iDatas     ������ɵ���Ŀ����
 *  \param a_iSize      ������ڴ�ߴ�
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ�ܷ�Ϊ�������
 *      \li �������ʧ��
 *      \li \a a_iSize ���ù�С�����ɲ��� \a a_iDatas ����Ŀ
 *      \li �ڴ��������ʧ��
 *      \li ������ʧ��
 *      \li ȡ�ù����ڴ��ʶ��ʧ��
 *      \li ���еĿ�汾��ƥ��
 */
TSF4G_API int32_t tmib_create(OUT HTMIB      *a_phMib,
                              IN  const char *a_pszKey,
                              IN  int32_t     a_iDatas,
                              IN  int32_t     a_iSize);

/*!
 *  \fn TSF4G_API int32_t tmib_destroy(IN const char *a_pszKey);
 *  \brief              ��������ͷ�һ���Ѵ��ڵ���Ϣ���ݿ�
 *  \param a_pszKey     ����ı�ʶ�ַ������ڲ������������͹����ڴ�
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ�ܷ�Ϊ�������
 *      \li �������ʧ��
 *      \li û���ҵ� a_pszKey��Ӧ����Ϣ���ݿ�
 */
TSF4G_API int32_t tmib_destroy(IN const char *a_pszKey);

/*!
 *  \fn TSF4G_API int32_t tmib_open(OUT HTMIB      *a_phMib,
 *                                  IN  const char *a_pszKey);
 *  \brief              ����������� a_pszKey ȡ�ö�Ӧ��Ϣ���ݿ���ʾ��
 *  \param a_phMib      ����Ϊ�գ����ָ��ָ���λ�û���д���ؾ��
 *  \param a_pszKey     ����ı�ʶ�ַ������ڲ������������͹����ڴ�
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ�ܷ�Ϊ�������
 *      \li �������ʧ��
 *      \li �ڴ��������ʧ��
 *      \li ������ʧ��
 *      \li ȡ�ù����ڴ��ʶ��ʧ��
 *      \li ���еĿ�汾��ƥ��
 */
TSF4G_API int32_t tmib_open(OUT HTMIB      *a_phMib,
                            IN  const char *a_pszKey);

/*!
 *  \fn TSF4G_API int32_t tmib_close(IN HTMIB *a_phMib);
 *  \brief              ��������ر���Ϣ���ݿ⣬���ͷž���ڴ�
 *  \param a_phMib      ����Ϊ�գ����ָ��ָ��Ҫ�رյ���Ϣ���ݿ���
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ��ʱ��ʾ
 *      \li �������ʧ��
 */
TSF4G_API int32_t tmib_close(IN HTMIB *a_phMib);

/*!
 *  \fn TSF4G_API int32_t tmib_set_extern_size(INOUT HTMIB   a_hMib,
 *                                             IN    int32_t a_iExSize);
 *  \brief              ����������ö�Ӧ��Ϣ���ݿ�� iExternSize����
 *                      �������������Ϣ���ݿ���չ�ǵ����ߴ�
 *                      ���ù����л�������Ϣ���ݿ���
 *  \param a_hMib       ����Ϊ�գ����ָ������Ϣ���ݿ���
 *  \param a_iExSize    Ҫ���õ���չ�ߴ磬����������㣬�����ó�ȱʡ�ߴ�
 *                      ����ߴ���ҳ�����
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ��ʱ��ʾ
 *      \li �������ʧ��
 */
TSF4G_API int32_t tmib_set_extern_size(INOUT HTMIB   a_hMib,
                                       IN    int32_t a_iExSize);

/*!
 *  \fn TSF4G_API int32_t tmib_set_updtime_unlock(INOUT HTMIB     a_hMib,
 *                                                INOUT TMIBDATA *a_pstData,
 *                                                IN    time_t    a_tUpdTime);
 *  \brief              �����������Ϣ���ݿ��в���һ����Ŀ�����ø���ʱ��
 *                      ���ù�����û��������Ϣ���ݿ�
 *  \param a_hMib       ����Ϊ�գ����ָ������Ϣ���ݿ���
 *  \param a_pstData    ����Ϊ�գ��������Ŀ��ѯ����
 *  \param a_tUpdTime   �����õ�ʱ��
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ�ܷ�Ϊ�������
 *      \li �������ʧ��
 *      \li û���ҵ���Ӧ��Ŀ
 *      \li ��Ӧ��Ŀ����Խ��
 */
TSF4G_API int32_t tmib_set_updtime_unlock(INOUT HTMIB     a_hMib,
                                          INOUT TMIBDATA *a_pstData,
                                          IN    time_t    a_tUpdTime);

/*!
 *  \fn TSF4G_API int32_t tmib_set_updtime_now(INOUT HTMIB     a_hMib,
 *                                             INOUT TMIBDATA *a_pstData);
 *  \brief              �����������Ϣ���ݿ��в���һ����Ŀ��ˢ�¸���ʱ��
 *                      ���ù�������������Ϣ���ݿ�
 *  \param a_hMib       ����Ϊ�գ����ָ������Ϣ���ݿ���
 *  \param a_pstData    ����Ϊ�գ��������Ŀ��ѯ����
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ�ܷ�Ϊ�������
 *      \li �������ʧ��
 *      \li ��Ϣ���ݿ�����ʧ��
 *      \li û���ҵ���Ӧ��Ŀ
 *      \li ��Ӧ��Ŀ����Խ��
 */
TSF4G_API int32_t tmib_set_updtime_now(INOUT HTMIB     a_hMib,
                                       INOUT TMIBDATA *a_pstData);

/*!
 *  \fn TSF4G_API int32_t tmib_find_data_unlocked(
 *                                          IN    HTMIB     a_hMib,
 *                                          INOUT TMIBDATA *a_pstData,
 *                                          IN    int32_t   a_bIncludeDelete);
 *  \brief              ����Ϣ���ݿ��в���ƥ�� a_pstData����Ŀ
 *                      ��ѯ������û��������Ϣ���ݿ�
 *  \param a_hMib       ����Ϊ�գ����ָ������Ϣ���ݿ���
 *  \param a_pstData    ����Ϊ�գ�����ṹ�ṩ��ѯ����
 *                      ����ɹ����أ�����ṹ�л���д�ҵ�����Ŀ
 *                      \n
 *                      ��ѯ����Ϊ��
 *                      \li szName����ƥ��
 *                      \li szLib, szMeta, szDomainΪ��ʱĬ��Ϊƥ��
 *                      \li iProcID ����ƥ��
 *                      \li iVersionΪ��Ĭ��Ϊƥ��
 *                      \li iPeriodsΪ��Ĭ��Ϊƥ��
 *                      ��ѯ�ɹ�ʱ��д�� szLib, szMeta, szDomain֮����ֶ�
 *  \param a_bIncludeDelete
 *                      �����ʾ���ҷ�Χ��������ɾ������Ŀ���������
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ�ܷ�Ϊ�������
 *      \li �������ʧ��
 *      \li û���ҵ�ƥ�����Ŀ
 */
TSF4G_API int32_t tmib_find_data_unlocked(IN    HTMIB     a_hMib,
                                          INOUT TMIBDATA *a_pstData,
                                          IN    int32_t   a_bIncludeDelete);

/*!
 *  \fn TSF4G_API int32_t tmib_register_data_unlocked(
 *                                                  INOUT HTMIB     a_hMib,
 *                                                  INOUT TMIBDATA *a_pstData);
 *  \brief              ����Ϣ���ݿ������һ������Ŀ
 *                      ��ӹ�����û��������Ϣ���ݿ�
 *  \param a_hMib       ����Ϊ�գ����ָ������Ϣ���ݿ���
 *  \param a_pstData    ����Ϊ�գ�����ṹ����ӵ���Ŀ
 *                      �ɹ�ʱ��ȫ���е��ֶ�
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ�ܷ�Ϊ�������
 *      \li �������ʧ��
 *      \li a_pstData �е� iSize��������
 *      \li �Ѿ�û�д洢����Ŀ�Ŀռ���
 */
TSF4G_API int32_t tmib_register_data_unlocked(INOUT HTMIB     a_hMib,
                                              INOUT TMIBDATA *a_pstData);

/*!
 *  \fn TSF4G_API int32_t tmib_check_entry_unlocked(INOUT HTMIB     a_hMib,
 *                                                  IN    TMIBDATA *a_pstData);
 *  \brief              �����Ϣ���ݿ��е���Ŀ�������ɾ���ģ����ɾ����־
 *                      ������û��������Ϣ���ݿ�
 *  \param a_hMib       ����Ϊ�գ����ָ������Ϣ���ݿ���
 *  \param a_pstData    ����Ϊ�գ�����ṹ�ǲ�ѯ����
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ��ʱ��ʾ
 *      \li �������ʧ��
 */
TSF4G_API int32_t tmib_check_entry_unlocked(INOUT HTMIB     a_hMib,
                                            IN    TMIBDATA *a_pstData);

/*!
 *  \fn TSF4G_API int32_t tmib_register_data(INOUT HTMIB     a_hMib,
 *                                           INOUT TMIBDATA *a_pstData);
 *  \brief              ����Ϣ���ݿ������һ������Ŀ
 *                      ��ӹ�������������Ϣ���ݿ�
 *  \param a_hMib       ����Ϊ�գ����ָ������Ϣ���ݿ���
 *  \param a_pstData    ����Ϊ�գ�����ṹ����ӵ���Ŀ
 *                      �ɹ�ʱ��ȫ���е��ֶ�
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ�ܷ�Ϊ�������
 *      \li �������ʧ��
 *      \li ��Ϣ���ݿ�����ʧ��
 *      \li ���� tmib_register_data_unlocked�� tmib_check_entry_unlockedʧ��
 */
TSF4G_API int32_t tmib_register_data(INOUT HTMIB     a_hMib,
                                     INOUT TMIBDATA *a_pstData);

/*!
 *  \fn TSF4G_API int32_t tmib_register_data_by_mbhd(INOUT HTMIB     a_hMib,
 *                                                   IN    HTMBDESC  a_hDesc,
 *                                                   INOUT TMIBDATA *a_pstData);
 *  \brief              ����Ԫ���ݿ��е���Ŀ���������� a_pstData�� iSize 
 *                      ������Ϣ���ݿ�������������Ŀ
 *                      ��ӹ�������������Ϣ���ݿ�
 *  \param a_hMib       ����Ϊ�գ����ָ������Ϣ���ݿ���
 *  \param a_hDesc      ����Ϊ�գ����ָ����Ԫ���ݿ���
 *  \param a_pstData    ����Ϊ�գ�����ṹ����ӵ���Ŀ
 *                      �ɹ�ʱ��ȫ���е��ֶ�
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ�ܷ�Ϊ�������
 *      \li �������ʧ��
 *      \li Ԫ���ݿ���û���ҵ���Ӧ����Ŀ
 *      \li ���� tmib_register_data ʧ��
 */
TSF4G_API int32_t tmib_register_data_by_mbhd(INOUT HTMIB     a_hMib,
                                             IN    HTMBDESC  a_hDesc,
                                             INOUT TMIBDATA *a_pstData);

/*!
 *  \fn TSF4G_API int32_t tmib_register_data_by_mbkey(
 *                                              INOUT HTMIB       a_hMib,
 *                                              IN    const char *a_pszBase,
 *                                              INOUT TMIBDATA   *a_pstData);
 *  \brief              ʹ�ô� a_pszBase��ѯԪ���ݿ��е���Ŀ����
 *                      ������ a_pstData�� iSize 
 *                      ������Ϣ���ݿ�������������Ŀ
 *                      ��ӹ�������������Ϣ���ݿ�
 *  \param a_hMib       ����Ϊ�գ����ָ������Ϣ���ݿ���
 *  \param a_pszBase    ����Ϊ�գ��������Ԫ���ݿ��ʶ
 *  \param a_pstData    ����Ϊ�գ�����ṹ����ӵ���Ŀ
 *                      �ɹ�ʱ��ȫ���е��ֶ�
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ�ܷ�Ϊ�������
 *      \li �������ʧ��
 *      \li ��Ԫ���ݿ�ʧ��
 *      \li ���� tmib_register_data_by_mbhd ʧ��
 */
TSF4G_API int32_t tmib_register_data_by_mbkey(INOUT HTMIB       a_hMib,
                                              IN    const char *a_pszBase,
                                              INOUT TMIBDATA   *a_pstData);

/*!
 *  \fn TSF4G_API int32_t tmib_protect_data(IN TMIBDATA *a_pstData,
 *                                          IN int32_t   a_bReadOnly);
 *  \brief              ���� a_bReadOnly���� a_pstData������Ϊֻ����ɶ�д
 *  \param a_pstData    ����Ϊ�գ�����ṹ�ṩ�����ö�д���Ե�����
 *  \param a_bReadOnly  �����ʾ���ҵ�����ĿΪֻ��������ɶ���д
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ�ܷ�Ϊ�������
 *      \li �������ʧ��
 *      \li a_pstData �� pszDataΪ��
 *      \li a_pstData �� iSize��������
 */
TSF4G_API int32_t tmib_protect_data(IN TMIBDATA *a_pstData,
                                    IN int32_t   a_bReadOnly);

/*!
 *  \fn TSF4G_API int32_t tmib_unregister_data_unlocked(
 *                                                  INOUT HTMIB     a_hMib,
 *                                                  IN    TMIBDATA *a_pstData);
 *  \brief              ����Ϣ���ݿ��в���ƥ�� a_pstData����Ŀ
 *                      ����ҵ�����Ѷ�Ӧ��Ŀ����Ϊ��ɾ��
 *                      ������ִ�й�����û��������Ϣ���ݿ�
 *  \param a_hMib       ����Ϊ�գ����ָ������Ϣ���ݿ���
 *  \param a_pstData    ����Ϊ�գ�����ṹ�ṩ��ѯ����
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ�ܷ�Ϊ�������
 *      \li �������ʧ��
 *      \li û���ҵ�ƥ�����Ŀ
 */
TSF4G_API int32_t tmib_unregister_data_unlocked(INOUT HTMIB     a_hMib,
                                                IN    TMIBDATA *a_pstData);

/*!
 *  \fn TSF4G_API int32_t tmib_unregister_data(INOUT HTMIB     a_hMib,
 *                                             IN    TMIBDATA *a_pstData);
 *  \brief              ����Ϣ���ݿ��в���ƥ�� a_pstData����Ŀ
 *                      ����ҵ�����Ѷ�Ӧ��Ŀ����Ϊ��ɾ��
 *                      ������ִ�й�������������Ϣ���ݿ�
 *  \param a_hMib       ����Ϊ�գ����ָ������Ϣ���ݿ���
 *  \param a_pstData    ����Ϊ�գ�����ṹ�ṩ��ѯ����
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ�ܷ�Ϊ�������
 *      \li �������ʧ��
 *      \li û���ҵ�ƥ�����Ŀ
 */
TSF4G_API int32_t tmib_unregister_data(INOUT HTMIB     a_hMib,
                                       IN    TMIBDATA *a_pstData);

/*!
 *  \fn TSF4G_API int32_t tmib_unregister_domain_unlocked(
 *                                          INOUT HTMIB       a_hMib,
 *                                          IN    const char *a_pszDomain);
 *  \brief              ����Ϣ���ݿ��н�ƥ�����Ŀ��ʶΪɾ��
 *  \param a_hMib       ����Ϊ�գ����ָ������Ϣ���ݿ���
 *  \param a_pszDomain  ����Ϊ�գ������ָʾ��ɾ���� szDomain
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ��ʱ��ʾ
 *      \li �������ʧ��
 */
TSF4G_API int32_t tmib_unregister_domain_unlocked(
                                                INOUT HTMIB       a_hMib,
                                                IN    const char *a_pszDomain);

/*!
 *  \fn TSF4G_API int32_t tmib_unregister_domain(INOUT HTMIB       a_hMib,
 *                                               IN    const char *a_pszDomain);
 *  \brief              ����Ϣ���ݿ��н�ƥ�����Ŀ��ʶΪɾ��
 *                      �� tmib_unregister_domain_unlocked��֮ͬ��
 *                      �Ǳ�����ɾ������������������
 *  \param a_hMib       ����Ϊ�գ����ָ������Ϣ���ݿ���
 *  \param a_pszDomain  ����Ϊ�գ������ָʾ��ɾ���� szDomain
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ��ʱ��ʾ
 *      \li �������ʧ��
 */
TSF4G_API int32_t tmib_unregister_domain(INOUT HTMIB       a_hMib,
                                         IN    const char *a_pszDomain);

/*!
 *  \fn TSF4G_API int32_t tmib_get_data(IN    HTMIB     a_hMib,
 *                                      INOUT TMIBDATA *a_pstData,
 *                                      IN    int32_t   a_bReadOnly);
 *  \brief              ����Ϣ���ݿ��в���ƥ�� a_pstData����Ŀ
 *  \param a_hMib       ����Ϊ�գ����ָ������Ϣ���ݿ���
 *  \param a_pstData    ����Ϊ�գ�����ṹ�ṩ��ѯ����
 *                      ����ɹ����أ�����ṹ�л���д�ҵ�����Ŀ
 *                      \n
 *                      ��ѯ����Ϊ��
 *                      \li szDomain�� szName����ƥ��
 *                      \li iProcID Ϊ��Ĭ��Ϊƥ��
 *                      \li iVersionΪ����ߴ�ƥ�����ĿΪ�㣬Ĭ��Ϊƥ��
 *                      \li iPeriodsΪ����ߴ�ƥ�����ĿΪ�㣬Ĭ��Ϊƥ��
 *                      ��ѯ�ɹ�ʱ��д�� iProcID�� bExtern֮����ֶ�
 *  \param a_bReadOnly  �����ʾ���ҵ�����ĿΪֻ��������ɶ���д
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ�ܷ�Ϊ�������
 *      \li �������ʧ��
 *      \li û���ҵ�ƥ�����Ŀ
 *      \li �ҵ�����Ŀû��ҳ����
 */
TSF4G_API int32_t tmib_get_data(IN    HTMIB     a_hMib,
                                INOUT TMIBDATA *a_pstData,
                                IN    int32_t   a_bReadOnly);

/*!
 *  \fn TSF4G_API int32_t tmib_validate_data(IN    HTMIB     a_hMib,
 *                                           INOUT TMIBDATA *a_pstData,
 *                                           IN    int32_t   a_bReadOnly);
 *  \brief              У�� a_pstData����Ϣ���ݿ����Ƿ����
 *  \param a_hMib       ����Ϊ�գ����ָ������Ϣ���ݿ���
 *  \param a_pstData    ����Ϊ�գ�����ṹ�Ǵ�У�������
 *  \param a_bReadOnly  ��ѯ a_hMib ʱ�����ʾ���ҵ�����ĿΪֻ��������ɶ���д
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ�ܷ�Ϊ�������
 *      \li �������ʧ��
 *      \li a_pstData ��������Խ��
 *      \li a_pstData ��ʶΪɾ����������Ϣ���ݿ����Ҳ�����Ӧ��Ŀ
 */
TSF4G_API int32_t tmib_validate_data(IN    HTMIB     a_hMib,
                                     INOUT TMIBDATA *a_pstData,
                                     IN    int32_t   a_bReadOnly);

/*!
 *  \fn TSF4G_API int32_t tmib_dump_head(IN HTMIB  a_hMib,
 *                                       IN FILE  *a_fp);
 *  \brief              �������������Ϣ���ݿ��������ͷ���ݵ� a_fp ��
 *  \param a_hMib       ����Ϊ�գ����ָ���Ǵ��������Ϣ���ݿ���
 *  \param a_fp         ������������͵�����ļ������
 *                      ��� a_fp �ǿգ�������� stdout
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ��ʱ��ʾ
 *      \li \a a_hMib Ϊ��
 */
TSF4G_API int32_t tmib_dump_head(IN HTMIB  a_hMib,
                                 IN FILE  *a_fp);

/*!
 *  \fn TSF4G_API int32_t tmib_dump(IN HTMBDESC  a_hDesc,
 *                                  IN TMIBDATA *a_pstData,
 *                                  IN FILE     *a_fp);
 *  \brief              ����ָ���� a_pstData��ָ���� Lib�� Meta ����Ԫ���ݿ�
 *                      ����ҵ���������ҵ���Ԫ���ݸ�ʽ�� a_pstData
 *                      ���Ѹ�ʽ����Ľ������� a_fp �ļ������
 *  \param a_hDesc      ����Ϊ�գ����ָ����Ԫ���ݿ���
 *  \param a_pstData    ����Ϊ�գ�����ṹ�е� Lib�� Meta �ǲ�ѯ����
 *  \param a_fp         ������������͵�����ļ������
 *                      ��� a_fp �ǿգ�������� stdout
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ�ܷ�Ϊ�������
 *      \li �������ʧ��
 *      \li û���ҵ�ƥ�����Ŀ
 *      \li ���� tdr_fprintf����ʧ��
 */
TSF4G_API int32_t tmib_dump(IN HTMBDESC  a_hDesc,
                            IN TMIBDATA *a_pstData,
                            IN FILE     *a_fp);

/*!
 *  \fn TSF4G_API int32_t tmib_dump_once(IN const char *a_pszBase,
 *                                       IN TMIBDATA   *a_pstData,
 *                                       IN FILE       *a_fp);
 *  \brief              ����ָ���� a_pstData��ָ���� Lib�� Meta ����Ԫ���ݿ�
 *                      Ԫ���ݿ��ɸ����ı�ʶ�� a_pszBase��ȡ��
 *                      ����ҵ���������ҵ���Ԫ���ݸ�ʽ�� a_pstData
 *                      ���Ѹ�ʽ����Ľ������� a_fp �ļ������
 *  \param a_pszBase    ����Ϊ�գ������ʶ�ַ������ڲ������������͹����ڴ�
 *                      ͨ��������ҵ���Ӧ��Ԫ���ݿ�
 *  \param a_pstData    ����Ϊ�գ�����ṹ�е� Lib�� Meta �ǲ�ѯ����
 *  \param a_fp         ������������͵�����ļ������
 *                      ��� a_fp �ǿգ�������� stdout
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ�ܷ�Ϊ�������
 *      \li �������ʧ��
 *      \li û���ҵ�ƥ�����Ŀ
 *      \li ���� tdr_fprintf����ʧ��
 */
TSF4G_API int32_t tmib_dump_once(IN const char *a_pszBase,
                                 IN TMIBDATA   *a_pstData,
                                 IN FILE       *a_fp);

/*!
 *  \fn TSF4G_API int32_t tmib_dump_all(IN HTMIB       a_hMib,
 *                                      IN const char *a_pszBase,
 *                                      IN FILE       *a_fp);
 *  \brief              �����������Ϣ���ݿ��е�������Ŀ����� a_fp ��
 *                      Ԫ���ݿ�ͨ�� a_pszBase��ȡ��
 *  \param a_hMib       ����Ϊ�գ���Ϣ���ݿ���
 *  \param a_pszBase    ����Ϊ�գ������ʶ�ַ������ڲ������������͹����ڴ�
 *                      ͨ��������ҵ���Ӧ��Ԫ���ݿ�
 *  \param a_fp         ������������͵�����ļ������
 *                      ��� a_fp �ǿգ�������� stdout
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ�ܷ�Ϊ�������
 *      \li �������ʧ��
 *      \li û���ҵ� a_pszBase��Ӧ��Ԫ���ݿ�
 */
TSF4G_API int32_t tmib_dump_all(IN HTMIB       a_hMib,
                                IN const char *a_pszBase,
                                IN FILE       *a_fp);


#ifdef __cplusplus
}
#endif

#endif /* TMNG_MIB_H  */
