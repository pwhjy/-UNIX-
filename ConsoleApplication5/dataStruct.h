#include"define.h"


struct superBlock	//超级块
{
	unsigned int Fimod1;//文件权限码
	unsigned int Fimod2;//文件夹权限码
	unsigned int size;	//磁盘大小
	unsigned int freeINodeNum;	//空闲INode数
	unsigned int freeINode[INODE_NUM];	//空闲INode栈
	unsigned int nextFreeINode;	//栈中下一个INode
	unsigned int freeBlockNum;	//空闲盘块数
	unsigned int freeBlock[BLOCK_GROUP_SIZE];	//空闲盘块栈
	unsigned int nextFreeBlock;	//栈中下一个盘块
};


struct dINode	//磁盘iNode
{
	unsigned int fileSize;	//文件大小
	unsigned int linkNum;	//文件的链接数
	unsigned int addr[6];	//文件地址:四个直接块号，一个一级间址，一个二级间址
	unsigned short ownerId;	//文件拥有者Id
	unsigned short groupId;	//文件所属组Id
	unsigned int mod;	//文件权限和类型
	long int createTime;	//文件创建时间
	long int modifyTime;	//文件最后修改时间
	long int readTime;		//文件最后访问时间
};


struct iNode	//内存iNode
{
	dINode dINode;
	iNode *parent;	//所属的目录i节点
	unsigned int nodeId;	//i节点Id
	unsigned int users;	//引用计数
};


struct direct	//目录项结构
{
	char name[MAX_NAME_SIZE];	//文件或目录的名字
	unsigned short iNodeId;	//文件或目录的i节点号
};

struct block 
{
	int blockId;
	int name[127];
};

struct dir	//目录结构
{
	unsigned short dirNum;	//目录项数
	direct direct[MAX_DIRECT_NUM];	//目录项数组
	char padding[DIR_PADDING_SIZE];	//目录结构填充字符组
};

struct owner	//用户
{
	unsigned short ownerId;	//用户Id
	unsigned short groupId;	//组Id
	char ownerName[MAX_NAME_SIZE];	//用户名
	char ownerPassword[MAX_NAME_SIZE];//用户密码
};

struct owners	//用户组
{
	unsigned short ownerNum;	//用户项数
	owner os[MAX_OWNER_NUM];	//用户项组
};

struct group	//组
{
	unsigned short groupId;	//用户组Id
	char groupName[MAX_NAME_SIZE];//组名
};

struct groups	//组集合
{
	unsigned short groupNum;	//组数
	group gs[MAX_GROUP_NUM];
};
