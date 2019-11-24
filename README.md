# AppTools
UI参考：[ 应用模版 ](https://github.com/xtuer/template-app/tree/master/template-qt "xtuer/template-app")
QT的一套UI模板以及一些小工具合集

1. 编译构建后
2. 把[源码bin文件夹](AppTools/bin)复制到编译出来的可执行文件所在目录，覆盖编译生成的`bin文件夹`；
3. 将`bin/sql`目录下的[AppTools.sql](bin/sql/AppTools.sql)导入MySql数据库；
> 支持MYSQL和SQLITE数据库

程序内部配置，可根据实际情况修改[config.json](bin/cfg/config.json)

然后运行，先显示

## 登录界面 ##

<div align=center><img src="pic/Login.png"></div>
用户名和密码均为`admin`

登陆成功显示主界面`你好呀！`

> 考虑到之后可能会有许多按钮,于是又写了树形按钮结构的代码，正在修改中
> 
> 更改界面显示的代码如下，在[mainwindow.cpp](ui/mainwindow.cpp)文件的`构造`函数里

    1. //initListButton();
    2. //ui->menubtnwidget->hide();
    3. //树形菜单
    4. initMenuButton();
    5. ui->listbtnwidget->hide();

## 界面1： ##
<div align=center><img src="pic/AppTools_1.png"></div>
## 界面2： ##
<div align=center><img src="pic/AppTools_2.png"></div>