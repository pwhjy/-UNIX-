# -UNIX-
## 模拟UNIX文件系统
### 一、	实验内容
本实验要求大家完成一个 UNIX文件系统的子集的模拟实现。实验的提示与要求：<br>
>**（1）	文件卷结构设计 <br>**
>>0#块可省略 <br>
>>I节点栈及空间为20项 <br>
>>块大小为512字节 <br>
>>卷盘块数大于100 <br>
>>I节点盘块数大于10块 <br>

>**（2）	I节点结构设计 <br>**
>>文件大小 <br>
文件联接计数 <br>
文件地址 <br>
文件拥有者 <br>
文件所属组 <br>
文件权限及类别 <br>
文件最后修改时间 <br>
其中文件地址为六项：四个直接块号，一个一次间址，一个两次间址 <br>

>**（3）	目录结构 <br>**
>>用16字节表示，其中14字节为文件名，2字节为I节点号 <br>

>**（4）	用户及组结构 <br>**
>>用户信息中包括用户名、口令，所属组，用户打开文件表 <br>
（文件树结构应与用户相对应：有每个用户的HOME目录） <br>
组信息中可只包含组名 <br>

>**（5）	文件树结构 <br>**
>>除（4）要求外，适当考虑UNIX本身文件树结构 <br>

>**（6）	实现功能如下命令 <br>**
>>Ls			显示文件目录 <br>
Chmod		改变文件权限 <br>
Chown			改变文件拥有者 <br>
Chgrp			改变文件所属组 <br>
Pwd			显示当前目录 <br>
Cd			改变当前目录 <br>
Mkdir			创建子目录 <br>
Rmdir			删除子目录 <br>
Umask			文件创建屏蔽码 <br>
Mv			改变文件名 <br>
Cp			文件拷贝 <br>
Rm			文件删除 <br>
Ln           	建立文件联接 <br>
Cat			连接显示文件内容 <br>
Passwd		修改用户口令 <br>

>**（7）	可考虑构造一个简单的编辑器，用于创建文件 <br>**

>**（8）	需要创建“sbinfo”命令查看超级块里面数据块内容  <br>**
>>a)	空闲盘块栈 <br>
b)	Inode使用情况 <br>
c)	….. <br>

### 二、	实验环境 <br>
>**系统操作环境：Win10 <br>**
>**系统开发工具：Visual Studio 2017 <br>**

### 三、	实验设计 <br>
**系统流程<br>**
![](https://github.com/pwhjy/-UNIX-/raw/master/ConsoleApplication5/img.jpg)

