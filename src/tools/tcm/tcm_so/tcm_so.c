#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fstream>
#include <iostream>
#include "tcm_config_plunin.h"
#include "tcm_config_api.h"
#include "xreplacer.h"

using namespace std;

#define UNUSED_ARG(x) x __attribute__((__unused__))

struct SoEnv
{
    HAPIHANDLE handle; /*maintain the api handle*/
	string template_home_path;
};

static SoEnv mydata;

static string get_path(string tcm_template_path, string work_path, string proc_name)
{
	work_path = work_path.substr(work_path.rfind("mango"), work_path.length());

	string proc_work_path  = tcm_template_path + "/" + work_path;
    string proc_config_path = proc_work_path;

    // 如果目录下有文件说明其配置文件所属路径，那么以配置文件内容为准
	string config_path_file = proc_work_path + "/config_path.ini";
	ifstream fin(config_path_file.c_str());
	if (!fin.fail())
	{
		string tmp;
		fin >> tmp;
		fin.close();
        proc_config_path = proc_work_path + "/" + tmp;
	}

    return proc_config_path;
}

static int clear_dir(const string &dir)
{
    DIR *dp = opendir(dir.c_str());
    if (dp == NULL)
    {
        cerr << "[ERROR] Target directory '" << dir << "' not exsits" << endl;
        return 0;
    }

    int result = 0;
    struct dirent *dirp = NULL;

    while ((dirp = readdir(dp)) != NULL)
    {
        string dname(dirp->d_name);
        if ((dname == ".") || (dname == ".."))
        {       
            continue;
        }       

        string fullpath = dir + "/" + dname;
        int ret = remove(fullpath.c_str());
        if (ret != 0)
        {       
            cerr << "[ERROR] Failed to remove " << fullpath 
                 << ". errno=" << errno
                 << endl;
            result = -1;
        }       
    }

    closedir(dp);
    return result;
}

static int create_all_config(const string &bus_addr, const string &template_dir, const string &output_dir, const string &custom_attr)
{
	DIR *dp = opendir(template_dir.c_str());
	if(dp == NULL)
	{
		cerr << "[ERROR] Failed to open config path: " << template_dir << endl;
		return -1;
	}

    int ret = clear_dir(output_dir);
    if (ret != 0)
    {
        cerr << "[ERROR] Failed to clear target directory '" << output_dir << "'" << endl;
      	closedir(dp);
        return -1;
    }

	cout << "template_dir is " << template_dir;

    int total_cfgfile = 0;
    int total_success = 0;

	struct dirent *dirp = NULL;

	while ((dirp = readdir(dp)) != NULL)
	{
		string dname(dirp->d_name);			

		if ((dname==".") || (dname==".."))
		{
			continue;
		}
		
		struct stat buf;
		string target_file = template_dir + "/" + dname;
		int ret = lstat(target_file.c_str(), &buf);
		if (ret != 0)
		{
            cerr << "[ERROR] Failed to lstat " << template_dir + dname << ", errno " << errno << endl;
		}
		if (!S_ISREG(buf.st_mode))
		{
            continue;
		}
		
		string file_name = dname;

        // 我们能支持的模版文件是'.template'和'.bus_template'
        size_t pos_sep = file_name.find_last_of('.');
        if (pos_sep == string::npos)
        {
            continue;
        }

        string ext_name = file_name.substr(pos_sep + 1);
        file_name = file_name.substr(0, pos_sep);

        string template_filename = template_dir + "/" + file_name + "." + ext_name;
        string target_filename = output_dir + "/" + file_name;

		if (ext_name == "template")
		{				
            // nothing
        }
        else if (ext_name == "bus_template")
        {
            target_filename += "_" + bus_addr;
        }
        else
        {
            continue;
        }

        ++total_cfgfile;
        ret = replace_template(template_filename.c_str(), target_filename.c_str(), custom_attr.c_str());
        if (ret == 0)
        {
            ++total_success;
            cout << "[NSS] Successfully expand " << target_filename << endl;
        }
        else
        {
            cerr << "[ERROR] Failed to expand " << target_filename << endl;
        }
	}

	closedir(dp);

	if (total_success == total_cfgfile)
	{
	    cout << "[NSS] Successfully expand all(" << total_cfgfile
	         << ") template files" << "\n" << endl;
	    return 0;
	}
	else
	{
	    cerr << "[ERROR] Failed to expand " << total_cfgfile - total_success
	         << " template files" << "\n" << endl;
        return -1;
	}
}
  

char* mango_getname(struct config_plunin *p __attribute__((__unused__)))
{
    return "mango";
}

char* mango_getversion(struct config_plunin *p __attribute__((__unused__)))
{
    return "1.0.0";
}

int mango_init(struct config_plunin *p)
{
    printf("[mango] Enter Mango plugin built at 2020-06-16\n");

    SoEnv* pstEnv = (SoEnv*)p->data;
	pstEnv->template_home_path = "../conf/template";

    return 0;
}

int mango_cleanup(UNUSED_ARG(struct config_plunin *p))
{
    return 0;
}

int mango_createconfig(struct config_plunin *p, int id, char* dir, UNUSED_ARG(char* error), UNUSED_ARG(int maxlen))
{
	APIPROCINST proc;
	SoEnv *env = (SoEnv*)p->data;

	int ret = tcmapi_get_proc_inst(env->handle, &proc,id);
	if (ret != 0)
	{
		cout << "[ERROR] Failed to get proc inst. ret " << ret << endl;
		return -1;
	}

	cout << "[mango] Start creating cfg. Proc: " << proc.szProcName << endl;	

    string proc_customattr;

    const char *group_customattr = tcmapi_get_group_customattr(env->handle, proc.iID);    
    if (group_customattr)
    {
        proc_customattr.append(group_customattr);
    }

    const char *layer_customattr = tcmapi_get_layer_customattr(env->handle, proc.iID);
    if (layer_customattr)
    {
        proc_customattr.append(layer_customattr);
    }

    proc_customattr += "ADDR=";
    proc_customattr += proc.szAddr;
    proc_customattr += ";";

	string config_path = get_path(env->template_home_path, proc.szWorkPath, proc.szProcName); 
    cout << "[mango] customattr: " << proc_customattr << endl;

	return create_all_config(proc.szAddr, config_path, dir, proc_customattr);
}

int config_plunin_init(struct config_plunin *p, HAPIHANDLE handle)
{
    printf("[mango] Start init plugin\n");
    p->getname = mango_getname;
    p->getversion = mango_getversion;
    p->init = mango_init;
    p->cleanup = mango_cleanup;
    p->createconfig = mango_createconfig;

    mydata.handle = handle;
    p->data = (void*)&mydata;

    return 0;
}

