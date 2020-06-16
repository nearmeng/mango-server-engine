/**
 *
 * @file     tdr_metalib_kernel_i.h
 * @brief    TDRԪ���ݿ���Ľṹ
 *
 * @author steve jackyai
 * @version 1.0
 * @date 2007-04-16
 *
 *
 * Copyright (c)  2007, ��Ѷ�Ƽ����޹�˾���������з���
 * All rights reserved.
 *
 */

#ifndef TDR_METALIB_KERNEL_H
#define TDR_METALIB_KERNEL_H

#include <stddef.h>
#include "tdr/tdr_types.h"
#include "tdr/tdr_define.h"
#include "tdr/tdr_ctypes_info.h"
#include "tdr/tdr_error.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     * This is the type delcaration for TDR MetaLib.
     */
    typedef struct tagTDRMetaLib TDRMETALIB;
    typedef struct tagTDRMetaLib32 TDRMETALIB32;
    typedef struct tagTDRMetaLib64 TDRMETALIB64;

    /**
     * This is the type delcaration for TDR Meta.
     */
    typedef struct tagTDRMeta TDRMETA;
    typedef struct tagTDRMeta32 TDRMETA32;
    typedef struct tagTDRMeta64 TDRMETA64;

    /**
     * This is the type delcaration for TDR MetaEntry.
     */
    typedef struct tagTDRMetaEntry TDRMETAENTRY;
    typedef struct tagTDRMetaEntry32 TDRMETAENTRY32;
    typedef struct tagTDRMetaEntry64 TDRMETAENTRY64;

    /**
      �ض���net/host��Ϣ
      */
    struct tagTDRSelector
    {
        size_t iUnitSize; /**<��ռ�洢�ռ�*/
        TDROFF iHOff;     /**<���ش洢��Ϣƫ��*/
        TDRPTR ptrEntry;  /**<����entry��ָ��*/
    };

    typedef struct tagTDRSelector TDRSelector;
    typedef struct tagTDRSelector *LPTDRSelector;

    /**
      �ض���net/host��Ϣ
      */
#pragma pack(4)
    struct tagTDRSelector32
    {
        unsigned int iUnitSize; /**<��ռ�洢�ռ�*/
        TDROFF32 iHOff;         /**<���ش洢��Ϣƫ��*/
        TDRPTR32 ptrEntry;      /**<����entry��ָ��*/
    };
#pragma pack()

    typedef struct tagTDRSelector32 TDRSelector32;
    typedef struct tagTDRSelector32 *LPTDRSelector32;

    /**
      �ض���net/host��Ϣ
      */
    struct tagTDRSelector64
    {
        uint64_t iUnitSize; /**<��ռ�洢�ռ�*/
        TDROFF64 iHOff;     /**<���ش洢��Ϣƫ��*/
        TDRPTR64 ptrEntry;  /**<����entry��ָ��*/
    };

    typedef struct tagTDRSelector64 TDRSelector64;
    typedef struct tagTDRSelector64 *LPTDRSelector64;

    /*�����紦����ص��ض�����
    */
    struct tagTDRRedirector
    {
        TDROFF iNOff; /**<���紫����Ϣƫ��*/
        TDROFF iHOff; /**<���ش洢��Ϣƫ��*/
        size_t iUnitSize;
    };

    typedef struct tagTDRRedirector TDRREDIRECTOR;
    typedef struct tagTDRRedirector *LPTDRREDIRECTOR;

    /*�����紦����ص��ض�����
    */
#pragma pack(4)
    struct tagTDRRedirector32
    {
        TDROFF32 iNOff; /**<���紫����Ϣƫ��*/
        TDROFF32 iHOff; /**<���ش洢��Ϣƫ��*/
        unsigned int iUnitSize;
    };
#pragma pack()

    typedef struct tagTDRRedirector32 TDRREDIRECTOR32;
    typedef struct tagTDRRedirector32 *LPTDRREDIRECTOR32;

    /*�����紦����ص��ض�����
    */
    struct tagTDRRedirector64
    {
        TDROFF64 iNOff; /**<���紫����Ϣƫ��*/
        TDROFF64 iHOff; /**<���ش洢��Ϣƫ��*/
        uint64_t iUnitSize;
    };

    typedef struct tagTDRRedirector64 TDRREDIRECTOR64;
    typedef struct tagTDRRedirector64 *LPTDRREDIRECTOR64;

    /**
      sortkey��Ϣ
      */
    struct tagTDRSortKeyInfo
    {
        TDRIDX idxSortEntry;   /**< ���������entry��meta�е�����*/
        TDROFF iSortKeyOff;    /**< ����Ԫ�صı��ش洢ƫ��. */
        TDRPTR ptrSortKeyMeta; /**< ���������entry����meta������*/
    };
    typedef struct tagTDRSortKeyInfo TDRSORTKEYINFO;
    typedef struct tagTDRSortKeyInfo *LPTDRSORTKEYINFO;

    /**
      sortkey��Ϣ
      */
#pragma pack(4)
    struct tagTDRSortKeyInfo32
    {
        TDRIDX idxSortEntry;     /**< ���������entry��meta�е�����*/
        TDROFF32 iSortKeyOff;    /**< ����Ԫ�صı��ش洢ƫ��. */
        TDRPTR32 ptrSortKeyMeta; /**< ���������entry����meta������*/
    };
#pragma pack()
    typedef struct tagTDRSortKeyInfo32 TDRSORTKEYINFO32;
    typedef struct tagTDRSortKeyInfo32 *LPTDRSORTKEYINFO32;

    /**
      sortkey��Ϣ
      */
    struct tagTDRSortKeyInfo64
    {
        TDRIDX idxSortEntry;     /**< ���������entry��meta�е�����*/
        TDROFF64 iSortKeyOff;    /**< ����Ԫ�صı��ش洢ƫ��. */
        TDRPTR64 ptrSortKeyMeta; /**< ���������entry����meta������*/
    };
    typedef struct tagTDRSortKeyInfo64 TDRSORTKEYINFO64;
    typedef struct tagTDRSortKeyInfo64 *LPTDRSORTKEYINFO64;

    /**DB������Ϣ, new change in tbase2.7.34
    */
    struct tagTDRDBKeyInfo
    {
        TDROFF iHOff;    /**< (For splittablekey, The first) Ԫ�� �ı��ش洢ƫ�� (here to keep compatiable). */
        TDRPTR ptrEntry; /**< point to the string value of attribute "splittablekey" in xml*/
    };
    typedef struct tagTDRDBKeyInfo TDRDBKEYINFO;
    typedef struct tagTDRDBKeyInfo *LPTDRDBKEYINFO;

    /**32λDB������Ϣ
    */
#pragma pack(4)
    struct tagTDRDBKeyInfo32
    {
        TDROFF32 iHOff;    /**< Ԫ�صı��ش洢ƫ��. */
        TDRPTR32 ptrEntry; /**< entryָ��*/
    };
#pragma pack()
    typedef struct tagTDRDBKeyInfo32 TDRDBKEYINFO32;
    typedef struct tagTDRDBKeyInfo32 *LPTDRDBKEYINFO32;

    /**64λDB������Ϣ
    */
    struct tagTDRDBKeyInfo64
    {
        TDROFF64 iHOff;    /**< Ԫ�صı��ش洢ƫ��. */
        TDRPTR64 ptrEntry; /**< entryָ��*/
    };
    typedef struct tagTDRDBKeyInfo64 TDRDBKEYINFO64;
    typedef struct tagTDRDBKeyInfo64 *LPTDRDBKEYINFO64;

    /**
     * �˽ṹ����һ�����ݳ�Ա����iCountΪ0ʱ��ʾ�˳�ԱΪһ�ɱ����飬
     * struct/Union�н��������һ���ɱ������Ա
     */
    struct tagTDRMetaEntry
    {
        int iID;        /**< ��Ԫ���ݵ�ID*/
        int iVersion;   /**< ��Ԫ���ݼӵ�MetaLib����ʱ�İ汾*/
        int iType;      /**< the typeid of this entry. */
        TDRPTR ptrName; /**< ptr of name, used for generating c/c++ header. */

        size_t iHRealSize;       /**<���ش洢�����ܹ��Ŀռ�*/
        size_t iNRealSize;       /**<���紫��ʱ����ĵ��ܿռ�*/
        size_t iHUnitSize;       /**<���ش洢����entry��Ԫ����ռ�*/
        size_t iNUnitSize;       /**<����洢����entry��Ԫ����ռ�*/
        size_t iCustomHUnitSize; /**< �Զ���洢��Ԫ��С*/

        int iCount; /**< 1 means single, >1 means array, 0 means variable array */

        TDROFF iNOff; /**< ���紫��ʱ��ƫ�ƣ�1�ֽڶ���*/
        TDROFF iHOff; /**< ���ش洢ʱ��ƫ�ƣ�ʹ��ָ�����뷽ʽ*/

        TDRIDX idxID;              /**< the id macro's index. */
        TDRIDX idxVersion;         /**< ����汾ȡֵΪ�궨�壬��˳�Ա�����Ӧ�궨���ں궨���б��е�����*/
        TDRIDX idxCount;           /**< the macro's referred by count. */
        TDRIDX idxType;            /**< the index of the typeinfo. */
        TDRIDX idxCustomHUnitSize; /**< �Զ���洢��Ԫ��С�궨��ֵ������*/

        unsigned short wFlag; /**< ��ȡ��Ԫ�ض�Ӧ��־��Ϣ����ָ�룬���õ�*/
        char chDBFlag;        /**< TDR-DB �����ϵӳ�� */
        char chOrder;         /**< 1 if ascending, -1 if desending, else 0 */

        TDRSIZEINFO stSizeInfo; /**<sizeinfo����ֵ*/
        TDRSelector stRefer;    /**<refer����ֵ*/
        TDRSelector stSelector; /**<select ����*/

        int iIO;   /**< the input/output control. */
        int idxIO; /**< the idx of input/output control macro. */

        TDRPTR ptrMeta; /**<���˳�ԱΪ������������ʱ����¼�临����������Ԫ����meta���ݵ�ָ��. */

        /**<��selector��ֵ��[iMinId,iMaxID]����ʱ��ѡ���Ԫ��*/
        int iMaxId;
        int iMinId;

        /**< index of macro of iMaxId,iMinId*/
        TDRIDX iMaxIdIdx;
        TDRIDX iMinIdIdx;

        /*
         *��Ա��ʾ��, ������Ϣ����������ȱʡֵ�ַ���������Metalib������ַ����ڴ��
         *�У��˴��������ַ�������ʼ��ַ�ͳ���
         */
        size_t iDefaultValLen; /**<ȱʡֵ�ĳ���*/
        TDRPTR ptrDesc;        /**< ptr of  the description info. */
        TDRPTR ptrChineseName; /**< ptr of chinse name of entry*/
        TDRPTR ptrDefaultVal;  /**< ptr of default value of entry*/

        TDRPTR ptrMacrosGroup; /*�˳�ԱԪ�ذ󶨵ĺ궨����ָ�룬 Add at TDR build Version: 0x00000008*/
        TDRPTR ptrCustomAttr;  /*ָ���Զ�������ֵ��ָ�룬 Add at TDR build Version: 0x00000008*/
        TDRPTR iOffToMeta;     /*����ڴ˳�Ա�����ĸ������ݽṹ��������ƫ�ƣ� Add at TDR build Version: 0x00000009*/

        TDRPTR iFlag;          /**< field extend flag, 2.7.1�汾����������Ԥ���ֶ� */
        TDRPTR ptrCustomAttr2; /**< �Զ�������ָ��,2.7.29�汾����,����Ԥ���ֶ�. Add at TDR build Version: 0x0000000a */
        TDRPTR iReserve3;      /**< reserve for extend . Add at TDR build Version: 0x0000000a */
    };

    /**
     * �˽ṹ����һ�����ݳ�Ա(32λ)����iCountΪ0ʱ��ʾ�˳�ԱΪһ�ɱ����飬
     * struct/Union�н��������һ���ɱ������Ա
     */
#pragma pack(4)
    struct tagTDRMetaEntry32
    {
        int iID;          /**< ��Ԫ���ݵ�ID*/
        int iVersion;     /**< ��Ԫ���ݼӵ�MetaLib����ʱ�İ汾*/
        int iType;        /**< the typeid of this entry. */
        TDRPTR32 ptrName; /**< ptr of name, used for generating c/c++ header. */

        unsigned int iHRealSize;       /**<���ش洢�����ܹ��Ŀռ�*/
        unsigned int iNRealSize;       /**<���紫��ʱ����ĵ��ܿռ�*/
        unsigned int iHUnitSize;       /**<���ش洢����entry��Ԫ����ռ�*/
        unsigned int iNUnitSize;       /**<����洢����entry��Ԫ����ռ�*/
        unsigned int iCustomHUnitSize; /**< �Զ���洢��Ԫ��С*/

        int iCount; /**< 1 means single, >1 means array, 0 means variable array */

        TDROFF32 iNOff; /**< ���紫��ʱ��ƫ�ƣ�1�ֽڶ���*/
        TDROFF32 iHOff; /**< ���ش洢ʱ��ƫ�ƣ�ʹ��ָ�����뷽ʽ*/

        TDRIDX idxID;              /**< the id macro's index. */
        TDRIDX idxVersion;         /**< ����汾ȡֵΪ�궨�壬��˳�Ա�����Ӧ�궨���ں궨���б��е�����*/
        TDRIDX idxCount;           /**< the macro's referred by count. */
        TDRIDX idxType;            /**< the index of the typeinfo. */
        TDRIDX idxCustomHUnitSize; /**< �Զ���洢��Ԫ��С�궨��ֵ������*/

        unsigned short wFlag; /**< ��ȡ��Ԫ�ض�Ӧ��־��Ϣ����ָ�룬���õ�*/
        char chDBFlag;        /**< TDR-DB �����ϵӳ�� */
        char chOrder;         /**< 1 if ascending, -1 if desending, else 0 */

        TDRSIZEINFO32 stSizeInfo; /**<sizeinfo����ֵ*/
        TDRSelector32 stRefer;    /**<refer����ֵ*/
        TDRSelector32 stSelector; /**<select ����*/

        int iIO;   /**< the input/output control. */
        int idxIO; /**< the idx of input/output control macro. */

        TDRPTR32 ptrMeta; /**<���˳�ԱΪ������������ʱ����¼�临����������Ԫ����meta���ݵ�ָ��. */

        /**<��selector��ֵ��[iMinId,iMaxID]����ʱ��ѡ���Ԫ��*/
        int iMaxId;
        int iMinId;

        /**< index of macro of iMaxId,iMinId*/
        TDRIDX iMaxIdIdx;
        TDRIDX iMinIdIdx;

        /*
         *��Ա��ʾ��, ������Ϣ����������ȱʡֵ�ַ���������Metalib������ַ����ڴ��
         *�У��˴��������ַ�������ʼ��ַ�ͳ���
         */
        unsigned int iDefaultValLen; /**<ȱʡֵ�ĳ���*/
        TDRPTR32 ptrDesc;            /**< ptr of  the description info. */
        TDRPTR32 ptrChineseName;     /**< ptr of chinse name of entry*/
        TDRPTR32 ptrDefaultVal;      /**< ptr of default value of entry*/

        TDRPTR32 ptrMacrosGroup; /*�˳�ԱԪ�ذ󶨵ĺ궨����ָ�룬 Add at TDR build Version: 0x00000008*/
        TDRPTR32 ptrCustomAttr;  /*ָ���Զ�������ֵ��ָ�룬 Add at TDR build Version: 0x00000008*/
        TDRPTR32 iOffToMeta;     /*����ڴ˳�Ա�����ĸ������ݽṹ��������ƫ�ƣ� Add at TDR build Version: 0x00000009*/

        TDRPTR32 iFlag;          /**< field extend flag, 2.7.1�汾����������Ԥ���ֶ� */
        TDRPTR32 ptrCustomAttr2; /**< �Զ�������ָ��,2.7.29�汾����,����Ԥ���ֶ�. Add at TDR build Version: 0x0000000a */
        TDRPTR32 iReserve3;      /**< reserve for extend . Add at TDR build Version: 0x0000000a */
    };
#pragma pack()

    /**
     * �˽ṹ����һ�����ݳ�Ա(64λ)����iCountΪ0ʱ��ʾ�˳�ԱΪһ�ɱ����飬
     * struct/Union�н��������һ���ɱ������Ա
     */
    struct tagTDRMetaEntry64
    {
        int iID;          /**< ��Ԫ���ݵ�ID*/
        int iVersion;     /**< ��Ԫ���ݼӵ�MetaLib����ʱ�İ汾*/
        int iType;        /**< the typeid of this entry. */
        TDRPTR64 ptrName; /**< ptr of name, used for generating c/c++ header. */

        uint64_t iHRealSize;       /**<���ش洢�����ܹ��Ŀռ�*/
        uint64_t iNRealSize;       /**<���紫��ʱ����ĵ��ܿռ�*/
        uint64_t iHUnitSize;       /**<���ش洢����entry��Ԫ����ռ�*/
        uint64_t iNUnitSize;       /**<����洢����entry��Ԫ����ռ�*/
        uint64_t iCustomHUnitSize; /**< �Զ���洢��Ԫ��С*/

        int iCount; /**< 1 means single, >1 means array, 0 means variable array */

        TDROFF64 iNOff; /**< ���紫��ʱ��ƫ�ƣ�1�ֽڶ���*/
        TDROFF64 iHOff; /**< ���ش洢ʱ��ƫ�ƣ�ʹ��ָ�����뷽ʽ*/

        TDRIDX idxID;              /**< the id macro's index. */
        TDRIDX idxVersion;         /**< ����汾ȡֵΪ�궨�壬��˳�Ա�����Ӧ�궨���ں궨���б��е�����*/
        TDRIDX idxCount;           /**< the macro's referred by count. */
        TDRIDX idxType;            /**< the index of the typeinfo. */
        TDRIDX idxCustomHUnitSize; /**< �Զ���洢��Ԫ��С�궨��ֵ������*/

        unsigned short wFlag; /**< ��ȡ��Ԫ�ض�Ӧ��־��Ϣ����ָ�룬���õ�*/
        char chDBFlag;        /**< TDR-DB �����ϵӳ�� */
        char chOrder;         /**< 1 if ascending, -1 if desending, else 0 */

        TDRSIZEINFO64 stSizeInfo; /**<sizeinfo����ֵ*/
        TDRSelector64 stRefer;    /**<refer����ֵ*/
        TDRSelector64 stSelector; /**<select ����*/

        int iIO;   /**< the input/output control. */
        int idxIO; /**< the idx of input/output control macro. */

        TDRPTR64 ptrMeta; /**<���˳�ԱΪ������������ʱ����¼�临����������Ԫ����meta���ݵ�ָ��. */

        /**<��selector��ֵ��[iMinId,iMaxID]����ʱ��ѡ���Ԫ��*/
        int iMaxId;
        int iMinId;

        /**< index of macro of iMaxId,iMinId*/
        TDRIDX iMaxIdIdx;
        TDRIDX iMinIdIdx;

        /*
         *��Ա��ʾ��, ������Ϣ����������ȱʡֵ�ַ���������Metalib������ַ����ڴ��
         *�У��˴��������ַ�������ʼ��ַ�ͳ���
         */
        uint64_t iDefaultValLen; /**<ȱʡֵ�ĳ���*/
        TDRPTR64 ptrDesc;        /**< ptr of  the description info. */
        TDRPTR64 ptrChineseName; /**< ptr of chinse name of entry*/
        TDRPTR64 ptrDefaultVal;  /**< ptr of default value of entry*/

        TDRPTR64 ptrMacrosGroup; /*�˳�ԱԪ�ذ󶨵ĺ궨����ָ�룬 Add at TDR build Version: 0x00000008*/
        TDRPTR64 ptrCustomAttr;  /*ָ���Զ�������ֵ��ָ�룬 Add at TDR build Version: 0x00000008*/
        TDRPTR64 iOffToMeta;     /*����ڴ˳�Ա�����ĸ������ݽṹ��������ƫ�ƣ� Add at TDR build Version: 0x00000009*/

        TDRPTR64 iFlag;          /**< field extend flag, 2.7.1�汾����������Ԥ���ֶ� */
        TDRPTR64 ptrCustomAttr2; /**< �Զ�������ָ��,2.7.29�汾����,����Ԥ���ֶ�. Add at TDR build Version: 0x0000000a */
        TDRPTR64 iReserve3;      /**< reserve for extend . Add at TDR build Version: 0x0000000a */
    };

    /**
     * �洢meta��Ϣ�Ľṹ
     */
    struct tagTDRMeta
    {
        uint32_t uFlags; /**< flag info of mata*/

        int iID;          /**< id of meta*/
        int iBaseVersion; /**< base version of meta*/
        int iCurVersion;  /**< current version of meta*/
        int iType;        /**< type of meta*/
        size_t iMemSize;  /**< ��meta�ṹ�ڴ�ṹ��С*/

        size_t iNUnitSize; /**<���紫��ʱ�ĵ�Ԫ��С*/
        size_t iHUnitSize; /**<���ش洢��Ԫ��С*/

        size_t iCustomHUnitSize; /**<�Զ��屾�ش洢��Ԫ��С*/
        int idxCustomHUnitSize;  /**<�Զ��屾�ش洢��Ԫ��С�ĺ궨��ֵ���� */

        int iMaxSubID;   /**< max id of child entry*/
        int iEntriesNum; /**< num of child entries*/

        /* Add at TDR build version: 0x0000000b */
        int iIndexesNum;   /**< num of child indexes*/
        TDRPTR ptrIndexes; /**< ָ��index�������ʼ��ַ����(entries, primarykeys)֮��*/
        TDRPTR ptrColumns; /**< ָ��column�������ʼ��ַ����(entries, primarykeys, indexies)֮��*/
        /* Add at TDR build version: 0x0000000b */

        TDRPTR ptrMeta; /**< offset of this meta from "data" member of head. */

        TDRIDX iIdx;       /**< index of this mata in metalib*/
        TDRIDX idxID;      /**< index of macro of id*/
        TDRIDX idxType;    /**< index of meta's type*/
        TDRIDX idxVersion; /**< index of macro of meta's version*/

        int iCustomAlign; /**< structsԪ������ ָ���ṹ����Ա�����Ķ����С Default is 1*/
        int iValidAlign;  /**< metaԪ����Ч�Ķ���ֵ��Ϊ����Ա����ֵ�������Ǹ�ֵ*/

        int iVersionIndicatorMinVer; /**< �汾ָʾ����ָ�������ٱ���*/
        TDRSIZEINFO stSizeType;      /**< ��¼�����Ϣ*/
        TDRREDIRECTOR stVersionIndicator;

        TDRSORTKEYINFO stSortKey; /**<sortkey����ֵ*/

        TDRPTR ptrName; /**< ptr of Name of meta*/

        /*������Ϣ���������ַ���������Metalib������ַ����ڴ��,�˴��������ַ�������ʼ��ַ�ͳ���*/
        TDRPTR ptrDesc;        /**< ptr of  the description info. */
        TDRPTR ptrChineseName; /**< ptr of chinse name of entry*/

        int iSplitTableFactor;     /**<���ݿ�ֱ�����*/
        short nSplitTableRuleID;   /**<���ݿ�ֱ����id*/
        short nPrimayKeyMemberNum; /**<��������ĳ�Ա����*/

        TDRIDX idxSplitTableFactor;   /**< index of macro of meta's wSplitTableFactor*/
        TDRDBKEYINFO stSplitTableKey; /**<���ݿ�ֱ�������Ա��ָ�룬ʵ�ʴ洢���metalib������ƫ��ֵ*/
        TDRPTR ptrPrimayKeyBase;      /**<���ݿ�������Ա�Ļ�ַָ�룬ʵ�ʴ洢���metalib������ƫ��ֵ*/
        TDRPTR ptrDependonStruct;     /*�˽ṹ�̳е�Ԫ����ָ��*/

        TDRPTR ptrCustomAttr;  /**< for customAttr. 2.7.29�汾����. Add at TDR build Version: 0x00000009 */
        TDRPTR ptrCustomAttr2; /**< for customAttr2. 2.7.29�汾����. Add at TDR build Version: 0x0000000a */
        TDRPTR iReserve3;      /**< reserve for extend . Add at TDR build Version: 0x0000000a */

        TDRMETAENTRY stEntries[1];
    };

    /**
     * �洢meta��Ϣ�Ľṹ(32λ)
     */
#pragma pack(4)
    struct tagTDRMeta32
    {
        uint32_t uFlags; /**< flag info of mata*/

        int iID;               /**< id of meta*/
        int iBaseVersion;      /**< base version of meta*/
        int iCurVersion;       /**< current version of meta*/
        int iType;             /**< type of meta*/
        unsigned int iMemSize; /**< ��meta�ṹ�ڴ�ṹ��С*/

        unsigned int iNUnitSize; /**<���紫��ʱ�ĵ�Ԫ��С*/
        unsigned int iHUnitSize; /**<���ش洢��Ԫ��С*/

        unsigned int iCustomHUnitSize; /**<�Զ��屾�ش洢��Ԫ��С*/
        int idxCustomHUnitSize;        /**<�Զ��屾�ش洢��Ԫ��С�ĺ궨��ֵ���� */

        int iMaxSubID;   /**< max id of child entry*/
        int iEntriesNum; /**< num of child entries*/

        /* Add at TDR build version: 0x0000000b */
        int iIndexesNum;     /**< num of child indexes*/
        TDRPTR32 ptrIndexes; /**< ָ��index�������ʼ��ַ����(entries, primarykeys)֮��*/
        TDRPTR32 ptrColumns; /**< ָ��column�������ʼ��ַ����(entries, primarykeys, indexies)֮��*/
        /* Add at TDR build version: 0x0000000b */

        TDRPTR32 ptrMeta; /**< offset of this meta from "data" member of head. */

        TDRIDX iIdx;       /**< index of this mata in metalib*/
        TDRIDX idxID;      /**< index of macro of id*/
        TDRIDX idxType;    /**< index of meta's type*/
        TDRIDX idxVersion; /**< index of macro of meta's version*/

        int iCustomAlign; /**< structsԪ������ ָ���ṹ����Ա�����Ķ����С Default is 1*/
        int iValidAlign;  /**< metaԪ����Ч�Ķ���ֵ��Ϊ����Ա����ֵ�������Ǹ�ֵ*/

        int iVersionIndicatorMinVer; /**< �汾ָʾ����ָ�������ٱ���*/
        TDRSIZEINFO32 stSizeType;    /**< ��¼�����Ϣ*/
        TDRREDIRECTOR32 stVersionIndicator;

        TDRSORTKEYINFO32 stSortKey; /**<sortkey����ֵ*/

        TDRPTR32 ptrName; /**< ptr of Name of meta*/

        /*������Ϣ���������ַ���������Metalib������ַ����ڴ��,�˴��������ַ�������ʼ��ַ�ͳ���*/
        TDRPTR32 ptrDesc;        /**< ptr of  the description info. */
        TDRPTR32 ptrChineseName; /**< ptr of chinse name of entry*/

        int iSplitTableFactor;     /**<���ݿ�ֱ�����*/
        short nSplitTableRuleID;   /**<���ݿ�ֱ����id*/
        short nPrimayKeyMemberNum; /**<��������ĳ�Ա����*/

        TDRIDX idxSplitTableFactor;     /**< index of macro of meta's wSplitTableFactor*/
        TDRDBKEYINFO32 stSplitTableKey; /**<���ݿ�ֱ�������Ա��ָ�룬ʵ�ʴ洢���metalib������ƫ��ֵ*/
        TDRPTR32 ptrPrimayKeyBase;      /**<���ݿ�������Ա�Ļ�ַָ�룬ʵ�ʴ洢���metalib������ƫ��ֵ*/
        TDRPTR32 ptrDependonStruct;     /*�˽ṹ�̳е�Ԫ����ָ��*/

        TDRPTR32 ptrCustomAttr;  /**< for customAttr. 2.7.29�汾����. Add at TDR build Version: 0x00000009 */
        TDRPTR32 ptrCustomAttr2; /**< for customAttr2. 2.7.29�汾����. Add at TDR build Version: 0x0000000a */
        TDRPTR32 iReserve3;      /**< reserve for extend . Add at TDR build Version: 0x0000000a */

        TDRMETAENTRY32 stEntries[1];
    };
#pragma pack()

    /**
     * �洢meta��Ϣ�Ľṹ(64λ)
     */
    struct tagTDRMeta64
    {
        uint32_t uFlags; /**< flag info of mata*/

        int iID;           /**< id of meta*/
        int iBaseVersion;  /**< base version of meta*/
        int iCurVersion;   /**< current version of meta*/
        int iType;         /**< type of meta*/
        uint64_t iMemSize; /**< ��meta�ṹ�ڴ�ṹ��С*/

        uint64_t iNUnitSize; /**<���紫��ʱ�ĵ�Ԫ��С*/
        uint64_t iHUnitSize; /**<���ش洢��Ԫ��С*/

        uint64_t iCustomHUnitSize; /**<�Զ��屾�ش洢��Ԫ��С*/
        int idxCustomHUnitSize;    /**<�Զ��屾�ش洢��Ԫ��С�ĺ궨��ֵ���� */

        int iMaxSubID;   /**< max id of child entry*/
        int iEntriesNum; /**< num of child entries*/

        /* Add at TDR build version: 0x0000000b */
        int iIndexesNum;     /**< num of child indexes*/
        TDRPTR64 ptrIndexes; /**< ָ��index�������ʼ��ַ����(entries, primarykeys)֮��*/
        TDRPTR64 ptrColumns; /**< ָ��column�������ʼ��ַ����(entries, primarykeys, indexies)֮��*/
        /* Add at TDR build version: 0x0000000b */

        TDRPTR64 ptrMeta; /**< offset of this meta from "data" member of head. */

        TDRIDX iIdx;       /**< index of this mata in metalib*/
        TDRIDX idxID;      /**< index of macro of id*/
        TDRIDX idxType;    /**< index of meta's type*/
        TDRIDX idxVersion; /**< index of macro of meta's version*/

        int iCustomAlign; /**< structsԪ������ ָ���ṹ����Ա�����Ķ����С Default is 1*/
        int iValidAlign;  /**< metaԪ����Ч�Ķ���ֵ��Ϊ����Ա����ֵ�������Ǹ�ֵ*/

        int iVersionIndicatorMinVer; /**< �汾ָʾ����ָ�������ٱ���*/
        TDRSIZEINFO64 stSizeType;    /**< ��¼�����Ϣ*/
        TDRREDIRECTOR64 stVersionIndicator;

        TDRSORTKEYINFO64 stSortKey; /**<sortkey����ֵ*/

        TDRPTR64 ptrName; /**< ptr of Name of meta*/

        /*������Ϣ���������ַ���������Metalib������ַ����ڴ��,�˴��������ַ�������ʼ��ַ�ͳ���*/
        TDRPTR64 ptrDesc;        /**< ptr of  the description info. */
        TDRPTR64 ptrChineseName; /**< ptr of chinse name of entry*/

        int iSplitTableFactor;     /**<���ݿ�ֱ�����*/
        short nSplitTableRuleID;   /**<���ݿ�ֱ����id*/
        short nPrimayKeyMemberNum; /**<��������ĳ�Ա����*/

        TDRIDX idxSplitTableFactor;     /**< index of macro of meta's wSplitTableFactor*/
        TDRDBKEYINFO64 stSplitTableKey; /**<���ݿ�ֱ�������Ա��ָ�룬ʵ�ʴ洢���metalib������ƫ��ֵ*/
        TDRPTR64 ptrPrimayKeyBase;      /**<���ݿ�������Ա�Ļ�ַָ�룬ʵ�ʴ洢���metalib������ƫ��ֵ*/
        TDRPTR64 ptrDependonStruct;     /*�˽ṹ�̳е�Ԫ����ָ��*/

        TDRPTR64 ptrCustomAttr;  /**< for customAttr. 2.7.29�汾����. Add at TDR build Version: 0x00000009 */
        TDRPTR64 ptrCustomAttr2; /**< for customAttr2. 2.7.29�汾����. Add at TDR build Version: 0x0000000a */
        TDRPTR64 iReserve3;      /**< reserve for extend . Add at TDR build Version: 0x0000000a */

        TDRMETAENTRY64 stEntries[1];
    };

    /**
      Ԫ���ݿ�Ľṹ��Ϣ
      In order to refer the meta data fastly, We use two-level mapping.
      First, A map entry for each meta data.
      Second, A index value for each map entry.
      There are two index array, one for id, one for name.
      */
    struct tagTDRMetaLib
    {
        unsigned short wMagic;
        short nBuild;
        uint32_t dwPlatformArch; /*��¼��32λƽ̨����64λƽ̨������*/
        size_t iSize;

        unsigned int checksum; /*  calculated. */
        int iReserve[3];

        int iID;
        int iXMLTagSetVer; /**<XMLTag Set Verion of this metalib used*/

        int iMaxID;

        int iMaxMetaNum;
        int iCurMetaNum;
        int iMaxMacroNum;
        int iCurMacroNum;

        int iMaxMacrosGroupNum; /*��������ɵĺ궨������Ŀ, Add at TDR build Version: 0x00000008*/
        int iCurMacrosGroupNum; /*�궨���鵱ǰ��Ŀ�� Add at TDR build Version: 0x00000008*/

        int iMaxValFieldDefNum; /*��������ɵ�ֵ��������Ŀ, Add at TDR build Version: 0x00000008*/
        int iCurValFieldDefNum; /*ֵ�����鵱ǰ��Ŀ�� Add at TDR build Version: 0x00000008*/

        int iVersion;

        /** all the offset is start from the 'data' member.
        */
        TDRPTR ptrMacro;    /*ptr for macro info block*/
        TDRPTR ptrID;       /*ptr of begin address for id-metaidx mapping info block*/
        TDRPTR ptrName;     /*ptr of begin address for name-metaidx mapping info block*/
        TDRPTR ptrMap;      /*ptr of begin address for metaidx-metaOff mapping info block*/
        TDRPTR ptrMeta;     /*ptr of begin address for meta info block*/
        TDRPTR ptrLaseMeta; /*ptr of last meta in mata*/

        /*�ַ�������������*/
        size_t iFreeStrBufSize; /*�ַ�����������������Ĵ�С*/
        TDRPTR ptrStrBuf;       /*�ַ�����������ƫ�Ƶ�ַ����data��Ա��ʼ����*/
        TDRPTR ptrFreeStrBuf;   /*���õĿ��л��������׵�ַ*/

        TDRPTR ptrMacroGroupMap; /*ptr of macrosgroup index-off mapping info block Add at TDR build Version: 0x00000008*/
        TDRPTR ptrMacrosGroup;   /*ptr of macrosgroup, Add at TDR build Version: 0x00000008*/

        size_t iMacrosGroupSize; /*�궨�������ʹ�õ����ռ䣬 Add at TDR build Version: 0x00000008*/

        TDRPTR ptrValueFiledDefinitionsMap; /*ֵ���������������� Add at TDR build Version: 0x00000008*/
        TDRPTR ptrValueFiledDefinitions;    /*ֵ�����ַ, Add at TDR build Version: 0x00000008*/
        size_t iValueFiledDefinitionsSize;  /*ֵ������������С�� Add at TDR build Version: 0x00000008*/

        /* Ԥ���ֶ� */
        TDRPTR iFlag;               /**< 2.7.1�汾����Ԥ���ֶ�*/
        TDRPTR iMultiSplitTableKey; /* 2.7.37�汾 ��������splittablekey֧��, �ж�tagTDRDBKeyInfo�е�ptrEntryָ�� Ϊ�ַ���ָ��(�°汾)����Entryָ��(�ɰ汾) */
        TDRPTR iReserve3;           /**< reserve for extend . Add at TDR build Version: 0x0000000a */

        char szName[TDR_NAME_LEN];
        char data[1]; /* only used for reference data. */
    };

    /**
      32λƽ̨��Ԫ���ݿ�Ľṹ��Ϣ
      In order to refer the meta data fastly, We use two-level mapping.
      First, A map entry for each meta data.
      Second, A index value for each map entry.
      There are two index array, one for id, one for name.
      */

#pragma pack(4)
    struct tagTDRMetaLib32
    {
        unsigned short wMagic;
        short nBuild;
        uint32_t dwPlatformArch; /*��¼��32λƽ̨����64λƽ̨������*/
        unsigned int iSize;

        unsigned int checksum; /*  calculated. */
        int iReserve[3];

        int iID;
        int iXMLTagSetVer; /**<XMLTag Set Verion of this metalib used*/

        int iMaxID;

        int iMaxMetaNum;
        int iCurMetaNum;
        int iMaxMacroNum;
        int iCurMacroNum;

        int iMaxMacrosGroupNum; /*��������ɵĺ궨������Ŀ, Add at TDR build Version: 0x00000008*/
        int iCurMacrosGroupNum; /*�궨���鵱ǰ��Ŀ�� Add at TDR build Version: 0x00000008*/

        int iMaxValFieldDefNum; /*��������ɵ�ֵ��������Ŀ, Add at TDR build Version: 0x00000008*/
        int iCurValFieldDefNum; /*ֵ�����鵱ǰ��Ŀ�� Add at TDR build Version: 0x00000008*/

        int iVersion;

        /** all the offset is start from the 'data' member.
        */
        TDRPTR32 ptrMacro;    /*ptr for macro info block*/
        TDRPTR32 ptrID;       /*ptr of begin address for id-metaidx mapping info block*/
        TDRPTR32 ptrName;     /*ptr of begin address for name-metaidx mapping info block*/
        TDRPTR32 ptrMap;      /*ptr of begin address for metaidx-metaOff mapping info block*/
        TDRPTR32 ptrMeta;     /*ptr of begin address for meta info block*/
        TDRPTR32 ptrLaseMeta; /*ptr of last meta in mata*/

        /*�ַ�������������*/
        unsigned int iFreeStrBufSize; /*�ַ�����������������Ĵ�С*/
        TDRPTR32 ptrStrBuf;           /*�ַ�����������ƫ�Ƶ�ַ����data��Ա��ʼ����*/
        TDRPTR32 ptrFreeStrBuf;       /*���õĿ��л��������׵�ַ*/

        TDRPTR32 ptrMacroGroupMap; /*ptr of macrosgroup index-off mapping info block Add at TDR build Version: 0x00000008*/
        TDRPTR32 ptrMacrosGroup;   /*ptr of macrosgroup, Add at TDR build Version: 0x00000008*/

        unsigned int iMacrosGroupSize; /*�궨�������ʹ�õ����ռ䣬 Add at TDR build Version: 0x00000008*/

        TDRPTR32 ptrValueFiledDefinitionsMap;    /*ֵ���������������� Add at TDR build Version: 0x00000008*/
        TDRPTR32 ptrValueFiledDefinitions;       /*ֵ�����ַ, Add at TDR build Version: 0x00000008*/
        unsigned int iValueFiledDefinitionsSize; /*ֵ������������С�� Add at TDR build Version: 0x00000008*/

        /* Ԥ���ֶ� */
        TDRPTR32 iFlag;               /**< 2.7.1�汾����Ԥ���ֶ�*/
        TDRPTR32 iMultiSplitTableKey; /* 2.7.37�汾 ��������splittablekey֧��, �ж�tagTDRDBKeyInfo�е�ptrEntryָ�� Ϊ�ַ���ָ��(�°汾)����Entryָ��(�ɰ汾) */
        TDRPTR32 iReserve3;           /**< reserve for extend . Add at TDR build Version: 0x0000000a */

        char szName[TDR_NAME_LEN];
        char data[1]; /* only used for reference data. */
    };
#pragma pack()

    /**
      64λƽ̨��Ԫ���ݿ�Ľṹ��Ϣ
      In order to refer the meta data fastly, We use two-level mapping.
      First, A map entry for each meta data.
      Second, A index value for each map entry.
      There are two index array, one for id, one for name.
      */

    struct tagTDRMetaLib64
    {
        unsigned short wMagic;
        short nBuild;
        uint32_t dwPlatformArch; /*��¼��32λƽ̨����64λƽ̨������*/
        uint64_t iSize;

        unsigned int checksum; /*  calculated. */
        int iReserve[3];

        int iID;
        int iXMLTagSetVer; /**<XMLTag Set Verion of this metalib used*/

        int iMaxID;

        int iMaxMetaNum;
        int iCurMetaNum;
        int iMaxMacroNum;
        int iCurMacroNum;

        int iMaxMacrosGroupNum; /*��������ɵĺ궨������Ŀ, Add at TDR build Version: 0x00000008*/
        int iCurMacrosGroupNum; /*�궨���鵱ǰ��Ŀ�� Add at TDR build Version: 0x00000008*/

        int iMaxValFieldDefNum; /*��������ɵ�ֵ��������Ŀ, Add at TDR build Version: 0x00000008*/
        int iCurValFieldDefNum; /*ֵ�����鵱ǰ��Ŀ�� Add at TDR build Version: 0x00000008*/

        int iVersion;

        /** all the offset is start from the 'data' member.
        */
        TDRPTR64 ptrMacro;    /*ptr for macro info block*/
        TDRPTR64 ptrID;       /*ptr of begin address for id-metaidx mapping info block*/
        TDRPTR64 ptrName;     /*ptr of begin address for name-metaidx mapping info block*/
        TDRPTR64 ptrMap;      /*ptr of begin address for metaidx-metaOff mapping info block*/
        TDRPTR64 ptrMeta;     /*ptr of begin address for meta info block*/
        TDRPTR64 ptrLaseMeta; /*ptr of last meta in mata*/

        /*�ַ�������������*/
        uint64_t iFreeStrBufSize; /*�ַ�����������������Ĵ�С*/
        TDRPTR64 ptrStrBuf;       /*�ַ�����������ƫ�Ƶ�ַ����data��Ա��ʼ����*/
        TDRPTR64 ptrFreeStrBuf;   /*���õĿ��л��������׵�ַ*/

        TDRPTR64 ptrMacroGroupMap; /*ptr of macrosgroup index-off mapping info block Add at TDR build Version: 0x00000008*/
        TDRPTR64 ptrMacrosGroup;   /*ptr of macrosgroup, Add at TDR build Version: 0x00000008*/

        uint64_t iMacrosGroupSize; /*�궨�������ʹ�õ����ռ䣬 Add at TDR build Version: 0x00000008*/

        TDRPTR64 ptrValueFiledDefinitionsMap; /*ֵ���������������� Add at TDR build Version: 0x00000008*/
        TDRPTR64 ptrValueFiledDefinitions;    /*ֵ�����ַ, Add at TDR build Version: 0x00000008*/
        uint64_t iValueFiledDefinitionsSize;  /*ֵ������������С�� Add at TDR build Version: 0x00000008*/

        /* Ԥ���ֶ� */
        TDRPTR64 iFlag;               /* 2.7.1�汾����Ԥ���ֶ�*/
        TDRPTR64 iMultiSplitTableKey; /* 2.7.37�汾 ��������splittablekey֧��, �ж�tagTDRDBKeyInfo�е�ptrEntryָ�� Ϊ�ַ���ָ��(�°汾)����Entryָ��(�ɰ汾) */
        TDRPTR64 iReserve3;           /**< reserve for extend . Add at TDR build Version: 0x0000000a */

        char szName[TDR_NAME_LEN];
        char data[1]; /* only used for reference data. */
    };

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TDR_METALIB_KERNEL_H */
