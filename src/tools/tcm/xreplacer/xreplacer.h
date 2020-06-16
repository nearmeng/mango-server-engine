#ifndef XREPLACER_H__
#define XREPLACER_H__

#ifdef __cplusplus
extern "C" {
#endif

// 展开一个xml模版文件
// src_filename: 输入模版文件名，现在有2种: A.xml.template和A.xml.bus_template
// dst_filename: 输出展开后文件名，即为A.xml
// custom_attrs: tcm配置文件proc_deploy.xml配置的属性值，格式为"Key1=Value1;Key2=Value2;"
int replace_template(const char *src_filename, const char *dst_filename, const char *custom_attrs);

#ifdef __cplusplus
}
#endif

#endif //XREPLACER_H__

