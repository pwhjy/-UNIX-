#pragma warning(disable:4996)
#include<iostream>
#include<fstream>
#include<stdio.h>
#include<ctime>
#include<string>
#include<conio.h>
#include<vector>
#include"dataStruct.h"
#include"status.h"
using namespace std;
superBlock *sp = new superBlock();		//������ȫ�ֱ���
iNode *root = new iNode();				//��Ŀ¼ȫ�ֱ���
owners *os = new owners();		//�û���ȫ�ֱ���
groups *gs = new groups();		//�鼯��ȫ�ֱ���
owner *curOwner;		//��ǰ�û�
iNode *curINode = root;			//��ǰiNode
vector<direct*> ds;				//��ǰ·������
dir* d = new dir();	//��ǰĿ¼�ṹ
//int FirstmodOFdir; //�ļ���Ĭ�ϵ�Ȩ����
//int FirstmodOFFILE;//�ļ�Ĭ�ϵ�Ȩ����

void initGlobal(FILE* f);
void initSystem();
bool readINode(iNode* r);
bool writeINode(iNode* w);
bool writeSuperBlock();
bool writeDir(unsigned int blockId, dir* d);
bool readDir(unsigned int blockId, dir* d);
bool readNextBG();
int mkdir(iNode* parent, char name[MAX_NAME_SIZE]);
int getFreeBlock();
int getFreeINode();
int login();
void commandDispatcher();
void displayCommands();
string pwd();
string trim(string s);
int readCurDir();
string ls();
string ls2();
int makebigfile(iNode* parent, char name[MAX_NAME_SIZE]);
int umask1();
int umask2();
void changeumask(string name,int n);
bool checkFileName(char name[MAX_NAME_SIZE]);
int cd(char name[MAX_NAME_SIZE]);
int superMkdir(iNode* parent, char name[MAX_NAME_SIZE], unsigned short ownerId, unsigned short groupId);
int chmod(char name[MAX_NAME_SIZE], unsigned int mod);
int chown(char name[MAX_NAME_SIZE], unsigned short ownerId);
int chgrp(char name[MAX_NAME_SIZE], unsigned short groupId);
int passwd();
bool writeOS();
int mv(char oldName[MAX_NAME_SIZE], char newName[MAX_NAME_SIZE]);
int touch(iNode* parent, char name[MAX_NAME_SIZE]);
int writeText(iNode* temp, string text);
int textAppend(char name[MAX_NAME_SIZE]);
int cat(char name[MAX_NAME_SIZE]);
int readText(iNode *temp);
int rm(char name[MAX_NAME_SIZE]);
int returnFreeBlock(unsigned int blockId);
int returnFreeINode(unsigned int iNodeId);
int ln(char source[MAX_NAME_SIZE], char des[MAX_NAME_SIZE]);
void rmIter(unsigned short iNodeId);
int rmdir(char name[MAX_NAME_SIZE]);
int cp(char source[MAX_NAME_SIZE], char des[MAX_NAME_SIZE]);
string getText(iNode* temp);

//�������ļ�
//int makebigfile(iNode* parent, char name[MAX_NAME_SIZE]) {
//	bool exist = checkFileName(name);
//	if (parent->dINode.fileSize == 0 && parent->dINode.mod != 12 && !exist
//		&& (parent->dINode.ownerId == curOwner->ownerId || curOwner->ownerId == ROOT))
//	{
//		//for(int i=)
	//	FILE *f = fopen(FILE_PATH, "rb+");
	//	if (f == NULL)
	//	
	//	fseek(f, BLOCK_SIZE*blockId, SEEK_SET);
	//	fwrite("1", BLOCK_SIZE, 1, f);
	//}
//}

int umask1()
{
	int a=sp->Fimod1;
	return a;
}

int umask2()
{
	int a = sp->Fimod2;
	return a;
}

void changeumask(string name,int n)//�޸�umask��ֵ
{
	if (name == "dir") {
		sp->Fimod2 = n;
	}
	else if (name == "file") {
		sp->Fimod1 = n;
	}
}

void initGlobal(FILE* f)	//��ʼ��ȫ�ֱ���
{
	sp->Fimod1 = 6;//�ļ�
	sp->Fimod2 = 14;
	sp->size = DISK_SIZE;
	sp->freeBlockNum = BLOCK_NUM;
	sp->freeINodeNum = INODE_NUM - 1;
	for (int j = 0; j < INODE_NUM; j++)
		sp->freeINode[j] = INODE_NUM + 1 - j;//
	sp->nextFreeINode = INODE_NUM - 2;

	root->parent = NULL;
	root->nodeId = 2;
	root->users = 1;
	time_t now;			//��ȡ��ǰʱ���
	now = time(NULL);
	root->dINode.createTime = now;
	root->dINode.fileSize = 0;
	root->dINode.groupId = 0;
	root->dINode.linkNum = 0;
	root->dINode.mod = 14;
	root->dINode.modifyTime = now;
	root->dINode.ownerId = 0;
	root->dINode.readTime = now;

	os->ownerNum = 2;
	os->os[0].ownerId = 1;
	os->os[0].groupId = 1;
	strcpy(os->os[0].ownerName, "pwh14");
	strcpy(os->os[0].ownerPassword, "123456");
	os->os[1].ownerId = 2;
	os->os[1].groupId = 2;
	strcpy(os->os[1].ownerName, "czxt");
	strcpy(os->os[1].ownerPassword, "123456");

	gs->groupNum = 2;
	gs->gs[0].groupId = 1;
	strcpy(gs->gs[0].groupName, "zjut");
	gs->gs[1].groupId = 2;
	strcpy(gs->gs[1].groupName, "zju");

	f = fopen(FILE_PATH, "wb");//д��ȥ
	int bGs = BLOCK_NUM / BLOCK_GROUP_SIZE;		//�����̿������
	int left = BLOCK_NUM - bGs * BLOCK_GROUP_SIZE;	//ʣ��һ���̿���

	fseek(f, DISK_SIZE, SEEK_SET);
	fwrite("?", 1, 1, f);	//����200KBģ����̿ռ�
	if (left == 0)
	{
		for (int i = 0; i < BLOCK_GROUP_SIZE; i++)
			sp->freeBlock[i] = BLOCK_START + BLOCK_GROUP_SIZE - 1 - i;
		sp->nextFreeBlock = BLOCK_GROUP_SIZE - 1;

		for (int j = 0; j < bGs; j++)		//��������
		{
			unsigned int blocksNum = BLOCK_GROUP_SIZE;
			unsigned int blocks[BLOCK_GROUP_SIZE]; 
			for (int k = 0; k < BLOCK_GROUP_SIZE; k++)
				blocks[k] = left + BLOCK_START + BLOCK_GROUP_SIZE * (j + 1) - k - 1;
			fseek(f, BLOCK_SIZE*(left + BLOCK_START + BLOCK_GROUP_SIZE * j - 1), SEEK_SET);
			fwrite(&blocksNum, sizeof(blocksNum), 1, f);
			fwrite(&blocks, sizeof(blocks), 1, f);
		}
		unsigned int blocksNum = 0;
		unsigned int blocks[BLOCK_GROUP_SIZE];
		fseek(f, BLOCK_SIZE*(left + BLOCK_START + BLOCK_GROUP_SIZE * bGs - 1), SEEK_SET);
		fwrite(&blocksNum, sizeof(blocksNum), 1, f);
		fwrite(&blocks, sizeof(blocks), 1, f);
	}
	else
	{
		for (int i = 0; i < left; i++)
			sp->freeBlock[i] = BLOCK_START + left - 1 - i;
		sp->nextFreeBlock = left - 1;

		for (int j = 0; j < bGs; j++)		//��������
		{
			unsigned int blocksNum = BLOCK_GROUP_SIZE;
			unsigned int blocks[BLOCK_GROUP_SIZE];
			for (int k = 0; k < BLOCK_GROUP_SIZE; k++)
				blocks[k] = left + BLOCK_START + BLOCK_GROUP_SIZE * (j + 1) - k - 1;
			fseek(f, BLOCK_SIZE*(left + BLOCK_START + BLOCK_GROUP_SIZE * j - 1), SEEK_SET);
			fwrite(&blocksNum, sizeof(blocksNum), 1, f);
			fwrite(&blocks, sizeof(blocks), 1, f);
		}
		unsigned int blocksNum = 0;
		unsigned int blocks[BLOCK_GROUP_SIZE];
		fseek(f, BLOCK_SIZE*(left + BLOCK_START + BLOCK_GROUP_SIZE * bGs - 1), SEEK_SET);
		fwrite(&blocksNum, sizeof(blocksNum), 1, f);
		fwrite(&blocks, sizeof(blocks), 1, f);
	}
	fseek(f, BLOCK_SIZE, SEEK_SET);
	fwrite(sp, sizeof(superBlock), 1, f);	//д��superBlock
	fwrite(os, sizeof(owners), 1, f);		//д���û���
	fwrite(gs, sizeof(groups), 1, f);		//д���鼯��
	fseek(f, BLOCK_SIZE*root->nodeId, SEEK_SET);	//д���Ŀ¼iNode
	fwrite(&root->dINode, sizeof(dINode), 1, f);
	fclose(f);
	direct* dt = new direct();
	dt->iNodeId = 2;
	strcpy(dt->name, "root");
	ds.push_back(dt);

	char cname1[] = "coderTT";
	char cname2[] = "TT";
	superMkdir(root, cname1, 1, 1);
	superMkdir(root, cname2, 2, 2);
	//superMkdir(root, "coderTT", 1, 1);
	//superMkdir(root, "TT", 2, 2);
}


void initSystem()	//��ʼ���ļ�ϵͳ
{
	FILE *f = fopen(FILE_PATH, "rb");
	if (f == NULL)		//ģ����̲�����
	{
		//cout << 1 << endl;
		initGlobal(f);
	}
	else		//����superBlock���ļ�ϵͳ������Ϣ
	{
		fseek(f, BLOCK_SIZE, SEEK_SET);
		fread(sp, sizeof(superBlock), 1, f);
		fread(os, sizeof(owners), 1, f);
		fread(gs, sizeof(groups), 1, f);
		root->nodeId = 2;
		root->parent = NULL;
		root->users = 2;
		fseek(f, BLOCK_SIZE*root->nodeId, SEEK_SET);
		fread(&root->dINode, sizeof(dINode), 1, f);
		direct* dt = new direct();
		dt->iNodeId = 2;
		strcpy(dt->name, "root");
		ds.push_back(dt);
		fclose(f);
		readCurDir();
	}
}

bool readINode(iNode* r)		//��ȡָ��iNode
{
	FILE *f = fopen(FILE_PATH, "rb");
	if (f == NULL)
		return false;
	else
	{
		fseek(f, BLOCK_SIZE*r->nodeId, SEEK_SET);
		fread(&r->dINode, sizeof(dINode), 1, f);
		fclose(f);
		return true;
	}
}


bool writeINode(iNode* w)	//д��ָ��iNode
{
	FILE *f = fopen(FILE_PATH, "rb+");
	if (f == NULL)
		return false;
	else
	{
		fseek(f, BLOCK_SIZE*w->nodeId, SEEK_SET);
		fwrite(&w->dINode, sizeof(dINode), 1, f);
		fclose(f);
		return true;
	}
}

bool writeSuperBlock()	//д�볬����
{
	FILE *f = fopen(FILE_PATH, "rb+");
	if (f == NULL)
		return false;
	else
	{
		fseek(f, BLOCK_SIZE, SEEK_SET);
		fwrite(sp, sizeof(superBlock), 1, f);
		fclose(f);
		return true;
	}
}


bool writeDir(unsigned int blockId, dir* d)	//д��Ŀ¼�ṹ
{
	FILE *f = fopen(FILE_PATH, "rb+");
	if (f == NULL)
		return false;
	else
	{
		fseek(f, BLOCK_SIZE*blockId, SEEK_SET);
		fwrite(d, sizeof(dir), 1, f);
		fclose(f);
		return true;
	}
}

bool readDir(unsigned int blockId, dir* d)	//��ȡĿ¼�ṹ
{
	FILE *f = fopen(FILE_PATH, "rb");
	if (f == NULL)
		return false;
	else
	{
		fseek(f, BLOCK_SIZE*blockId, SEEK_SET);
		fread(d, sizeof(dir), 1, f);
		fclose(f);
		return true;
	}
}
bool readNextBG()	//��ȡ��һ���̿�
{
	FILE *f = fopen(FILE_PATH, "rb");
	if (f == NULL)
		return false;
	else
	{
		fseek(f, BLOCK_SIZE*sp->freeBlock[0], SEEK_SET);
		fread(&sp->nextFreeBlock, sizeof(sp->nextFreeBlock), 1, f);
		fread(&sp->freeBlock, sizeof(sp->freeBlock), 1, f);
		sp->freeBlockNum--;
		sp->nextFreeBlock--;
		fclose(f);
		writeSuperBlock();
		return true;
	}
}


int mkdir(iNode* parent, char name[MAX_NAME_SIZE])	//�����ļ���
{
	bool exist = checkFileName(name);
	if (parent->dINode.fileSize == 0 && parent->dINode.mod != 12 && !exist && (parent->dINode.ownerId == curOwner->ownerId || curOwner->ownerId == ROOT))
	{
		int blockId = getFreeBlock();
		if (blockId < 0)
			return blockId;
		else
		{
			int iNodeId = getFreeINode();
			if (iNodeId < 0)
				return iNodeId;
			else
			{
				parent->dINode.fileSize = sizeof(dir);
				time_t now;
				now = time(NULL);
				parent->dINode.modifyTime = now;
				parent->dINode.addr[0] = blockId;
				writeINode(parent);		//����parent iNode�ڵ�
				direct *dt = new direct();	//�½�һ��Ŀ¼��
				strcpy(dt->name, name);
				dt->iNodeId = iNodeId;
				dINode *di = new dINode();	//�½�һ��dINode
				di->createTime = now;
				di->fileSize = 0;
				di->groupId = parent->dINode.groupId;
				di->linkNum = 0;
				di->mod = 14;
				di->modifyTime = now;
				di->ownerId = parent->dINode.ownerId;
				di->readTime = now;  
				iNode i;
				i.dINode = *di;
				i.nodeId = iNodeId;
				i.parent = parent;
				i.users = parent->users;
				writeINode(&i);		//���´�����Ŀ¼��iNode�ڵ�
				dir	dd;
				dd.dirNum = 1;
				dd.direct[0] = *dt;
				writeDir(blockId, &dd);	//ָ��blockд��Ŀ¼�ṹ
				//delete dd;
				*d = dd;
				//char name1[] = ".";
				//char name2[] = "..";
				//mkdir 
				//cout << "hell0" << endl;
				delete di;
				delete dt;
				return STATUS_OK;
			}
		}
	}
	else if (parent->dINode.fileSize != 0 && parent->dINode.mod != 12 && !exist && (parent->dINode.ownerId == curOwner->ownerId || curOwner->ownerId == ROOT))
		{
			int iNodeId = getFreeINode();
			if (iNodeId < 0)
				return iNodeId;
			else
			{
				time_t now;
				now = time(NULL);
				parent->dINode.modifyTime = now;
				direct *dt = new direct();	//�½�һ��Ŀ¼��
				strcpy(dt->name, name);
				dt->iNodeId = iNodeId;
				dINode *di = new dINode();	//�½�һ��dINode
				di->createTime = now;
				di->fileSize = 0;
				di->groupId = parent->dINode.groupId;
				di->linkNum = 0;
				di->mod = 14;
				di->modifyTime = now;
				di->ownerId = parent->dINode.ownerId;
				di->readTime = now;
				iNode i;
				i.dINode = *di;
				i.nodeId = iNodeId;
				i.parent = parent;
				i.users = parent->users;
				writeINode(&i);
				dir dd;
				readDir(parent->dINode.addr[0], &dd);
				if (dd.dirNum < MAX_DIRECT_NUM)
				{
					dd.direct[dd.dirNum] = *dt;
					dd.dirNum++;
					writeDir(parent->dINode.addr[0], &dd);
					*d = dd;
					delete di;
					delete dt;
					return STATUS_OK;
				}
				else
					return STATUS_BEYOND_DIRECT_NUM;
			}
		}
		else
			if (parent->dINode.ownerId != curOwner->ownerId&&curOwner->ownerId != ROOT)
				return STATUS_BEYOND_RIGHT;
			else
				if (parent->dINode.mod == 12)
					return STATUS_READ_ONLY;
				else
					if (exist)
						return STATUS_FILENAME_EXIST;
	return STATUS_ERROR;
}


int getFreeBlock()	//��ȡһ����п�
{
	if (sp->freeBlockNum > 0)
	{
		if (sp->nextFreeBlock > 0)
		{
			sp->freeBlockNum--;
			sp->nextFreeBlock--;
			writeSuperBlock();
			return sp->freeBlock[sp->nextFreeBlock + 1];
		}
		else
		{
			readNextBG();
			return getFreeBlock();
		}
	}
	else
		return STATUS_NO_BLOCK;
}

int getFreeINode()	//��ȡһ��iNode�ڵ�
{
	if (sp->freeINodeNum > 0)
	{
		sp->freeINodeNum--;
		sp->nextFreeINode--;
		writeSuperBlock();
		return sp->freeINode[sp->nextFreeINode + 1];
	}
	else
		return STATUS_NO_INODE;
}


int login()		//��¼
{
	cout << "please input your userName:";
	string userName;
	cin >> userName;
	cout << "please input your password:";
	char password[MAX_NAME_SIZE] = { 0 };
	int i = 0;
	while (i < MAX_NAME_SIZE)
	{
		password[i] = _getch();
		if (password[i] == 13)
		{
			password[i] = '\0';
			break;
		}
		i++;
	}
	cout << endl;
	cout << password << endl;
	cout << endl;
	char un[MAX_NAME_SIZE];
	strcpy(un, userName.c_str());
	cout << un << endl;

	/*for (int i = 0; i < 2; i++)
	{
		cout <<1<< os->os[i].ownerName << " " << os->os[i].ownerPassword << endl;
	}*/

	for (int j = 0; j < os->ownerNum; j++)
	{
		cout <<os->os[j].ownerName << " " << os->os[j].ownerPassword << endl;
		if (strcmp(un, os->os[j].ownerName) == 0 && strcmp(password, os->os[j].ownerPassword) == 0)
			{
					curOwner = &os->os[j];
			        return STATUS_OK;
			}
		/*else if(strcmp(un, os->os[j].ownerName) != 0 && strcmp(password, os->os[j].ownerPassword) == 0)
		{
			cout << "���ֲ���" << endl;
		}
		else if(strcmp(un, os->os[j].ownerName) == 0 && strcmp(password, os->os[j].ownerPassword) != 0)
		{
			cout << "���벻��" << endl;
		}*/
	}
	//return STATUS_OK;
	cout << "UserName or Password Wrong!!!" << endl;
	return login();
}

void commandDispatcher()	//���������
{
	cout << curOwner->ownerName << "@UnixFileSystem:~" << pwd() << "$";
	char c[100];
	cin.getline(c, 100, '\n');
	while (c[0] == NULL)
		cin.getline(c, 100, '\n');
	string command = c;
	command = trim(command);
	int flag = -1;
	int subPos = command.find_first_of(" ");
	if (subPos == -1)
	{
		if (command == "ls")
			flag = 1;
		else if (command == "pwd")
			flag = 5;
		else if (command == "passwd")
			flag = 14;
		else if (command == "sp")
			flag = 17;
		else if (command == "umask")
			flag = 19;
		else if (command == "help")
			flag = 21;
	}
	else
	{
		string c = command.substr(0, subPos);
		//string c1 = command.substr(subPos + 1);
		if (c=="umask")
		{
			flag = 20;
		}
		if (c == "ls")
			flag = 18;
		if (c == "chmod")
			flag = 2;
		else if (c == "chown")
			flag = 3;
		else if (c == "chgrp")
			flag = 4;
		else if (c == "cd")
			flag = 6;
		else if (c == "mkdir")
			flag = 7;
		else if (c == "rmdir")
			flag = 8;
		else if (c == "mv")
			flag = 9;
		else if (c == "cp")
			flag = 10;
		else if (c == "rm")
			flag = 11;
		else if (c == "ln")
		    flag = 12;
		else if (c == "cat")
			flag = 13;
		else if (c == "touch")
			flag = 15;
		else if (c == ">>")
			flag = 16;
	}
	switch (flag)
	{
	case 1: {
		cout << ls() << endl;
		break;
	};
	case 2: {
		string pattern = command.substr(subPos + 1);
		pattern = trim(pattern);
		int subPos2 = pattern.find_first_of(" ");
		if (subPos2 == -1)
			cout << "Error Pattern..." << endl;
		else
		{
			string mod = pattern.substr(0, subPos2);
			string fileName = pattern.substr(subPos2 + 1);
			fileName = trim(fileName);
			int subPos3 = fileName.find_first_of(" ");
			if (subPos3 != -1)
				cout << "Error pattern..." << endl;
			else
			{
				unsigned short m = atoi(mod.c_str());
				char name[MAX_NAME_SIZE];
				strcpy(name, fileName.c_str());
				int result = chmod(name, m);
				if (result == STATUS_MOD_ERROR)
					cout << "Error: Pattern is illegal..." << endl;
				else if (result == STATUS_FILENAME_NONEXIST)
					cout << "Error: FileName is nonexisted..." << endl;
				else if (result == STATUS_BEYOND_RIGHT)
					cout << "Error: Beyond Your Right..." << endl;
			}
		}
		break;
	};
	case 3: {
		string pattern = command.substr(subPos + 1);
		pattern = trim(pattern);
		int subPos2 = pattern.find_first_of(" ");
		if (subPos2 == -1)
			cout << "Error Pattern..." << endl;
		else
		{
			string ownerId = pattern.substr(0, subPos2);
			string fileName = pattern.substr(subPos2 + 1);
			fileName = trim(fileName);
			int subPos3 = fileName.find_first_of(" ");
			if (subPos3 != -1)
				cout << "Error pattern..." << endl;
			else
			{
				unsigned short oi = atoi(ownerId.c_str());
				char name[MAX_NAME_SIZE];
				strcpy(name, fileName.c_str());
				int result = chown(name, oi);
				if (result == STATUS_OWNER_NONEXIST)
					cout << "Error: OwnerId is nonexisted..." << endl;
				else if (result == STATUS_FILENAME_NONEXIST)
					cout << "Error: FileName is nonexisted..." << endl;
				else if (result == STATUS_BEYOND_RIGHT)
					cout << "Error: Beyond Your Right..." << endl;
			}

		}
		break;
	};
	case 4: {
		string pattern = command.substr(subPos + 1);
		pattern = trim(pattern);
		int subPos2 = pattern.find_first_of(" ");
		if (subPos2 == -1)
			cout << "Error Pattern..." << endl;
		else
		{
			string groupId = pattern.substr(0, subPos2);
			string fileName = pattern.substr(subPos2 + 1);
			fileName = trim(fileName);
			int subPos3 = fileName.find_first_of(" ");
			if (subPos3 != -1)
				cout << "Error pattern..." << endl;
			else
			{
				unsigned short gi = atoi(groupId.c_str());
				char name[MAX_NAME_SIZE];
				strcpy(name, fileName.c_str());
				int result = chgrp(name, gi);
				if (result == STATUS_GROUP_NONEXIST)
					cout << "Error: GroupId is nonexisted..." << endl;
				else if (result == STATUS_FILENAME_NONEXIST)
					cout << "Error: FileName is nonexisted..." << endl;
				else if (result == STATUS_BEYOND_RIGHT)
					cout << "Error: Beyond Your Right..." << endl;
			}
		}
		break;
	};
	case 5: {
		cout << pwd() << endl;
		break;
	};
	case 6: {
		string fileName = command.substr(subPos + 1);
		fileName = trim(fileName);
		int subPos2 = fileName.find_first_of(" ");
		if (subPos2 != -1)
			cout << "Error DirectName..." << endl;
		else
		{
			char name[MAX_NAME_SIZE];
			strcpy(name, fileName.c_str());
			int result = cd(name);
			if (result == STATUS_FILENAME_NONEXIST)
				cout << "Error: Dir is not existed..." << endl;
			else if (result == STATUS_NOT_DIRECT)
				cout << "Error: Not a direct..." << endl;
		}
		break;
	};
	case 7: {
		string fileName = command.substr(subPos + 1);
		fileName = trim(fileName);
		int subPos2 = fileName.find_first_of(" ");
		if (subPos2 != -1)
			cout << "Error FileName..." << endl;
		else
		{
			char name[MAX_NAME_SIZE];
			strcpy(name, fileName.c_str());
			int result = mkdir(curINode, name);
			if (result == STATUS_NO_BLOCK)
				cout << "Error: No Free Block..." << endl;
			else if (result == STATUS_NO_INODE)
				cout << "Error: No Free INode..." << endl;
			else if (result == STATUS_BEYOND_DIRECT_NUM)
				cout << "Error: Beyond Max Direct Num..." << endl;
			else if (result == STATUS_READ_ONLY)
				cout << "Error: The Dir can not be written..." << endl;
			else if (result == STATUS_ERROR)
				cout << "Error: Unexpected..." << endl;
			else if (result == STATUS_FILENAME_EXIST)
				cout << "Error: FileName has existed..." << endl;
			else if (result == STATUS_BEYOND_RIGHT)
				cout << "Error: Beyond Your Right..." << endl;
		}
		break;
	};
	case 8: {
		string fileName = command.substr(subPos + 1);
		fileName = trim(fileName);
		int subPos2 = fileName.find_first_of(" ");
		if (subPos2 != -1)
			cout << "Error FileName..." << endl;
		else
		{
			char name[MAX_NAME_SIZE];
			strcpy(name, fileName.c_str());
			int result = rmdir(name);
			if (result == STATUS_NOT_DIRECT)
				cout << "Error: Not a direct..." << endl;
			else if (result == STATUS_BEYOND_RIGHT)
				cout << "Error: Byond your right..." << endl;
			else if (result == STATUS_FILENAME_NONEXIST)
			    cout << "Error: FileName doesn't exist..." << endl;
		}
		break;
	};
	case 9: {
		string pattern = command.substr(subPos + 1);
		pattern = trim(pattern);
		int subPos2 = pattern.find_first_of(" ");
		if (subPos2 == -1)
			cout << "Error Pattern..." << endl;
		else
		{
			string oldName = pattern.substr(0, subPos2);
			string newName = pattern.substr(subPos2 + 1);
			newName = trim(newName);
			int subPos3 = newName.find_first_of(" ");
			if (subPos3 != -1)
				cout << "Error pattern..." << endl;
			else
			{
				char oldName1[MAX_NAME_SIZE];
				strcpy(oldName1, oldName.c_str());
				char newName1[MAX_NAME_SIZE];
				strcpy(newName1, newName.c_str());
				int result = mv(oldName1, newName1);
				if (result == STATUS_FILENAME_NONEXIST)
					cout << "Error: FileName is nonexisted..." << endl;
				else if (result == STATUS_BEYOND_RIGHT)
					cout << "Error: Beyond Your Right..." << endl;
				else if (result == STATUS_FILENAME_EXIST)
					cout << "Error: NewName has existed..." << endl;
			}
		}
		break;
	};
	case 10: {
		string pattern = command.substr(subPos + 1);
		pattern = trim(pattern);
		int subPos2 = pattern.find_first_of(" ");
		if (subPos2 == -1)
			cout << "Error Pattern..." << endl;
		else
		{
			string source = pattern.substr(0, subPos2);
			string des = pattern.substr(subPos2 + 1);
			des = trim(des);
			int subPos3 = des.find_first_of(" ");
			if (subPos3 != -1)
				cout << "Error pattern..." << endl;
			else
			{
				char s[MAX_NAME_SIZE];
				strcpy(s, source.c_str());
				char d[MAX_NAME_SIZE];
				strcpy(d, des.c_str());
				int result = cp(s, d);
				if (result == STATUS_FILENAME_NONEXIST)
					cout << "Error: FileName doesn't exist..." << endl;
				else if (result == STATUS_SDNAME_OVERLAP)
					cout << "Error: SourceName overlap desName..." << endl;
				else if (result == STATUS_NOT_FILE)
					cout << "Error: Not a file..." << endl;
				else if (result == STATUS_READ_ONLY)
					cout << "Error: Read only..." << endl;
				else if (result == STATUS_BEYOND_RIGHT)
					cout << "Error: Beyond your right..." << endl;
				else if (result == STATUS_NO_BLOCK)
					cout << "Error: No free block..." << endl;
				else if (result == STATUS_NO_INODE)
					cout << "Error: No free iNode..." << endl;
			}
		}
		break;
	};
	case 11: {
		string fileName = command.substr(subPos + 1);
		fileName = trim(fileName);
		int subPos2 = fileName.find_first_of(" ");
		if (subPos2 != -1)
			cout << "Error FileName..." << endl;
		else
		{
			char name[MAX_NAME_SIZE];
			strcpy(name, fileName.c_str());
			int result = rm(name);
			if (result == STATUS_NOT_FILE)
				cout << "Error: Not a file..." << endl;
			else if (result == STATUS_BEYOND_RIGHT)
				cout << "Error: Beyond your right..." << endl;
			else if (result == STATUS_FILENAME_NONEXIST)
				cout << "Error: FileName doesn't exist..." << endl;
		}
		break;
	};
	case 12: {
		string pattern = command.substr(subPos + 1);
		pattern = trim(pattern);
		int subPos2 = pattern.find_first_of(" ");
		if (subPos2 == -1)
			cout << "Error Pattern..." << endl;
		else
		{
			string source = pattern.substr(0, subPos2);
			string des = pattern.substr(subPos2 + 1);
			des = trim(des);
			int subPos3 = des.find_first_of(" ");
			if (subPos3 != -1)
				cout << "Error pattern..." << endl;
			else
			{
				char s[MAX_NAME_SIZE];
				strcpy(s, source.c_str());
				char d[MAX_NAME_SIZE];
				strcpy(d, des.c_str());
				int result = ln(s, d);
				if (result == STATUS_BEYOND_RIGHT)
					cout << "Error: Beyond your right..." << endl;
				else if (result == STATUS_FILENAME_NONEXIST)
					cout << "Error: FileName doesn't exist..." << endl;
				else if (result == STATUS_TYPE_NOT_MATCH)
					cout << "Error: Source Type doesn't match des type..." << endl;
				else if (result == STATUS_SDNAME_OVERLAP)
					cout << "Error: SourceName overlap desName..." << endl;
			}
		};
		break;
	}
	case 13: {
		string fileName = command.substr(subPos + 1);
		fileName = trim(fileName);
		int subPos2 = fileName.find_first_of(" ");
		if (subPos2 != -1)
			cout << "Error FileName..." << endl;
		else
		{
			char name[MAX_NAME_SIZE];
			strcpy(name, fileName.c_str());
			int result = cat(name);
			if (result == STATUS_BEYOND_RIGHT)
				cout << "Error: Beyond your right..." << endl;
			else if (result == STATUS_NOT_FILE)
				cout << "Error: Not a file..." << endl;
			else if (result == STATUS_WRITE_ONLY)
				cout << "Error: Write only..." << endl;
			else if (result == STATUS_FILENAME_NONEXIST)
				cout << "Error: FileName doesn't exist..." << endl;
			else if (result == STATUS_FILE_OPEN_ERROR)
				cout << "Error: File open error..." << endl;
		}
		break;
	}
	case 14: {
		int result = passwd();
		if (result == STATUS_PASSWORD_WRONG)
			cout << "Error: Password is wrong..." << endl;
		else if (result == STATUS_CONFIRM_WRONG)
			cout << "Error: Confirm password is wrong..." << endl;
		else if (result == STATUS_ERROR)
			cout << "Error: Unexpected error..." << endl;
		break;
	};
	case 15: {
		string fileName = command.substr(subPos + 1);
		fileName = trim(fileName);
		int subPos2 = fileName.find_first_of(" ");
		if (subPos2 != -1)
			cout << "Error FileName..." << endl;
		else
		{
			char name[MAX_NAME_SIZE];
			strcpy(name, fileName.c_str());
			int result = touch(curINode, name);
			if (result == STATUS_NO_BLOCK)
				cout << "Error: No Free Block..." << endl;
			else if (result == STATUS_NO_INODE)
				cout << "Error: No Free INode..." << endl;
			else if (result == STATUS_BEYOND_DIRECT_NUM)
				cout << "Error: Beyond Max Direct Num..." << endl;
			else if (result == STATUS_READ_ONLY)
				cout << "Error: The Dir can not be written..." << endl;
			else if (result == STATUS_ERROR)
				cout << "Error: Unexpected..." << endl;
			else if (result == STATUS_FILENAME_EXIST)
				cout << "Error: FileName has existed..." << endl;
			else if (result == STATUS_BEYOND_RIGHT)
				cout << "Error: Beyond Your Right..." << endl;
		}
		break;
	};
	case 16: {
		string fileName = command.substr(subPos + 1);
		fileName = trim(fileName);
		int subPos2 = fileName.find_first_of(" ");
		if (subPos2 != -1)
			cout << "Error FileName..." << endl;
		else
		{
			char name[MAX_NAME_SIZE];
			strcpy(name, fileName.c_str());
			int result = textAppend(name);
			if (result == STATUS_BEYOND_RIGHT)
				cout << "Error: Byond your right..." << endl;
			else if (result == STATUS_NOT_FILE)
				cout << "Error: Not a file..." << endl;
			else if (result == STATUS_READ_ONLY)
				cout << "Error: The file can not be written..." << endl;
			else if (result == STATUS_FILENAME_NONEXIST)
				cout << "Error: FileName doesn't exist..." << endl;
			else if (result == STATUS_BEYOND_SIZE)
				cout << "Error: Beyond file's max size..." << endl;
			else if (result == STATUS_FILE_OPEN_ERROR)
				cout << "Error: File open error..." << endl;
		}
		break;
	};
	case 17:
	{
		cout << "FreeBlockNum:" << sp->freeBlockNum << endl;
		cout << "NextFreeBlock:" << sp->nextFreeBlock << endl;
		cout << "FreeBlock:" << endl;
		for (int i = 0; i <= sp->nextFreeBlock; i++)
			cout << "[" << i << "]:" << sp->freeBlock[i] << endl;
		cout << "FreeINodeNum:" << sp->freeINodeNum << endl;
		cout << "NextFreeINode:" << sp->nextFreeINode << endl;
		cout << "FreeINode:" << endl;
		for (int j = 0; j <= sp->nextFreeINode; j++)
			cout << "[" << j << "]:" << sp->freeINode[j] << endl;
		break;
	}
	case 18:
	{
		cout << ls2() << endl;
		break;
	}
	case 19:
	{
		cout <<"fileumask:"<< umask1() << endl;
		cout <<"dirumask:"<< umask2() << endl;
		break;
	}
	case 20:
	{
		string pattern = command.substr(subPos + 1);
		pattern = trim(pattern);
		int subPos2 = pattern.find_first_of(" ");
		if (subPos2 == -1)
		{
			cout << "Error Pattern..." << endl;
		}
		else
		{
			string mod = pattern.substr(0, subPos2);
			string name = pattern.substr(subPos2 + 1);
			name = trim(name);
			int subPos3 = name.find_first_of(" ");
			if (subPos3 != -1)
				cout << "Error pattern..." << endl;
			else
			{
				unsigned short m = atoi(mod.c_str());
				if (m == 8 || m == 9 || m == 11 || m == 13 || m > 14)
				{
					cout << "No such mod" << endl;
				}
				else
				{
					if (name == "file") {
						if(m>=1&&m<=7)
						sp->Fimod1 = m;
						else
						{
							cout << "Not file's mod" << endl;
						}
					}
					else if(name=="dir")
					{
						if(m==10||m==12||m==14)
						sp->Fimod2 = m;
						else
						{
							cout << "Not director's mod" << endl;
						}
					}
					else
					{
						cout << "Error name..." << endl;
					}
				}
				
			}
		}
		break;
	}
	case 21:
	{
		displayCommands();
		break;
	}
	default: {
		cout << "Error Command..." << endl;
		break;
	};
	}
	commandDispatcher();
}

void displayCommands()		//��ʾ����
{
	cout << "ls		��ʾĿ¼�ļ�" << endl;
	cout << "chmod		�ı��ļ�Ȩ��" << endl;
	cout << "chown		�ı��ļ�ӵ����" << endl;
	cout << "chgrp		�ı��ļ�������" << endl;
	cout << "pwd		��ʾ��ǰĿ¼" << endl;
	cout << "cd		�ı䵱ǰĿ¼" << endl;
	cout << "mkdir		������Ŀ¼" << endl;
	cout << "rmdir		ɾ����Ŀ¼" << endl;
	cout << "mv		�ı��ļ���" << endl;
	cout << "cp		�ļ�����" << endl;
	cout << "rm		�ļ�ɾ��" << endl;
	cout << "umask      �ļ�Ȩ����" << endl;
	cout << "ln		�����ļ�����" << endl;
	cout << "cat		������ʾ�ļ�����" << endl;
	cout << "passwd		�޸��û�����" << endl;
	cout << "touch		�����ļ�" << endl;
	cout << ">>		�ı�����׷��" << endl;
}


string pwd()				//��ʾ��ǰ·��
{
	string path;
	for (int i = 0; i < ds.size(); i++)
	{
		path += ds[i]->name;
		path += "/";
	}
	return path;
}

string trim(string s)		//�ַ�����ʽ��
{
	if (s.empty())
		return s;
	s.erase(0, s.find_first_not_of(" "));
	s.erase(s.find_last_not_of(" ") + 1);
	return s;
}

int readCurDir()				//��ȡ��ǰĿ¼
{
	FILE *f = fopen(FILE_PATH, "rb");
	if (f == NULL)
		return STATUS_FILE_OPEN_ERROR;
	else
	{
		if (curINode->dINode.fileSize == 0)
			d->dirNum = 0;
		else
		{
			fseek(f, BLOCK_SIZE*curINode->dINode.addr[0], SEEK_SET);
			fread(d, sizeof(dir), 1, f);
			fclose(f);
		}
		return STATUS_OK;
	}
}

string ls()
{
	string ls;
	for (int i = 0; i < d->dirNum; i++)
	{
		if (d->direct[i].name[0] != '.')
		{
			ls += d->direct[i].name;
			ls += " ";
		}
	}
	return ls;
}

string ls2()//��ʾ�����ļ�
{
	string ls;
	for (int i = 0; i < d->dirNum; i++)
	{
		if (d->direct[i].name[0]=='.')
		{
			string a = d->direct[i].name;
			string b = a.substr(1,a.size());
			ls += b;
			ls += " ";
		}
		else
		{
			ls += d->direct[i].name;
			ls += " ";
		}
			
	}
	return ls;
}

bool checkFileName(char name[MAX_NAME_SIZE])
{
	FILE *f = fopen(FILE_PATH, "rb");
	if (f == NULL)
		return true;
	else
	{
		dir d;
		fseek(f, BLOCK_SIZE*curINode->dINode.addr[0], SEEK_SET);
		fread(&d, sizeof(dir), 1, f);
		fclose(f);
		for (int i = 0; i < d.dirNum; i++)
			if (strcmp(d.direct[i].name, name) == 0)
				return true;
		return false;
	}
}

int cd(char name[MAX_NAME_SIZE])	//�ı䵱ǰĿ¼
{
	direct *dt = new direct();
	if (strcmp(name, ".") == 0)//
	{
		delete dt;
		return STATUS_OK;
	}
	else
		if (strcmp(name, "..") == 0)//������һ��
		{
			if (curINode->parent != NULL)
			{
				iNode *temp = curINode;
				curINode = curINode->parent;
				delete temp;
				direct* t = ds[ds.size() - 1];
				ds.pop_back();
				delete t;
				readCurDir();
				return STATUS_OK;
			}
			delete dt;
			return STATUS_OK;
		}
		else
		{
			bool dtExist = false;
			for (int i = 0; i < d->dirNum; i++)
				if (strcmp(d->direct[i].name, name) == 0)
				{
					*dt = d->direct[i];
					dtExist = true;
				}
			if (!dtExist)
			{
				delete dt;
				return STATUS_FILENAME_NONEXIST;
			}
			else
			{
				iNode *now = new iNode();
				now->nodeId = dt->iNodeId;
				now->parent = curINode;
				now->users = curINode->users;
				readINode(now);
				if (now->dINode.mod > 7)
				{
					curINode = now;
					ds.push_back(dt);
					readCurDir();
					return STATUS_OK;
				}
				else
				{
					delete dt;
					delete now;
					return STATUS_NOT_DIRECT;
				}
			}
		}
}

//�����û�Ŀ¼
int superMkdir(iNode* parent, char name[MAX_NAME_SIZE], unsigned short ownerId, unsigned short groupId)	
{
	bool exist = checkFileName(name);
	if (parent->dINode.fileSize == 0)
	{
		int blockId = getFreeBlock();
		if (blockId < 0)
			return blockId;
		else
		{
			int iNodeId = getFreeINode();
			if (iNodeId < 0)
				return iNodeId;
			else
			{
				parent->dINode.fileSize = sizeof(dir);
				time_t now;
				now = time(NULL);
				parent->dINode.modifyTime = now;
				parent->dINode.addr[0] = blockId;
				writeINode(parent);		//����parent iNode�ڵ�
				direct *dt = new direct();	//�½�һ��Ŀ¼��
				strcpy(dt->name, name);
				dt->iNodeId = iNodeId;
				dINode *di = new dINode();	//�½�һ��dINode
				di->createTime = now;
				di->fileSize = 0;
				di->groupId = groupId;
				di->linkNum = 0;
				di->mod = 14;
				di->modifyTime = now;
				di->ownerId = ownerId;
				di->readTime = now;
				iNode i;
				i.dINode = *di;
				i.nodeId = iNodeId;
				i.parent = parent;
				i.users = 0;
				writeINode(&i);		//���´�����Ŀ¼��iNode�ڵ�
				dir	dd;
				dd.dirNum = 1;
				dd.direct[0] = *dt;
				writeDir(blockId, &dd);	//ָ��blockд��Ŀ¼�ṹ
				//delete dd;
				*d = dd;
				delete di;
				delete dt;
				return STATUS_OK;
			}
		}
	}
	else
		if (parent->dINode.fileSize != 0)
		{
			int iNodeId = getFreeINode();
			if (iNodeId < 0)
				return iNodeId;
			else
			{
				time_t now;
				now = time(NULL);
				parent->dINode.modifyTime = now;
				direct *dt = new direct();	//�½�һ��Ŀ¼��
				strcpy(dt->name, name);
				dt->iNodeId = iNodeId;
				dINode *di = new dINode();	//�½�һ��dINode
				di->createTime = now;
				di->fileSize = 0;
				di->groupId = groupId;
				di->linkNum = 0;
				di->mod = 14;
				di->modifyTime = now;
				di->ownerId = ownerId;
				di->readTime = now;
				iNode i;
				i.dINode = *di;
				i.nodeId = iNodeId;
				i.parent = parent;
				i.users = 0;
				writeINode(&i);
				dir dd;
				readDir(parent->dINode.addr[0], &dd);
				if (dd.dirNum < MAX_DIRECT_NUM)
				{
					dd.direct[dd.dirNum] = *dt;
					dd.dirNum++;
					writeDir(parent->dINode.addr[0], &dd);
					*d = dd;
					delete di;
					delete dt;
					return STATUS_OK;
				}
				else
					return STATUS_BEYOND_DIRECT_NUM;
			}
		}
	return STATUS_ERROR;
}

int chmod(char name[MAX_NAME_SIZE], unsigned int mod)	//�ı��ļ�Ȩ��
{
	for (int i = 0; i < d->dirNum; i++)
		if (strcmp(d->direct[i].name, name) == 0)
		{
			if (mod == 8 || mod == 9 || mod == 11 || mod == 13 || mod > 14)
				return STATUS_MOD_ERROR;
			else
			{
				iNode *temp = new iNode();
				temp->nodeId = d->direct[i].iNodeId;
				readINode(temp);
				if (mod < 8 && temp->dINode.mod < 8 && (temp->dINode.ownerId == curOwner->ownerId || curOwner->ownerId == ROOT))
				{
					temp->dINode.mod = mod;
					writeINode(temp);
					delete temp;
					return STATUS_OK;
				}
				else if (mod > 9 && temp->dINode.mod > 9 && (temp->dINode.ownerId == curOwner->ownerId || curOwner->ownerId == ROOT))
					{
						temp->dINode.mod = mod;
						writeINode(temp);
						delete temp;
						return STATUS_OK;
					}
				else if (temp->dINode.ownerId != curOwner->ownerId&&curOwner->ownerId != ROOT)
					{
						delete temp;
						return STATUS_BEYOND_RIGHT;
					}
				else
					{
						delete temp;
						return STATUS_MOD_ERROR;
					}
			}
		}
	return STATUS_FILENAME_NONEXIST;
}

int chown(char name[MAX_NAME_SIZE], unsigned short ownerId)	//�ı��ļ�ӵ����
{
	for (int i = 0; i < d->dirNum; i++)
		if (strcmp(d->direct[i].name, name) == 0)
		{
			for (int j = 0; j < os->ownerNum; j++)
				if (ownerId == os->os[j].ownerId)
				{
					iNode *temp = new iNode();
					temp->nodeId = d->direct[i].iNodeId;
					readINode(temp);
					if (curOwner->ownerId == temp->dINode.ownerId || curOwner->ownerId == ROOT)
					{
						temp->dINode.ownerId = ownerId;
						writeINode(temp);
						delete temp;
						return STATUS_OK;
					}
					else
					{
						delete temp;
						return STATUS_BEYOND_RIGHT;
					}
				}
			return STATUS_OWNER_NONEXIST;
		}
	return STATUS_FILENAME_NONEXIST;
}

int chgrp(char name[MAX_NAME_SIZE], unsigned short groupId)	//�ı��ļ�������
{
	for (int i = 0; i < d->dirNum; i++)
		if (strcmp(d->direct[i].name, name) == 0)
		{
			for (int j = 0; j < gs->groupNum; j++)
				if (groupId == gs->gs[j].groupId)
				{
					iNode *temp = new iNode();
					temp->nodeId = d->direct[i].iNodeId;
					readINode(temp);
					if (curOwner->ownerId == temp->dINode.ownerId || curOwner->ownerId == ROOT)
					{
						temp->dINode.groupId = groupId;
						writeINode(temp);
						delete temp;
						return STATUS_OK;
					}
					else
					{
						delete temp;
						return STATUS_BEYOND_RIGHT;
					}
				}
			return STATUS_GROUP_NONEXIST;
		}
	return STATUS_FILENAME_NONEXIST;
}

int passwd()	//�޸��û�����
{
	cout << "please input your old password:";
	char password[MAX_NAME_SIZE] = { 0 };
	int i = 0;
	while (i < MAX_NAME_SIZE)
	{
		password[i] = _getch();
		if (password[i] == 13)
		{
			password[i] = '\0';
			break;
		}
		i++;
	}
	cout << endl;
	if (strcmp(curOwner->ownerPassword, password) != 0)
		return STATUS_PASSWORD_WRONG;
	else
	{
		cout << "please input your new password:";
		char p1[MAX_NAME_SIZE] = { 0 };
		int i = 0;
		while (i < MAX_NAME_SIZE)
		{
			p1[i] = _getch();
			if (p1[i] == 13)
			{
				p1[i] = '\0';
				break;
			}
			i++;
		}
		cout << endl;
		cout << "please input confirm your new password:";
		char p2[MAX_NAME_SIZE] = { 0 };
		int j = 0;
		while (j < MAX_NAME_SIZE)
		{
			p2[j] = _getch();
			if (p2[j] == 13)
			{
				p2[j] = '\0';
				break;
			}
			j++;
		}
		cout << endl;
		if (strcmp(p1, p2) != 0)
			return STATUS_CONFIRM_WRONG;
		else
		{
			strcpy(curOwner->ownerPassword, p1);
			for (int i = 0; i < os->ownerNum; i++)
				if (os->os[i].ownerName == curOwner->ownerName)
				{
					strcpy(os->os[i].ownerPassword, p1);
					writeOS();
					return STATUS_OK;
				}
		}
	}
	return STATUS_ERROR;
}

bool writeOS()	//д���û���
{
	FILE *f = fopen(FILE_PATH, "rb+");
	if (f == NULL)
		return false;
	else
	{
		fseek(f, BLOCK_SIZE + sizeof(superBlock), SEEK_SET);
		fwrite(os, sizeof(owners), 1, f);
		fclose(f);
		return true;
	}
}

int mv(char oldName[MAX_NAME_SIZE], char newName[MAX_NAME_SIZE])	//�ı��ļ���
{
	for (int i = 0; i < d->dirNum; i++)
	{
		if (strcmp(d->direct[i].name, oldName) == 0)
		{
			iNode* temp = new iNode();
			temp->nodeId = d->direct[i].iNodeId;
			readINode(temp);
			if (temp->dINode.ownerId != curOwner->ownerId&&curOwner->ownerId != ROOT)
			{
				delete temp;
				return STATUS_BEYOND_RIGHT;
			}
			else
			{
				for (int j = 0; j < d->dirNum; j++)
					if (strcmp(d->direct[j].name, newName) == 0)
						return STATUS_FILENAME_EXIST;
				strcpy(d->direct[i].name, newName);
				writeDir(curINode->dINode.addr[0], d);
				delete temp;
				return STATUS_OK;
			}
		}
	}
	return STATUS_FILENAME_NONEXIST;
}

int touch(iNode* parent, char name[MAX_NAME_SIZE])	//�����ļ�
{
	bool exist = checkFileName(name);
	if (parent->dINode.fileSize == 0 && parent->dINode.mod != 12 && !exist && (parent->dINode.ownerId == curOwner->ownerId || curOwner->ownerId == ROOT))
	{
		int blockId = getFreeBlock();
		if (blockId < 0)
			return blockId;
		else
		{
			int iNodeId = getFreeINode();
			if (iNodeId < 0)
				return iNodeId;
			else
			{
				parent->dINode.fileSize = sizeof(dir);
				time_t now;
				now = time(NULL);
				parent->dINode.modifyTime = now;
				parent->dINode.addr[0] = blockId;
				writeINode(parent);		//����parent iNode�ڵ�
				direct *dt = new direct();	//�½�һ��Ŀ¼��
				strcpy(dt->name, name);
				dt->iNodeId = iNodeId;
				dINode *di = new dINode();	//�½�һ��dINode
				di->createTime = now;
				di->fileSize = 0;
				di->groupId = parent->dINode.groupId;
				di->linkNum = 0;
				di->mod = 6;
				di->modifyTime = now;
				di->ownerId = parent->dINode.ownerId;
				di->readTime = now;
				iNode i;
				i.dINode = *di;
				i.nodeId = iNodeId;
				i.parent = parent;
				i.users = parent->users;
				writeINode(&i);		//���´�����Ŀ¼��iNode�ڵ�
				dir	dd;
				dd.dirNum = 1;
				dd.direct[0] = *dt;
				writeDir(blockId, &dd);	//ָ��blockд��Ŀ¼�ṹ
				//delete dd;
				*d = dd;
				delete di;
				delete dt;
				return STATUS_OK;
			}
		}
	}
	else if (parent->dINode.fileSize != 0 && parent->dINode.mod != 12 && !exist && (parent->dINode.ownerId == curOwner->ownerId || curOwner->ownerId == ROOT))
		{
			int iNodeId = getFreeINode();
			if (iNodeId < 0)
				return iNodeId;
			else
			{
				time_t now;
				now = time(NULL);
				parent->dINode.modifyTime = now;
				direct *dt = new direct();	//�½�һ��Ŀ¼��
				strcpy(dt->name, name);
				dt->iNodeId = iNodeId;
				dINode *di = new dINode();	//�½�һ��dINode
				di->createTime = now;
				di->fileSize = 0;
				di->groupId = parent->dINode.groupId;
				di->linkNum = 0;
				di->mod = 6;
				di->modifyTime = now;
				di->ownerId = parent->dINode.ownerId;
				di->readTime = now;
				iNode i;
				i.dINode = *di;
				i.nodeId = iNodeId;
				i.parent = parent;
				i.users = parent->users;
				writeINode(&i);
				dir dd;
				readDir(parent->dINode.addr[0], &dd);
				if (dd.dirNum < MAX_DIRECT_NUM)
				{
					dd.direct[dd.dirNum] = *dt;
					dd.dirNum++;
					writeDir(parent->dINode.addr[0], &dd);
					*d = dd;

					delete di;
					delete dt;
					return STATUS_OK;
				}
				else
					return STATUS_BEYOND_DIRECT_NUM;
			}
		}
		else if (parent->dINode.ownerId != curOwner->ownerId&&curOwner->ownerId != ROOT)
			return STATUS_BEYOND_RIGHT;
		else if (parent->dINode.mod == 12)
			return STATUS_READ_ONLY;
		else if (exist)
			return STATUS_FILENAME_EXIST;
	return STATUS_ERROR;
}

int textAppend(char name[MAX_NAME_SIZE])	//�ı�����׷��
{
	for (int i = 0; i < d->dirNum; i++)
	{
		if (strcmp(d->direct[i].name, name) == 0)
		{
			iNode* temp = new iNode();
			temp->nodeId = d->direct[i].iNodeId;
			readINode(temp);
			if (temp->dINode.ownerId != curOwner->ownerId&&curOwner->ownerId != ROOT)
				return STATUS_BEYOND_RIGHT;
			else if (temp->dINode.mod > 7)
			{
				delete temp;
				return STATUS_NOT_FILE;
			}
			else if (temp->dINode.mod != 2 && temp->dINode.mod != 3 && temp->dINode.mod != 6 && temp->dINode.mod != 7)
			{
				delete temp;
				return STATUS_READ_ONLY;
			}
			else
			{
				if (temp->dINode.fileSize < FILE_MAX_SIZE)
				{
					string text;
					getline(cin, text, char(17));	//����ctrl+q��������
					if (text.size() + temp->dINode.fileSize > FILE_MAX_SIZE)
					{
						delete temp;
						return STATUS_BEYOND_SIZE;
					}
					else
					{
						int result = writeText(temp, text);
						writeINode(temp);
						delete temp;
						return result;
					}
				}
			}
		}
	}
	return STATUS_FILENAME_NONEXIST;
}

//int blocknum,blocknum2,blocknum3;

int writeText(iNode* temp, string text)	//�ļ�����׷�������
{
	FILE *f = fopen(FILE_PATH, "rb+");
	if (f == NULL)
		return STATUS_FILE_OPEN_ERROR;
	else
	{
		int as = temp->dINode.fileSize / BLOCK_SIZE;
		int ps = temp->dINode.fileSize % BLOCK_SIZE;//ԭ�����һ��������ֽ���
		int bs = text.size() / BLOCK_SIZE;//��д�Ķ��ٸ�����������
		int ls = text.size() % BLOCK_SIZE;//ʣ�µ��ǲ���
		int aa = as + bs;
		int pos = 0;	//�ı�����д��ָ��
		if (ps == 0)
		{
			for (int i = 0; i < bs; i++)
			{
				int blockId = getFreeBlock();
				if (blockId < 0)
				{
					fclose(f);
					return blockId;
				}
				else
				{
					if (as < 4) {
						temp->dINode.addr[as++] = blockId;
						fseek(f, BLOCK_SIZE*blockId, SEEK_SET);
						fwrite(text.substr(pos, BLOCK_SIZE).c_str(), BLOCK_SIZE, 1, f);
						temp->dINode.fileSize += BLOCK_SIZE;
						pos += BLOCK_SIZE;
					}
					else if (as == 4) {//��һ�ν���һ��
						temp->dINode.addr[as++] = blockId;
						fseek(f, BLOCK_SIZE*blockId, SEEK_SET);
						//int blocknum = blockId;
						int blockId1 = getFreeBlock();
						fwrite(&blockId1,sizeof(int),1,f);
						fseek(f, BLOCK_SIZE*blockId1, SEEK_SET);
						fwrite(text.substr(pos, BLOCK_SIZE).c_str(), BLOCK_SIZE, 1, f);
						temp->dINode.fileSize += BLOCK_SIZE;
						pos += BLOCK_SIZE;
					}
					else if (as >= 5&&as<=131) {//�������һ��
						fseek(f, BLOCK_SIZE*temp->dINode.addr[4]+ (as - 4) * sizeof(int), SEEK_SET);
						fwrite(&blockId, sizeof(int), 1, f);
						fseek(f, BLOCK_SIZE*blockId, SEEK_SET);
						fwrite(text.substr(pos, BLOCK_SIZE).c_str(), BLOCK_SIZE, 1, f);
						temp->dINode.fileSize += BLOCK_SIZE;
						pos += BLOCK_SIZE;
						as++;
					}
					else if (as == 132) {//��һ�ν������
						temp->dINode.addr[5] = blockId;
						//blocknum2 = blockId;
						fseek(f, BLOCK_SIZE*blockId, SEEK_SET);
						int blockId1 = getFreeBlock();
						fwrite(&blockId1, sizeof(int), 1, f);
						fseek(f, BLOCK_SIZE*blockId1, SEEK_SET);
						int blockId2 = getFreeBlock();
						fwrite(&blockId2, sizeof(int), 1, f);
						fseek(f, BLOCK_SIZE*blockId2, SEEK_SET);
						fwrite(text.substr(pos, BLOCK_SIZE).c_str(), BLOCK_SIZE, 1, f);
						temp->dINode.fileSize += BLOCK_SIZE;
						pos += BLOCK_SIZE;
						as++;//����Ϊ68096
					}
					//else if (as > 132) {

					//}
				}
			}
			if (ls > 0)//ʣ�µ��ǲ���
			{
				if (as < 4) {
					int blockId = getFreeBlock();
					if (blockId < 0)
					{
						fclose(f);
						return blockId;
					}
					else
					{
						temp->dINode.addr[as++] = blockId;
						fseek(f, BLOCK_SIZE*blockId, SEEK_SET);
						fwrite(text.substr(pos, ls).c_str(), BLOCK_SIZE, 1, f);
						temp->dINode.fileSize += ls;
						pos += ls;
					}
					fclose(f);
					return STATUS_OK;
				}
				else if (as == 4) {//4.x����
					int blockId = getFreeBlock();
					if (blockId < 0)
					{
						fclose(f);
						return blockId;
					}
					else {
						temp->dINode.addr[4] = blockId;
						fseek(f, BLOCK_SIZE*blockId, SEEK_SET);
						int blockId1 = getFreeBlock();
						fwrite(&blockId1, sizeof(int), 1, f);
						fseek(f, BLOCK_SIZE*blockId1, SEEK_SET);
						fwrite(text.substr(pos, ls).c_str(), BLOCK_SIZE, 1, f);
						temp->dINode.fileSize += ls;
						pos += ls;
					}
					fclose(f);
					return STATUS_OK;
				}
				else if (as >= 5&&as<=131) {//���������
					fseek(f, BLOCK_SIZE*temp->dINode.addr[4]+(as-4)*sizeof(int), SEEK_SET);
					int blockId1 = getFreeBlock();
					fwrite(&blockId1, sizeof(int), 1, f);
					fseek(f, BLOCK_SIZE*blockId1, SEEK_SET);
					fwrite(text.substr(pos, ls).c_str(), BLOCK_SIZE, 1, f);
					temp->dINode.fileSize += ls;
					pos += ls;
					fclose(f);
					return STATUS_OK;
				}
				else if (as==132) {
					int blockId = getFreeBlock();
					if (blockId < 0)
					{
						fclose(f);
						return blockId;
					}
					else
					{
						temp->dINode.addr[5] = blockId;
						//int blocknum2 = blockId;
						fseek(f, BLOCK_SIZE*temp->dINode.addr[5], SEEK_SET);
						int blockId1 = getFreeBlock();
						fwrite(&blockId1, sizeof(int), 1, f);
						fseek(f, BLOCK_SIZE*blockId1, SEEK_SET);
						int blockId2 = getFreeBlock();
						fwrite(&blockId2, sizeof(int), 1, f);
						fseek(f, BLOCK_SIZE*blockId2, SEEK_SET);
						fwrite(text.substr(pos, ls).c_str(), BLOCK_SIZE, 1, f);
						temp->dINode.fileSize += ls;
						pos += ls;
					}
					fclose(f);
					return STATUS_OK;
				}
			}
			fclose(f);
			return STATUS_OK;
		}
		else//���ﻹû���޸�
		{
			int lps = BLOCK_SIZE - ps;//����ǿ��л�ʣ�µĿ����ֽ���
			if (as < 4) 
			{//�������4��
				if (text.size() <= lps)
				{
					fseek(f, BLOCK_SIZE*temp->dINode.addr[as] + ps, SEEK_SET);
					fwrite(text.c_str(), text.size(), 1, f);
					temp->dINode.fileSize += text.size();
					fclose(f);
					return STATUS_OK;
				}
				else
				{
					fseek(f, BLOCK_SIZE*temp->dINode.addr[as++] + ps, SEEK_SET);
					//�Ȱѿ�ȱ�Ĳ���
					fwrite(text.c_str(), lps, 1, f);
					temp->dINode.fileSize += lps;
					int lts = text.size() - lps;
					int lbs = lts / BLOCK_SIZE;
					int lls = lts % BLOCK_SIZE;
					int tpos = lps;//

					for (int i = 0; i < lbs; i++)
					{
						int blockId = getFreeBlock();
						if (blockId < 0)
						{
							fclose(f);
							return blockId;
						}
						else
						{
							if (as < 4) {
								temp->dINode.addr[as++] = blockId;
								fseek(f, BLOCK_SIZE*blockId, SEEK_SET);
								fwrite(text.substr(tpos, BLOCK_SIZE).c_str(), BLOCK_SIZE, 1, f);
								temp->dINode.fileSize += BLOCK_SIZE;
								tpos += BLOCK_SIZE;
							}
							else if(as==4)
							{
								temp->dINode.addr[as++] = blockId;
								fseek(f, BLOCK_SIZE*blockId, SEEK_SET);
								//int blocknum = blockId;
								int blockId1 = getFreeBlock();
								fwrite(&blockId1, sizeof(int), 1, f);
								fseek(f, BLOCK_SIZE*blockId1, SEEK_SET);
								fwrite(text.substr(tpos, BLOCK_SIZE).c_str(), BLOCK_SIZE, 1, f);
								temp->dINode.fileSize += BLOCK_SIZE;
								tpos += BLOCK_SIZE;
							}
							else if(as>=5&&as<=131)
							{
								fseek(f, BLOCK_SIZE*temp->dINode.addr[4] + (as - 4) * sizeof(int), SEEK_SET);
								fwrite(&blockId, sizeof(int), 1, f);
								fseek(f, BLOCK_SIZE*blockId, SEEK_SET);
								fwrite(text.substr(tpos, BLOCK_SIZE).c_str(), BLOCK_SIZE, 1, f);
								temp->dINode.fileSize += BLOCK_SIZE;
								tpos += BLOCK_SIZE;
								as++;
							}
							else if (as==132) {
								temp->dINode.addr[5] = blockId;
								//blocknum2 = blockId;
								fseek(f, BLOCK_SIZE*blockId, SEEK_SET);
								int blockId1 = getFreeBlock();
								fwrite(&blockId1, sizeof(int), 1, f);
								fseek(f, BLOCK_SIZE*blockId1, SEEK_SET);
								int blockId2 = getFreeBlock();
								fwrite(&blockId2, sizeof(int), 1, f);
								fseek(f, BLOCK_SIZE*blockId2, SEEK_SET);
								fwrite(text.substr(tpos, BLOCK_SIZE).c_str(), BLOCK_SIZE, 1, f);
								temp->dINode.fileSize += BLOCK_SIZE;
								tpos += BLOCK_SIZE;
								as++;//����Ϊ68096
							}
						}
					}
					if (lls > 0)
					{
						int blockId = getFreeBlock();
						if (blockId < 0)
						{
							fclose(f);
							return blockId;
						}
						else
						{
							if (as<4)
							{
								temp->dINode.addr[as++] = blockId;
								fseek(f, BLOCK_SIZE*blockId, SEEK_SET);
								fwrite(text.substr(tpos, lls).c_str(), BLOCK_SIZE, 1, f);
								temp->dINode.fileSize += lls;
								tpos += lls;
							}
							else if (as==4) {
								temp->dINode.addr[4] = blockId;
								fseek(f, BLOCK_SIZE*blockId, SEEK_SET);
								int blockId1 = getFreeBlock();
								fwrite(&blockId1, sizeof(int), 1, f);
								fseek(f, BLOCK_SIZE*blockId1, SEEK_SET);
								fwrite(text.substr(tpos, lls).c_str(), BLOCK_SIZE, 1, f);
								temp->dINode.fileSize += lls;
								tpos += lls;
							}
							else if(as>=5&&as<=131)
							{
								fseek(f, BLOCK_SIZE*temp->dINode.addr[4] + (as - 4) * sizeof(int), SEEK_SET);
								//int blockId1 = getFreeBlock();
								fwrite(&blockId, sizeof(int), 1, f);
								fseek(f, BLOCK_SIZE*blockId, SEEK_SET);
								fwrite(text.substr(tpos, lls).c_str(), BLOCK_SIZE, 1, f);
								temp->dINode.fileSize += lls;
								tpos += lls;
								fclose(f);
								return STATUS_OK;
							}
							else if(as==132)
							{
								temp->dINode.addr[5] = blockId;
								//int blocknum2 = blockId;
								fseek(f, BLOCK_SIZE*temp->dINode.addr[5], SEEK_SET);
								int blockId1 = getFreeBlock();
								fwrite(&blockId1, sizeof(int), 1, f);
								fseek(f, BLOCK_SIZE*blockId1, SEEK_SET);
								int blockId2 = getFreeBlock();
								fwrite(&blockId2, sizeof(int), 1, f);
								fseek(f, BLOCK_SIZE*blockId2, SEEK_SET);
								fwrite(text.substr(tpos, lls).c_str(), BLOCK_SIZE, 1, f);
								temp->dINode.fileSize += lls;
								tpos += lls;
							}
						}
						fclose(f);
						return STATUS_OK;
					}
					fclose(f);
					return STATUS_OK;
				}
			}
			else if (as<=131&&as>=4) {
				if (text.size() <= lps)
				{
					fseek(f, BLOCK_SIZE*temp->dINode.addr[4]+(as-4)*sizeof(int), SEEK_SET);
					int blockId;
					fread(&blockId,sizeof(int),1,f);
					int blockId1 = (int)blockId;//�е�����,�����ܲ��ܶ�?
					fseek(f, BLOCK_SIZE*blockId1+ps, SEEK_SET);
					fwrite(text.c_str(), text.size(), 1, f);
					temp->dINode.fileSize += text.size();
					fclose(f);
					return STATUS_OK;
				}
				else 
				{
					fseek(f, BLOCK_SIZE*temp->dINode.addr[4]+sizeof(int)*(as-4), SEEK_SET);
					as++;
					//�Ȱѿ�ȱ�Ĳ���
					int blockId;
					fread(&blockId, sizeof(int), 1, f);
					int blockId1 = (int)blockId;//�е�����,�����ܲ��ܶ�?
					fseek(f, BLOCK_SIZE*blockId1 + ps, SEEK_SET);
					fwrite(text.c_str(), lps, 1, f);
					temp->dINode.fileSize += lps;
					int lts = text.size() - lps;
					int lbs = lts / BLOCK_SIZE;
					int lls = lts % BLOCK_SIZE;
					int tpos = lps;//

					for (int i = 0; i < lbs; i++)
					{
						int blockId = getFreeBlock();
						if (blockId < 0)
						{
							fclose(f);
							return blockId;
						}
						else
						{
							if (as >= 5 && as <= 131)
							{
								fseek(f, BLOCK_SIZE*temp->dINode.addr[4] + (as - 4) * sizeof(int), SEEK_SET);
								fwrite(&blockId, sizeof(int), 1, f);
								fseek(f, BLOCK_SIZE*blockId, SEEK_SET);
								fwrite(text.substr(tpos, BLOCK_SIZE).c_str(), BLOCK_SIZE, 1, f);
								temp->dINode.fileSize += BLOCK_SIZE;
								tpos += BLOCK_SIZE;
								as++;
							}
							else if (as == 132) {
								temp->dINode.addr[5] = blockId;
								//blocknum2 = blockId;
								fseek(f, BLOCK_SIZE*blockId, SEEK_SET);
								int blockId1 = getFreeBlock();
								fwrite(&blockId1, sizeof(int), 1, f);
								fseek(f, BLOCK_SIZE*blockId1, SEEK_SET);
								int blockId2 = getFreeBlock();
								fwrite(&blockId2, sizeof(int), 1, f);
								fseek(f, BLOCK_SIZE*blockId2, SEEK_SET);
								fwrite(text.substr(tpos, BLOCK_SIZE).c_str(), BLOCK_SIZE, 1, f);
								temp->dINode.fileSize += BLOCK_SIZE;
								tpos += BLOCK_SIZE;
								as++;//����Ϊ68096
							}
						}
					}
					if (lls > 0)
					{
						int blockId = getFreeBlock();
						if (blockId < 0)
						{
							fclose(f);
							return blockId;
						}
						else
						{
							if (as >= 5 && as <= 131)
							{
								fseek(f, BLOCK_SIZE*temp->dINode.addr[4] + (as - 4) * sizeof(int), SEEK_SET);
								//int blockId1 = getFreeBlock();
								fwrite(&blockId, sizeof(int), 1, f);
								fseek(f, BLOCK_SIZE*blockId, SEEK_SET);
								fwrite(text.substr(tpos, lls).c_str(), BLOCK_SIZE, 1, f);
								temp->dINode.fileSize += lls;
								tpos += lls;
								fclose(f);
								return STATUS_OK;
							}
							else if (as == 132)
							{
								temp->dINode.addr[5] = blockId;
								//int blocknum2 = blockId;
								fseek(f, BLOCK_SIZE*temp->dINode.addr[5], SEEK_SET);
								int blockId1 = getFreeBlock();
								fwrite(&blockId1, sizeof(int), 1, f);
								fseek(f, BLOCK_SIZE*blockId1, SEEK_SET);
								int blockId2 = getFreeBlock();
								fwrite(&blockId2, sizeof(int), 1, f);
								fseek(f, BLOCK_SIZE*blockId2, SEEK_SET);
								fwrite(text.substr(tpos, lls).c_str(), BLOCK_SIZE, 1, f);
								temp->dINode.fileSize += lls;
								tpos += lls;
							}
						}
						fclose(f);
						return STATUS_OK;
					}
				}
			}
			else if (as==132) //��ûд�꣡��������������������
			{
					fseek(f, BLOCK_SIZE*temp->dINode.addr[5], SEEK_SET);
					int blockId;
					fread(&blockId, sizeof(int), 1, f);
					int blockId1 = (int)blockId;
					fseek(f, BLOCK_SIZE*blockId1 , SEEK_SET);
					int blockId2;
					fread(&blockId2, sizeof(int), 1, f);
					fseek(f, BLOCK_SIZE*blockId2, SEEK_SET);
					fwrite(text.c_str(), text.size(), 1, f);
					temp->dINode.fileSize += text.size();
					fclose(f);
					return STATUS_OK;
			}
		}
	}
}

int cat(char name[MAX_NAME_SIZE])	//������ʾ�ļ�����
{
	for (int i = 0; i < d->dirNum; i++)
		if (strcmp(d->direct[i].name, name) == 0)
		{
			iNode *temp = new iNode();
			temp->nodeId = d->direct[i].iNodeId;
			readINode(temp);
			if (temp->dINode.ownerId != curOwner->ownerId&&curOwner->ownerId != ROOT)
				return STATUS_BEYOND_RIGHT;
			else if (temp->dINode.mod > 7)
				return STATUS_NOT_FILE;
			else if (temp->dINode.mod < 4)
				return STATUS_WRITE_ONLY;
			else
			{
				int result = readText(temp);
				delete temp;
				return result;
			}
		}
	return STATUS_FILENAME_NONEXIST;
}

int readText(iNode *temp)	//��ȡ�ļ�����
{
	FILE *f = fopen(FILE_PATH, "rb");
	if (f == NULL)
		return STATUS_FILE_OPEN_ERROR;
	else
	{
		int as = temp->dINode.fileSize / BLOCK_SIZE;
		int ls = temp->dINode.fileSize % BLOCK_SIZE;
		char content[BLOCK_SIZE+1];
		for (int i = 0; i < as; i++)
		{
			memset(content,'\0',sizeof(content));
			if (i < 4) {
				fseek(f, BLOCK_SIZE*temp->dINode.addr[i], SEEK_SET);
				fread(content, BLOCK_SIZE, 1, f);
				cout << content;
			}
			else if (i >= 4&&i<=132) {
				fseek(f, BLOCK_SIZE*temp->dINode.addr[4]+(i-4)*sizeof(int), SEEK_SET);
				int blockId;
				fread(&blockId,sizeof(int),1,f);
				fseek(f, BLOCK_SIZE*blockId, SEEK_SET);
				fread(content, BLOCK_SIZE, 1, f);
				cout << content;
				cout << "-------------test1----------------:" <<as<< endl;
			}
			else if(as==133)
			{
				fseek(f, BLOCK_SIZE*temp->dINode.addr[5] , SEEK_SET);
				int blockId;
				fread(&blockId, sizeof(int), 1, f);
				fseek(f, BLOCK_SIZE*blockId, SEEK_SET);
				int blockId1;
				fread(&blockId1, sizeof(int), 1, f);
				fseek(f, BLOCK_SIZE*blockId1, SEEK_SET);
				fread(content, BLOCK_SIZE, 1, f);
				cout << content;
				cout << "hello world" << endl;
			}
		}
		if (ls > 0)
		{
			if (as < 4) {
				fseek(f, BLOCK_SIZE*temp->dINode.addr[as], SEEK_SET);
				fread(content, ls, 1, f);
				for (int i = 0; i < ls; i++)
					cout << content[i];
			}
			else if(as>=4&&as<=131)
			{
				fseek(f, BLOCK_SIZE*temp->dINode.addr[4]+(as-4)*sizeof(int), SEEK_SET);
				int blockId;
				fread(&blockId, sizeof(int), 1, f);
				fseek(f, BLOCK_SIZE*blockId, SEEK_SET);
				fread(content, ls, 1, f);
				for (int i = 0; i < ls; i++)
					cout << content[i];
			}
			else if(as==132)
			{
				fseek(f, BLOCK_SIZE*temp->dINode.addr[4] + (as - 4) * sizeof(int), SEEK_SET);
				int blockId;
				fread(&blockId, sizeof(int), 1, f);
				fseek(f, BLOCK_SIZE*blockId, SEEK_SET);
				int blockId1;
				fread(&blockId1, sizeof(int), 1, f);
				fseek(f, BLOCK_SIZE*blockId1, SEEK_SET);
				fread(content, ls, 1, f);
				for (int i = 0; i < ls; i++)
					cout << content[i];
				cout << "hello" << endl;
			}
		}
		fclose(f);
		return STATUS_OK;
	}
}

int rm(char name[MAX_NAME_SIZE])	//ɾ���ļ�
{
	for (int i = 0; i < d->dirNum; i++)
		if (strcmp(d->direct[i].name, name) == 0)
		{
			iNode *temp = new iNode();
			temp->nodeId = d->direct[i].iNodeId;
			readINode(temp);
			if (temp->dINode.mod > 7)
			{
				delete temp;
				return STATUS_NOT_FILE;
			}
			else if (temp->dINode.ownerId != curOwner->ownerId&&curOwner->ownerId != ROOT)
			{
				delete temp;
				return STATUS_BEYOND_RIGHT;
			}
			else if (temp->dINode.linkNum > 0)
			{
				temp->dINode.linkNum--;
				writeINode(temp);
				dir* td = new dir();
				td->dirNum = d->dirNum - 1;
				int k = 0;
				for (int j = 0; j < d->dirNum; j++)
					if (j != i)
						td->direct[k++] = d->direct[j];
						writeDir(curINode->dINode.addr[0], td);
						dir* dd = d;
						d = td;
						delete temp;
						delete dd;
						return STATUS_OK;
			}
			else
			{
				int bs = temp->dINode.fileSize / BLOCK_SIZE;
				int ls = temp->dINode.fileSize%BLOCK_SIZE;
				if (temp->dINode.fileSize == 0)
				{
					returnFreeINode(temp->nodeId);
					dir* td = new dir();
					td->dirNum = d->dirNum - 1;
					int k = 0;
					for (int j = 0; j < d->dirNum; j++)
					if (j != i)
						td->direct[k++] = d->direct[j];
					writeDir(curINode->dINode.addr[0], td);
					dir* dd = d;
					d = td;
					delete temp;
					delete dd;
					return STATUS_OK;
				}
				else
				{
					for (int ii = 0; ii < (ls > 0 ? bs + 1 : bs); ii++)
						returnFreeBlock(temp->dINode.addr[ii]);
					returnFreeINode(temp->nodeId);
					dir* td = new dir();
					td->dirNum = d->dirNum - 1;
					int k = 0;
					for (int j = 0; j < d->dirNum; j++)
					if (j != i)
						td->direct[k++] = d->direct[j];
					writeDir(curINode->dINode.addr[0], td);
					dir* dd = d;
					d = td;
					delete temp;
					delete dd;
					return STATUS_OK;
				}
			}
		}
	return STATUS_FILENAME_NONEXIST;
}

int returnFreeBlock(unsigned int blockId)	//�ͷ��̿�
{
	if (sp->nextFreeBlock <= 18)
	{
		sp->freeBlockNum++;
		sp->nextFreeBlock++;
		sp->freeBlock[sp->nextFreeBlock] = blockId;
		writeSuperBlock();
		return STATUS_OK;
	}
	else
	{
		FILE *f = fopen(FILE_PATH, "rb+");
		if (f == NULL)
			return STATUS_FILE_OPEN_ERROR;
		else
		{
			unsigned int blocksNum = BLOCK_GROUP_SIZE;
			fseek(f, BLOCK_SIZE*(sp->freeBlock[0] - BLOCK_GROUP_SIZE * 2), SEEK_SET);
			fwrite(&blocksNum, sizeof(unsigned int), 1, f);
			fwrite(&sp->freeBlock, sizeof(sp->freeBlock), 1, f);
			fclose(f);
			sp->freeBlockNum += 2;
			sp->freeBlock[0] = sp->freeBlock[0] - BLOCK_GROUP_SIZE * 2;
			sp->freeBlock[1] = blockId;
			sp->nextFreeBlock = 1;
			writeSuperBlock();
			return STATUS_OK;
		}
	}
}

int returnFreeINode(unsigned int iNodeId)	//�ͷ�iNode
{
	sp->freeINodeNum++;
	sp->nextFreeINode++;
	sp->freeINode[sp->nextFreeINode] = iNodeId;
	writeSuperBlock();
	return STATUS_OK;
}

int ln(char source[MAX_NAME_SIZE], char des[MAX_NAME_SIZE])	//�����ļ�����
{
	if (strcmp(source, des) == 0)
		return STATUS_SDNAME_OVERLAP;
	else
		for (int i = 0; i < d->dirNum; i++)
			if (strcmp(d->direct[i].name, source) == 0)
			{
				iNode *temp = new iNode();
				temp->nodeId = d->direct[i].iNodeId;
				readINode(temp);
				if (temp->dINode.ownerId != curOwner->ownerId&&curOwner->ownerId != ROOT)
				{
					delete temp;
					return STATUS_BEYOND_RIGHT;
				}
				else
				{
					for (int j = 0; j < d->dirNum; j++)
						if (strcmp(d->direct[j].name, des) == 0)
						{
							iNode* t = new iNode();
							t->nodeId = d->direct[j].iNodeId;
							readINode(t);
							if (t->dINode.ownerId != curOwner->ownerId&&curOwner->ownerId != ROOT)
							{
								delete t;
								delete temp;
								return STATUS_BEYOND_RIGHT;
							}
							if (t->dINode.mod < 8 && temp->dINode.mod>7 || t->dINode.mod > 7 && temp->dINode.mod < 8)
							{
								delete t;
								delete temp;
								return STATUS_TYPE_NOT_MATCH;
							}
							rmIter(d->direct[j].iNodeId);
							d->direct[j].iNodeId = d->direct[i].iNodeId;
							writeDir(curINode->dINode.addr[0], d);
							delete t;
							delete temp;
							return STATUS_OK;
						}
					direct dt;
					dt.iNodeId = d->direct[i].iNodeId;
					strcpy(dt.name, des);
					d->direct[d->dirNum] = dt;
					d->dirNum++;
					writeDir(curINode->dINode.addr[0], d);
					delete temp;
					return STATUS_OK;
				}
			}
	return STATUS_FILENAME_NONEXIST;
}

int rmdir(char name[MAX_NAME_SIZE])	//ɾ����Ŀ¼
{
	for (int i = 0; i < d->dirNum; i++)
		if (strcmp(d->direct[i].name, name) == 0)
		{
			iNode *temp = new iNode();
			temp->nodeId = d->direct[i].iNodeId;
			readINode(temp);
			if (temp->dINode.mod < 8)
			{
				delete temp;
				return STATUS_NOT_DIRECT;
			}
			else if (temp->dINode.ownerId != curOwner->ownerId&&curOwner->ownerId != ROOT)
			{
				delete temp;
				return STATUS_BEYOND_RIGHT;
			}
			else
			{
				rmIter(d->direct[i].iNodeId);
				dir* td = new dir();
				td->dirNum = d->dirNum - 1;
				int k = 0;
				for (int j = 0; j < d->dirNum; j++)
					if (j != i)
					td->direct[k++] = d->direct[j];
				writeDir(curINode->dINode.addr[0], td);
				dir* dd = d;
				d = td;
				delete temp;
				delete dd;
				return STATUS_OK;
			}
		}
	return STATUS_FILENAME_NONEXIST;
}

void rmIter(unsigned short iNodeId)	//����ɾ��
{
	iNode* temp = new iNode();
	temp->nodeId = iNodeId;
	readINode(temp);
	if (temp->dINode.mod < 8)
	{
		if (temp->dINode.linkNum > 0)
		{
			temp->dINode.linkNum--;
			writeINode(temp);
			delete temp;
		}
		else
		{
			returnFreeINode(temp->nodeId);
			if (temp->dINode.fileSize != 0)
			{
				int bs = temp->dINode.fileSize / BLOCK_SIZE;
				int ls = temp->dINode.fileSize%BLOCK_SIZE;
				for (int i = 0; i < (ls > 0 ? bs + 1 : bs); i++)
					returnFreeBlock(temp->dINode.addr[i]);
				delete temp;
			}
		}
	}
	else
	{
		if (temp->dINode.linkNum > 0)
		{
			temp->dINode.linkNum--;
			writeINode(temp);
			delete temp;
		}
		else
		{
			returnFreeINode(temp->nodeId);
			if (temp->dINode.fileSize != 0)
			{
				dir *dd = new dir();
				readDir(temp->dINode.addr[0], dd);
				for (int i = 0; i < dd->dirNum; i++)
					rmIter(dd->direct[i].iNodeId);
				returnFreeBlock(temp->dINode.addr[0]);
				delete dd;
				delete temp;
			}
		}
	}
}


int cp(char source[MAX_NAME_SIZE], char des[MAX_NAME_SIZE])		//�ļ�����
{
	if (strcmp(source, des) == 0)
		return STATUS_SDNAME_OVERLAP;
	else
		for (int i = 0; i < d->dirNum; i++)
			if (strcmp(d->direct[i].name, source) == 0)
			{
				iNode *temp = new iNode();
				temp->nodeId = d->direct[i].iNodeId;
				readINode(temp);
				if (temp->dINode.mod > 7)//�����ļ�����Ŀ¼��
				{
					delete temp;
					return STATUS_NOT_FILE;
				}
				else if (temp->dINode.mod < 4)
				{
					delete temp;
					return STATUS_READ_ONLY;
				}
				else if (temp->dINode.ownerId != curOwner->ownerId&&temp->dINode.ownerId != ROOT)
				{
					delete temp;
					return STATUS_BEYOND_RIGHT;
				}
				else
				{
					for (int j = 0; j < d->dirNum; j++)
						if (strcmp(d->direct[j].name, des) == 0)
						{
							int r1 = rm(des);
							if (r1 != STATUS_OK)
							{
								delete temp;
								return r1;
							}
						}
						int r2 = touch(curINode, des);
						if (r2 != STATUS_OK)
						{
							delete temp;
							return r2;
						}
						for (int k = 0; k < d->dirNum; k++)
							if (strcmp(d->direct[k].name, des) == 0)
							{
								iNode *t = new iNode();
								t->nodeId = d->direct[k].iNodeId;
								readINode(t);
								string text = getText(temp);
								writeText(t, text);
								writeINode(t);
								delete t;
								delete temp;
								return STATUS_OK;
							}
				}
			}
	return STATUS_FILENAME_NONEXIST;
}


int cp1(char source[MAX_NAME_SIZE], char des[MAX_NAME_SIZE])		//�ļ��п���
{
	if (strcmp(source, des) == 0)
		return STATUS_SDNAME_OVERLAP;
	else
		for (int i = 0; i < d->dirNum; i++)
			if (strcmp(d->direct[i].name, source) == 0)
			{
				iNode *temp = new iNode();
				temp->nodeId = d->direct[i].iNodeId;
				readINode(temp);
				if (temp->dINode.mod > 7)//�����ļ�����Ŀ¼��
				{
					int r1=mkdir(temp,des);
					if (r1 != STATUS_OK) {
						delete temp;
						return r1;
					}
					//cd()
					//delete temp;
					//return STATUS_NOT_FILE;
				}
				else if (temp->dINode.mod < 4)
				{
					delete temp;
					return STATUS_READ_ONLY;
				}
				else if (temp->dINode.ownerId != curOwner->ownerId&&temp->dINode.ownerId != ROOT)
				{
					delete temp;
					return STATUS_BEYOND_RIGHT;
				}
				else
				{
					for (int j = 0; j < d->dirNum; j++)
						if (strcmp(d->direct[j].name, des) == 0)
						{
							int r1 = rm(des);
							if (r1 != STATUS_OK)
							{
								delete temp;
								return r1;
							}
						}
					int r2 = touch(curINode, des);
					if (r2 != STATUS_OK)
					{
						delete temp;
						return r2;
					}
					for (int k = 0; k < d->dirNum; k++)
						if (strcmp(d->direct[k].name, des) == 0)
						{
							iNode *t = new iNode();
							t->nodeId = d->direct[k].iNodeId;
							readINode(t);
							string text = getText(temp);
							writeText(t, text);
							writeINode(t);
							delete t;
							delete temp;
							return STATUS_OK;
						}
				}
			}
	return STATUS_FILENAME_NONEXIST;
}
















string getText(iNode* temp)		//��ȡԴ�ļ�����
{
	string text;
	FILE *f = fopen(FILE_PATH, "rb");
	if (f == NULL)
		return "";
	else
	{
		int as = temp->dINode.fileSize / BLOCK_SIZE;
		int ls = temp->dINode.fileSize % BLOCK_SIZE;
		char content[BLOCK_SIZE];
		for (int i = 0; i < as; i++)
		{
			fseek(f, BLOCK_SIZE*temp->dINode.addr[i], SEEK_SET);
			fread(content, BLOCK_SIZE, 1, f);
			text += content;
		}
		if (ls > 0)
		{
			fseek(f, BLOCK_SIZE*temp->dINode.addr[as], SEEK_SET);
			fread(content, ls, 1, f);
			for (int i = 0; i < ls; i++)
				text += content[i];
		}
		fclose(f);
		return text;
	}
}




