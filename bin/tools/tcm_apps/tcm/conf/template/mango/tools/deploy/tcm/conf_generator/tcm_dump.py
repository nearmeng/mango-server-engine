#!/usr/bin/python
# -*- coding:gb2312 -*-

def dumpSpace(file, indent):
    file.write(' ' * 4 * indent)

def dumpOneAttribute(file, attrDict):
    for key, value in attrDict.items():
        file.write('%s="%s" ' % (key, value))

def dumpProcInfo(file, indent, procs):
    for key, value in procs.items():
        dumpSpace(file, indent)
        file.write("<Proc ")
        dumpOneAttribute(file, value)
        file.write("/>\n")

def dumpProcConfig(filename, procConfig, procGroupConfig):
    file = open(filename, "w")
    file.write("<tcmcenter>\n")

    file.write("<cluster ")
    dumpOneAttribute(file, procConfig.ClusterAttrs)
    file.write(">\n")

    dumpProcInfo(file, 1, procConfig.ClusterProc)

    dumpSpace(file, 1)
    file.write("<world ")
    dumpOneAttribute(file, procConfig.IDCAttrs)
    file.write(">\n")

    dumpProcInfo(file, 2, procConfig.IDCProc)

    dumpSpace(file, 2)
    file.write("<zone ")
    dumpOneAttribute(file, procConfig.ZoneAttrs)
    file.write(">\n")

    dumpProcInfo(file, 3, procConfig.ZoneProc)

    dumpSpace(file, 2)
    file.write("</zone>\n")
    dumpSpace(file, 1)
    file.write("</world>\n")
    file.write("</cluster>\n\n")

    # dump proc group
    for (key,sets) in procGroupConfig.items():
        if key.startswith("world_"):
            file.write('<ProcGroup Name="%s" Layer="world">\n' % (key[6:]))
        elif key.startswith("zone_"):
            file.write('<ProcGroup Name="%s" Layer="zone">\n' % (key[5:]))
        else:
            print "[ERROR] invalid group_name %s" % (key)
            sys.exit(-1)

        for proc in sets:
            dumpSpace(file, 1)
            file.write('<Proc FuncName="%s"/>\n' % (proc))
        file.write("</ProcGroup>\n")
    file.write("\n")

    file.write("</tcmcenter>\n")

def dumpCustomAttr(file, indent, customAttr):
    if len(customAttr) < 1:
        return
    file.write(' CustomAttr="')
    for key, value in customAttr.items():
        file.write("\n")
        #dumpSpace(file, indent)
        file.write('%s=%s;' % (key, str(value).replace('"', '&quot;')))
    file.write('"')

def dumpDeployGroups(file, indent, deployGroups):
    for dg in deployGroups:
        dumpSpace(file, indent)
        file.write('<DeloyGroup Group="%s" Host="%s" InstID="%d" ' % (dg.Group, dg.Host, dg.InstID))
        dumpCustomAttr(file, indent+3, dg.CustomAttr)
        file.write("/>\n")

def dumpHostConfig(filename, hostConfig):
    file = open(filename, "w")
    file.write("<tcmcenter>\n")

    dumpSpace(file, 1)
    file.write("<HostTab>\n")

    for host in hostConfig:
        dumpSpace(file, 2)
        file.write('<Host ') 
        dumpOneAttribute(file, host)
        file.write('> </Host>\n') 

    dumpSpace(file, 1)
    file.write("</HostTab>\n")

    file.write("</tcmcenter>\n")

def dumpProcConnConfig(filename, procConnConfig):
    file = open(filename, "w")
    file.write("<tcmcenter>\n")

    dumpSpace(file, 1)
    file.write("<TbusConf ")
    dumpOneAttribute(file, procConnConfig['attrs'])
    file.write(" >\n")

    for channel in procConnConfig['channels']:
        dumpSpace(file, 2)
        file.write('<Channel ') 
        dumpOneAttribute(file, channel)
        file.write(' />\n') 

    dumpSpace(file, 1)
    file.write("</TbusConf>\n")

    file.write("</tcmcenter>\n")

def dumpDeployConfig(filename, deployResult):
    file = open(filename, "w")

    file.write("<tcmcenter>\n")
    dumpSpace(file, 1)
    file.write("<ClusterDeploy ")
    dumpCustomAttr(file, 4, deployResult.CustomAttr)
    file.write(">\n")

    dumpDeployGroups(file, 2, deployResult.ClusterDeployGroups)

    for idcID in deployResult.getIDCList():
        dumpSpace(file, 2)
        file.write('<world ID="%s" ' % (idcID))
        dumpCustomAttr(file, 5, deployResult.IDCDeployResult[idcID].CustomAttr)
        file.write('>\n')

        dumpDeployGroups(file, 3, deployResult.IDCDeployResult[idcID].IDCDeployGroups)

        for zoneID in deployResult.getZoneList(idcID):
            dumpSpace(file, 3)
            file.write('<zone ID="%s" ' % (zoneID))
            dumpCustomAttr(file, 6, deployResult.IDCDeployResult[idcID].ZoneDeployResult[zoneID].CustomAttr)
            file.write('>\n')

            dumpDeployGroups(file, 4, deployResult.IDCDeployResult[idcID].ZoneDeployResult[zoneID].ZoneDeployGroups)

            dumpSpace(file, 3)
            file.write('</zone>\n')

        dumpSpace(file, 2)
        file.write('</world>\n')

    dumpSpace(file, 1)
    file.write("</ClusterDeploy>\n")
    file.write('</tcmcenter>\n')

