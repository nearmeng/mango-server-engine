/*
 * =====================================================================================
 * 
 *       Filename:  deploy_customattr.h
 *    Description:
 *        Created:  03/28/2012 05:49:19 PM CST
 *         Author:  andyzhu , andyzhu@tencent.com
 * 
 * =====================================================================================
 */

#ifndef  DEPLOY_CUSTOMATTR_INC
#define  DEPLOY_CUSTOMATTR_INC


using namespace std;

//-----------------------------------------------
// 部署文件给出的宏定义
struct DEPLOY_MACRO
{
    string key;
    string value;
};

bool load_customattr(const char *custom_attrs, const char *dst_filename);
bool contains_group(const string &group);
const char* lookup_key(const string &key_name, const string &group_name);

void debug_dump_customattr(const char *dump_file_name);

#endif   /* ----- #ifndef DEPLOY_CUSTOMATTR_INC  ----- */

