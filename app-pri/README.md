# app-pri  
UI参考：[ 应用模版 ](https://github.com/xtuer/template-app/tree/master/template-qt "xtuer/template-app")  
QT的一套UI模板以及一些工具合集  



UI reference:[ 应用模版 ](https://github.com/xtuer/template-app/tree/master/template-qt "xtuer/template-app")  
A set of QT UI templates and a collection of tools  



1. 先编译构建；  

2. 把[源码bin文件夹](AppTools/bin)复制到编译出来的可执行文件所在目录(`源码路径bin-64(32)/debug(release)/`)；  

3. 将`bin/sql`目录下的[AppTools.sql](bin/sql/AppTools.sql)导入MySql数据库；

1. Compile and build first;  
2. Copy the [source bin folder](AppTools/bin) to the directory where the compiled executable file is located (`source path bin-64(32)/debug(release)/`);  
3. Import [AppTools.sql](bin / sql / AppTools.sql) under the `bin/sql` directory into the MySql database;  



> 支持MYSQL和SQLITE数据库  
> Support MYSQL and SQLITE database  



程序内部配置，可根据实际情况修改[config.json](bin/cfg/config.json)     

The internal configuration of the program can be modified according to the actual situation [config.json](bin / cfg / config.json)    



然后运行，先显示  

Then run, display first  



## 登录界面 login interface ##   

<div align=center><img src="pic/Login.png"></div>  
用户名和密码均为`admin`  
Username and password are both `admin`  

登陆成功显示主界面`你好呀！`  
Successful login shows the main interface `Hello! `  



## 主界面 Main interface ##
<div align=center><img src="pic/AppTools_2.png"></div>  