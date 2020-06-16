#!/usr/bin/python
# -*- coding: gb2312 -*-

import os
import sys
import ConfigParser
import yaml

class HostData:
    '''Name|InnerIP|OuterIP[]|...'''

#To parse host.xml 
class Host:
    def __init__(self, filename):
        self.host = dict()
        self.host_group = dict()

        if not os.path.exists(filename):
            print "[ERROR] Host '%s' not exist!" % (filename)
            return None

        print "[Begin] Parse Host Config"
        hostConfig = yaml.safe_load(open(filename, 'r'))

        print hostConfig

        for hostNode in hostConfig["hosts"]:
            if hostNode['name'] in self.host:
                print "[ERROR] Duplicate host ", hostNode['name']

            hostData = HostData()
            hostData.Name = hostNode['name']
            hostData.InnerIP = hostNode['ip']
            # current not contain outerIP
            self.host[hostNode['name']] = hostData

        for k,v in hostConfig["host_group"].items():
            self.host_group[k] = v
        print "[End] Parse Host Config"

    def getHostList(self):
        return self.host.keys()

    def isHostExist(self, hostName):
        return (hostName in self.host)

    def getIPByHost(self, hostName):
        return self.host[hostName].InnerIP
    
    def getHostGroup(self, groupName):
        return self.host_group[groupName]

##################################
# data hierarchy
#
# + DeployConfig
#   + ClusterDeployConfig
#   + IDC
#       + IDCDeployConfig
#       + ZoneDeployConfig
####################################
class IDCDeployConfig:
    def __init__(self):
        self.IDCDeployConfig = dict()
        self.ZoneDeployConfig = dict()  # key is zoneID

#Can be describe to third level map
class DeployConfig:
    def __init__(self):
        self.ClusterDeployConfig = dict()
        self.IDC = dict()  # key is idcID

    def addClusterDeployConfig(self, key, dict):
        self.ClusterDeployConfig[key] = dict 

    def addIDCDeployConfig(self, idcID, keyValueDict):
        if idcID not in self.IDC:
            self.IDC[idcID] = IDCDeployConfig()
        self.IDC[idcID].IDCDeployConfig = keyValueDict

    def addZoneDeployConfig(self, idcID, zoneID, keyValueDict):
        if idcID not in self.IDC:
            self.IDC[idcID] = IDCDeployConfig()
        self.IDC[idcID].ZoneDeployConfig[zoneID] = keyValueDict

    def getClusterDeployConfig(self):
        return self.ClusterDeployConfig

    def getIDCDeployConfig(self, idcID):
        return self.IDC[idcID].IDCDeployConfig

    def getZoneDeployConfig(self, idcID, zoneID):
        return self.IDC[idcID].ZoneDeployConfig[zoneID]

    def getIDCList(self):
        return sorted(self.IDC.keys())

    def getZoneList(self, idcID):
        return sorted(self.IDC[idcID].ZoneDeployConfig.keys())

    def getAttrValue(self, idcID, zoneID, attr_name):
        if attr_name in self.IDC[idcID].ZoneDeployConfig[zoneID]:
            return self.IDC[idcID].ZoneDeployConfig[zoneID][attr_name];
        if attr_name in self.IDC[idcID].IDCDeployConfig:
            return self.IDC[idcID].IDCDeployConfig[attr_name]
        print "[ERROR] attr '%s' not found" % (attr_name)
        sys.exit(-1)

def loadProcConfig(filename):
    if not os.path.exists(filename):
        print "[ERROR] proc config '%s' not exist!" % (filename)
        sys.exit(-1)

    print "[Begin] Parse Proc Config"
    loadConfig = yaml.safe_load(open(filename, 'r'))

    print loadConfig

    if not loadConfig.has_key('procs'):
        print "[ERROR] procs section is not exist"
        sys.exit(-1)
    procConfig = loadConfig['procs'];

    if not procConfig.has_key('proc_list'):
        print "[ERROR] proc config do not has proc_list section"
    
    for proc,currConfig in procConfig['proc_list'].items():
        if not currConfig.has_key('layer'):
            print "[ERROR] proc config do not has layer section"
        if not currConfig.has_key('func_id'):
            print "[ERROR] proc config do not has func_id section"
        if not currConfig.has_key('deploy_unit'):
            print "[ERROR] proc config do not has deploy_unit section"
    
    if not loadConfig.has_key('proc_connect'):
        print "[ERROR] proc connect section is not exist"
        sys.exit(-1)
    procConnectConfig = loadConfig['proc_connect'];
    
    print "[End] Parse Proc Config"

    return procConfig, procConnectConfig

def loadDeployConfig(filename):
    deploy = DeployConfig()
    if not os.path.exists(filename):
        print "[ERROR] deploy config '%s' not exist!" % (filename)
        sys.exit(-1)
    
    print "[Begin] Parse Deploy Config"

    loadConfig = yaml.safe_load(open(filename, 'r'))
    if not loadConfig.has_key('cluster'):
        print "[ERROR] cluster is not exist!" 
        sys.exit(-1)

    print loadConfig

    if loadConfig['cluster'].has_key('attrs'):
        deploy.addClusterDeployConfig('attrs', loadConfig['cluster']['attrs'])
    
    if loadConfig['cluster'].has_key('deploys'):
        deploy.addClusterDeployConfig('deploys', loadConfig['cluster']['deploys'])

    if loadConfig['cluster'].has_key('worlds'):
        for world in loadConfig['cluster']['worlds']:
            assert(world.has_key('id'))
            worldID = int(world['id'])
            deploy.addIDCDeployConfig(worldID, world)

            if world.has_key('zones'):
                for zone in world['zones']:
                    assert(zone.has_key('id'))
                    zoneID = int(zone['id'])
                    deploy.addZoneDeployConfig(worldID, zoneID, zone)
    
    print "[End] Parse Deploy Config"

    return deploy

if __name__ == '__main__':
    # unit test
    host_file = "../config/host.yml"
    hostConfig = Host(host_file)

    host = hostConfig.getHostConfig()
    for name in host:
        print "%s\n\t%s\n" % (name, host[name].ip)

    procConfig = loadDeployConfig("../config/proc.yml")
    deployConfig = loadDeployConfig("../config/tcm_deploy.yml")
