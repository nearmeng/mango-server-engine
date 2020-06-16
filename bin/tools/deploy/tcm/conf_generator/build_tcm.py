#!/usr/bin/python
# -*- coding:gb2312 -*-

import os,sys
import getopt
import string
import tcm_conf
import tcm_dump

class TLogdInfo:
    def __init__(self):
        self.TLogdReqMgr = dict()
    
    def addTLogdRequest(self, host, logdReq):
        if not self.TLogdReqMgr.has_key(host):
            self.TLogdReqMgr[host] = []
        self.TLogdReqMgr[host].append(logdReq)

    def getTLogdRequest(self, host):
        return self.TLogdReqMgr[host]

    def getTLogdHostList(self):
        return self.TLogdReqMgr.keys()
    
    def getTLogdReqCount(self, host):
        return len(self.TLogdReqMgr[host])

    def getRequestName(self, host, listIndex):
        return self.TLogdReqMgr[host][listIndex]['name']
    
    def getRequestPort(self, host, listIndex):
        return self.TLogdReqMgr[host][listIndex]['port']

class ProcInfo:
    def __init__(self):
        self.ClusterAttrs = dict()
        self.ClusterProc = dict()
        self.IDCAttrs = dict()
        self.IDCProc = dict()
        self.ZoneAttrs = dict()
        self.ZoneProc = dict()

    def addClusterProc(self, proc, keyValueDict):
        self.ClusterProc[proc] = keyValueDict

    def addIDCProc(self, proc, keyValueDict):
        self.IDCProc[proc] = keyValueDict

    def addZoneProc(self, proc, keyValueDict):
        self.ZoneProc[proc] = keyValueDict

def safe_dict_append(dstDict, sourceDict):
    for k, v in sourceDict.items():
        if type(v) == int or type(v) == str:
            dstDict[k] = v
        elif type(v) == list:
            for listIndex in range(0, len(v)):
                assert(type(v[listIndex]) == dict)
                for curKey, curValue in v[listIndex].items():
                    newKey = "%s[%d].%s" % (k, listIndex, curKey) 
                    newValue = curValue
                    dstDict[newKey] = newValue
        else:
            print "invalid dict append type %s" % type(v)
            sys.exit(-1)

class DeployGroup:
    def __init__(self, Group, Host, InstID=1):
        self.Group = Group
        self.Host = Host
        self.InstID = InstID
        self.CustomAttr = dict()

    def addCustomAttr(self, key, value):
        self.CustomAttr[key] = value
	
    def addCustomAttrByDict(self, Dict):
        safe_dict_append(self.CustomAttr, Dict)

class ZoneDeployResult:
    def __init__(self):
        self.CustomAttr = dict()
        self.ZoneDeployGroups = []

    def addCustomAttr(self, key, value):
        self.CustomAttr[key] = value

class IDCDeployResult:
    def __init__(self):
        self.CustomAttr = dict()
        self.IDCDeployGroups = []
        self.ZoneDeployResult = dict()

    def addCustomAttr(self, key, value):
        self.CustomAttr[key] = value
    
    def addCustomAttrByDict(self, Dict):
        safe_dict_append(self.CustomAttr, Dict)

    def getCustomAttr(self):
        return self.CustomAttr

class ClusterDeployResult:
    def __init__(self):
        self.CustomAttr = dict()
        self.ClusterDeployGroups = []  # DeployGroup
        self.IDCDeployResult = dict()

    def addCustomAttr(self, key, value):
        self.CustomAttr[key] = value
    
    def addCustomAttrByDict(self, Dict):
        safe_dict_append(self.CustomAttr, Dict)

    def getCustomAttr(self):
        return self.CustomAttr

    def addClusterDeploy(self, deployGroup):
        self.ClusterDeployGroups.extend(deployGroup)

    def addIDCDeploy(self, idcID, deployGroup):
        if idcID not in self.IDCDeployResult:
            self.IDCDeployResult[idcID] = IDCDeployResult()
        self.IDCDeployResult[idcID].IDCDeployGroups.extend(deployGroup)

    def addZoneDeploy(self, idcID, zoneID, deployGroup):
        if idcID not in self.IDCDeployResult:
            self.IDCDeployResult[idcID] = IDCDeployResult()
        if zoneID not in self.IDCDeployResult[idcID].ZoneDeployResult:
            self.IDCDeployResult[idcID].ZoneDeployResult[zoneID] = ZoneDeployResult()
        self.IDCDeployResult[idcID].ZoneDeployResult[zoneID].ZoneDeployGroups.extend(deployGroup)

    def addIDCCustomAttr(self, idcID, key, value):
        if idcID not in self.IDCDeployResult:
            self.IDCDeployResult[idcID] = IDCDeployResult()
        self.IDCDeployResult[idcID].addCustomAttr(key, value)
    
    def addIDCCustomAttrByDict(self, idcID, thisDict):
        if idcID not in self.IDCDeployResult:
            self.IDCDeployResult[idcID] = IDCDeployResult()
        self.IDCDeployResult[idcID].addCustomAttrByDict(thisDict)

    def addZoneCustomAttr(self, idcID, worldID, key, value):
        if idcID not in self.IDCDeployResult:
            self.IDCDeployResult[idcID] = IDCDeployResult()
        if zoneID not in self.IDCDeployResult[idcID].ZoneDeployResult:
            self.IDCDeployResult[idcID].ZoneDeployResult[worldID] = ZoneDeployResult()
        self.IDCDeployResult[idcID].ZoneDeployResult[zoneID].addCustomAttr(key, value)
    
    def addZoneCustomAttrByDict(self, idcID, worldID, Dict):
        if idcID not in self.IDCDeployResult:
            self.IDCDeployResult[idcID] = IDCDeployResult()
        if zoneID not in self.IDCDeployResult[idcID].ZoneDeployResult:
            self.IDCDeployResult[idcID].ZoneDeployResult[worldID] = ZoneDeployResult()
        self.IDCDeployResult[idcID].ZoneDeployResult[zoneID].addCustomAttrByDict(Dict)

    def getIDCList(self):
        return sorted(self.IDCDeployResult)

    def getZoneList(self, idcID):
        return sorted(self.IDCDeployResult[idcID].ZoneDeployResult)

def getProcArgs(funcName):
    conf_arg = "--use-bus "

    if procConfig['proc_list'][funcName].has_key('conf_file_prefix'):
        conf_arg += " --conf-file=./%s_$world.$zone.$function.$instance" % procConfig['proc_list'][funcName]['conf_file_prefix']
    else:
        conf_arg += " --noloadconf"

    if not funcName == "tlogd":
        if procConfig['proc_list'][funcName].has_key('tlog_file_prefix'):
            conf_arg += " --tlogconf=./%s_$world.$zone.$function.$instance"  % procConfig['proc_list'][FuncName]['tlog_file_prefix']
        else :
            conf_arg += " --tlogconf=./tlog.xml_$world_$zone.$function.$instance"

        conf_arg += " --rundata_timer=./rundata_timer.xml_$world_$zone.$function.$instance"
    return conf_arg

def buildProcAttr(funcName):
    #attrs = ["FuncName", "ProcName", "Seq", "FuncID", "Flag", "WorkPath", "Args", "AutoScript"]

    attrDict = dict()
    attrDict['FuncName'] = funcName

    attrDict['Seq'] = 1
    attrDict['FuncID'] = procConfig['proc_list'][funcName]['func_id']
    attrDict['WorkPath'] = funcName
    attrDict['ConfigPath'] = '.'
    attrDict['AutoScript'] = procConfig['tcm_auto_script']
    attrDict['StartCheckEndTime'] = 6
    attrDict['Flag'] = "start|check|stop|auto|resume|restart|reload|signal"
    attrDict['PidFile'] = '/tmp/%s_$world.$zone.$function.$instance.pid' % (funcName)
    attrDict['Agrs'] = getProcArgs(funcName)
    
    if procConfig['proc_list'][funcName].has_key('proc_name'):
        attrDict['ProcName'] = procConfig['proc_list'][funcName]['proc_name']

    if procConfig['proc_list'][funcName].has_key('work_path'):
        attrDict['WorkPath'] = procConfig['proc_list'][funcName]['work_path']
    
    if procConfig['proc_list'][funcName].has_key('config_path'):
        attrDict['ConfigPath'] = procConfig['proc_list'][funcName]['config_path']

    if procConfig['proc_list'][funcName].has_key('flag'):
        attrDict['Flag'] = procConfig['proc_list'][funcName]['flag']
        if attrDict['Flag'] == "notapp":
            del attrDict['Agrs'] 
            del attrDict['PidFile']

    return attrDict

def buildProcConfig():
    print "[Begin] construct tcm proc config"
    procInfo = ProcInfo()
    procInfo.ClusterAttrs['WorkPath'] = procConfig['tcm_work_path']
    procInfo.ClusterAttrs['AutoTimeGap'] = procConfig['tcm_auto_time_gap']
    procInfo.ClusterAttrs['OpTimeout'] = "1800"
    procInfo.ClusterAttrs['MsgRoundTime'] = "1800"

    procInfo.IDCAttrs['Isolated'] = 0
    procInfo.IDCAttrs['OpTimeout'] = 1800
    procInfo.IDCAttrs['MsgRoundTime'] = 6
    procInfo.IDCAttrs['AutoTimeGap'] = 10

    procInfo.ZoneAttrs['Isolated'] = 0
    procInfo.ZoneAttrs['OpTimeout'] = 1800
    procInfo.ZoneAttrs['AutoTimeGap'] = 10

    for procName, currConfig in procConfig['proc_list'].items():
        procAttrs = buildProcAttr(procName)
        procInfo.addIDCProc(procName, procAttrs)

    print "[End] construct tcm proc config"
    return procInfo

def addProcSet(groupInfo, setName, layer, procName):
    if layer == "world":
        setName = "world_" + setName
    elif layer == "zone":
        setName = "zone_" + setName

    if not groupInfo.has_key(setName):
        groupInfo[setName] = []
    groupInfo[setName].append(procName)

def buildProcSetConfig():
    groupInfo = {}
    for proc, config in procConfig['proc_list'].items():
        if config.has_key('deploy_unit'):
            addProcSet(groupInfo, config['deploy_unit'], config['layer'], proc)
    return groupInfo

def build_group_default_attr(dg, worldID, zoneID, instID, groupName, deployListConfig):
    #base attr
    dg.addCustomAttr("world", worldID)
    dg.addCustomAttr("zone", zoneID)
    dg.addCustomAttr("inst", instID)

    for proc,currConfig in procConfig['proc_list'].items():
        if currConfig['deploy_unit'] == groupName:
            #log_priority
            if currConfig.has_key('log_priority'):
                dg.addCustomAttr("%s_log_priority" % proc, currConfig['log_priority'])
            elif procConfig.has_key('log_priority'):
                dg.addCustomAttr("%s_log_priority" % proc, procConfig['log_priority'])
            else:
                dg.addCustomAttr("%s_log_priority" % proc, "INFO")
            #tbus
            dg.addCustomAttr("%s_tbus" % proc, "%s.%s.%s.%s" % (worldID, zoneID, currConfig['func_id'], instID))

            #self attr
            if currConfig.has_key('attrs'):
                dg.addCustomAttrByDict(currConfig['attrs'])
            #self inst attr
            if currConfig.has_key('inst_attrs'):
                for k,v in currConfig['inst_attrs'].items():
                    inst = instID
                    exec("value = %s" % v)
                    dg.addCustomAttr(k, value)
            
            #record tlogd req
            if currConfig.has_key('tlog_config'):
                for tlogCategory, categoryConfig in currConfig['tlog_config'].items():
                    for element in categoryConfig:
                        if element == 'tlogd':
                            tlogdReq = dict()
                            deployIP = ""
                            #check if deploy config has log host config
                            if deployListConfig.has_key('log'):
                                tlogdReq['name'] = "%s_%s.%s.%s.%s.%s" % (proc, tlogCategory, worldID, zoneID, currConfig['func_id'], instID)
                                tlogdReq['port'] = int(currConfig['func_id']) * 1000 + instID
                                tlogdInfo.addTLogdRequest(deployListConfig['log'], tlogdReq)

                                deployIP = hostConfig.getIPByHost(deployListConfig['log'])
                            else:
                                tlogdReq['name'] = "default_name"
                                tlogdReq['port'] = 1234

                                deployIP = "127.0.0.1"

                            #releated attr 
                            dg.addCustomAttr("%s_%s_tlogd_addr" % (proc, tlogCategory), "udp://%s:%s" % (deployIP, tlogdReq['port']))

def build_group_deploy(worldID, zoneID, groupName, groupConfig):
    result = []

    for deployList in groupConfig:
        if deployList.has_key('id_begin') and deployList.has_key('id_end'):
            countPerHost = 1   #default
            if deployList.has_key('count_per_host'):
                countPerHost = deployList['count_per_host']
            
            if deployList.has_key('host_group'):
                if deployList['host_group'] == 'ALL':
                    hostGroup = hostConfig.getHostList()
                else:
                    hostGroup = hostConfig.getHostGroup(deployList['host_group'])
            elif deployList.has_key('host'):
                if deployList['host'] == 'ALL':
                    hostGroup = hostConfig.getHostList()
                else:
                    assert(hostConfig.isHostExist(deployList['host']))
                    hostGroup = [ deployList['host'] ]

            assert(hostGroup)
            assert(countPerHost * len(hostGroup) >= int(deployList['id_end']) - int(deployList['id_begin']) + 1)

            for inst in range(int(deployList['id_begin']), int(deployList['id_end']) + 1):
                index = (inst - int(deployList['id_begin'])) / countPerHost
                dg = DeployGroup(groupName, hostGroup[index], inst)

                build_group_default_attr(dg, worldID, zoneID, inst, groupName, deployList)
                result.append(dg)
        elif deployList.has_key('id'):
            if not deployList.has_key('host'):
                print "[ERROR] deploy section do not has field 'host', while has field 'id'"
                sys.exit(-1)
            dg = DeployGroup(groupName, deployList['host'], deployList['id'])
            build_group_default_attr(dg, worldID, zoneID, deployList['id'], groupName, deployList)
            result.append(dg)
        else:
            print "[ERROR] deploy list section is invalid, %s" % deployList
   
    return result

def buildTLogdDeploy(worldID, worldDeployConfig):
    result = []

    totalCount = 0
    tlogdInstID = 0   #global tlogd inst
    ProcessNumPerTLogd = 10

    for host in tlogdInfo.getTLogdHostList():
        listCount = 0
        lastTLogdProcessNum = 0
        totalCount = tlogdInfo.getTLogdReqCount(host)

        if totalCount == 0:
            continue
	    
        #calc tlogd count per host
        if totalCount % ProcessNumPerTLogd == 0: 
            instCount = totalCount / ProcessNumPerTLogd
            lastTLogdProcessNum = ProcessNumPerTLogd
        else:
            instCount = (totalCount / ProcessNumPerTLogd + 1)
            lastTLogdProcessNum = totalCount % ProcessNumPerTLogd
        
        #deploy
        for instID in range(0, instCount):
            dg = DeployGroup(procConfig['proc_list']['tlogd']['deploy_unit'], host, tlogdInstID + 1)
            assert(worldDeployConfig['attrs'].has_key('tlogd_home'))
            dg.addCustomAttr("tlogd_home", worldDeployConfig['attrs']['tlogd_home'])

            #check is last tlogd
            if (instID + 1) == instCount:
                processCount = lastTLogdProcessNum
            else:
                processCount = processNumPerTLogd
                
            #select served process for this tlogd
            for processIndex in range(0, processCount):
                Name = tlogdInfo.getRequestName(host, listCount)
                IP = hostConfig.getIPByHost(host)
                Port = tlogdInfo.getRequestPort(host, listCount)
                print "[tlogd deploy info] name %s ip %s port %d inst %d" % (Name, IP, Port, tlogdInstID + 1)

                dg.addCustomAttr("tlogd_listener[%d].name" % processIndex, Name)
                dg.addCustomAttr("tlogd_listener[%d].ip" % processIndex, IP)
                dg.addCustomAttr("tlogd_listener[%d].port" % processIndex, Port)
                dg.addCustomAttr("tlogd_serializer[%d].name" % processIndex, Name)
                dg.addCustomAttr("tlogd_serializer[%d].log_pattern" % processIndex, "%s_%%H" % Name)
                dg.addCustomAttr("tlogd_net_trans[%d].name" % processIndex, Name)
                dg.addCustomAttr("tlogd_net_trans[%d].listener" % processIndex, Name)
                dg.addCustomAttr("tlogd_net_trans[%d].serializer" % processIndex, Name)
                listCount = listCount + 1

            dg.addCustomAttr("tlogd_tbus", "%s.%d.%s.%d" % (worldID, 0, procConfig['proc_list']['tlogd']['func_id'], tlogdInstID + 1))
            tlogdInstID = tlogdInstID + 1
            result.append(dg)

    return result

def checkDeployGroup(groupName, layer):
    find = False
    for proc, config in procConfig['proc_list'].items():
        if config['deploy_unit'] == groupName and config['layer'] == layer:
            find = True
    if find == False:
        print "[ERROR]: can not find group: %s of layer: %s" % (groupName, layer)
    assert find

def buildDeployConfig():
    print "[Begin] construct deploy config"

    base_key = 7001
    global gcim_key
    global grm_key
    global tbusd_port
    global tlogdInfo

    deployResult = ClusterDeployResult()
    tlogdInfo = TLogdInfo()

    clusterDeployConfig = deployConfig.getClusterDeployConfig()
    if clusterDeployConfig.has_key('attrs'):
        base_key = clusterDeployConfig['attrs']['base_key']
    
    #generate port releated
    gcim_key		  = 660000 + base_key
    grm_key			  = 690000 + base_key
    tbusd_port        = base_key + 300

    #cluster add generated attr
    deployResult.addCustomAttr('tbus_gcim_key', gcim_key)
    deployResult.addCustomAttr('tbus_grm_key', grm_key)
    deployResult.addCustomAttr('tbusd_port', tbusd_port)

    #cluster add attr if attr field defined
    if clusterDeployConfig.has_key('attrs'):
        deployResult.addCustomAttrByDict(clusterDeployConfig['attrs'])
    
    #cluster add deploys
    if clusterDeployConfig.has_key('deploys'):
        for k,v in clusterDeployConfig['deploys'].items():
            checkDeployGroup(k, "cluster")
            deployResult.addClusterDeploy(build_group_deploy(0, 0, k, v))

    for worldID in deployConfig.getIDCList():
	    #world attr
        deployResult.addIDCCustomAttrByDict(worldID, deployResult.getCustomAttr())

        worldDeployConfig = deployConfig.getIDCDeployConfig(worldID)
        if worldDeployConfig.has_key('attrs'):
            deployResult.addIDCCustomAttrByDict(worldID, worldDeployConfig['attrs'])

        #world deploy
        if worldDeployConfig.has_key('deploys'):
            for k,v in worldDeployConfig['deploys'].items():
                checkDeployGroup(k, "world")
                deployResult.addIDCDeploy(worldID, build_group_deploy(worldID, 0, k, v))

        #zones
        for zoneID in deployConfig.getZoneList(worldID):
            #zone attr
            deployResult.addZoneCustomAttrByDict(worldID, zoneID, worldDeployConfig.getCustomAttr())

            zoneDeployConfig = deployConfig.getZoneDeployConfig()
            deployResult.addZoneCustomAttr(worldID, zoneID, zoneDeployConfig['attrs'])

            #zone deploy
            if zoneDeployConfig.has_key('deploys'):
                for k,v in zoneDeployConfig['deploys'].items():
                    checkDeployGroup(k, "zone")
                    deployResult.addZoneDeploy(build_group_deploy(worldID, zoneID, k, v))
        #tlogd for this world
        buildTLogdDeploy(worldID, worldDeployConfig)
        
    print "[End] construct deploy config"
    return deployResult

def buildHostConfig():
    hostResult = []

    print "[Begin] construct Host config"
    for hostName in hostConfig.getHostList():
        host = dict()
        host['Name'] = hostName
        host['InnerIP'] = hostConfig.getIPByHost(hostName)

        hostResult.append(host)
    
    print "[End] construct Host config"
    return hostResult

def buildProcConnConfig():
    procConnResult = dict()
    
    print "[Begin] construct Proc Conn config"

    #conn attr
    procConnAttr = dict()
    procConnAttr['BussinessID']  = 0
    procConnAttr['GCIMShmKey'] = gcim_key
    procConnAttr['GRMShmKey'] = grm_key
    procConnAttr['TbusdServicePort'] = tbusd_port
    procConnAttr['TbusdConfPath'] = "%s/tbusd" % procConfig['tcm_work_path']
    procConnAttr['ChannelSize'] = 10240000
    procConnAttr['SendBuff'] = 10240000
    procConnAttr['RecvBuff'] = 10240000
    procConnResult['attrs'] = procConnAttr

    procConnResult['channels'] = []
    for conn in procConnConfig:
        channel = dict()
        channel['ProcSet1'] = conn['peer1']
        channel['ProcSet2'] = conn['peer2']
        channel['Mask'] = conn['Mask']
        if conn.has_key('MaskCtrl'):
            channel['MaskCtrl'] = conn['MaskCtrl']
        if conn.has_key('recv_queue_size'):
            channel['RecvQueSize'] = conn['recv_queue_size']
        else:
            channel['RecvQueSize'] = 10240000

        if conn.has_key('send_queue_size'):
            channel['SendQueSize'] = conn['send_queue_size']
        else:
            channel['SendQueSize'] = 10240000
        
        procConnResult['channels'].append(channel)

    print "[End] construct Proc Conn config"
    return procConnResult

#=================== main ======================
hostConfig = tcm_conf.Host("../../config/hosts.yml")
assert(hostConfig)
procConfig, procConnConfig = tcm_conf.loadProcConfig("../../config/proc.yml")
assert(procConfig)
assert(procConnConfig)
deployConfig = tcm_conf.loadDeployConfig("../../config/tcm_deploy.yml")
assert(deployConfig)

tcmProcConfig = buildProcConfig()
tcmProcGroupConfig = buildProcSetConfig()
tcm_dump.dumpProcConfig("../conf_output/proc.xml", tcmProcConfig, tcmProcGroupConfig)

deployResult = buildDeployConfig()
tcm_dump.dumpDeployConfig("../conf_output/proc_deploy.xml", deployResult)

tcmHostConfig = buildHostConfig()
tcm_dump.dumpHostConfig("../conf_output/host.xml", tcmHostConfig)

tcmProcConnConfig = buildProcConnConfig()
tcm_dump.dumpProcConnConfig("../conf_output/bus_relation.xml", tcmProcConnConfig)