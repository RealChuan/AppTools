# app-subdir  
基于QtCreator插件系统的一套UI模板以及一些工具合集。  
如有侵权，请联系删除，谢谢！  



A set of UI templates and a collection of tools based on the QtCreator plug-in system.  
If there is any infringement, please contact to delete, thank you!        




1. 先编译构建；  
2. 把[源码bin文件夹](AppTools/bin)复制到编译出来的可执行文件所在目录(`源码路径bin-64(32)/debug(release)/`)；   
3. 将`bin/sql`目录下的[apptools.sql](bin/sql/apptools.sql)导入MySql数据库，或者把整个sql文件夹放入可执行程序目录，使用SQLite数据库；    
4. 用Qtcreator工具-外部-Qt语言家发布翻译，然后将translations复制到可执行文件路径下，同2，*.ts文件可不用。




1. Compile and build first;  
2. Copy the [source bin folder](AppTools/bin) to the directory where the compiled executable file is located (`source path bin-64(32)/debug(release)/`);    
3. Import [apptools.sql](bin/sql/apptools.sql) under the `bin/sql` directory into the MySql database, or put the entire sql folder into the executable program directory, use SQLite database;      
4. Use the Qtcreator tool-external-Qt language master to publish the translation(lrelease), and then copy the translations to the executable file path, the same as 2, * .ts files can be used.



> 支持MYSQL和SQLITE数据库  
> Support MYSQL and SQLITE database  




程序内部配置，可根据实际情况修改[config.json](bin/cfg/config.json)     

The internal configuration of the program can be modified according to the actual situation [config.json](bin/cfg/config.json)    



登录或者跳过；  

Login or skip;  



显示主界面`你好哇！`  
Shows the main interface `Hello World! `  



## 主界面 Main interface ##
<div align=center><img src="pic/app.png"></div>  