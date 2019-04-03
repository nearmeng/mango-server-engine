/*! 
 *  \file tmetabase.h
 *  \brief Ԫ���ݿ���� API
 *  \version 1.0
 *  \date 2010-01-20
 */

#ifndef TMETABASE_H
#define TMETABASE_H

#include "pal/pal.h"
#include "pal/ttypes.h"
#include "tdr/tdr.h"

#define TMB_VERSION  0x01

#define TMB_IPC_TYPE '2'

#define TMB_DFT_KEY  30000

/*!
 * \brief �����ڴ���Ԫ���ݿ��ͷ��Ϣ
 * \struct TMETABASE
 */
struct tagTMetaBase
{
    int32_t iVersion;   /*!< �汾�� */
    int32_t iSize;      /*!< �����ù����ڴ�ߴ� */
    int32_t iMaxLibs;   /*!< ����ܹ����ɵ� Lib���� */
    int32_t iCurLibs;   /*!< ��ǰ�Ѿ����ɵ� Lib���� */
    int32_t offs[1];    /*!< ƫ�������� */
};

typedef struct tagTMetaBase  TMETABASE;
typedef struct tagTMetaBase *LPTMETABASE;

/*!
 * \brief �û�����Ԫ���ݿ�ľ��
 * \struct TMBDESC
 */
struct tagTMBDesc
{
    HANDLE       iLock;     /*!< Ԫ���ݿ������ */
    HANDLE       iShm;      /*!< Ԫ���ݿ⹲���ڴ���ʱ�ʶ�� */
    LPTMETABASE  pstBase;   /*!< Ԫ���ݿ�ָ�� */
    LPTDRMETALIB pstLib;    /*!< ��ǰ���ʵ� Libָ�� */
};

typedef struct tagTMBDesc  TMBDESC;
typedef struct tagTMBDesc *LPTMBDESC;
typedef struct tagTMBDesc *HTMBDESC;

#ifdef __cplusplus
extern "C"
{
#endif

/*!
 *  \fn TSF4G_API int32_t tmb_create(OUT HTMBDESC   *a_phDesc,
 *                                   IN  const char *a_pszKey,
 *                                   IN  int32_t     a_iMaxLibs,
 *                                   IN  int32_t     a_iSize);
 *  \brief              �����������һ��Ԫ���ݿ���ʾ��
 *                      ������ݿⲻ���ڣ����Ƚ�����
 *  \param a_phDesc     ����Ϊ�գ����óɹ�ʱ�����ָ��ָ���λ�û���д���ؾ��
 *  \param a_pszKey     ����ı�ʶ�ַ������ڲ������������͹����ڴ�
 *  \param a_iMaxLibs   ������ɵ� Lib����
 *  \param a_iSize      ������ڴ�ߴ�
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ�ܷ�Ϊ�������
 *      \li �������ʧ��
 *      \li \a a_iSize ���ù�С�����ɲ��� \a a_iMaxLibs �� Lib
 *      \li �ڴ��������ʧ��
 *      \li ������ʧ��
 *      \li ȡ�ù����ڴ��ʶ��ʧ��
 *      \li ���еĿ�汾��ƥ��
 */
TSF4G_API int32_t tmb_create(OUT HTMBDESC  *a_phDesc,
                             IN const char *a_pszKey,
                             IN int32_t     a_iMaxLibs,
                             IN int32_t     a_iSize);

/*!
 *  \fn TSF4G_API int32_t tmb_destroy(IN const char *a_pszKey);
 *  \brief              ��������ͷ� tmb_create ������Ԫ���ݿ����빲���ڴ�
 *  \param a_pszKey     ����ı�ʶ�ַ������ڲ������������͹����ڴ�
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 */
TSF4G_API int32_t tmb_destroy(IN const char *a_pszKey);

/*!
 *  \fn TSF4G_API int32_t tmb_open(OUT HTMBDESC   *a_phDesc,
 *                                 IN  const char *a_pszKey,
 *                                 IN  int32_t     a_fReadOnly);
 *  \brief              ���������һ��Ԫ���ݿ���ʾ��
 *  \param a_phDesc     ����Ϊ�գ����óɹ�ʱ�����ָ��ָ���λ�û���д���ؾ��
 *  \param a_pszKey     ����ı�ʶ�ַ������ڲ������������͹����ڴ�
 *  \param a_fReadOnly  ������㣬��ֻ��ģʽ�򿪣������д
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ�ܷ�Ϊ�������
 *      \li �������ʧ��
 *      \li �ڴ��������ʧ��
 *      \li ȡ�ù����ڴ��ʶ��ʧ��
 *      \li ȡ�ù����ڴ�ָ��ʧ��
 *      \li ���еĿ�汾��ƥ��
 */
TSF4G_API int32_t tmb_open(OUT HTMBDESC   *a_phDesc,
                           IN  const char *a_pszKey,
                           IN  int32_t     a_fReadOnly);

/*!
 *  \fn TSF4G_API int32_t tmb_close(INOUT HTMBDESC *a_phDesc);
 *  \brief              ��������ر�һ���Ѵ򿪵�Ԫ���ݿ���ʾ��
 *  \param a_phDesc     ����Ϊ�գ����óɹ������ָ��ָ���λ�û���д NULL
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ��ʱ��ʾ
 *      \li �������ʧ��
 */
TSF4G_API int32_t tmb_close(INOUT HTMBDESC *a_phDesc);

/*!
 *  \fn TSF4G_API int32_t tmb_lock(IN HTMBDESC a_hDesc);
 *  \brief              ���������һ��Ԫ���ݿ���ʾ������������
 *  \param a_hDesc      ����Ϊ�գ����ָ����Ԫ���ݿ�ķ��ʾ��
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ�ܷ�Ϊ�������
 *      \li �������ʧ��
 *      \li �������ɷ���
 *      \li ����ʧ��
 */
TSF4G_API int32_t tmb_lock(IN HTMBDESC a_hDesc);

/*!
 *  \fn TSF4G_API int32_t tmb_unlock(IN HTMBDESC a_hDesc);
 *  \brief              ���������һ��Ԫ���ݿ���ʾ������������
 *  \param a_hDesc      ����Ϊ�գ����ָ����Ԫ���ݿ���ʾ��
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ�ܷ�Ϊ�������
 *      \li �������ʧ��
 *      \li �������ɷ���
 *      \li ����ʧ��
 */
TSF4G_API int32_t tmb_unlock(IN HTMBDESC a_hDesc);

/*!
 *  \fn TSF4G_API int32_t tmb_dump(IN HTMBDESC  a_hDesc,
 *                                 IN int32_t   a_fDetail,
 *                                 IN FILE     *a_fp);
 *  \brief              ����������Ԫ���ݿ⵽�ļ������
 *  \param a_hDesc      ����Ϊ�գ����ָ����Ԫ���ݿ���ʾ��
 *  \param a_fDetail    �������������㣬���Ԫ���ݿ���ϸ��Ϣ
 *                      ����ֻ���ͷ��Ϣ
 *  \param a_fp         ����������ļ�ָ��
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ��ʱ��ʾ
 *      \li �������ʧ��
 */
TSF4G_API int32_t tmb_dump(IN HTMBDESC  a_hDesc,
                           IN int32_t   a_fDetail,
                           IN FILE     *a_fp);

/*!
 *  \fn TSF4G_API int32_t tmb_dump_files(IN HTMBDESC    a_hDesc,
 *                                       IN int32_t     a_fDetail,
 *                                       IN const char *a_szStyle,
 *                                       IN const char *a_szPath);
 *  \brief              ����������԰�Ԫ���ݿ���������ָ�ʽ���ļ���
 *  \param a_hDesc      ����Ϊ�գ����ָ����Ҫ���ʵ�Ԫ���ݿ���
 *  \param a_fDetail    ������㣬�����ļ��������ϸ��Ϣ������ֻ���ͷ��Ϣ
 *  \param a_szStyle    �����ʽ��֧�� "xml", "tdr", ����Ĭ��Ϊ "txt"
 *  \param a_szPath     ������������ļ���Ŀ¼����Ϊ�յĻ������������׼���
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ��ʱ��ʾ
 *      \li �������ʧ��
 */
TSF4G_API int32_t tmb_dump_files(IN HTMBDESC    a_hDesc,
                                 IN int32_t     a_fDetail,
                                 IN const char *a_szStyle,
                                 IN const char *a_szPath);

/*!
 *  \fn TSF4G_API int32_t tmb_append_unlocked(INOUT HTMBDESC     a_hDesc,
 *                                            IN    LPTDRMETALIB a_pszLib);
 *  \brief              ���������Ԫ���ݿ���׷��һ�� tdrԪ����
 *                      ������û��ִ��Ԫ���ݿ�����
 *  \param a_hDesc      ����Ϊ�գ����ָ����Ԫ���ݿ���ʾ��
 *  \param a_pszLib     ����Ϊ�գ����ָ����Ҫ׷�ӵ���Ŀ
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ�ܷ�Ϊ�������
 *      \li �������ʧ��
 *      \li Ԫ���ݿ�����
 *      \li ������ʧ��
 *      \li Ԫ���ݿ��д������Ԫ���ݣ����� build�汾��ͬ���߳ߴ粻ͬ
 *      \li ��� tdr�ļ�̫����������򳬳��˹����ڴ潨��ʱ���õ����ߴ�
 */
TSF4G_API int32_t tmb_append_unlocked(INOUT HTMBDESC     a_hDesc,
                                      IN    LPTDRMETALIB a_pszLib);

/*!
 *  \fn TSF4G_API int32_t tmb_append(INOUT HTMBDESC    a_hDesc,
 *                                   IN    const char *a_pszPath);
 *  \brief              ���������Ԫ���ݿ���׷��һ�� tdrԪ����
 *                      ����������Ԫ���ݿ�
 *  \param a_hDesc      ����Ϊ�գ����ָ����Ԫ���ݿ���ʾ��
 *  \param a_pszPath    tdr �ļ�·��
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ�ܷ�Ϊ�������
 *      \li �������ʧ��
 *      \li Ԫ���ݿ�����
 *      \li ������ʧ��
 *      \li Ԫ���ݿ��д������Ԫ���ݣ����� build�汾��ͬ���߳ߴ粻ͬ
 *      \li ��� tdr�ļ�̫����������򳬳��˹����ڴ潨��ʱ���õ����ߴ�
 */
TSF4G_API int32_t tmb_append(INOUT HTMBDESC    a_hDesc,
                             IN    const char *a_pszPath);

/*!
 *  \fn TSF4G_API int32_t tmb_open_metalib(INOUT HTMBDESC    a_hDesc,
 *                                         IN    const char *a_pszLib,
 *                                         IN    int32_t     a_iVersion);
 *  \brief              ��������������ֺͰ汾��Ԫ���ݿ��в��� Lib
 *                      ����ҵ���������Ԫ���ݿ����е� Lib����
 *  \param a_hDesc      ����Ϊ�գ����ָ����Ԫ���ݿ���ʾ��
 *  \param a_pszLib     Ҫ���ҵ� Lib����
 *  \param a_iVersion   �������������İ汾��
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ�ܷ�Ϊ�������
 *      \li �������ʧ��
 *      \li û���ҵ����������� Lib
 */
TSF4G_API int32_t tmb_open_metalib(INOUT HTMBDESC    a_hDesc,
                                   IN    const char *a_pszLib,
                                   IN    int32_t     a_iVersion);

/*!
 *  \fn TSF4G_API int32_t tmb_meta_by_name(IN    HTMBDESC    a_hDesc,
 *                                         IN    const char *a_pszName,
 *                                         INOUT LPTDRMETA  *a_ppstMeta);
 *  \brief              ���������Ԫ���ݿ�����ָ���� Lib���ҷ������ֵ���Ŀ
 *  \param a_hDesc      ����Ϊ�գ����ָ����Ԫ���ݿ���ʾ��
 *  \param a_pszName    ���ָ���� Lib�������ַ��������������Ŀ
 *  \param a_ppstMeta   ����Ϊ�գ�����ҵ��ˣ����ָ��ָ�����Ŀ��д��Ŀ��ַ
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ�ܷ�Ϊ�������
 *      \li �������ʧ��
 *      \li Ԫ���ݿ���û��ָ�� Lib
 *      \li ָ���� Lib��û���ҵ����ϵ���Ŀ
 */
TSF4G_API int32_t tmb_meta_by_name(IN    HTMBDESC    a_hDesc,
                                   IN    const char *a_pszName,
                                   INOUT LPTDRMETA  *a_ppstMeta);

/*!
 *  \fn TSF4G_API int32_t tmb_meta_by_id(IN    HTMBDESC   a_hDesc,
 *                                       IN    int32_t    a_iID,
 *                                       INOUT LPTDRMETA *a_ppstMeta);
 *  \brief              ���������Ԫ���ݿ�����ָ���� Lib���ҷ��� id ����Ŀ
 *  \param a_hDesc      ����Ϊ�գ����ָ����Ԫ���ݿ���ʾ��
 *  \param a_iID        ���ָ���� Lib���ҷ������ id ����Ŀ
 *  \param a_ppstMeta   ����Ϊ�գ�����ҵ��ˣ����ָ��ָ�����Ŀ��д��Ŀ��ַ
 *  \return             �ɹ����� 0; ʧ�ܷ���С���������;
 *  \n
 *      ʧ�ܷ�Ϊ�������
 *      \li �������ʧ��
 *      \li Ԫ���ݿ���û��ָ�� Lib
 *      \li ָ���� Lib��û���ҵ����ϵ���Ŀ
 */
TSF4G_API int32_t tmb_meta_by_id(IN    HTMBDESC   a_hDesc,
                                 IN    int32_t    a_iID,
                                 INOUT LPTDRMETA *a_ppstMeta);

#ifdef __cplusplus
}
#endif

#endif /* TMETABASE_H */
