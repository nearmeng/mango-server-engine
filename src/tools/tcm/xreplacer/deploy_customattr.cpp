/*
 * =====================================================================================
 *
 *       Filename:  deploy_customattr.cpp
 *    Description:  
 *        Created:  03/28/2012 05:49:21 PM CST
 *         Author:  andyzhu, andyzhu@tencent.com
 *
 * =====================================================================================
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>

#include "deploy_customattr.h"

static vector<DEPLOY_MACRO> attrs;

void debug_dump_customattr(const char *dump_file_name)
{
    ofstream fout(dump_file_name, ios_base::app);
    if (fout.fail())
    {
        cout << "[WARNING] Failed to write debug info" << endl;
        return;
    }

    fout << "====== Result customt attr" << endl;

    // For pretty output :) (naughty)
    size_t max_width = 0;
    for (size_t i=0; i<attrs.size(); ++i)
    {
        if (attrs[i].key.length() > max_width)
        {
            max_width = attrs[i].key.length();
        }
    }

    fout.setf(ios::left);
    for (size_t i=0; i<attrs.size(); ++i)
    {
        fout << setw(max_width) 
             << attrs[i].key
             << "="
             << attrs[i].value
             << endl;
    }

    fout << endl;
    fout.close();
}

bool load_customattr(const char *custom_attrs, const char *dst_filename)
{
    attrs.clear();

    string input = custom_attrs;

    size_t pos = 0;
    size_t len = input.length();

    while (pos < len)
    {
        // 跳过空格
        while (isspace(input[pos]) && (pos < len))
        {
            ++pos;
        }

        if (pos >= len)
        {
            break;
        }
        
        size_t pos_equal = input.find('=', pos);
        size_t pos_semicolon = input.find(';', pos_equal);

        //TODO bug for last key:value not endwith ';' 
        if ((pos_equal == string::npos) || (pos_semicolon == string::npos))
        {
            cout << "[ERROR] Failed to parse custom attr: "
                 << input.substr(pos)
                 << endl;
            return false;
        }

        DEPLOY_MACRO macro;
        //TODO maybe bug while key or value have space,such as "key1 = value1; key2= value2;"
        macro.key = input.substr(pos, pos_equal - pos);
        macro.value = input.substr(pos_equal + 1, pos_semicolon - pos_equal - 1);
        attrs.push_back(macro);

        pos = pos_semicolon + 1;
    }
    
    return true;
}

static DEPLOY_MACRO* lookup_macro(const string &key_name)
{
    for (size_t i=0; i<attrs.size(); ++i)
    {
        if (attrs[i].key == key_name)
        {
            return &attrs[i];
        }
    }

    return NULL;
}

// 这里传入参数应该是类似"GroupName."或者"GroupName[0]."
bool contains_group(const string &group_name)
{
    if (group_name.length() == 0)
    {
        return false;
    }
    
    for (size_t i=0; i<attrs.size(); ++i)
    {
        if (attrs[i].key.length() <= group_name.length())
        {
            continue;
        }

        if (0 == attrs[i].key.compare(0, group_name.length(), group_name))
        {
            return true;
        }
    }

    return false;
}

const char* lookup_key(const string &key_name, const string &group_name)
{
    DEPLOY_MACRO *m = NULL;

    if (group_name.length() > 0)
    {
        string full_key = group_name + key_name;
        m = lookup_macro(full_key);
    }

    if (m != NULL)
    {
        return m->value.c_str();
    }

    m = lookup_macro(key_name);
    if (m != NULL)
    {
        return m->value.c_str();
    }

    return NULL;
}

