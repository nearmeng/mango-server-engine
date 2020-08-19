CREATE TABLE IF NOT EXISTS `Host` (
 `Name` VARCHAR(64) NOT NULL  ,
 `InnerIP` INT UNSIGNED   ,
 `OuterIPCount` INT   DEFAULT '1',
 `OuterIP_1` INT UNSIGNED  ,
 `OuterIP_2` INT UNSIGNED  ,
 `OuterIP_3` INT UNSIGNED  ,
 `CustomAttr` TEXT   DEFAULT '',
 `IsVirtual` INT   DEFAULT '0',
PRIMARY KEY(`Name`))
 ENGINE=InnoDB DEFAULT CHARSET=latin1;

 CREATE TABLE IF NOT EXISTS `ProcLayerDescNode` (
 `LayerName` VARCHAR(64) NOT NULL  ,
 `WorkPath` TEXT   DEFAULT '',
 `AutoTimeGap` INT   DEFAULT '60',
 `MsgRoundTime` INT   DEFAULT '6',
 `OpTimeout` INT   DEFAULT '30',
 `Isolated` INT   DEFAULT '0',
 `AutoResumeNum` INT   DEFAULT '-1',
 `LayerPos` INT   ,
 `FirstProcIdx` INT   DEFAULT '-1',
 `FirstProcGroupIdx` INT   DEFAULT '-1',
PRIMARY KEY (`LayerName`))
 ENGINE=InnoDB DEFAULT CHARSET=latin1;


 CREATE TABLE IF NOT EXISTS `ProcUnit` (
 `LayerName` VARCHAR(64) NOT NULL  ,
 `FuncName` VARCHAR(32) NOT NULL  ,
 `FuncID` INT NOT NULL  ,
 `ProcName` VARCHAR(64)   DEFAULT '',
 `WorkPath` TEXT   DEFAULT '',
 `Flag` VARCHAR(128)   DEFAULT 'start|stop|check',
 `FlagVal` INT   ,
 `OpTimeout` INT   DEFAULT '0',
 `AutoTimeGap` INT   DEFAULT '0',
 `Agrs` TEXT   DEFAULT '',
 `Args` TEXT   DEFAULT '',
 `StartCmd` TEXT   DEFAULT '',
 `ReStartCmd` TEXT   DEFAULT '',
 `StopCmd` TEXT   DEFAULT '',
 `KillCmd` TEXT   DEFAULT '',
 `RunShellCmd` TEXT   DEFAULT '',
 `AutoScript` TEXT   DEFAULT '',
 `PreAutoScript` TEXT   DEFAULT '',
 `IsCommon` INT   DEFAULT '0',
 `Seq` INT   ,
 `PidFile` TEXT   DEFAULT '',
 `ConfigPath` TEXT   DEFAULT '',
 `ReloadCmd` TEXT   DEFAULT '',
 `StartCheckBeginTime` INT   DEFAULT '3',
 `StartCheckEndTime` INT   DEFAULT '10',
 `StartCheckCmd` TEXT   DEFAULT '',
 `TAppCtrlTdrFile` TEXT   DEFAULT '',
 `AutoResumeNum` INT   DEFAULT '-1',
 `LayerIdx` INT   ,
 `NextInLayer` INT   ,
 `FirstProcInst` INT   ,
PRIMARY KEY(`FuncName`))
 ENGINE=InnoDB DEFAULT CHARSET=latin1;


 CREATE TABLE IF NOT EXISTS `ProcUnitGroupEx` (
 `GroupName` VARCHAR(64) NOT NULL  ,
 `LayerName` VARCHAR(64) NOT NULL  ,
PRIMARY KEY(`GroupName`))
 ENGINE=InnoDB DEFAULT CHARSET=latin1;

 CREATE TABLE IF NOT EXISTS `ProcUnitInGroup` (
 `GroupName` VARCHAR(64) NOT NULL  ,
 `FuncName` VARCHAR(32)   NOT NULL,
 `ProcIdx` INT   ,
 `InstID` INT   DEFAULT '1',
 `MaxInstID` INT   DEFAULT '1',
 `WorkPath` TEXT   DEFAULT '',
 `Agrs` TEXT   DEFAULT '',
 `Args` TEXT   DEFAULT '',
 `StartCmd` TEXT   DEFAULT '',
 `ReStartCmd` TEXT   DEFAULT '',
 `StopCmd` TEXT   DEFAULT '',
 `KillCmd` TEXT   DEFAULT '',
 `RunShellCmd` TEXT   DEFAULT '',
 `PidFile` TEXT   DEFAULT '',
 `ConfigPath` TEXT   DEFAULT '',
 `ReloadCmd` TEXT   DEFAULT '',
 `TAppCtrlTdrFile` TEXT   DEFAULT '',
PRIMARY KEY(`GroupName`, `FuncName`, `InstID`))
 ENGINE=InnoDB DEFAULT CHARSET=latin1;

 CREATE TABLE IF NOT EXISTS `DeloyGroup` (
 `DeployId` VARCHAR(64) NOT NULL  ,
 `GroupIdx` INT   ,
 `HostIdx` INT   ,
 `WorkPath` TEXT   DEFAULT '',
 `Group` VARCHAR(64)   NOT NULL,
 `Host` VARCHAR(64)   NOT NULL,
 `InstID` INT   DEFAULT '1',
 `CustomAttr` TEXT   DEFAULT '',
PRIMARY KEY(`DeployId`, `Group`, `InstID`))
 ENGINE=InnoDB DEFAULT CHARSET=latin1;

 CREATE TABLE IF NOT EXISTS `DeployLayerNodeEx` (
 `ParentId` VARCHAR(64)  ,
 `DeployId` VARCHAR(64) NOT NULL  ,
 `Name` VARCHAR(64)   ,
 `ID` INT   DEFAULT '0',
 `CustomAttr` TEXT   DEFAULT '',
 `WorkPath` TEXT   DEFAULT '',
PRIMARY KEY(`DeployId`))
 ENGINE=InnoDB DEFAULT CHARSET=latin1;

 CREATE TABLE IF NOT EXISTS `ChannelDeployRule` (
 `ProcSet1` VARCHAR(64) NOT NULL  ,
 `ProcSet2` VARCHAR(64)  NOT NULL ,
 `Mask` TEXT   ,
 `RecvQueSize` INT   DEFAULT '0',
 `SendQueSize` INT   DEFAULT '0',
 `WanType` SMALLINT UNSIGNED   DEFAULT '256',
  `CtrlMask` TEXT ,
PRIMARY KEY(`ProcSet1`, `ProcSet2`))
 ENGINE=InnoDB DEFAULT CHARSET=latin1;

 CREATE TABLE IF NOT EXISTS `TbusLocalConf` (
 `Host` VARCHAR(64) NOT NULL  ,
 `GCIMShmKey` TEXT   DEFAULT '',
 `GRMShmKey` TEXT   DEFAULT '',
 `TbusdServicePort` SMALLINT UNSIGNED   DEFAULT '0',
 `TbusdConfPath` TEXT   DEFAULT '',
 `PkgMaxSize` INT   DEFAULT '0',
 `SendBuff` INT   DEFAULT '0',
 `RecvBuff` INT   DEFAULT '0',
 `NeedAckPkg` INT   DEFAULT '-1',
 `DynamicConfFile` TEXT   DEFAULT '',
 `WanBU` SMALLINT UNSIGNED   DEFAULT '256',
 `WanType` SMALLINT UNSIGNED   DEFAULT '256',
 `Domain` TEXT DEFAULT '',
 `CheckRelayUrl` SMALLINT UNSIGNED   DEFAULT '256',
PRIMARY KEY(`Host`))
 ENGINE=InnoDB DEFAULT CHARSET=latin1;

 CREATE TABLE IF NOT EXISTS `TbusGlobalConf` (
 `ChannelDataAlign` INT   DEFAULT '8',
 `GCIMShmKey` TEXT   DEFAULT '',
 `GRMShmKey` TEXT  DEFAULT '',
 `TbusdServicePort` SMALLINT UNSIGNED   DEFAULT '9000',
 `TbusdConfPath` TEXT   DEFAULT '',
 `PkgMaxSize` INT   DEFAULT '65536',
 `SendBuff` INT   DEFAULT '65536',
 `RecvBuff` INT   DEFAULT '65536',
 `NeedAckPkg` INT   DEFAULT '1',
 `ChannelSize` INT   DEFAULT '2048000',
 `WinTbusdConfPath` TEXT   DEFAULT '',
 `DynChannelMask` TEXT   DEFAULT '',
 `DynChnlMask`  INT DEFAULT '-1',
 `DynamicConfFile` TEXT   DEFAULT '',
 `WanBU` SMALLINT UNSIGNED   DEFAULT '256',
 `WanType` SMALLINT UNSIGNED   DEFAULT '256',
 `CheckRelayUrl` SMALLINT UNSIGNED   DEFAULT '256')
 ENGINE=InnoDB DEFAULT CHARSET=latin1;

 CREATE TABLE IF NOT EXISTS `AccessWhiteListEx` (
 `Host` VARCHAR(64) NOT NULL  ,
PRIMARY KEY(`Host`))
 ENGINE=InnoDB DEFAULT CHARSET=latin1;


 CREATE TABLE IF NOT EXISTS `BaseConfig` (
 `BussinessID` INT   DEFAULT '0',
 `AddrTemple` TEXT   DEFAULT '',
 `ConfigSoName` TEXT  DEFAULT '' ,
 `ConfigBaseDir` TEXT   DEFAULT '',
 `CollectConfigBaseDir` TEXT   DEFAULT '',
 `HostConfFile` TEXT   DEFAULT '',
 `ProcConfFile` TEXT   DEFAULT '',
 `ProcDeployFile` TEXT   DEFAULT '',
 `BusRelationFile` TEXT   DEFAULT '',
 `TbusConfOutputDir` TEXT   DEFAULT '',
 `ExportDeployOutputDir` TEXT   DEFAULT '',
 `CenterdAddr` TEXT   DEFAULT '',
 `TconndAddr` TEXT   DEFAULT '',
 `MaxAgentMsgPerloop` INT   DEFAULT '500',
 `MaxConsoleMsgPerloop` INT   DEFAULT '100',
 `RegisterToCenterdGap` INT   DEFAULT '30',
 `HostConfDir` TEXT   DEFAULT '',
 `TransTimeout` INT   DEFAULT '300',
 `AccessWhiteListFile` TEXT   DEFAULT '',
 `OpenCreateCfgByScript` INT   DEFAULT '0',
 `CreateCfgScriptFile` TEXT   DEFAULT '',
 `TcmDumpDir` TEXT   DEFAULT '',
 `BinBaseDir` TEXT   DEFAULT '',
 `WaterLogBaseDir` TEXT   DEFAULT '',
 `ToolsSrcBaseDir` TEXT   DEFAULT '',
 `ToolsDstBaseDir` TEXT   DEFAULT '',
 `CheckTcmConfGap` INT DEFAULT '1800',
 `TappPidFileDir` TEXT   DEFAULT ''
 )
 ENGINE=InnoDB DEFAULT CHARSET=latin1;


CREATE TABLE IF NOT EXISTS `ConsistencyFlag` (
 `Flag` int(11) default '0'
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
CREATE TABLE IF NOT EXISTS `ProcRunningStatus` (
 `ProcId` int(10) UNSIGNED ,
 `ProcStr` VARCHAR(64) ,
 `Status`   INT DEFAULT '0',
 PRIMARY KEY(`ProcId`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
