#define DISK_SIZE 204800	//��������200KB
#define BLOCK_SIZE 512	//�̿��С512B
#define DINODE_SIZE 64	//����INODE��ռ��С64B
#define SUPER_BLOCK_SIZE 260	//��������ռ��С260B
#define BLOCK_NUM 358	//�̿���(��ָ���ڴ洢�ļ����̿�)
#define BLOCK_GROUP_SIZE 20		//�̿����С
#define INODE_NUM 40	//INode��
#define MAX_NAME_SIZE 14	//������޶�
#define MAX_DIRECT_NUM 31	//Ŀ¼�������Ŀ
#define MAX_OWNER_NUM 5	//�û����������Ŀ
#define MAX_GROUP_NUM 5 //�������Ŀ
#define OWNER_SIZE 16	//�û����С16B
#define GROUP_SIZE 16	//���С16B
#define OWNERS_SIZE 160	//�û������С160B
#define GROUPS_SIZE 82	//�鼯�ϴ�С82B
#define FILE_PATH "C:\\Users\\12492\\Desktop\\ConsoleApplication5\\ConsoleApplication5\\disk.dat"	//ģ������ļ�·��
#define BLOCK_START 42	//��ʼ�̿�
#define DIR_PADDING_SIZE 14 //Ŀ¼�ṹ����ַ���
//#define FILE_MAX_SIZE 2048	//�ļ��������
//#define FILE_MAX_SIZE 4096
#define FILE_MAX_SIZE 68096
#define ROOT 0	//����Ȩ��ID