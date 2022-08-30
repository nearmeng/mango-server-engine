#!/usr/bin/python
#-*- coding: utf-8 -*-

import os, stat

#for linux
import sys
import getopt
import shutil
import platform
import pdb
import getpass
import yaml
from shutil import copyfile 

if sys.version > '3':
    import configparser
else:
    import ConfigParser

file = None
deploy_config = dict()
procs_config = dict()
procs_connect_config = []

def load_local_deploy_config(filename):
    global deploy_config
    deploy_config = yaml.safe_load(open(filename, 'r'))
    print("deploy_config: %s\n" % deploy_config)

def load_proc_config(filename): 
    global procs_config
    load_config = yaml.safe_load(open(filename, 'r'))
    procs_config = load_config['procs']
    print("procs_config: %s\n" % procs_config)

def load_proc_connect_config(filename): 
    global procs_connect_config
    load_config = yaml.safe_load(open(filename, 'r'))
    procs_connect_config = load_config['proc_connect']
    print("proc connnect config: %s\n" % procs_connect_config)

def get_proc_list():
    get_proc_list = list()
    if not ('proc_list' in procs_config):
        print("ERROR")
    for k,v in procs_config['proc_list'].items():
        if not ('local_start' in v) or v['local_start'] == True:
            get_proc_list.append(k)
    return get_proc_list

def proc_funcid(proc_name):
    if not (proc_name in procs_config['proc_list']):
        print("[ERROR] key '%s' not found in 'proc.py'" % (proc_name))
        sys.exit(-1)
    return procs_config['proc_list'][proc_name]["func_id"]

def proc_count(proc_name):
    if ((proc_name + "_count") in deploy_config):
        count = deploy_config[proc_name + "_count"]
    else:
        count = 1
    return count

# to save key value pairs for expand
class KeyValuePairs:
    def __init__(self):
        self.pairs = []
    def add(self, key, val):
        self.pairs.append((key, val))
    def items(self):
        return self.pairs
    def keys(self):
        return [pair[0] for pair in self.pairs]

# expand proc
def expand_proc(proc_name, key_value_pairs=None, postfix=None):
    proc_cfg_dir = proc_name
    print("Expand", proc_cfg_dir)

    replace_filename = proc_cfg_dir + "/" + "_replace.ini"
    replace_file = open(replace_filename, "w")

    if key_value_pairs == None:
        key_value_pairs = KeyValuePairs()
    
    # add default vars
    if "tbus_gcim_key" not in key_value_pairs.keys():
        replace_file.write("tbus_gcim_key=%s;\n" %  tbus_key)
    if "tconnd_port" not in key_value_pairs.keys():
        replace_file.write("tconnd_port=%s;\n" % tconnd_port)
    if "tbus_grm_key" not in key_value_pairs.keys():
        replace_file.write("tbus_grm_key=%s;\n" % tbus_grm_key)
    if "tbusd_port" not in key_value_pairs.keys():
        replace_file.write("tbusd_port=%s;\n" % tbusd_port)

    # add external vars
    if key_value_pairs:
        for key, val in key_value_pairs.items():
            replace_file.write("%s=%s;\n" % (key, val))

    # include local deploy
    if deploy_config:
        for key,val in deploy_config.items():
            replace_file.write("%s=%s;\n" % (key, val))

    # add file append vars
    default_filename = proc_cfg_dir + "/" + "default.ini"
    if os.path.exists(default_filename):
        for line in open(default_filename, "r").readlines():
            if not line.strip() : continue
            replace_file.write(line[:-1] + ";\n")

    replace_file.close()

    # 执行template文件展开
    xfile_linux = "tools/xfile/xfiled"
    xfile_win = ".\\tools\\xfile\\xfile.exe"
    if (is_linux):
        cmd = "%s -p %s -f %s" % (xfile_linux, proc_cfg_dir, replace_filename)
    else:
        cmd = "%s -p %s -f %s" % (xfile_win, proc_cfg_dir, replace_filename)
    if postfix:
        cmd += " --postfix " + postfix

    output = os.popen(cmd).read()
    if output:
        print(output)
        sys.exit(-1)

    os.remove(replace_filename)

def write_channel(addr1, addr2, desc1, desc2, inst1, inst2, size = 4096000):
    file.write("<Channels>\n")
    file.write("    <Priority>8</Priority>\n")
    file.write("    <Address>%s</Address>\n" % (addr1))
    file.write("    <Address>%s</Address>\n" % (addr2))
    file.write("    <SendSize>%s</SendSize>\n" % (size))
    file.write("    <RecvSize>%s</RecvSize>\n" % (size))
    file.write("    <Desc>%s%d_to_%s%d</Desc>\n" % (desc1, inst1, desc2, inst2))
    file.write("</Channels>\n")
    file.write("\n")

def write_across_channel(proc1, proc2, inst_num1 = 1, inst_num2 = 1):
    if proc1 == proc2 :
        return

    for inst1 in range(inst_num1):
        for inst2 in range(inst_num2):
            proc1_addr = "%s.%s.%s.%s" % (idc_id, zone_id, proc_funcid(proc1), inst1 + 1)
            proc2_addr = "%s.%s.%s.%s" % (idc_id, zone_id, proc_funcid(proc2), inst2 + 1)
            write_channel(proc1_addr, proc2_addr, proc1, proc2, inst1 + 1, inst2 + 1)

def generate_tbus_mgr_cfg():
    global file
    file = open("./tools/tbus_mgr/tbusmgr.xml", "w")
    file.write("<TbusGCIM>\n")
    file.write("<BusinessID>0</BusinessID>\n")
    file.write("<AddrTemplet>6.8.8.10</AddrTemplet>\n")
    file.write("<GCIMShmKey>%s</GCIMShmKey>\n" % tbus_key)

    for conn in procs_connect_config:
        print("build channel across %s to %s" % (conn['peer1'], conn['peer2']))
        write_across_channel(conn['peer1'], conn['peer2'], proc_count(conn['peer1']), proc_count(conn['peer2']))

    file.write("</TbusGCIM>\n")
    file.close()

def generate_tlog_file_by_config(filename, proc_name, tlog_config):
    file_header = '''<?xml version="1.0" encoding="GBK" standalone="yes" ?>
    <TLOGConf version="2">
    <Magic>1548 </Magic>
    <PriorityHigh>NULL </PriorityHigh> ''' + '\n    <PriorityLow> $%s_log_priority$ </PriorityLow>' % proc_name;

    file_header += '''
    <DelayInit>0 </DelayInit>
    <SuppressError>0</SuppressError> ''';
    
    file_category_all = ""
    for category,category_config in tlog_config.items():
        file_category_header = '''
    <CategoryList type="TLOGCategory">
    <Name>text</Name>
    <PriorityHigh>NULL </PriorityHigh>
    <PriorityLow>DEBUG </PriorityLow>
    <LevelDispatch>0 </LevelDispatch>
    <MustSucc>1</MustSucc>
    <MaxMsgSize>0 </MaxMsgSize>
    <Format>%d.%u [%p] %m%n</Format>
    <ForwardCat></ForwardCat> ''' + '''
        <Device type="TLOGDevAny">
            <Type>VEC</Type>
            <Device>
                <Vec> 
                    <Count>%d</Count>''' % (len(category_config));
        file_category_element_all = ""
        for element in category_config:
            if element == "tlogd":
                file_category_element = '''
                        <Elements>
                            <ID>0</ID>
                            <Ctrl>0</Ctrl>
                            <Type>NET</Type>
                            <Device>
                                <Net> 
                                    <Url>$%s_%s_tlogd_addr$</Url>
                                    <Sendbuff>1024000</Sendbuff>
                                    <RecvBuff>1024000</RecvBuff>
                                    <AutoReconnect>10</AutoReconnect>
                                    <MaxRetry>20</MaxRetry>
                                    <ConnTimeout>1000</ConnTimeout>
                                    <SendTimeout>1000</SendTimeout>
                                </Net>
                            </Device>
                        </Elements> ''' % (proc_name, category);
            elif element == "local":
                file_category_element = '''
                        <Elements>
                            <ID>0</ID>
                            <Ctrl>0</Ctrl>
                            <Type>FILE</Type>
                            <Device>
                               <File> 
                                   <Pattern>$tlog_home$/%s_%s.log_$%s_tbus$</Pattern>
                                   <BuffSize>1024000</BuffSize>
                                   <SizeLimit>10240000</SizeLimit>
                                   <Precision>1</Precision>
                                   <MaxRotate>0</MaxRotate>
                                   <SyncTime>$tlog_write_interval$</SyncTime>
                                   <NoFindlatest>0</NoFindlatest>
                               </File>
                            </Device>
                        </Elements> ''' % (proc_name, category, proc_name);
            elif element == "tglog":
                file_category_element = '''
				         <Elements>
                             <ID>0</ID>
                             <Ctrl>0</Ctrl>
                             <Type>NET</Type>
                             <Device>
                                 <Net>
                                     <Url>$tglog_addr$</Url>
                                     <Sendbuff>1024000</Sendbuff>
                                     <RecvBuff>1024</RecvBuff>
                                     <AutoReconnect>10</AutoReconnect>
                                     <MaxRetry>20</MaxRetry>
                                     <ConnTimeout>1000</ConnTimeout>
                                     <SendTimeout>1000</SendTimeout>
                                 </Net>
                             </Device>
                         </Elements> ''';
            else:
                print("proc %s tlog config element '%s' is not support" % (proc_name, element))
                sys.exit(-1)
            file_category_element_all += file_category_element
            
        file_category_tail = '''
                </Vec>
            </Device>
        </Device>
    </CategoryList> '''
        
        file_category_all += file_category_header + file_category_element_all + file_category_tail
    file_tail = "\n</TLOGConf>"
    file_all = file_header + file_category_all + file_tail
    
    file = open(filename, "w")
    file.write(file_all)
    file.close()

def generate_cfg():
    proc_list = get_proc_list()

    for proc_name in proc_list:
        print("check cfg file for proc " + proc_name)
        log_filename = "./%s/tlog.xml.bus_template" % (proc_name)

        if ('tlog_config' in procs_config['proc_list'][proc_name]):
            print("generate cfg file for proc " + proc_name)
            generate_tlog_file_by_config(log_filename, proc_name, procs_config['proc_list'][proc_name]['tlog_config'])
        else:
            if not os.path.exists(log_filename):
                print("[ERROR] proc '%s' lost tlog file, and lost tlog_config in proc config file" % proc_name)
                sys.exit(-1)
    
def proc_real_name(proc_name): 
    if not (proc_name in procs_config['proc_list']):
        print("[ERROR] key %s not found in procs config" % (proc_name))
        sys.exit(-1) 

    if "real_name" in procs_config['proc_list'][proc_name].keys():
        return procs_config['proc_list'][proc_name]['real_name']
    else: 
        return proc_name 

def generate_procmgr():
    procmgr_file = open("procmgr.py", "w")	   
	
    procmgr_file.write('''
#!/usr/bin/python 
#-*- coding: utf-8 -*-
    
import os
import sys
import getopt
import time
    
idc_id = {idc_id}
zone_id = {zone_id}
is_linux = {is_linux}
tbus_key = {tbus_key}
    
def get_pid(proc_name, proc_str):
    if is_linux: 
        find_result = int(os.popen("ps ux | grep %s |grep \\"\-\-id=%s\\" | wc -l" % (proc_name, proc_str)).read())
        if find_result >= 1 : 
            return int(os.popen("ps ux | grep %s | grep \\"\-\-id=%s\\" | awk '{{print $2}}'" % (proc_name, proc_str)).read()) 
        else: 
            return 0 
    else: 
        find_result = os.popen("tasklist ^ | findstr %s" % proc_name).read() 
        find_list = [] 
        for string in find_result.split(\' \'):
            if not string == "": 
                find_list.append(string) 
        if len(find_list) >= 2: 
            return find_list[1] 
        else: 
            return 0 

def check_input_args(opt_result, arg):
    for k, v in opt_result: 
        if k == arg: 
            return True 
    return False 

def is_debug():
    if check_input_args(opts, '-d') or check_input_args(opts, '--debug'): 
        return True 
    else: 
        return False 

def proc_tbus_flag(use_tbus):
    return use_tbus and ("--use-bus --bus-key=%s " % tbus_key ) or "" 

def proc_conf_flag(proc_dir, conf_prefix, tbus_str): 
    if conf_prefix == "": return "--noloadconf " 
    if is_linux: 
        return " --conf-file=./%s_%s" % (conf_prefix, tbus_str) 
    else: 
        split_str = conf_prefix.split('.') 
        if not len(split_str) == 2: 
            print("[ERROR]: conf_prefix str format error") 
            sys.exit(-1) 
        else: 
            return " --conf-file=%s\%s_%s.%s" % (os.getcwd(), split_str[0], tbus_str, split_str[1]) 

def proc_log_flag(proc_dir, tbus_str): 
    if is_linux: 
        return " --tlogconf=./tlog.xml_%s" % (tbus_str) 
    else: 
        if proc_dir.find("tconnd") < 0: 
            return " --tlogconf=%s\%s_%s.xml" % (os.getcwd(), "tlog", tbus_str) 
        else: 
            return " " 

def proc_rundata_timer_flag(proc_dir): 
    return " --rundata_timer=./rundata_timer.xml" 

def usage(): 
    print("Usage: python procmgr.py [OPTION] start|stop|reload|resume|gdb|check|control [TARGET_PROC_NAME]") 
    print("Example: python procmgr.py start") 
    print("Example: python procmgr.py stop game_server") 
    print("Example: python procmgr.py -d reload game_server") 
    print("Options Definition:") 
    print("    -h, --help          show the usage of procmgr") 
    print("    -d, --debug         show the debug info of procmgr") 
    print("Attention: If you do not specify the target_proc, then all proc will be executed") 
    print("Attention: If you want to show debug info, please use -d or --debug in your command") 

class Proc: 
    def __init__(self, proc_dir, proc_name, proc_funcid, inst_count = 1, use_tbus = True, conf_prefix = "", proc_param = ""): 
        self.dir  = proc_dir 
        self.name = proc_name 
        self.funcid = proc_funcid 
        self.inst_count = inst_count 
        self.param = proc_param 
        self.fn_execute_flags = lambda tbus_str: (" --id=%s " % tbus_str) + proc_tbus_flag(use_tbus) + proc_conf_flag(proc_dir, conf_prefix, tbus_str) + proc_log_flag(proc_dir, tbus_str) + proc_rundata_timer_flag(proc_dir) 

    def get_proc_dir(self): 
        return self.dir 

    def get_tbus_str(self, inst_id): 
        return "%s.%s.%s.%s" % (idc_id, zone_id, self.funcid, inst_id) 

    def start(self, inst = 0): 
        inst_list = (inst == 0) and range(1, self.inst_count + 1) or [inst] 
        for inst_id in inst_list: 
            if is_linux: 
                os.chdir("%s" % self.dir) 
                if is_debug(): 
                    print("[DBG] ./%s %s --daemon start" % (self.name, self.fn_execute_flags(self.get_tbus_str(inst_id)))) 
                os.system("./%s %s --daemon start" % (self.name, self.fn_execute_flags(self.get_tbus_str(inst_id)))) 
                os.chdir("..") 
            else: 
                os.chdir("%s" % self.dir) 
                if is_debug(): 
                    print("[DBG] start .\%s_d.exe %s" % (self.name, self.fn_execute_flags(self.get_tbus_str(inst_id)))) 
                os.system("start .\%s_d.exe %s" % (self.name, self.fn_execute_flags(self.get_tbus_str(inst_id)))) 
                os.chdir("..") 

    def stop(self, inst = 0): 
        inst_list = (inst == 0) and range(1, self.inst_count + 1) or [inst] 
        for inst_id in inst_list: 
            if is_linux: 
                os.chdir("%s" % self.dir) 
                if is_debug(): 
                    print("[DBG] ./%s %s stop" % (self.name, self.fn_execute_flags(self.get_tbus_str(inst_id)))) 
                os.system("./%s %s stop" % (self.name, self.fn_execute_flags(self.get_tbus_str(inst_id)))) 
                os.chdir("..") 
            else: 
                print("[DBG] signal.exe %s" % get_pid(self.name, self.get_tbus_str(inst_id))) 
                if int(get_pid(self.name, self.get_tbus_str(inst_id))) > 0: 
                    os.system("taskkill /F /pid %s" % get_pid(self.name, self.get_tbus_str(inst_id))) 

    def resume(self, inst = 0): 
        inst_list = (inst == 0) and range(1, self.inst_count + 1) or [inst] 
        for inst_id in inst_list: 
            if is_linux: 
                os.chdir("%s" % self.dir) 
                if is_debug(): 
                    print("[DBG] ./%s %s --daemon resume" % (self.name, self.fn_execute_flags(self.get_tbus_str(inst_id)))) 
                os.system("./%s %s --daemon resume" % (self.name, self.fn_execute_flags(self.get_tbus_str(inst_id)))) 
                os.chdir("..") 
            else: 
                os.chdir("%s" % self.dir) 
                if is_debug(): 
                    print("[DBG] .\%s %s resume" % (self.name, self.fn_execute_flags(self.get_tbus_str(inst_id)))) 
                os.system(".\%s_d.exe %s resume" % (self.name, self.fn_execute_flags(self.get_tbus_str(inst_id)))) 
                os.chdir("..") 

    def reload(self, inst = 0): 
        inst_list = (inst == 0) and range(1, self.inst_count + 1) or [inst] 
        for inst_id in inst_list: 
            if is_linux: 
                os.chdir("%s" % self.dir) 
                if is_debug(): 
                    print("[DBG] ./%s %s --daemon reload" % (self.name, self.fn_execute_flags(self.get_tbus_str(inst_id)))) 
                os.system("./%s %s reload" % (self.name, self.fn_execute_flags(self.get_tbus_str(inst_id)))) 
                os.chdir("..") 
            else: 
                os.chdir("%s" % self.dir) 
                if is_debug(): 
                    print("[DBG] .\%s_d.exe %s reload" % (self.name, self.fn_execute_flags(self.get_tbus_str(inst_id)))) 
                os.system(".\%s_d.exe %s reload" % (self.name, self.fn_execute_flags(self.get_tbus_str(inst_id)))) 
                os.chdir("..") 

    def gdb(self, inst = 0): 
        if not is_linux: 
            print("[WARN]: gdb only support in linux")
        else: 
            inst_list = (inst == 0) and range(1, self.inst_count + 1) or [inst] 
            for inst_id in inst_list: 
                os.chdir("%s" % self.dir) 
                if is_debug(): 
                    print("[DBG] gdb --args ./%s %s start" % (self.name, self.fn_execute_flags(self.get_tbus_str(inst_id)))) 
                os.system("gdb --args ./%s %s start" % (self.name, self.fn_execute_flags(self.get_tbus_str(inst_id)))) 
                os.chdir("..") 

    def check(self, inst = 0): 
        inst_list = (inst == 0) and range(1, self.inst_count + 1) or [inst] 
        for inst_id in inst_list: 
            tbus_str = self.get_tbus_str(inst_id) 
            proc_alive_count = len(os.popen("ps ux | grep %s | grep \\"\-\-id=%s\\" " % (self.name, tbus_str)).readlines()) 
            if proc_alive_count > 0: 
                print("check proc %s %s %s [40;32m [RUNNING] [0m" % (self.name.ljust(25), tbus_str.ljust(10), get_pid(self.name, self.get_tbus_str(inst_id)))) 
            else: 
                print("check proc %s %s %s [40;31m [NOT RUNNING] [0m" % (self.name.ljust(25), tbus_str.ljust(10), get_pid(self.name, self.get_tbus_str(inst_id)))) 

    def fstop(self, inst = 0): 
        inst_list = (inst == 0) and range(1, self.inst_count + 1) or [inst] 
        for inst_id in inst_list: 
            pid = int(get_pid(self.name, self.get_tbus_str(inst_id))) 
            if is_linux: 
                if pid > 0: 
                    if is_debug(): 
                        print("[DBG] process %s kill -9 %s" % (self.name, pid)) 
                    os.system("kill -9 %s" % (pid)) 
            else: 
                print("[DBG] process %s signal.exe %s" % (self.name, pid)) 
                if pid > 0: 
                    os.system("taskkill /F /pid %s" % pid) 

    def control(self, inst = 0): 
        inst_list = (inst == 0) and range(1, self.inst_count + 1) or [inst] 
        for inst_id in inst_list: 
            tbus_str = self.get_tbus_str(inst_id) 
            if is_linux: 
                os.chdir("%s" % self.dir) 
                if is_debug(): 
                    print("[DBG] ./%s %s control" % (self.name, self.fn_execute_flags(self.get_tbus_str(inst_id)))) 
                os.system("./%s %s control" % (self.name, self.fn_execute_flags(self.get_tbus_str(inst_id)))) 
                os.chdir("..") 
            else: 
                os.chdir("%s" % self.dir) 
                if is_debug(): 
                    print("[DBG] .\%s_d.exe %s control" % (self.name, self.fn_execute_flags(self.get_tbus_str(inst_id)))) 
                os.system(".\%s_d.exe %s control" % (self.name, self.fn_execute_flags(self.get_tbus_str(inst_id)))) 
                os.chdir("..") 

all_proc_list = [
'''.format(idc_id = idc_id, zone_id = zone_id, is_linux = is_linux, tbus_key = tbus_key))

    proc_list = get_proc_list()
    for proc_name in proc_list: 
        if ('conf_file_prefix' in procs_config['proc_list'][proc_name]):
            curr_conf = procs_config['proc_list'][proc_name]['conf_file_prefix']
        else: 
            curr_conf = ""
        procmgr_file.write('        Proc("%s", "%s", %s, inst_count = %s, conf_prefix="%s"), \n' % (proc_name, proc_real_name(proc_name), proc_funcid(proc_name), proc_count(proc_name), curr_conf))

    procmgr_file.write('''
] 

def get_operate_proc_list(proc_name): 
   result = [] 
   for proc in all_proc_list: 
        if(proc.dir == proc_name): 
            result += [proc] 
   return result 

def proc_check(): 
    if not is_linux: 
        print("[WARN]: proc check only support in linux") 
    else: 
        for proc in proc_list: 
            proc.check() 


if len(sys.argv) < 2: 
    print("[ERROR]: param input error!") 
    usage() 
    sys.exit(-1) 

opts, args= getopt.getopt(sys.argv[1:], 'dh', ['--debug', '--help']) 

if check_input_args(opts, '-h') or check_input_args(opts, '--help'): 
    usage()
    sys.exit(-1) 

cmd = args[0] 
proc_list = len(args) <= 1 and all_proc_list or get_operate_proc_list(args[1]) 
if len(proc_list) == 0: 
    print("[ERROR] proc %s is not find" % (args[1])) 
    usage() 
    sys.exit(-1) 

if cmd == "start": 
    for proc in proc_list: 
            proc.start() 
    proc_check() 
elif cmd == "stop": 
    for proc in proc_list: 
        proc.stop() 
    if is_linux: 
        time.sleep(3) 
    proc_check() 
elif cmd == "fstop": 
    for proc in proc_list: 
        proc.fstop() 
    proc_check() 
elif cmd == "resume": 
    for proc in proc_list: 
        proc.resume() 
    proc_check() 
elif cmd == "reload": 
    for proc in proc_list: 
        proc.reload() 
    proc_check() 
elif cmd == "gdb": 
    proc_list[0].gdb() 
elif cmd == "check": 
    proc_check() 
elif cmd == "control": 
    if not proc_list[0].get_proc_dir() == "control_server": 
        print("[WARN] only control server can be controled!") 
        proc_list = get_operate_proc_list("control_server") 
    proc_list[0].control() 
else: 
    print("[ERROR] invalid cmd %s" % cmd) 
    usage() 
    sys.exit(-1) 
''')
    os.chmod("procmgr.py", 0o750)	   

def start_expanding():
    proc_list = get_proc_list()

    for proc_name in proc_list:
        print("begin expand " + proc_name)
        for proc_inst in range(proc_count(proc_name)):
            proc_str = "%s.%s.%s.%s" % (idc_id, zone_id, proc_funcid(proc_name), proc_inst + 1)

            attrs = KeyValuePairs()
            attrs.add("world", idc_id)
            attrs.add("zone", zone_id)
            attrs.add("func", proc_funcid(proc_name))
            attrs.add("inst", proc_inst + 1)
            attrs.add("%s_tbus" % proc_name, proc_str);
            attrs.add("%s_log_priority" % proc_name, deploy_config['log_priority'])

            if ('attrs' in procs_config['proc_list'][proc_name]):
                for k,v in procs_config['proc_list'][proc_name]['attrs'].items():
                    attrs.add(k, v)
            
            if ('tlog_config' in procs_config['proc_list'][proc_name]):
                for category,category_config in procs_config['proc_list'][proc_name]['tlog_config'].items():
                    attrs.add("%s_%s_tlogd_addr" % (proc_name, category), "udp://127.0.0.1:6666")
            
            expand_proc(proc_name.lower(), attrs, proc_str)
        
def clear_cfg_file(file_path, suffix):
    length = len(suffix)
    for root, dirs, files in os.walk(file_path):
        for filepath in files:
            if filepath[-length:] == suffix:
                os.chmod(os.path.join(root, filepath), 0o777)
                os.remove(os.path.join(root, filepath))

def clear():
    proc_list = get_proc_list()
    for proc_name in proc_list:
        clear_cfg_file("./%s" % proc_name, "xml")

def refresh_tbus_info():
    print("Begin to refresh tbus")
    if (is_linux):
        os.system("./tools/tbus_mgr/tbusmgr --conf-file=./tools/tbus_mgr/tbusmgr.xml -X")
        os.system("./tools/tbus_mgr/tbusmgr --conf-file=./tools/tbus_mgr/tbusmgr.xml -W")
    else:
        os.system(".\\tools\\tbus_mgr\\tbusmgr.exe --conf-file=.\\tools\\tbus_mgr\\tbusmgr.xml -X")
        os.system(".\\tools\\tbus_mgr\\tbusmgr.exe --conf-file=.\\tools\\tbus_mgr\\tbusmgr.xml -W")

###################### MAIN #######################

# cmdline
port = 0
idc_id = 0
opts, args = getopt.getopt(sys.argv[1:], "i:", ["idc_id="])
for opt, val in opts:
    if opt in("-i", "--idc_id"):
        idc_id = int(val)

zone_id = 0
is_linux = 'Linux' in platform.system()

if port == 0:
    port = input("please input port: ")

port = int(port) 
if (port <= 0 or port >= 10000):
    print("[ERROR] invalid port %s" % port)
    sys.exit(-1)

# generate port releated varibale
zone_id = 0
tconnd_port = port
tbus_key = 660000 + port
tbus_grm_key = 661000 + port
tbusd_port = port + 1000

if idc_id == 0:
    idc_id  = port % 100
if idc_id == 0:
    idc_id = 1

print("[PREPARE]: =========key info show==============")
print("bus key  :", tbus_key)
print("idc id   :", idc_id)
print("zone id  :", zone_id)
print("port     :", port)
print("\n")

#work dir
os.chdir('../../../')

#load config
load_local_deploy_config('./tools/deploy/config/local_deploy.yml')
load_proc_config('./tools/deploy/config/proc.yml')
load_proc_connect_config('./tools/deploy/config/proc.yml')

#clear
print("[PHASE 1]: =============clear===================")
clear()
print("[PHASE 1]: =============finished!===============")
print("\n")

#generate cfg file
print("[PHASE 2]: =============generate cfg============")
generate_cfg()
print("[PHASE 2]: =============finished!===============")
print("\n")

#generate procmgr
print("[PHASE 3]: =============generate procmgr========")
generate_procmgr()
print("[PHASE 3]: =============finished!===============")
print("\n")

#config expand
print("[PHASE 4]: =============start expand============")
start_expanding()
print("[PHASE 4]: =============finished!===============")
print("\n")

#refresh tbus
print("[PHASE 5]: =============refresh tbus============")
generate_tbus_mgr_cfg()
refresh_tbus_info()
print("[PHASE 5]: =============finished!===============")
