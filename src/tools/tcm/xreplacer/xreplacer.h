#ifndef XREPLACER_H__
#define XREPLACER_H__

#ifdef __cplusplus
extern "C" {
#endif

// չ��һ��xmlģ���ļ�
// src_filename: ����ģ���ļ�����������2��: A.xml.template��A.xml.bus_template
// dst_filename: ���չ�����ļ�������ΪA.xml
// custom_attrs: tcm�����ļ�proc_deploy.xml���õ�����ֵ����ʽΪ"Key1=Value1;Key2=Value2;"
int replace_template(const char *src_filename, const char *dst_filename, const char *custom_attrs);

#ifdef __cplusplus
}
#endif

#endif //XREPLACER_H__

