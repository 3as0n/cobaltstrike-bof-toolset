### 简介
收集网络中在cobaltstrike中使用的bof工具集。项目基于CS-Situational-Awareness-BOF推荐的创建方式和流程。代码均来之网络。

### 直接使用
```
git clone https://github.com/AttackTeamFamily/cobaltstrike-bof-toolset.git
使用CobaltStrike加载cobaltstrike-bof-toolset/bof/bof.cna脚本
```

### 自己编译
```
编译环境：kali linux
命令：
sudo apt install mingw-x64
git clone https://github.com/AttackTeamFamily/cobaltstrike-bof-toolset.git
cd cobaltstrike-bof-toolset
cd src/bof
for i in `ls`;do cd $i && make && cd ..;done
```

### 功能
|command|Usage|notes|
|-------|-----|-----|
|arp|arp| Lists ARP table|
|adcs_enum | adcs_enum | Enumerates CAs and templates in the AD using Win32 functions |
|adcs_enum_com | adcs_enum_com | Enumerates CAs and templates in the AD using ICertConfig COM object |
|adcs_enum_com2 | adcs_enum_com2 | Enumerates CAs and templates in the AD using IX509PolicyServerListManager COM object |
|adv_audit_policies | adv_audit_policies | Retrieves advanced security audit policies |
|cacls|cacls [filepath]|lists user permissions for the specified file, wildcards supported|
|dir|dir [directory] [/s]|List files in a directory. Supports wildcards (e.g. "C:\Windows\S*") the CobaltStrike `ls` command|
|driversigs|driversigs| enumerate installed services Imagepaths to check the signing cert against known edr/av vendors|
|enum_filter_driver|enum_filter_driver [opt:computer] | Enumerates all the filter drivers|
|enumLocalSessions|enumLocalSessions| Enumerate the currently attached user sessions both local and over rdp|
|env|env| Prints process environment variables|
|findLoadedModule|findLoadedModule [modulepart] [opt:procnamepart]| Finds what processes \*modulepart\* is loaded into, optionally searching just \*procnamepart\*|
|ipconfig|ipconfig| Simply gets ipv4 addresses, hostname and dns server|
|ldapsearch|ldapsearch [query] [opt: attribute] [opt: results_limit] | Executes LDAP searches |
|listdns|listdns| Pulls dns cache entries, attempts to query and resolve each|
|listmods|listmods [opt: pid]| List a process modules (DLL). Target current process if pid is empty. Complement to driversigs to determine if our process was injected by edr/av.|
|listpipes|listpipes| Lists named pipes|
|netstat|netstat| tcp / udp ipv4 netstat listing|
|netuser|netuser [username] [opt: domain]| Pulls info about specific user.  Pulls from domain if a domainname is specified|
|netview|netview| Gets a list of reachable servers in the current domain|
|netGroupList|netGroupList [opt: domain]|Lists Groups from the default (or specified) domain|
|netGroupListMembers|netGroupListMembers [groupname] [opt: domain]| Lists group members from the default (or specified) domain|
|netLocalGroupList|netLocalGroupList [opt: server]|List local groups from the local (or specified) computer|
|netLocalGroupListMembers|netLocalGroupListMembers [groupname] [opt: server]| Lists local groups from the local (or specified) computer|
|nslookup|nslookup [hostname] [opt:dns server] [opt: record type]| Makes a dns query.<br/>  dns server is the server you want to query (do not specify or 0 for default) <br/>record type is something like A, AAAA, or ANY.  Some situations are limited due to observed crashes.|
|reg_query|[opt:hostname] [hive] [path] [opt: value to query]|queries a registry value or enumerates a single key|
|reg_query_recursive|[opt:hostname] [hive] [path]| recursively enumerates a key starting at path|
|routeprint|routeprint| prints ipv4 configured routes|
|schtasksenum|schtasksenum [opt: server]| Enumerates all scheduled tasks on the local or if provided remote machine|
|schtasksquery|schtasksquery [opt: server] [taskpath]| Queries the given task from the local or if provided remote machine|
|sc_enum| sc_enum [opt:server] | Enumerates all services for qc, query, qfailure, and qtriggers info |
|sc_qc|sc_qc [service name] [opt:server]| sc qc impelmentation in bof|
|sc_qfailure|sc_qfailure [service name] [opt:server] | Queries a service for failure conditions |
|sc_qtriggerinfo|sc_qtriggerinfo [service name] [opt:server] | Queries a service for trigger conditions |
|sc_query|sc_query [opt: service name] [opt: server]| sc query implementation in bof|
|sc_qdescription|sc_qdescription [service name] [opt: server] | sc qdescription implementation in bof|
|tasklist|tasklist [opt: server]| Get a list of running processes including PID, PPID and ComandLine (uses wmi)|
|whoami|whoami| simulates whoami /all|
|windowlist|windowlist [opt:all]| lists visible windows in the current users session|
|wmi_query|wmi_query query [opt: server] [opt: namespace]| Run a wmi query and display results in CSV format|
|netsession|netsession [opt:computer] | Enumerates all sessions on the specified computer or the local one|
|resources|resources| Prints memory usage and available disk space on the primary hard drive|
|uptime|uptime| Prints system boot time and how long it's been since then|
|vssenum|vssenum [hostname] [opt:sharename]| Enumerates shadow copies on some server 2012+ machines|


### 来源
https://github.com/ajpc500/BOFs
https://github.com/rvrsh3ll/BOF_Collection
https://github.com/trustedsec/CS-Situational-Awareness-BOF

### 免责声明
本项目代码仅能在取得足够合法授权的企业安全建设中使用，在使用本项目过程中，您应确保自己所有行为符合当地的法律法规。 如您在使用本项目的过程中存在任何非法行为，您将自行承担所有后果，本项目所有开发者和所有贡献者不承担任何法律及连带责任。 除非您已充分阅读、完全理解并接受本协议所有条款，否则，请您不要安装并使用本项目。 您的使用行为或者您以其他任何明示或者默示方式表示接受本协议的，即视为您已阅读并同意本协议的约束。
