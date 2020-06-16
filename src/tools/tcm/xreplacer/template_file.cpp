/*
 * =====================================================================================
 *
 *       Filename:  template_file.cpp
 *    Description:  
 *        Created:  03/28/2012 05:51:01 PM CST
 *         Author:  andyzhu, andyzhu@tencent.com
 *
 * =====================================================================================
 */

#include <iostream>
#include <fstream>
#include <assert.h>
#include <string.h>
#include "template_file.h"


static bool add_macro(TEMPLATE_FILE *file, int pos_begin, int pos_end)
{
    assert(file->macro_num < sizeof(file->macros)/sizeof(file->macros[0]));

    file->macros[file->macro_num].macro_pos_begin = pos_begin;
    file->macros[file->macro_num].macro_pos_end   = pos_end;
    file->macro_num++;

    return true;
}

static bool add_group(TEMPLATE_FILE *file, int pos_begin, int pos_end, int group_pos_begin, int group_pos_end)
{
    assert(file->group_num < sizeof(file->groups)/sizeof(file->groups[0]));

    file->groups[file->group_num].macro_pos_begin = pos_begin;
    file->groups[file->group_num].macro_pos_end   = pos_end;
    file->groups[file->group_num].group_pos_begin = group_pos_begin;
    file->groups[file->group_num].group_pos_end   = group_pos_end;
    file->group_num++;

    return true;
}

static bool load_template_contents(const char *filename, string &contents)
{
    char buff[4*1024];
    int buff_len = 0;

    // 载入template文件
    ifstream fin(filename, ifstream::in);
    if (fin.fail())
    {
        cout << "[ERROR] failed to open " << filename << endl;
        return false;
    }

    fin.read(buff, sizeof(buff));
    buff_len = fin.gcount();
    while (buff_len > 0)
    {
        contents.append(buff, buff_len);
        fin.read(buff, sizeof(buff));
        buff_len = fin.gcount();
    }

    fin.close();
    return true;
}

static bool filter_placeholder(TEMPLATE_FILE *file)
{
    size_t pos1 = file->contents.find(SYMBOL_MACRO, 0);
    size_t pos2 = file->contents.find(SYMBOL_MACRO, pos1 + 1);

    // 找到一对$符号
    while ((pos1 != string::npos) && (pos2 != string::npos))
    {
        size_t last_valid_pos = -1;

        for (size_t i=pos1+1; i<pos2; ++i)
        {
            // 合法的符号定义仅允许: a-z, A-Z, 0-9, '_'
            if (!isalnum(file->contents[i]) && (file->contents[i] != '_'))
            {
                break;    
            }
            
            last_valid_pos = i;
        }

        // 这是一个简单的占位符
        if (last_valid_pos == (pos2-1))
        {
            add_macro(file, pos1, pos2);

            pos1 = file->contents.find(SYMBOL_MACRO, pos2 + 1);
            pos2 = file->contents.find(SYMBOL_MACRO, pos1 + 1);
        }
        // 是否是一个组类型的占位符(以[]结尾?)
        else if ((last_valid_pos == (pos2 - 3))
            && (file->contents[last_valid_pos+1] == '[')
            && (file->contents[last_valid_pos+2] == ']'))
        {
            // 组类型占位符(以[]结尾)要求出现一对'${$'和'$}$'来标记组的范围
            // 如果未出现，说明定义无效，报错并退出
            size_t group_pos1 = file->contents.find(SYMBOL_GROUP_BEGIN, pos2 + 1);
            if (group_pos1 == string::npos)
            {
                cout << "[ERROR] missing group begin symbol '${$' for macro "
                     << file->contents.substr(pos1+1, pos2 - pos1 - 1)
                     << endl;
                return false;
            }
            
            size_t group_pos2 = file->contents.find(SYMBOL_GROUP_END, pos2 + 1);
            if (group_pos2 == string::npos)
            {
                cout << "[ERROR] missing group end symbol '$}$' for macro "
                     << file->contents.substr(pos1+1, pos2 - pos1 - 1)
                     << endl;
                return false;
            }

            add_group(file, pos1, pos2, group_pos1, group_pos2);
            
            pos1 = file->contents.find(SYMBOL_MACRO, group_pos1 + strlen(SYMBOL_GROUP_BEGIN));
            pos2 = file->contents.find(SYMBOL_MACRO, pos1 + 1);
        }
        else if ((pos2 == pos1 + 2)
                && (file->contents[pos1+1] == '{' || file->contents[pos1+1] == '}'))
        {
            pos1 = pos2 + 1;
            pos2 = file->contents.find(SYMBOL_MACRO, pos1 + 1);
        }
        // 其它的均认为非法，直接跳过
        else
        {
            pos1 = pos2;
            pos2 = file->contents.find(SYMBOL_MACRO, pos1 + 1);
        }
    }
   
    return true;
}


bool load_template(const char *filename, TEMPLATE_FILE *file)
{
    file->macro_num = 0;
    file->group_num = 0;
    file->filename = filename;

    bool success = load_template_contents(filename, file->contents);
    if (!success) return false;

    success = filter_placeholder(file);
    if (!success) return false;

    return true;
}

