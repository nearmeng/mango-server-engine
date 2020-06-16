#include "xreplacer.h"
#include "template_file.h"
#include "deploy_customattr.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

using namespace std;

static string debug_dump_file = "";
static const int MAX_DUMP_SIZE = 4 * 1024 * 1024;

static void truncate_debug_dump()
{
    struct stat buf;
    int ret = stat(debug_dump_file.c_str(), &buf);
    if ((ret == 0) && (buf.st_size > MAX_DUMP_SIZE))
    {
        ret = remove(debug_dump_file.c_str());
        if (ret < 0)
        {
            cout << "[WARNING] Failed to truncate debug dump. err: " << ret << endl;
        }
    }
}

static void debug_dump_line(const string &debug_info)
{
    ofstream fout(debug_dump_file.c_str(), ios_base::app);
    if (fout.fail())
    {
        cout << "[WARNING] Failed to write debug info:" << debug_dump_file
             << ". Please remove it at first!" << endl;
        return;
    }

    fout << debug_info << endl;
    fout.close();
}


static bool calc_macro_range(TEMPLATE_FILE *file, int pos_begin, int pos_end, int &macro_idx_begin, int &macro_idx_end)
{
    macro_idx_begin = -1;
    macro_idx_end = -1;

    for (size_t i=0; i<file->macro_num; ++i)
    {
        if ((file->macros[i].macro_pos_begin >= pos_begin)
            && (file->macros[i].macro_pos_end <= pos_end))
        {
            macro_idx_begin = i;
            break;
        }
    }

    for (int i=(int)file->macro_num-1; i>=0; --i)
    {
        if ((file->macros[i].macro_pos_begin >= pos_begin)
            && (file->macros[i].macro_pos_end <= pos_end))
        {
            macro_idx_end = i;
            break;
        }
    }
    
    return true;
}

static bool replace_content(TEMPLATE_FILE *file, int pos_begin, int pos_end, const string &group_name, string &result)
{
    int macro_idx_begin = -1;
    int macro_idx_end = -1;

    calc_macro_range(file, pos_begin, pos_end, macro_idx_begin, macro_idx_end);

    // 没有一个宏需要展开
    if (macro_idx_begin < 0)
    {
        result += file->contents.substr(pos_begin, pos_end - pos_begin + 1);
        return true;
    }

    // 整个文本被宏分割
    int cur_pos = pos_begin;
    for (int i=macro_idx_begin; i<=macro_idx_end; ++i)
    {
        TEMPLATE_MACRO *m = &file->macros[i];
        result += file->contents.substr(cur_pos, m->macro_pos_begin - cur_pos);

        const string key_name = file->contents.substr(m->macro_pos_begin + 1, m->macro_pos_end - m->macro_pos_begin - 1);
        const char *value = lookup_key(key_name, group_name);

        if (value == NULL)
        {
            if (group_name.length() == 0)
            {
                cout << "[ERROR] Undefined attribute '" << key_name << "'" << endl;
            }
            else
            {
                cout << "[ERROR] Undefined attribute '" << key_name << "'"
                     << " in Group '" << group_name << "'" << endl;
            }
            return false;
        }
        debug_dump_line("Replace Key: " + key_name + " -> " + value);

        result += value;
        cur_pos = m->macro_pos_end + 1;
    }

    result += file->contents.substr(cur_pos, pos_end - cur_pos + 1);

    return true;
}

static bool replace_group(TEMPLATE_FILE *file, int pos_begin, int pos_end, const string &group_name, string &result)
{
    string single_group = group_name + ".";
    string multi_groups = group_name + "[";

    bool contains_single = contains_group(single_group);
    bool contains_multiple = contains_group(multi_groups);

    if (contains_single && contains_multiple)
    {
        cout << "[ERROR] Found conflict definitions for group '"
             << group_name << "'" << endl;
        return false;
    }

    // group定义一个都没有，那么表示要忽略掉
    if (!contains_single && !contains_multiple)
    {
        debug_dump_line("[WARNING] Ignore Group " + group_name);
        return true;
    }
    
    if (contains_single)
    {
        return replace_content(file, pos_begin, pos_end, single_group, result);    
    }

    int num_idx = 0;
    char str_idx[6];

    sprintf(str_idx, "%d", num_idx);    
    string full_group_name = multi_groups + str_idx + "].";
    
    debug_dump_line("Replace Group " + full_group_name);
    while (contains_group(full_group_name))
    {
        bool success = replace_content(file, pos_begin, pos_end, full_group_name, result);
        if (!success) return false;

        num_idx++;
        sprintf(str_idx, "%d", num_idx);    
        full_group_name = multi_groups + str_idx + "].";
    }

    return true;
}

static bool replace_template_file(TEMPLATE_FILE *file, string &result)
{
    // 对整个文档按group定义分割
    if (file->group_num <= 0)
    {
        return replace_content(file, 0, file->contents.length(), "", result);
    }

    int cur_pos = 0;
    for (size_t i=0; i<file->group_num; ++i)
    {
        TEMPLATE_GROUP *g = &file->groups[i];
        bool success = replace_content(file, cur_pos, g->macro_pos_begin - 1, "", result);
        if (!success) return false;

        // 这里的'-3'是因为group的定义格式为'$ABC[]$'
        const string group_name = file->contents.substr(g->macro_pos_begin + 1, g->macro_pos_end - g->macro_pos_begin - 3);
        int content_pos_begin = g->group_pos_begin + strlen(SYMBOL_GROUP_BEGIN);
        int content_pos_end = g->group_pos_end - 1;

        success = replace_group(file, content_pos_begin, content_pos_end, group_name, result);
        if (!success)
        {
            cout << "[ERROR] Failed to Expand group '" << group_name << endl;
            return false;
        }

        cur_pos = g->group_pos_end + strlen(SYMBOL_GROUP_END);
    }

    return replace_content(file, cur_pos, file->contents.length(), "", result);
}

int replace_template(const char *src_filename, const char *dst_filename, const char *custom_attrs)
{
    bool success = false;
    TEMPLATE_FILE file;

    debug_dump_file = "/tmp/tcm_customattr_tmp.debug";

    const char *username = getenv("USER");
    if (username != NULL)
    {
        debug_dump_file += ".";
        debug_dump_file += username;
    }

    truncate_debug_dump();

    time_t now = time(NULL);
    struct tm *tm_now = localtime(&now);

    debug_dump_line("===========================================");
    debug_dump_line(string("Time: ") + asctime(tm_now));
    debug_dump_line(string("Replace file ") + src_filename);
    debug_dump_line(string("Target file ") + dst_filename);

    success = load_template(src_filename, &file);
    if (!success) return -1;

    debug_dump_line("Input custom attr");
    debug_dump_line(custom_attrs);
    
    success = load_customattr(custom_attrs, dst_filename);
    if (!success) return -1;

    debug_dump_customattr(debug_dump_file.c_str());

    string result;
    success = replace_template_file(&file, result);
    if (!success) return -1;

    ofstream fout(dst_filename);
    if (fout.fail())
    {
        cout << "[ERROR] Failed to save " << dst_filename << endl;
        return -1;
    }

    fout << result;
    fout.close();

    return 0;
}

