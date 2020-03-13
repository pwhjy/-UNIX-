#include"define.h"


struct superBlock	//������
{
	unsigned int Fimod1;//�ļ�Ȩ����
	unsigned int Fimod2;//�ļ���Ȩ����
	unsigned int size;	//���̴�С
	unsigned int freeINodeNum;	//����INode��
	unsigned int freeINode[INODE_NUM];	//����INodeջ
	unsigned int nextFreeINode;	//ջ����һ��INode
	unsigned int freeBlockNum;	//�����̿���
	unsigned int freeBlock[BLOCK_GROUP_SIZE];	//�����̿�ջ
	unsigned int nextFreeBlock;	//ջ����һ���̿�
};


struct dINode	//����iNode
{
	unsigned int fileSize;	//�ļ���С
	unsigned int linkNum;	//�ļ���������
	unsigned int addr[6];	//�ļ���ַ:�ĸ�ֱ�ӿ�ţ�һ��һ����ַ��һ��������ַ
	unsigned short ownerId;	//�ļ�ӵ����Id
	unsigned short groupId;	//�ļ�������Id
	unsigned int mod;	//�ļ�Ȩ�޺�����
	long int createTime;	//�ļ�����ʱ��
	long int modifyTime;	//�ļ�����޸�ʱ��
	long int readTime;		//�ļ�������ʱ��
};


struct iNode	//�ڴ�iNode
{
	dINode dINode;
	iNode *parent;	//������Ŀ¼i�ڵ�
	unsigned int nodeId;	//i�ڵ�Id
	unsigned int users;	//���ü���
};


struct direct	//Ŀ¼��ṹ
{
	char name[MAX_NAME_SIZE];	//�ļ���Ŀ¼������
	unsigned short iNodeId;	//�ļ���Ŀ¼��i�ڵ��
};

struct block 
{
	int blockId;
	int name[127];
};

struct dir	//Ŀ¼�ṹ
{
	unsigned short dirNum;	//Ŀ¼����
	direct direct[MAX_DIRECT_NUM];	//Ŀ¼������
	char padding[DIR_PADDING_SIZE];	//Ŀ¼�ṹ����ַ���
};

struct owner	//�û�
{
	unsigned short ownerId;	//�û�Id
	unsigned short groupId;	//��Id
	char ownerName[MAX_NAME_SIZE];	//�û���
	char ownerPassword[MAX_NAME_SIZE];//�û�����
};

struct owners	//�û���
{
	unsigned short ownerNum;	//�û�����
	owner os[MAX_OWNER_NUM];	//�û�����
};

struct group	//��
{
	unsigned short groupId;	//�û���Id
	char groupName[MAX_NAME_SIZE];//����
};

struct groups	//�鼯��
{
	unsigned short groupNum;	//����
	group gs[MAX_GROUP_NUM];
};
