/*
 * =====================================================================================
 * 
 *       Filename:  template_file.h
 *    Description:
 *        Created:  03/28/2012 05:50:58 PM CST
 *         Author:  andyzhu , andyzhu@tencent.com
 * 
 * =====================================================================================
 */

#ifndef  TEMPLATE_FILE_INC
#define  TEMPLATE_FILE_INC

#include <vector>
#include <string>

using namespace std;

static const char SYMBOL_MACRO = '$';
static const char *SYMBOL_GROUP_BEGIN = "${$";
static const char *SYMBOL_GROUP_END = "$}$";

// $ABC$
struct TEMPLATE_MACRO
{
    int macro_pos_begin;    // 占位符起始位置(左边$符号位置)
    int macro_pos_end;      // 占位符结束位置(右边$符号位置)
};

// $ABC[]$
// ${$
//    ...
// $}$
struct TEMPLATE_GROUP
{
    int macro_pos_begin;    // 占位符起始位置(左边$符号位置)
    int macro_pos_end;      // 占位符结束位置(右边$符号位置)
    int group_pos_begin;    // '${$'所在位置
    int group_pos_end;      // '$}$'所在位置
};

struct TEMPLATE_FILE
{
    string filename;
    string contents;
    size_t macro_num;
    TEMPLATE_MACRO macros[1024];
    size_t group_num;
    TEMPLATE_GROUP groups[256];
};

bool load_template(const char *filename, TEMPLATE_FILE *file);

#endif   /* ----- #ifndef TEMPLATE_FILE_INC  ----- */

