#define DISK_SIZE 204800	//磁盘容量200KB
#define BLOCK_SIZE 512	//盘块大小512B
#define DINODE_SIZE 64	//磁盘INODE所占大小64B
#define SUPER_BLOCK_SIZE 260	//超级块所占大小260B
#define BLOCK_NUM 358	//盘块数(仅指用于存储文件的盘块)
#define BLOCK_GROUP_SIZE 20		//盘块组大小
#define INODE_NUM 40	//INode数
#define MAX_NAME_SIZE 14	//名字最长限度
#define MAX_DIRECT_NUM 31	//目录项最大数目
#define MAX_OWNER_NUM 5	//用户项组最大数目
#define MAX_GROUP_NUM 5 //组最大数目
#define OWNER_SIZE 16	//用户项大小16B
#define GROUP_SIZE 16	//组大小16B
#define OWNERS_SIZE 160	//用户项组大小160B
#define GROUPS_SIZE 82	//组集合大小82B
#define FILE_PATH "C:\\Users\\12492\\Desktop\\ConsoleApplication5\\ConsoleApplication5\\disk.dat"	//模拟磁盘文件路径
#define BLOCK_START 42	//起始盘块
#define DIR_PADDING_SIZE 14 //目录结构填充字符数
//#define FILE_MAX_SIZE 2048	//文件最大容量
//#define FILE_MAX_SIZE 4096
#define FILE_MAX_SIZE 68096
#define ROOT 0	//超级权限ID