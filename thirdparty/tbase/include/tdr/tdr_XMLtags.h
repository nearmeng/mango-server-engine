/**
*
* @file     tdr_XMLtags.h
* @brief    Ԫ��������XML��ǩ����
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



#ifndef TDR_XMLTAGS_H
#define TDR_XMLTAGS_H

/**@defgroup XMLTag TDR_Ԫ��������XML��ǩ����
* @{
* @note ����XML��ǩ���������˵�����ǵ�ǰ�汾��tagsetversion = 1)����ı�ǩ����
*/

/** @name Ԫ��������Ԫ����
* @{
*/
#define TDR_TAG_METALIB            "metalib"           /**< Ԫ��������XML���ĸ�Ԫ�� */
#define TDR_TAG_INCLUDE            "include"           /**< �ļ��������� */
#define TDR_TAG_MACRO              "macro"             /**< �궨��Ԫ�� */
#define TDR_TAG_MACROSGROUP        "macrosgroup"       /**< �궨���� */
#define TDR_TAG_STRUCT             "struct"            /**< �ṹ��Ԫ�� */
#define TDR_TAG_UNION              "union"             /**< ������Ԫ�� */
#define TDR_TAG_ENTRY              "entry"             /**< Ԫ���ݳ�Ա */
#define TDR_TAG_INDEX              "index"             /**< sql�������� */

#define TDR_TAG_SERVICE            "service"           /**< idl service */
#define TDR_TAG_RPC                "rpc"               /**< idl service rpc */
#define TDR_TAG_ARGUMENTS          "arguments"         /**< idl service rpc arguments */
#define TDR_TAG_RETURNS            "returns"           /**< idl service rpc returns */
/**
*֮ǰXML��ǩ����tagsetversion=0)�����Ԫ���ݳ�Ա
* @deprecated ��Ԫ���ѱ�����TDR_TAG_FIELD��ȡ�����°�XML�ļ��в���ʹ�á�
* @see TDR_TAG_FIELD
*/
#define TDR_TAG_ITEM              "item"

/**
*֮ǰXML��ǩ����tagsetversion=0)�����Ԫ����������Ԫ��
* @deprecated ��Ԫ���ѱ�����TDR_TAG_METALIB��ȡ�����°�XML�ļ��в���ʹ�á�
* @see TDR_TAG_METALIB
*/
#define TDR_TAG_TYPELIB             "typelib"

/**
*֮ǰXML��ǩ����tagsetversion=0)����ĺ궨����ı�ǩ
* @deprecated Ŀǰû�к궨����ĸ���˱�ǩֱ��Ԫ�ر�TDR_TAG_MACRO��ȡ�����°�XML�ļ��в���ʹ�á�
*  @par ʹ��˵��:
*  @code
    //tagsetversion=0 �ı�ǩ����
    <macros>
        <macro name="SS_Version" id="99" />
	<macro name="SS_ID_REQ" id="1" />
	<macro name="SS_ID_RES" id="2" />
    </macros>

    //�µĶ���
    <macro name="SS_Version" id="99" />
    <macro name="SS_ID_REQ" id="1" />
    <macro name="SS_ID_RES" id="2" />
*  @endcode
* @see TDR_TAG_MACRO
*/
#define TDR_TAG_MACROS              "macros"

/**
*֮ǰXML��ǩ����tagsetversion=0)������Զ����������͵ı�ǩ
* @deprecated Ŀǰ�Ѿ���TDR_TAG_STRUCT��TDR_TAG_UNION��ȡ�����°�XML�ļ��в���ʹ�á�
*  @par ʹ��˵��:
*  @code
    //struct�ĵȼ۴����ɵĶ��巽ʽΪ��
    <type class="struct" >
        ����
    </type>

    //��class��ȱʡֵΪ"struct"��
    <type >
        ����
    </type>

    //�������巽ʽ�����µı�ǩ��������·�ʽ�ȼۣ�
    <struct>
        ����
    </struct>

    //union�ĵȼ۴����ɵĶ��巽ʽΪ��
    <type class="union" >
        ����
    </type>

    //�������巽ʽ�����µı�ǩ��������·�ʽ�ȼۣ�
    <union>
        ����
    </union>
*  @endcode
* @see TDR_TAG_STRUCT
* @see TDR_TAG_UNION
*/
#define TDR_TAG_COMPOSITE_TYPE                "type"
/**
*֮ǰXML��ǩ����tagsetversion=0)������Զ����������͵ı�ǩ
* @deprecated Ŀǰ�Ѿ���TDR_TAG_STRUCT��TDR_TAG_UNION��ȡ�����°�XML�ļ��в���ʹ�á�
*@ see TDR_TAG_COMPOSITE_TYPE
*/
#define TDR_TAG_CLASS                         "class"
/** @} */ //Ԫ��������Ԫ����


/**@name Ԫ����Ԫ��ͨ������
* @{
*/
#define TDR_TAG_ID                 "id"                /**< Ԫ�ص���ֵ��ʶ */
#define TDR_TAG_VERSION            "version"           /**< Ԫ�صİ汾 */

/** ��Ա����Ԫ�ص��ַ���ʶ
*@note ��union�ṹ�⣬ͬһ��Ԫ���µ�������Ԫ�ص�name���Ե�ֵ�����ظ���
*/
#define TDR_TAG_NAME               "name"
#define TDR_TAG_CNNAME             "cname"            /**< Ԫ�ص����ı�ʶ */
#define TDR_TAG_DESCIPTION               "desc"              /**< Ԫ��������*/
#define TDR_TAG_DOT                '.'                  /**< Ԫ��path���Էָ���*/
/** @} */ // Ԫ����Ԫ��ͨ������



/** @name metalibԪ�ص���������
* @{
*/
#define TDR_TAG_TAGSET_VERSION         "tagsetversion"   /**< ������XML�ļ���Ԫ����������ǩ���弯�İ汾*/
#define TDR_TAG_ENABLE_TLV             "enabletlv"       /**< ������XML�Ƿ����TLV����뷽ʽ*/
/** @ } */  // metalibԪ�ص���������

/** @name indexԪ�ص���������
* @{
*/
#define TDR_TAG_INDEX_COLUMN   "column"                 /**< ָ����������ʹ�õ����ݱ����*/
#define TDR_TAG_INDEX_TYPE     "index_type"             /**< ָ���洢���������ݽṹ����*/
#define TDR_TAG_INDEX_UNIQUE   "UNIQUE"                 /**< ָ�����ݱ�������unique����*/
/** @ } */  // indexԪ�ص���������

/** @name entryԪ�ص���������
* @{
*/
#define TDR_TAG_TYPE               "type"              /**< Ԫ���ݵ��ֶε����� */
#define TDR_TAG_COUNT               "count"         /**< count����*/
#define TDR_TAG_IO                  "io"            /**< io���Ա�ǩ*/
#define TDR_TAG_REFER               "refer"         /**< refer����*/
#define TDR_TAG_THIS		"this"              /**< ָ����ǰԪ��*/
#define TDR_TAG_POINTER_TYPE             '*'         /**< ָ������*/
#define TDR_TAG_REFER_TYPE              '@'         /**<��������*/
#define TDR_TAG_SELECT                  "select"    /**< union��������ѡ���Ա������*/
#define TDR_TAG_SIZEINFO                "sizeinfo"  /**< ָ������ζԸ�Ԫ���ݳ�Ա�Ĵ��������Ϣ���д��*/
#define TDR_TAG_DATASET		"dataset"				/**<������*/

#define TDR_TAG_REQUIRED    "required"              /**<field�ֶε���Ҫ�Ա�־����ѡ���ѡ*/

/**
*֮ǰXML��ǩ����tagsetversion=0)�����ָ�����ݳ�Ա������ȵĵı�ǩ
* @deprecated Ŀǰ�Ѿ���TDR_TAG_SIZEINFO��ȡ�����°�XML�ļ��в���ʹ�á�
*  @par �ȼ۴���:
*  @code
    //�ɵĶ��巽ʽΪ��
    <struct name="type1" >
	<entry name="item1" type="int" />
	<entry name="item2" type="int" />
    </struct>

    <struct name="type2" >
	<entry name="BodySize" type="int" />
	<entry name="Body" type="type1" target="BodySize" />
    </struct>


    //�������巽ʽ�����µı�ǩ��������·�ʽ�ȼۣ�
    <struct name="type1" >
	<entry name="item1" type="int" />
	<entry name="item2" type="int" />
    </struct>

    <struct name="type2" >
	<entry name="BodySize" type="int" />
	<entry name="Body" type="type1" sizeinfo="BodySize" />
    </struct>

*  @endcode
* @see TDR_TAG_SIZEINFO
*/
#define TDR_TAG_TARGET              "target"
#define TDR_TAG_UNIQUE              "unique"            /**< �Ƿ�Ψһ��ȡֵfalse��true,ȱʡfalse����Ψһ */
#define TDR_TAG_NOTNULL             "notnull"           /**< �Ƿ�Ϊ�գ�ȡֵfalse��true,ȱʡfalse������Ϊ�� */
#define TDR_TAG_SIZE                "size"              /**< ָ��entry��Ԫ�Ĵ�С*/
#define TDR_TAG_SORTMETHOD          "sortmethod"        /**< ���򷽷�*/
#define TDR_TAG_SORTKEY             "sortkey"           /**< ����ؼ���*/
#define TDR_TAG_DEFAULT_VALUE       "defaultvalue"      /**< ����Ĭ��ֵ */
#define TDR_TAG_MINID               "minid"             /**< ѡ�� union entry����Сid */
#define TDR_TAG_MAXID               "maxid"             /**< ѡ�� union entry�����id */
#define TDR_TAG_EXTENDTOTABLE		"extendtotable"		/**< ��struct��Աչ�����浽���ݿ����*/

#define TDR_TAG_COMPRESS			"compress"		    /**< ������ѹ���洢,extendtotable��compress����ͬʱΪtrue*/
#define TDR_TAG_STRING_COMPRESS     "stringcompress"    /**< stringѹ����־��֧����Դת�����ߵ��ַ������մ洢*/

#define TDR_TAG_IO_NO_LIMIT			"nolimit"           /**< ���io����ֵΪ��ֵ������������������û������*/
#define TDR_TAG_IO_NO_INPUT 		"noinput"           /**< ���io����ֵΪ��ֵ����������ݲ���xml�ļ�������*/
#define TDR_TAG_IO_NO_OUTPUT 		"nooutput"          /**< ���io����ֵΪ��ֵ����������ݲ������xml�ļ���*/
#define TDR_TAG_IO_NO_IO 	        "noio"              /**< ���io����ֵΪ��ֵ����������ݲ��ܽ���xml�ļ��������*/
#define TDR_TAG_BIND_MACROSGROUP	"bindmacrosgroup"	/**< �󶨵ĺ궨����*/
#define TDR_TAG_AUTOINCREMENT		"autoincrement"		/**< auto increment����*/
#define TDR_TAG_CUSTOMATTR			"customattr"		/**< �Զ�������*/
#define TDR_TAG_USESUBTABLE     	"usesubtable"       /**< entry����ʱ�������ӱ�*/
/** @}*/  //entryԪ�ص���������




/** @name metalibԪ�ص��������Ե�����ֵ��ǩ
* @{
*/
#define TDR_TAG_FALSE             "false"           /**< unique,notnull������ֵ֮һ*/
#define TDR_TAG_TRUE              "true"            /**< unique,notnull������ֵ֮һ*/
#define TDR_TAG_ASC               "asc"             /**< sortmethod������ֵ֮һ����ʾ����������*/
#define TDR_TAG_DESC              "desc"            /**< sortmethod������ֵ֮һ����ʾ����������*/
#define TDR_TAG_NO                "no"              /**< sortmethod������ֵ֮һ����ʾ������*/
#define TDR_TAG_SPLITTABLERULE_BY_MOD		"modulebyfactor"  /**< �ֱ����֮һ����ʾʹ�÷ֱ�����ȡģ�ķ�ʽ�����зֱ�*/
/* @ }*/




/** @name macro�궨��Ԫ�ص���������
* @{
*/
#define TDR_TAG_MACRO_VALUE            "value"           /**< macroԪ�ص����ԣ���ʾ�궨���ֵ */

/**
*֮ǰXML��ǩ����tagsetversion=0)����ı�ʾ�궨��ֵ�����Ա�ǩ
* @deprecated Ŀǰ�Ѿ���TDR_TAG_MACRO_VALUE���°�XML�ļ��в���ʹ�á�
*  @par �ȼ۴���:
*  @code
    //�ɵĶ��巽ʽΪ��
    <macro name ="LOGIN_NAME_LEN" id ="32" >
        ����
    //�����µı�ǩ��������·�ʽ�ȼۣ�
    <macro name ="LOGIN_NAME_LEN" value ="32" >
*  @endcode
* @see TDR_TAG_MACRO_VALUE
*/
#define TDR_TAG_MACRO_ID                "id"
/** @}*/  //macro�궨��Ԫ�ص���������


/** @name structԪ�ص���������
* @{
*/
#define TDR_TAG_SIZE                "size"              /**< ָ��entry��Ԫ�Ĵ�С*/
#define TDR_TAG_ALIGN               "align"             /**<�ֽڶ��뷽ʽ����Ӧ�ڱ�������\#pragma packָ��*/
#define TDR_TAG_VERSIONINDICATOR    "versionindicator"  /**<versionindicator����ָ���ĸ�Ԫ���ݳ�Ա���Ϊ��Ԫ���ݽṹ�İ汾ָʾ��*/


#define TDR_TAG_PRIMARY_KEY			"primarykey"		/**<ָ���ṹ���Ӧ���ݿ���������Ϣ*/
#define TDR_TAG_SPLITTABLEFACTOR	"splittablefactor"	/**<ָ���ṹ���Ӧ���ݿ��ķֱ�����*/
#define TDR_TAG_SPLITTABLEKEY	"splittablekey"	/**<ָ���ṹ���Ӧ���ݿ��ķֱ�����*/
#define TDR_TAG_SPLITTABLERULE	"splittablerule"	/**<ָ���ṹ���Ӧ���ݿ��ķֱ����*/
#define TDR_TAG_STRICTINPUT	"strictinput"	/**<ָ���ṹ������ʱ�����ϸ�����ģʽ*/
#define TDR_TAG_DEPENDONSTRUCT	"dependonstruct"	/**<ָ���ṹ��������ĳ��Ԫ���ݣ���O/Rmapping��ʹ��*/
#define TDR_TAG_UNIQUEENTRYNAME	"uniqueentryname" /**<ָ���ṹ���Աչ���洢�ǽṹ���Ա���Ƿ�Ψһ*/
/**
*ָ������ζԸ�Ԫ���ݽṹ�Ĵ��������Ϣ���д��
* @deprecated �Ѿ���sizeinfo���������
*/
#define TDR_TAG_SIZETYPE            "sizetype"          /**< ָ������ζԸ�Ԫ���ݽṹ�Ĵ��������Ϣ���д��*/
/*@}*/

/** @name include Ԫ�ص���������
* @{
*/
#define TDR_TAG_INCLUDE_FILE            "file"           /**< includԪ�ص����ԣ���ʾ�������ļ� */

/*@}*/

/* Integer-type attribute */







/** @name ����������������ֵ
* @{
*/
#define	TDR_TAG_TINYINT				"tinyint"
#define	TDR_TAG_TINYUINT			"tinyuint"
#define	TDR_TAG_SMALLINT			"smallint"
#define	TDR_TAG_SMALLUINT			"smalluint"
#define TDR_TAG_INT                "int"
#define TDR_TAG_UINT               "uint"
#define	TDR_TAG_BIGINT				"bigint"
#define	TDR_TAG_BIGUINT				"biguint"
#define	TDR_TAG_INT8				"int8"
#define	TDR_TAG_INT16				"int16"
#define	TDR_TAG_INT32				"int32"
#define	TDR_TAG_INT64				"int64"
#define	TDR_TAG_UINT8				"uint8"
#define	TDR_TAG_UINT16				"uint16"
#define	TDR_TAG_UINT32				"uint32"
#define	TDR_TAG_UINT64				"uint64"
#define TDR_TAG_DATE               "date"
#define TDR_TAG_TIME               "time"
#define TDR_TAG_DATETIME           "datetime"
#define TDR_TAG_MONEY              "money"
#define TDR_TAG_FLOAT              "float"
#define TDR_TAG_DOUBLE             "double"
#define TDR_TAG_STRING             "string"
#define TDR_TAG_BYTE				"byte"
#define TDR_TAG_IP					"ip"        /**<IPv4��ַ*/
#define TDR_TAG_WCHAR				"wchar"
#define TDR_TAG_WSTRING				"wstring"
#define TDR_TAG_DECIMAL				"decimal"
#define TDR_TAG_VOID				"void"

/**
*֮ǰXML��ǩ����tagsetversion=0)����ı�ʾ�궨��ֵ�����Ա�ǩ
* @deprecated �°�XML�ļ��в���ʹ�á�
*/
#define TDR_TAG_CHAR               "char"
#define TDR_TAG_UCHAR              "uchar"
#define TDR_TAG_SHORT              "short"
#define TDR_TAG_USHORT             "ushort"
#define TDR_TAG_LONG               "long"
#define TDR_TAG_ULONG              "ulong"
#define TDR_TAG_LONGLONG           "longlong"
#define TDR_TAG_ULONGLONG          "ulonglong"
/** @}*/ // ����������������ֵ


/**@} */ // Ԫ����XML��ǩ����


#endif /* TDR_XMLTAGS_H */

