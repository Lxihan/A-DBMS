#include "stdio.h"
#include "stdlib.h"
#include <bits/stdc++.h>
#include "string.h"
#include "conio.h"
#include "windows.h"
#include <iostream>
#include "power.h"

using namespace std;

#define MAX_PROLEN 10	//��������ַ�����
#define MAX_PRONUM 8	//������Ը���
#define MAX_RECNUM 30	//����¼����
#define MAX_TNAME 15	//�����
#define MAX_TNUM 15		//�������
#define MAX_VIEWNUM 15		//�������
#define MAX_USERLEN 10	//�û�������
#define MAX_PWDLEN 18	//������󳤶�
#define MAX_USERNUM 10	//�û���������

//***************************
typedef struct
{
	int re_num;		//��¼�ĸ���
	int pro_num;	//���Ը���
	int key_no;		//�涨�ؼ����ǵڼ�������,ʹ�õ����±��0��ʼ
	//int pro_len[MAX_PRONUM];	//���Գ���
	char key[MAX_PROLEN];		//����ؼ���,�ؼ���Ҳ��һ������
	char tname[MAX_TNAME];		//����
	char proname[MAX_PRONUM][MAX_PROLEN];	//���Ե�����
	char protype[MAX_PRONUM];	//���Ե�����
	char record[MAX_RECNUM][MAX_PRONUM][MAX_PROLEN];		//��ά��,����������Ǽ�¼��ֵ
}table;

typedef struct
{
	char proname[MAX_PRONUM][MAX_PROLEN];	//�ɼ�������
	int prono[MAX_PRONUM];		//�ɼ����Ե��±�
	char tname[MAX_TNAME];		//��Ӧ�ı���
	char vname[MAX_TNAME];		//��ͼ��
	int pro_num;
}view;		//��ͼ������ͼ��Ӧһ����һ������Զ�Ӧ�����ͼ

typedef struct
{
	char iname[MAX_TNAME];		//һ��������Ӧһ����
	int key_num;
	char key[MAX_RECNUM][MAX_PROLEN];	//
}index;
typedef struct
{
	char username[MAX_USERLEN];
	char pwd[MAX_PWDLEN];
	int revoke;		//0��glz��1��nb
}user;

table NULLTABLE;	//�ձ���ʼ����ʱ����NULLTABLE.key_no=-1
table t[MAX_TNUM];
view vi[MAX_VIEWNUM];
index ind[MAX_TNUM];

user u[MAX_USERNUM];
FILE *alfp;

int t_num=0;
int i_num=0;
int v_num=0;
int u_num=0;

int CURRENTREVOKE;
int speedfactor=10;

bool dbislock()
{
	int databaselock;
	FILE *fp;
	if((fp=fopen("dblock.db","rb"))==NULL)
	{
		printf("���������ļ�,�������ʹ��.\a");
		fclose(fp);
		exit(-1);
	}
	fread(&databaselock,sizeof(int),1,fp);
	fclose(fp);
	if(databaselock==1)
		return true;
	else
		return false;
}
void dblock()
{
	int databaselock=1;
	FILE *fp;
	if((fp=fopen("dblock.db","wb"))==NULL)
	{
		printf("���������ļ�,�������ʹ��.\a");
		fclose(fp);
		exit(-1);
	}
	fwrite(&databaselock,sizeof(int),1,fp);
	fclose(fp);
}
void dbunlock()
{
	int databaselock=0;
	FILE *fp;
	if((fp=fopen("dblock.db","wb"))==NULL)
	{
		printf("���������ļ�,�������ʹ��.\a");
		fclose(fp);
		exit(-1);
	}
	fwrite(&databaselock,sizeof(int),1,fp);
	fclose(fp);
}
void BackUp()
{
	if(CURRENTREVOKE==0)
	{
		gotoxy(1,24);printf("\a�Բ���,����Ȩ�޲������������.�����������...");
		getch();
		return;
	}
	if(!dbislock())		//û�������Ǿͼ���@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		dblock();	//dblock();     dbunlock();
	else
	{
		clrscr();
		printf("���ݿ⴦����״̬����ʹ�����û�Ҳ���ܽ����޸�!\a");
		printf("\n�����������...");
		getch();
		return;
	}
	char filename[20];
	clrscr();
	PrintColorStringXY(36,7,0,14,0,15,"���ݵı���");
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("���뱸�����ݵ��ļ���:");
	gotoxy(27,10);
	gets(filename);
	FILE *fp;
	if((fp=fopen(filename,"wb"))==NULL)
		fp=fopen(filename,"wb+");
	//���¹�6���ļ��ṩȫ��������dblock.db,table.db,index.db,view.db,dic.db,log.txt
//*****************��dblock.db�ı���********************************************
	int databaselock;
	if(dbislock())
		databaselock=1;
	else
		databaselock=0;
	fwrite(&databaselock,sizeof(int),1,fp);
	PrintStringXY(1,24,"����dblock.db\a..   ");
	Sleep(300);
//******************��table.db�ı���********************************************
	fwrite(&t_num,sizeof(int),1,fp);
	fwrite(t,sizeof(table),MAX_TNUM,fp);
	PrintStringXY(1,24,"����table.db\a..   ");
	Sleep(300);
//******************��index.db�ı���********************************************
	fwrite(&i_num,sizeof(int),1,fp);
	fwrite(ind,sizeof(index),MAX_TNUM,fp);
	PrintStringXY(1,24,"����index.db\a..   ");
	Sleep(300);
//******************��view.db�ı���**********************************************
	fwrite(&v_num,sizeof(int),1,fp);
	fwrite(vi,sizeof(view),MAX_VIEWNUM,fp);
	PrintStringXY(1,24,"����view.db\a..   ");
	Sleep(300);
//******************��dic.db�ı���***********************************************
	//dic.db�������ֵ䣬����Ҫ���ݣ�ֻ���ڻָ���ʱ���table.db�зֽ��������.
//******************��log.txt�ı���***********************************************
	FILE *fp1;
	char ch;
	if((fp1=fopen("log.txt","rb"))==NULL)
	{
		printf("��������־�ļ�log.txt,���α���δ��ȫ�ɹ�.\a");
		fp1=fopen("log.txt","wb+");
		fclose(fp1);
		fclose(fp);
		if(dbislock())
			dbunlock();
		return;
	}
	ch=fgetc(fp1);
	while(ch!=EOF)
	{
		fputc(ch,fp);
		ch=fgetc(fp1);

	}
	fclose(fp1);
	fclose(fp);
	PrintStringXY(1,24,"����log.txt\a..   ");
	Sleep(300);
	PrintStringXY(1,24,"��ϲ�����������ѳɹ�������");
	printf("%s",filename);
	printf("\n�����������...");
	getch();
	if(dbislock())
		dbunlock();
}

void ReCovery()
{
	//��ĳ��ָ���ı��ݽ��лָ�
		if(CURRENTREVOKE==0)
	{
		gotoxy(1,24);printf("\a�Բ���,����Ȩ�޲������������.�����������...");
		getch();
		return;
	}
	if(!dbislock())		//û�������Ǿͼ���@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		dblock();	//dblock();     dbunlock();
	else
	{
		clrscr();
		printf("���ݿ⴦����״̬����ʹ�����û�Ҳ���ܽ����޸�!\a");
		printf("\n�����������...");
		getch();
		return;
	}
	char filename[20];
	clrscr();
	PrintColorStringXY(36,7,0,14,0,15,"���ݵĻָ�");
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("����ָ����ݵ��ļ���:");
	gotoxy(27,10);
	gets(filename);
	FILE *fp;
	FILE *fp2;
	if((fp=fopen(filename,"rb"))==NULL)
	{
		PrintStringXY(1,24,"δ���ִ˱����ļ�!�����������...\a");
		getch();
		if(dbislock())
		dbunlock();
		return;
	}
	//�����ļ��ṩȫ���ȡdblock.db,table.db,index.db,view.db,log.txt
//*****************��dblock.db�Ļָ�********************************************
	int databaselock;
	fread(&databaselock,sizeof(int),1,fp);
	if((fp2=fopen("dblock.db","wb"))==NULL)
		fp2=fopen("dblock.db","wb+");
	fwrite(&databaselock,sizeof(int),1,fp2);
	fclose(fp2);
	PrintStringXY(1,24,"�ָ�dblock.db\a..   ");
	Sleep(300);
//******************��table.db�Ļָ�********************************************
	fread(&t_num,sizeof(int),1,fp);
	fread(t,sizeof(table),MAX_TNUM,fp);
	if((fp2=fopen("table.db","wb"))==NULL)
		fp2=fopen("table.db","wb+");
	fwrite(&t_num,sizeof(int),1,fp2);
	fwrite(t,sizeof(table),MAX_TNUM,fp2);
	fclose(fp2);
	PrintStringXY(1,24,"�ָ�table.db\a..   ");
	Sleep(300);
//******************��index.db�Ļָ�********************************************
	fread(&i_num,sizeof(int),1,fp);
	fread(ind,sizeof(index),MAX_TNUM,fp);
	if((fp2=fopen("index.db","wb"))==NULL)
		fp2=fopen("index.db","wb+");
	fwrite(&i_num,sizeof(int),1,fp2);
	fwrite(ind,sizeof(index),MAX_TNUM,fp2);
	fclose(fp2);
	PrintStringXY(1,24,"�ָ�index.db\a..   ");
	Sleep(300);
//******************��view.db�Ļָ�**********************************************
	fread(&v_num,sizeof(int),1,fp);
	fread(vi,sizeof(view),MAX_VIEWNUM,fp);
	if((fp2=fopen("view.db","wb"))==NULL)
		fp2=fopen("view.db","wb+");
	fwrite(&v_num,sizeof(int),1,fp2);
	fwrite(vi,sizeof(view),MAX_VIEWNUM,fp2);
	fclose(fp2);
	PrintStringXY(1,24,"�ָ�view.db\a..   ");
	Sleep(300);
//******************��dic.db�Ļָ�***********************************************
	//dic.db�������ֵ䣬����Ҫ���ݣ�ֻ���ڻָ���ʱ���table.db�зֽ��������.
	int i,j;
	if((fp2=fopen("dic.db","wb"))==NULL)
		fp2=fopen("dic.db","wb+");
	for(i=0;i<t_num;i++)
	{
		fprintf(fp2,"����:%s   ��������:%d   ����:%s",t[i].tname,t[i].pro_num,t[i].key);
		fprintf(fp2,"\n----------------------------------------------------\n");
		for(j=0;j<t[i].pro_num;j++)
			fprintf(fp2,"%s(%c)    ",t[i].proname[j],t[i].protype[j]);
		fprintf(fp,"\n\n\n");
	}
	fclose(fp2);
	PrintStringXY(1,24,"�ָ�dic.db\a..   ");
	Sleep(300);
//******************��log.txt�ı���***********************************************
	char ch;
	if((fp2=fopen("log.txt","wb"))==NULL)
		fp2=fopen("dic.db","wb+");
	ch=fgetc(fp);
	while(ch!=EOF)
	{
		fputc(ch,fp2);
		ch=fgetc(fp);
	}
	fclose(fp2);
	fclose(fp);
	PrintStringXY(1,24,"�ָ�log.txt\a..   ");
	Sleep(300);
	PrintStringXY(1,24,"��ϲ�����������ѳɹ��ָ���");
	printf("%s",filename);
	printf("\n�����������...");
	getch();
	if(dbislock())
		dbunlock();
}
void fileread()
{
	clrscr();
	FILE *fp;

	//*****************��ȡconfig.db����
	if((fp=fopen("config.db","rb"))==NULL)
	{
		PrintStringXY(1,24,"û�з���ϵͳ�����ļ�config.db");
		fclose(fp);
	}
	fread(&speedfactor,sizeof(int),1,fp);
	fclose(fp);
	//*****************
	int ip,jp;
	if((fp=fopen("table.db","rb"))==NULL)
	{
		fp=fopen("table.db","wb+");
		fclose(fp);
	}
	fread(&t_num,sizeof(int),1,fp);
	printf("\n\n���ڽ���table.db...");
	Sleep(800/speedfactor);
	printf("\n��������t_num...");
	Sleep(500/speedfactor);
//�����������������!!!!!!ʱ��̫̫̫������,������ֲ�����.
	fread(t,sizeof(table),MAX_TNUM,fp);
	fclose(fp);
	for(ip=0;ip<t_num;ip++)
	{
		printf("OK\n\n���������%s...",t[ip].tname);
		Sleep(250/speedfactor);
		for(jp=0;jp<t[ip].re_num;jp++)
		{
			printf("OK\n���������¼%s...",t[ip].record[jp][t[ip].key_no]);
			Sleep(100/speedfactor);
		}
	}
//�����ȡview.db�����ݡ�
	if((fp=fopen("view.db","rb"))==NULL)
	{
		fp=fopen("dic.db","wb+");
		fclose(fp);
	}
	fread(&v_num,sizeof(int),1,fp);
	fread(vi,sizeof(view),MAX_VIEWNUM,fp);
	fclose(fp);
	printf("OK\n\n���ڽ���view.db...");
	Sleep(800/speedfactor);
	printf("\n��������v_num...");
	Sleep(500/speedfactor);
	for(ip=0;ip<v_num;ip++)
	{
		printf("OK\n����������ͼ%s...OK",vi[ip].vname);
		Sleep(100/speedfactor);
	}
//������Ӷ�ȡindex.db�Ĵ���
	if((fp=fopen("index.db","rb"))==NULL)
	{
		fp=fopen("dic.db","wb+");
		fclose(fp);
	}
	fread(&i_num,sizeof(int),1,fp);
	fread(ind,sizeof(index),MAX_TNUM,fp);
	fclose(fp);
	printf("\n\n���ڽ���index.db...");
	Sleep(800/speedfactor);
	printf("\n��������i_num...");
	Sleep(100/speedfactor);
	for(ip=0;ip<i_num;ip++)
	{
		printf("OK\n����������ͼ%s...",ind[ip].iname);
		Sleep(100/speedfactor);
	}
	printf("OK\n\n�ɹ�������������!");
	printf("\n\n�����û���������...");
	Sleep(200/speedfactor);
	printf("OK\n\n�����û���Ȩ��...");
	Sleep(200/speedfactor);
	printf("OK\n\n����������ӻ����Ч������ɫ��Ⱦ");
	for(ip=0;ip<9;ip++)
	{
		Sleep(200/speedfactor);
		printf(".");
	}
	printf("OK\n\n���ؽ������!\a�ȴ�����ϵͳ...");
	Sleep(1000/speedfactor);
}
void filewrite()
{
	FILE *fp;
//*****************��ȡconfig.db����
	if((fp=fopen("config.db","wb"))==NULL)
		fp=fopen("config.db","wb+");
	fwrite(&speedfactor,sizeof(int),1,fp);
	fclose(fp);
//д��table.db�Ĵ���
	if((fp=fopen("table.db","wb"))==NULL)
	{
		printf("������table.db�ļ�,�������ʹ��.\a");
		getchar();
		exit(-1);
	}
	fwrite(&t_num,sizeof(int),1,fp);
	fwrite(t,sizeof(table),MAX_TNUM,fp);
	fclose(fp);
//������д��dic.db�Ĵ���
	int i,j;
	if((fp=fopen("dic.db","wb"))==NULL)
	{
		printf("������dic.db�ļ�,�������ʹ��.\a");
		getchar();
		exit(-1);
	}
	for(i=0;i<t_num;i++)
	{
		fprintf(fp,"����:%s   ��������:%d   ����:%s",t[i].tname,t[i].pro_num,t[i].key);
		fprintf(fp,"\n----------------------------------------------------\n");
		for(j=0;j<t[i].pro_num;j++)
			fprintf(fp,"%s(%c)    ",t[i].proname[j],t[i].protype[j]);
		fprintf(fp,"\n\n\n");
	}
	fclose(fp);
//������д��view.db�Ĵ���
	if((fp=fopen("view.db","wb"))==NULL)
	{
		printf("������view.db�ļ�,�������ʹ��.\a");
		getchar();
		exit(-1);
	}
	fwrite(&v_num,sizeof(int),1,fp);
	fwrite(vi,sizeof(view),MAX_VIEWNUM,fp);
	fclose(fp);
//������д��index.db�Ĵ���
	if((fp=fopen("index.db","wb"))==NULL)
	{
		printf("������index.db�ļ�,�������ʹ��.\a");
		getchar();
		exit(-1);
	}
	fwrite(&i_num,sizeof(int),1,fp);
	fwrite(ind,sizeof(index),MAX_TNUM,fp);
	fclose(fp);
}
bool IsRecEqu(table *r,int i,table *s,int j)
{
	int p;
	if(r->pro_num!=s->pro_num)		//���Ը�����ƥ��
		return false;
	for(p=0;p<s->pro_num;p++)		//�������ֲ�ƥ��
		if(strcmp(s->proname[p],r->proname[p])!=0)
			return false;
	for(p=0;p<r->pro_num;p++)
		if(strcmp(r->record[i][p],s->record[j][p])!=0)
			return false;
	return true;
}
bool IsTableSameType(table *t1,table *t2)
{
	int p;
	if(t1->pro_num!=t2->pro_num)
	{
		gotoxy(1,20);printf("ErrorNo1:unequal number of properities.");
		return false;
	}
	for(p=0;p<t1->pro_num;p++)
		if(strcmp(t1->proname[p],t2->proname[p])!=0)
		{
			gotoxy(1,20);printf("ErrorNo2:different proname[%d]",p);
			return false;
		}
	return true;
}

void tablecreate()
{
	if(CURRENTREVOKE==0)
	{
		gotoxy(1,24);printf("\a�Բ���,����Ȩ�޲������������.�����������...");
		getch();
		return;
	}
	if(!dbislock())		//û�������Ǿͼ���@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		dblock();	//dblock();     dbunlock();
	else
	{
		clrscr();
		printf("���ݿ⴦����״̬����ʹ�����û�Ҳ���ܽ����޸�!\a");
		printf("\n�����������...");
		getch();
		return;
	}
	clrscr();
	char mn[MAX_TNAME];
	char ck[MAX_PROLEN];
	table tnew;
	int nnum;
	int i,j,k;
	gotoxy(37,7);setcolor(0,14);
	printf("�������");setcolor(0,15);
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("�����½��������:");
	//gets(mn);
	cin>>mn;
	//������������������
	strcpy(tnew.tname,mn);	//�±����
	gotoxy(27,10); printf("�����½�������Ը���:");
	cin>>nnum;
	//��Ӵ�����
	if(nnum>=MAX_TNUM)
	{
		gotoxy(1,20);printf("�Ѿ��ﵽ���������,�޷�����\a!");
		goto eotc1;
	}
	tnew.pro_num=nnum;
	//��ʼѭ��������������;
	//ע������������
	gotoxy(1,15);
	i=0;
	while(i<tnew.pro_num)
	{
		printf("\n�������%d����������:",i+1);
		//gets(mn);
		cin>>mn;
		//��������!!!!!
		strcpy(tnew.proname[i],mn);
		printf("\n�������%d����������n=num  c=char:",i+1);
		//������,�����ϵ�����Ҫ�ų���!
		cin>>tnew.protype[i];
		i++;
	}

	//��������,������Ψһ��ʾһ����¼������,����Ҫ������.������λ����tnew.key,�����������е��±�.
	printf("\n��������һ�����Ǳز����ٵ�.\n");
	printf("����������������,����������Զ�ѡ���һ������:");
	//gets(ck);
	cin>>ck;
	for(i=0;i<tnew.pro_num;i++)
	{
		if(strcmp(ck,tnew.proname[i])==0)
		{
			tnew.key_no=i;
			strcpy(tnew.key,ck);
			goto nex1;
		}
	}
	printf("\n�������,�����Զ�ѡ��Ϊ��һ������:\a%s",tnew.proname[0]);
	strcpy(tnew.key,tnew.proname[0]);
	tnew.key_no=0;
nex1:;
	tnew.re_num=0;		//��¼������Ϊ��
	for(i=0;i<MAX_RECNUM;i++)
		for(j=0;j<MAX_PRONUM;j++)
			for(k=0;k<MAX_PROLEN;k++)
				tnew.record[i][j][k]='\0';	//��ʼ����¼��
	t[t_num++]=tnew;
	printf("\n����ɹ�,�س�������һ���˵�.");
	getch();
eotc1:;
	dbunlock();		//����@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	fprintf(alfp,"\n[%d��%d��%d�� %d:%d:%d]ִ�����²���:\n",GetCurrentYear(),GetCurrentMonth(),GetCurrentDay(),
									GetCurrentHour(),GetCurrentMinute(),GetCurrentSecond() );
	fprintf(alfp,"�û�������ɹ�,���������:����:%s,�����Ը���:%d,����:%s.",tnew.tname,
								tnew.pro_num,tnew.key);fputc('\n',alfp);
}
void tabledrop()
{
	if(CURRENTREVOKE==0)
	{
		gotoxy(1,24);printf("\a�Բ���,����Ȩ�޲������������.�����������...");
		getch();
		return;
	}
	if(!dbislock())		//û�������Ǿͼ���@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		dblock();	//dblock();     dbunlock();
	else
	{
		clrscr();
		printf("���ݿ⴦����״̬����ʹ�����û�Ҳ���ܽ����޸�!\a");
		printf("\n�����������...");
		getch();
		return;
	}
	clrscr();
	char mn[MAX_TNAME];
	int i,j;
	table tdel;
	gotoxy(37,7);setcolor(0,14);
	printf("��ɾ������");setcolor(0,15);
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("����ɾ���ı���:");
	//gets(mn);
	cin>>mn;
	//��Ӵ�����
	for(i=0;i<t_num;i++)
		if(strcmp(mn,t[i].tname)==0)
		{
			//i��ֵ����Ҫɾ���ı��λ��.
			tdel=t[i];
			for(j=i;j<t_num-1;j++)
				t[j]=t[j+1];
			t_num--;
			goto eodrp;
		}
	gotoxy(1,20);printf("δ�ҵ�Ҫɾ���ı�,��ȷ������.\a");
	getchar();
	dbunlock();
	return;
eodrp:;
	gotoxy(1,20);
	printf("��%s�ѳɹ�ɾ��!�س�������һ���˵�.",mn);
	getch();
	dbunlock();
		fprintf(alfp,"\n[%d��%d��%d�� %d:%d:%d]ִ�����²���:\n",GetCurrentYear(),GetCurrentMonth(),GetCurrentDay(),
									GetCurrentHour(),GetCurrentMinute(),GetCurrentSecond() );
	fprintf(alfp,"�û��ɹ�ɾ��һ����,���������:����:%s,�����Ը���:%d,����:%s.",tdel.tname,
								tdel.pro_num,tdel.key);fputc('\n',alfp);
}


void tablechange() //����4�������Ը���
{
    getchar();
	if(CURRENTREVOKE==0)
	{
		gotoxy(1,24);printf("\a�Բ���,����Ȩ�޲������������.�����������...");
		getch();
		return;
	}
	if(!dbislock())		//û�������Ǿͼ���@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		dblock();	//dblock();     dbunlock();
	else
	{
		clrscr();
		printf("���ݿ⴦����״̬����ʹ�����û�Ҳ���ܽ����޸�!\a");
		printf("\n�����������...");
		getch();
		return;
	}

	clrscr();
	char mn[MAX_TNAME];
	char mn_new[MAX_TNAME];
	char wn[MAX_TNAME];
	char ck[MAX_PROLEN];
	int nnum;
	int i,j,k;
	gotoxy(30,7);setcolor(0,14);
	printf("��/ɾ/�ı�����Ϣ����");setcolor(0,15);
	DrawFrame(20,60,5,12);
	gotoxy(27,9); printf("������Ҫ�����ı�����:");
	cin>>mn;
	getchar();
    gotoxy(1,13);

    //-----------------��ʾ���б�����Ϣ-------------------
    for(i=0;i<t_num;i++)
    {
        if(strcmp(mn,t[i].tname)==0)
        {
            printf("�ñ�ǰ�ı�����Ϣ���£�\n");
            printf("\n����:%s   ���Ը���:%d    ����:%s",t[i].tname,t[i].pro_num,t[i].key);
            printf("\n----------------------------------------------------------------\n");
            for(j=0;j<t[i].pro_num;j++)
            {
                printf("%s(%c)\t",t[i].proname[j],t[i].protype[j]);
            }
            printf("\n\n");
            break;
        }
    }
    if(i==t_num)
    {
        printf("\nδ�ҵ�Ҫ��ѯ�ı�,���س�������һ���˵�.");
        getchar();
        return;
    }

    //-----------------ѡ����/ɾ/�ı�����Ϣ-------------------
    int flag=-1;
    printf("��ѡ������ִ�еĲ�����\n");
	printf("0���޸ı�����1���޸��ֶΣ�2�������ֶΣ�3��ɾ���ֶΣ�4����ѡ������\n");
	cin>>flag;
	getchar();

    if(flag==0)  //0���޸ı�������һ�ν��޸�һ���ֶΣ�
    {
        printf("�������µı���:");
        cin>>mn_new;
        getchar();

        for(i=0;i<t_num;i++)
        {
            if(strcmp(mn,t[i].tname)==0)//ȷ����
            {
                strcpy(t[i].tname,mn_new);
                strcpy(mn,mn_new);
                printf("\n\n�޸ĳɹ���");
                break;
            }
        }

    }
    else if(flag==1)  //1���޸��ֶ������ֶ����ͣ���һ�ν��޸�һ���ֶΣ�
    {
        printf("��������Ҫ�޸ĵ��ֶ���:");
	    cin>>wn;
	    getchar();

        for(i=0;i<t_num;i++)
        {
            if(strcmp(mn,t[i].tname)==0)//ȷ����
            {
                for(j=0;j<t[i].pro_num;j++)
                {
                    if(strcmp(wn,t[i].proname[j])==0)//ȷ���ֶ�
                    {
                        printf("�������µ��ֶ���:");
                        cin>>t[i].proname[j];
                        getchar();
                        printf("������ѡ����ֶε����ͣ�n=num��c=char��");
                        cin>>t[i].protype[j];
                        getchar();
                        break;
                    }
                }
                if(j==t[i].pro_num)
                {
                    printf("\nδ�ҵ�Ҫ�޸ĵ��ֶ���,���س�������һ���˵�.");
                    getchar();
                    return;
                }
                else
                    printf("\n\n�޸ĳɹ���");
                break;
            }
        }

    }
    else if(flag==2)  //2�������ֶΣ���һ�ο���������ֶΣ�Ĭ�ϳ�ʼֵΪ�գ�
    {
        printf("��������Ҫ�������ֶθ���:");
        cin>>nnum;
        getchar();

        for(i=0;i<t_num;i++)
        {
            if(strcmp(mn,t[i].tname)==0)//ȷ����
            {
                t[i].pro_num = t[i].pro_num + nnum;//���������ܸ���
                for(j=t[i].pro_num-nnum;j<t[i].pro_num;j++)
                {
                    printf("�����������ĵ�%d���ֶ���:",j-(t[i].pro_num-nnum)+1);
                    cin>>wn;
                    getchar();
                    strcpy(t[i].proname[j],wn);
                    printf("�����������ĵ�%d���ֶε����ͣ�n=num��c=char��",j-(t[i].pro_num-nnum)+1);
                    cin>>t[i].protype[j];
                    getchar();
                }
                printf("\n\n�����ɹ���");
                break;
            }
        }

    }
    else if(flag==3)  //3��ɾ���ֶΣ���һ�ν�ɾ��һ���ֶΣ�
    {
        printf("��������Ҫɾ�����ֶ���:");
        cin>>wn;
	    getchar();

        for(i=0;i<t_num;i++)
        {
            if(strcmp(mn,t[i].tname)==0)//ȷ����
            {
                for(j=0;j<t[i].pro_num;j++)
                {
                    if(strcmp(wn,t[i].proname[j])==0)//ȷ���ֶ�
                    {
                        for(k=j;k<t[i].pro_num-1;k++)
                        {
                            strcpy(t[i].proname[k],t[i].proname[k+1]);
                            t[i].protype[k]=t[i].protype[k+1];
                        }
                        break;
                    }
                }
                if(j==t[i].pro_num)
                {
                    printf("\nδ�ҵ���Ҫɾ�����ֶ���,���س�������һ���˵�.");
                    getchar();
                    return;
                }
                else
                {
                    t[i].pro_num = t[i].pro_num - 1;
                    printf("\n\nɾ���ɹ���");
                }
                break;
            }
        }

    }
    else  //4����ѡ����
    {
        printf("��ע����ȷ������ѡ���ֶ�������Ϊ������Ҫ�󣡣�\n");
        printf("����������ѡ��������:");
        cin>>ck;
	    getchar();

        for(i=0;i<t_num;i++)
        {
            if(strcmp(mn,t[i].tname)==0)//ȷ����
            {
                for(j=0;j<t[i].pro_num;j++)
                {
                    if(strcmp(ck,t[i].proname[j])==0)//ȷ���ֶ�
                    {
                        t[i].key_no=j;
                        strcpy(t[i].key,ck);
                        break;
                    }
                }
                if(j==t[i].pro_num)
                {
                    printf("\nδ�ҵ�����ѡ����������,���س�������һ���˵�.");
                    getchar();
                    return;
                }
                else
                    printf("\n\n��ѡ�ɹ���");
                break;
            }
        }

    }

    //-----------------��ʾ�޸��Ժ�ı�����Ϣ-------------------
    for(i=0;i<t_num;i++)
    {
        if(strcmp(mn,t[i].tname)==0)
        {
            printf("�޸ĺ�ı�����Ϣ���£�\n");
            printf("\n����:%s   ���Ը���:%d    ����:%s",t[i].tname,t[i].pro_num,t[i].key);
            printf("\n----------------------------------------------------------------\n");
            for(j=0;j<t[i].pro_num;j++)
            {
                printf("%s(%c)\t",t[i].proname[j],t[i].protype[j]);
            }
            printf("\n\n");
            break;
        }
    }

    //-----------------����-------------------
	if(dbislock())
		dbunlock();

    printf("\n����Ϣ������������س�������һ���˵�.");
    getchar();
    return;
}


void tableprint(table *t1)
{
	int m,n;
	printf("\n-----------------------------------------\n");
	for(m=0;m<t1->pro_num;m++)
		printf("%s\t",t1->proname[m]);
	printf("\n-----------------------------------------\n");
	for(m=0;m<t1->re_num;m++)
	{
		for(n=0;n<t1->pro_num;n++)
			printf("%s\t",t1->record[m][n]);
		printf("\n");
	}
}
void recordinsert()
{
	if(CURRENTREVOKE==0)
	{
		gotoxy(1,24);printf("\a�Բ���,����Ȩ�޲������������.�����������...");
		getch();
		return;
	}
	if(!dbislock())		//û�������Ǿͼ���@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		dblock();	//dblock();     dbunlock();
	else
	{
		clrscr();
		printf("���ݿ⴦����״̬����ʹ�����û�Ҳ���ܽ����޸�!\a");
		printf("\n�����������...");
		getch();
		return;
	}
	clrscr();
	char mn[MAX_TNAME];
	char mq[MAX_TNAME];
	int i,j;
	gotoxy(37,7);setcolor(0,14);
	printf("��¼�������");setcolor(0,15);
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("�������ı���:");
	//gets(mn);
    cin>>mn;

	//��Ӵ�����
	for(i=0;i<t_num;i++)
		if(strcmp(mn,t[i].tname)==0)
		{
			//i��ֵ����Ҫɾ���ı��λ��.
			goto eodrp;
		}
	gotoxy(1,20);printf("δ�ҵ�Ҫɾ���ı�,��ȷ������.\a");
	getch();
	if(dbislock())
		dbunlock();
	return;
eodrp:;//�ҵ�������λ��,�������ǲ����¼����,��t[i]�ǲ鵽�ı�.
	//����ж�,�Ƿ���Լ�����Ӽ�¼�����ﵽ��ֵ.������
	strcpy(mq,mn);		//������������õ�mq����
	if(t[i].re_num>=MAX_RECNUM)
	{
		gotoxy(1,20);printf("�ñ��Ѿ��ﵽ��¼�����ֵ!�޷�����.\a");
		getch();
		if(dbislock())
		dbunlock();
		return;
	}
	gotoxy(1,20);
	for(j=0;j<t[i].pro_num;j++)
	{
		printf("\n����������%s��ֵ:",t[i].proname[j]);
		//gets(mn);
		cin>>mn;
		//��Ӵ�����,���������������ظ��ȵ�.
		strcpy(t[i].record[t[i].re_num][j],mn);		//������ֵ������Ӧ�ļ�¼����λ��.
				//re_num++
		//////////////////////////////////////////////////////////////////////////////////////////////////////////
	}
	t[i].re_num++;

	printf("\n��%s�Ѿ����ɹ��ز�����һ���¼�¼!�س�������һ���˵�.",mq);
	getch();
	if(dbislock())
		dbunlock();
}

void recorddel()
{
	if(CURRENTREVOKE==0)
	{
		gotoxy(1,24);printf("\a�Բ���,����Ȩ�޲������������.�����������...");
		getch();
		return;
	}
	if(!dbislock())		//û�������Ǿͼ���@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		dblock();	//dblock();     dbunlock();
	else
	{
		clrscr();
		printf("���ݿ⴦����״̬����ʹ�����û�Ҳ���ܽ����޸�!\a");
		printf("\n�����������...");
		getch();
		return;
	}
	clrscr();
	char mn[MAX_TNAME];
	char mq[MAX_TNAME];
	char zj[MAX_PROLEN];
	int i,j,k;
	gotoxy(37,7);setcolor(0,14);
	printf("��¼ɾ������");setcolor(0,15);
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("����ɾ����¼�ı���:");
	//gets(mn);
    cin>>mn;
	//��Ӵ�����
	for(i=0;i<t_num;i++)
		if(strcmp(mn,t[i].tname)==0)
		{
			//i��ֵ����Ҫɾ���ı��λ��.
			goto eodrd;
		}
	gotoxy(1,20);printf("δ�ҵ�Ҫɾ����¼�ı�,��ȷ������.\a");
	getch();
	if(dbislock())
		dbunlock();
	return;
eodrd:;
	//�ҵ�������λ��,�������ǲ����¼����,��t[i]�ǲ鵽�ı�.
	//����ж�,�Ƿ���Լ�����Ӽ�¼�����ﵽ��ֵ.������
	strcpy(mq,mn);		//������������õ�mq����
	if(t[i].re_num<=0)
	{
		gotoxy(1,20);printf("�ñ��޼�¼!�޷�ɾ��.\a");
		getch();
		if(dbislock())
			dbunlock();
		return;
	}
	gotoxy(27,10); printf("����Ҫɾ����¼������ֵ:");
	//gets(zj);	//����������ֵ
	cin>>zj;

	//��Ӵ�����,�ж��Ƿ��дʼ�¼
	for(j=0;j<t[i].re_num;j++)
	{
		if( strcmp( t[i].record[j][t[i].key_no] , zj ) ==0)
			goto findkey1;
	}
	gotoxy(1,20);printf("δ�ҵ��ü�¼,��ȷ������.\a");
	getch();
	if(dbislock())
		dbunlock();
	return;
findkey1:;
	//�ҵ��ü�¼,��ʼ����ɾ������.j����Ҫɾ���ļ�¼���±�
	for(;j<t[i].re_num-1;j++)
	{
		for(k=0;k<t[i].pro_num;k++)
			strcpy(t[i].record[j][k],t[i].record[j+1][k]);
	}
	t[i].re_num--;
	printf("\n��%s�Ѿ����ɹ���ɾ����һ����¼!�س�������һ���˵�.",mq);
	getch();
	if(dbislock())
		dbunlock();
}

void query()	//select * from sth.table
{

}
bool IsInView(view *v1,char *proname)
{
	int i;
	for(i=0;i<v1->pro_num;i++)
		if(strcmp(v1->proname[i],proname)==0)
			return true;
	return false;
}
void viewcreate()
{
	if(CURRENTREVOKE==0)
	{
		gotoxy(1,24);printf("\a�Բ���,����Ȩ�޲������������.�����������...");
		getch();
		return;
	}
	if(!dbislock())		//û�������Ǿͼ���@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		dblock();	//dblock();     dbunlock();
	else
	{
		clrscr();
		printf("���ݿ⴦����״̬����ʹ�����û�Ҳ���ܽ����޸�!\a");
		printf("\n�����������...");
		getch();
		return;
	}
	clrscr();
	char mn[MAX_TNAME];	//Ϊ�˴������������ı���
	char zj[MAX_PROLEN];
	int pnum;
	int i,j,k;
	view vnew;
	gotoxy(37,7);setcolor(0,14);
	printf("������ͼ");setcolor(0,15);
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("������ͼ��:");
	//gets(vnew.vname);	//������ͼ��
	cin>>vnew.vname;

	//��Ӵ�������������ͼ�ȵ�
	gotoxy(27,10); printf("������ͼ��Ӧ�ı���:");
	//gets(mn);
	cin>>mn;
	for(i=0;i<t_num;i++)
		if(strcmp(t[i].tname,mn)==0)
			goto sse;
	gotoxy(1,20);printf("û���ҵ��ñ�,��ȷ������\a");
	getch();
	if(dbislock())
		dbunlock();
	return;
sse:;
	//�ҵ� �ñ�i���Ǳ���±�
	strcpy(vnew.tname,mn);
	gotoxy(1,20);
	printf("��������ͼ��ʾ�����Ը���:");
	cin>>pnum;
	vnew.pro_num=pnum;
	//��������������� ����˵�ˡ�
	k=0;
	while(k<pnum)
	{
		printf("\n�������%��������:",k+1);
		//gets(zj);
		cin>>zj;

		strcpy(vnew.proname[k],zj);
		for(j=0;j<t[i].pro_num;j++)
			if(strcmp(t[i].proname[j],zj)==0)
				goto asqq;
		printf("\a\nû���ҵ�������.");
		getch();
		if(dbislock())
			dbunlock();
		return;
asqq:;
		vnew.prono[k]=j;
		k++;
	}
	vi[v_num++]=vnew;
	printf("�ɹ���������ͼ%s",vnew.vname);
	getch();
	if(dbislock())
			dbunlock();
}
void viewdrop()
{
	if(CURRENTREVOKE==0)
	{
		gotoxy(1,24);printf("\a�Բ���,����Ȩ�޲������������.�����������...");
		getch();
		return;
	}
	if(!dbislock())		//û�������Ǿͼ���@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		dblock();	//dblock();     dbunlock();
	else
	{
		clrscr();
		printf("���ݿ⴦����״̬����ʹ�����û�Ҳ���ܽ����޸�!\a");
		printf("\n�����������...");
		getch();
		return;
	}
	clrscr();
	char mn[MAX_TNAME];	//Ϊ�˴������������ı���
	int i;
	gotoxy(37,7);setcolor(0,14);
	printf("������ͼ");setcolor(0,15);
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("������ͼ��:");
	//gets(mn);	//������ͼ��
	cin>>mn;
	//��Ӵ�������������ͼ�ȵ�
	if(v_num<=0)
	{
		gotoxy(1,20);
		printf("��ͼ��Ϊ0���޷�ɾ����ͼ\a");
	}
	for(i=0;i<v_num;i++)
	{
		if(strcmp(mn,vi[i].vname)==0)
			goto ae;
	}
	printf("\nû���ҵ�����ͼ%s\a",mn);
	getch();
	if(dbislock())
		dbunlock();
	return;
ae:;for(;i<v_num-1;i++)
		vi[i]=vi[i+1];
	v_num--;
	printf("\nɾ����ͼ�ɹ�! ���س�������һ���˵���");
	getch();
	if(dbislock())
		dbunlock();
}
void viewtable()
{
	clrscr();
	char mn[MAX_TNAME];
	int i,j,k,p;
	gotoxy(37,7);setcolor(0,14);
	printf("��ͼ�鿴��");setcolor(0,15);
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("������ͼ��:");
	//gets(mn);	//������ͼ��
	cin>>mn;
	//�ж���ͼ�Ƿ����
	for(i=0;i<v_num;i++)
		if(strcmp(vi[i].vname,mn)==0)
			goto val1;
	gotoxy(1,20);printf("�����ڵ���ͼ��!\a");
	getch();
	return;
val1:;//���ָ���ͼ�����Ϊi
	for(j=0;j<t_num;j++)
		if(strcmp(vi[i].tname,t[j].tname)==0)
			break;
	//jΪ�������±�
	gotoxy(1,20);
	printf("\n����:%s   ���Ը���:%d    ����:%s",t[j].tname,t[j].pro_num,t[j].key);
		printf("\n----------------------------------------------------------------\n");
		for(k=0;k<t[j].pro_num;k++)
		{
			if(IsInView(&vi[i],t[j].proname[k]))
				printf("%s\t",t[j].proname[k]);
		}
		printf("\n----------------------------------------------------------------\n");
		for(k=0;k<t[i].re_num;k++)
		{
			for(p=0;p<t[i].pro_num;p++)
				if(IsInView(&vi[i],t[j].proname[p]))
					printf("%s\t",t[j].record[k][p]);
			printf("\n");
		}
	getch();
}
void indexcreate()
{
	if(CURRENTREVOKE==0)
	{
		gotoxy(1,24);printf("\a�Բ���,����Ȩ�޲������������.�����������...");
		getch();
		return;
	}
	if(!dbislock())		//û�������Ǿͼ���@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		dblock();	//dblock();     dbunlock();
	else
	{
		clrscr();
		printf("���ݿ⴦����״̬����ʹ�����û�Ҳ���ܽ����޸�!\a");
		printf("\n�����������...");
		getch();
		return;
	}
	clrscr();
	char mn[MAX_TNAME];	//Ϊ�˴������������ı���
	int i,j;
	index inew;
	setcolor(0,14);
	gotoxy(37,7);setcolor(0,14);
	printf("��������");setcolor(0,15);
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("������������:");
	//gets(mn);
	cin>>mn;

	//��Ӵ�����
	for(i=0;i<t_num;i++)
		if(strcmp(mn,t[i].tname)==0)
		{
			//i��ֵ����Ҫɾ���ı��λ��.
			goto eodrp;
		}
	gotoxy(1,20);printf("δ�ҵ��ñ�,��ȷ������.\a");
	getch();
	if(dbislock())
		dbunlock();
	return;
eodrp:;//�ҵ�������λ��,�������ǽ�����������,��t[i]�ǲ鵽�ı�.
	//����ж�,�Ƿ���Լ�����Ӽ�¼�����ﵽ��ֵ.������
	strcpy(inew.iname,mn);
	inew.key_num=t[i].key_no;
	for(j=0;j<t[i].re_num;j++)
		strcpy(inew.key[j],t[i].record[j][t[i].key_no]);
	gotoxy(1,20);printf("�ڱ�%s�ϳɹ�����������,�س�������һ���˵�.",mn);
	ind[i_num++]=inew;
	getch();
	if(dbislock())
		dbunlock();
}
void indexdrop()
{
	if(CURRENTREVOKE==0)
	{
		gotoxy(1,24);printf("\a�Բ���,����Ȩ�޲������������.�����������...");
		getch();
		return;
	}
	if(!dbislock())		//û�������Ǿͼ���@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		dblock();	//dblock();     dbunlock();
	else
	{
		clrscr();
		printf("���ݿ⴦����״̬����ʹ�����û�Ҳ���ܽ����޸�!\a");
		printf("\n�����������...");
		getch();
		return;
	}
	if(dbislock())
		dbunlock();
}
void indexshow()			//��ʾ�������ݣ����������������ʾ����ľ�������!!
{
	clrscr();
	char mn[MAX_TNAME];
	int i,j,k;
	gotoxy(37,7);setcolor(0,14);
	printf("��ʾ����");setcolor(0,15);
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("������������:");
	//gets(mn);
	cin>>mn;
	for(i=0;i<v_num;i++)
		if(strcmp(mn,ind[i].iname)==0)
			goto aas;
	printf("\a\nû���ҵ�������!");
	getchar();
	return;
aas:;
	for(j=0;j<t_num;j++)
		if(strcmp(mn,t[j].tname)==0)
			break;
	gotoxy(1,20);
	printf("\n\n��������:%s   ����������Ϊ:%s",ind[i].iname,t[j].key);
	printf("\n-------------------------------------");
	for(k=0;k<t[j].re_num;k++)
		printf("\n%s",t[j].record[k][t[j].key_no]);
	printf("\n\n��ʾ�����������ļ�¼��������.�س�������һ���˵�.");
	getchar();
}
void recordupdate()
{
	if(CURRENTREVOKE==0)
	{
		gotoxy(1,24);printf("\a�Բ���,����Ȩ�޲������������.�����������...");
		getch();
		return;
	}
	if(!dbislock())		//û�������Ǿͼ���@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		dblock();	//dblock();     dbunlock();
	else
	{
		clrscr();
		printf("���ݿ⴦����״̬����ʹ�����û�Ҳ���ܽ����޸�!\a");
		printf("\n�����������...");
		getch();
		return;
	}
	clrscr();
	char mn[MAX_TNAME];
	char mq[MAX_TNAME];
	char zj[MAX_PROLEN];
	int i,j,k;
	gotoxy(37,7);setcolor(0,14);
	printf("��¼���Ĳ���");setcolor(0,15);
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("�����¼���ڵı���:");
	//gets(mn);
	cin>>mn;

	//��Ӵ�����
	for(i=0;i<t_num;i++)
		if(strcmp(mn,t[i].tname)==0)
		{
			//i��ֵ����Ҫɾ���ı��λ��.
			goto eodrp;
		}
	gotoxy(1,20);printf("δ�ҵ�Ҫɾ���ı�,��ȷ������.\a");
	getch();
	if(dbislock())
		dbunlock();
	return;
eodrp:;//�ҵ�������λ��,�������ǲ����¼����,��t[i]�ǲ鵽�ı�.
	//����ж�,�Ƿ���Լ�����Ӽ�¼�����ﵽ��ֵ.������
	strcpy(mq,mn);		//������������õ�mq����
	if(t[i].re_num<=0)
	{
		gotoxy(1,20);printf("�ñ��¼ֵΪ0!�޷�����.\a");
		getch();
		if(dbislock())
			dbunlock();
		return;
	}
	//�����￪ʼ��Ѱ������ֵ
	gotoxy(27,10); printf("�����¼����ֵ:");
	//gets(zj);	//����������ֵ
	cin>>zj;
	//��Ӵ�����,�ж��Ƿ��дʼ�¼
	printf("   key_no=%d   ",t[i].key_no);
	for(j=0;j<t[i].re_num;j++)
	{
		if( strcmp( t[i].record[j][t[i].key_no] , zj ) ==0)
			goto findkey1;
	}
	gotoxy(1,20);printf("δ�ҵ��ü�¼,��ȷ������.\a");
	getch();
	if(dbislock())
		dbunlock();
	return;
	//�ҵ��ü�¼,��ʼ����ɾ������.j���Ǽ�¼���±�,i�Ǳ���±�
findkey1:;
	gotoxy(1,20);
	for(k=0;k<t[i].pro_num;k++)
	{
		printf("\n����������%s��ֵ:",t[i].proname[k]);
		//gets(mn);
		cin>>mn;
		//��Ӵ�����,���������������ظ��ȵ�.
		strcpy(t[i].record[j][k],mn);		//������ֵ������Ӧ�ļ�¼����λ��.
				//re_num++
	}

	printf("\n��%s�ɹ��޸���һ����¼!�س�������һ���˵�.",mq);
	getch();
	if(dbislock())
		dbunlock();
}
void outputrec()
{
	clrscr();
	int i,j,k;
	for(i=0;i<t_num;i++)
	{
		printf("\n����:%s   ���Ը���:%d    ����:%s",t[i].tname,t[i].pro_num,t[i].key);
		printf("\n----------------------------------------------------------------\n");
		for(j=0;j<t[i].pro_num;j++)
		{
			printf("%s\t",t[i].proname[j]);
		}
		printf("\n----------------------------------------------------------------\n");
		for(k=0;k<t[i].re_num;k++)
		{
			for(j=0;j<t[i].pro_num;j++)
				printf("%s\t",t[i].record[k][j]);
			printf("\n");
		}
		printf("\n\n���س��������һ�ű�...\n");
		getchar();
	}
	printf("\n��������������س�������һ���˵�.");
	getchar();
}

void outputts()		//��Ļ�����Ϣ��3��������Ϣ��ʾ��
{
	clrscr();
	getchar();
	char mn[MAX_TNAME];

	int flag=0;
	printf("\n1����ѯ���б�ı�ṹ��2����ѯ������ı�ṹ��\n");
	cin>>flag;
	getchar();
	if(flag==1)
    {
        int i,j;
        for(i=0;i<t_num;i++)
        {
            printf("\n����:%s   ���Ը���:%d    ����:%s",t[i].tname,t[i].pro_num,t[i].key);
            printf("\n----------------------------------------------------------------\n");
            for(j=0;j<t[i].pro_num;j++)
            {
                printf("%s(%c)\t",t[i].proname[j],t[i].protype[j]);
            }
            printf("\n\n");
        }
    }
    else
    {
        printf("\n��������Ҫ��ѯ�ı�����");
        cin>>mn;
        getchar();
        int i,j;
        for(i=0;i<t_num;i++)
        {
            if(strcmp(mn,t[i].tname)==0)
            {
                printf("\n����:%s   ���Ը���:%d    ����:%s",t[i].tname,t[i].pro_num,t[i].key);
                printf("\n----------------------------------------------------------------\n");
                for(j=0;j<t[i].pro_num;j++)
                {
                    printf("%s(%c)\t",t[i].proname[j],t[i].protype[j]);
                }
                printf("\n\n");
                break;
            }
        }
        if(i==t_num)
        {
            printf("\nδ�ҵ�Ҫ��ѯ�ı�,���س�������һ���˵�.");
            getchar();
            return;
        }
    }
	printf("\n����Ϣ������������س�������һ���˵�.");
	getchar();
}


void initialize()
{
	int i,j,k,p;
	t_num=0;
	NULLTABLE.key_no=-1;	//�ձ�Ķ������key_no��-1�ı�Ҳ��һ�ֱ�ʶ
	for(i=0;i<MAX_TNUM;i++)
	{
		for(j=0;j<MAX_PRONUM;j++)
			t[i].key[j]='\0';
		t[i].key_no=0;
		t[i].pro_num=0;
		t[i].re_num=0;
		for(j=0;j<MAX_PRONUM;j++)
			t[i].protype[j]='\0';
		for(j=0;j<MAX_PRONUM;j++)
			for(k=0;k<MAX_PROLEN;k++)
				t[i].proname[j][k]='\0';
		for(j=0;j<MAX_TNAME;j++)
			t[i].tname[j];
		for(j=0;j<MAX_RECNUM;j++)
			for(k=0;k<MAX_PRONUM;k++)
				for(p=0;p<MAX_PROLEN;p++)
					t[i].record[j][k][p]='\0';
	}
}
table yununion(table *t1,table *t2)
{
	table tnew;	//�����
	int k,p,m,q;
		if(!IsTableSameType(t1,t2))
	{
		printf("\n�����������������ͬһ�����͵�!\a");
		getchar();
		return NULLTABLE;
	}
//����������T1��ֵ�������
	tnew.key_no=t1->key_no;
	tnew.pro_num=t1->pro_num;
	tnew.re_num=t1->re_num;
	strcpy(tnew.key,t1->key);
	strcpy(tnew.tname,"T");
	for(k=0;k<t1->pro_num;k++)
	{
		strcpy(tnew.proname[k],t1->proname[k]);
		tnew.protype[k]=t1->protype[k];
	}
	for(k=0;k<tnew.re_num;k++)
		for(p=0;p<tnew.pro_num;p++)
			strcpy(tnew.record[k][p],t1->record[k][p]);	//��T���ǽ����

	for(p=0;p<t2->re_num;p++)
		for(q=0;q<t1->re_num;q++)
		{
			if(IsRecEqu(t1,q,t2,p))	//��¼��ȡ�
				break;
			if((q==t1->re_num-1)&&(!IsRecEqu(t1,q,t2,p)))
			{
				for(m=0;m<t1->pro_num;m++)
					strcpy(tnew.record[tnew.re_num][m],t2->record[p][m]);
				tnew.re_num++;
			}
		}
	return tnew;
}
table yunjiao(table *t1,table *t2)
{
	table tnew;
	int m,q,p,k;
	//������룬�����������ͬ���Ĵ�����
	if(!IsTableSameType(t1,t2))
	{
		gotoxy(1,20);
		printf("�����������������ͬһ�����͵�!\a");
		getchar();
		return NULLTABLE;
	}
	tnew.key_no=t1->key_no;
	tnew.pro_num=t1->pro_num;	//ʹ�����T�ı����ͺ�R,S��ͬ
	tnew.re_num=0;		//�ѿռ����������T
	strcpy(tnew.key,t1->key);
	strcpy(tnew.tname,"T");
	for(k=0;k<t1->pro_num;k++)
	{
		strcpy(tnew.proname[k],t1->proname[k]);
		tnew.protype[k]=t1->protype[k];
	}
//��������ʼ��

	for(p=0;p<t2->re_num;p++)
		for(q=0;q<t1->re_num;q++)
		{
			if(IsRecEqu(t1,q,t2,p))	//��¼��ȡ�
			{
				for(m=0;m<t1->pro_num;m++)
					strcpy(tnew.record[tnew.re_num][m],t2->record[p][m]);
				tnew.re_num++;
				break;
			}
		}
	return tnew;
}
table yundiff(table *t1,table *t2)
{
	table tnew;
	int k,m,n,p,q;
		//������룬�����������ͬ���Ĵ�����
	if(!IsTableSameType(t1,t2))
	{
		gotoxy(1,20);
		printf("�����������������ͬһ�����͵�!\a");
		getchar();
		return NULLTABLE;
	}

	tnew.key_no=t1->key_no;
	tnew.pro_num=t1->pro_num;
	tnew.re_num=t1->re_num;
	strcpy(tnew.key,t1->key);
	strcpy(tnew.tname,"T");
	for(k=0;k<t1->pro_num;k++)
	{
		strcpy(tnew.proname[k],t1->proname[k]);
		tnew.protype[k]=t1->protype[k];
	}
	for(k=0;k<tnew.re_num;k++)
		for(p=0;p<tnew.pro_num;p++)
			strcpy(tnew.record[k][p],t1->record[k][p]);	//��T���ǽ����,����������ݾ���R
//��ʼ������ˣ�����
	for(p=0;p<t2->re_num;p++)
		for(q=0;q<tnew.re_num;q++)
		{
			if(IsRecEqu(&tnew,q,t2,p))	//��¼��ȡ�
			{
				for(m=q;m<tnew.re_num;m++)
					for(n=0;n<tnew.pro_num;n++)
						strcpy(tnew.record[m][n],tnew.record[m+1][n]);
				tnew.re_num--;
				break;
			}
		}
	return tnew;
}
table yunshadow(table *t1,char pro[MAX_PRONUM][MAX_PROLEN],int k)
{
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	table *t1			�Ǳ��ָ��
	char pro[][]		��ͶӰ���Ե����ƣ���ά����һά��������ţ��ڶ�ά��������
	int k				��ͶӰ�������Եĸ���
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	table tnew;
	int m,n,p;
	for(m=0;m<=k;m++)
	{
		strcpy(tnew.proname[m],pro[m]);
		//����tnew.protype�Ͳ������ˣ���Ϊʵ���Բ�ǿ�����Ҳ����ȼ۸��ӡ�
	}
	tnew.pro_num=k;
	tnew.key_no=0;
	tnew.re_num=t1->re_num;
	strcpy(tnew.tname,"T");
	for(m=0;m<t1->re_num;m++)		//��¼�ı仯
	{
		for(n=0;n<t1->pro_num;n++)	//���Եı仯
		{
			for(p=0;p<tnew.pro_num;p++)
				if(strcmp(t1->proname[n],tnew.proname[p])==0)
				{
					strcpy(tnew.record[m][p],t1->record[m][n]);
				}	//�ҵ���Ӧ����뵽�±���ȥ
		}
	}
	return tnew;
}
table yunsel0(table *t1,char *sx,int ysf,int shu,char *str)	//���������������ﴫ
{
	table tnew;
	int m,n,k;
	//�ȳ�ʼ��tnew��ʹ��tnew����t1��ͬ����
	tnew.pro_num=t1->pro_num;
	tnew.key_no=t1->key_no;
	tnew.re_num=0;
	strcpy(tnew.tname,"T");
	for(m=0;m<t1->pro_num;m++)
		strcpy(tnew.proname[m],t1->proname[m]);
	//���ж�sx������±��Ƕ��١�
	for(k=0;k<tnew.pro_num;k++)
		if(strcmp(tnew.proname[k],sx)==0)
			break;
	//k����sx���±꣬�������ŷ���!!!
	for(m=0;m<t1->re_num;m++)
	{
		//1.��  2.��  3.��  4.��  5.��  6.��
		if(ysf==1)		//1.��
		{
			if(strcmp(t1->record[m][k],str)==0)
			{
				for(n=0;n<t1->pro_num;n++)
					strcpy(tnew.record[tnew.re_num][n],t1->record[m][n]);
				tnew.re_num++;
			}
		}
		else if(ysf==2)	//2.��
		{
			if(strcmp(t1->record[m][k],str)!=0)
			{
				for(n=0;n<t1->pro_num;n++)
					strcpy(tnew.record[tnew.re_num][n],t1->record[m][n]);
				tnew.re_num++;
			}
		}
		else if(ysf==3)	//3.��
		{
			if(strtonum(t1->record[m][k])<=shu)
			{
				for(n=0;n<t1->pro_num;n++)
					strcpy(tnew.record[tnew.re_num][n],t1->record[m][n]);
				tnew.re_num++;
			}
		}
		else if(ysf==4)	//4.��
		{
			if(strtonum(t1->record[m][k])>=shu)
			{
				for(n=0;n<t1->pro_num;n++)
					strcpy(tnew.record[tnew.re_num][n],t1->record[m][n]);
				tnew.re_num++;
			}
		}
		else if(ysf==5)	//5.��
		{
			if(strtonum(t1->record[m][k])<shu)
			{
				for(n=0;n<t1->pro_num;n++)
					strcpy(tnew.record[tnew.re_num][n],t1->record[m][n]);
				tnew.re_num++;
			}
		}
		else	//ysf==6//6.��
		{
			if(strtonum(t1->record[m][k])>shu)
			{
				for(n=0;n<t1->pro_num;n++)
					strcpy(tnew.record[tnew.re_num][n],t1->record[m][n]);
				tnew.re_num++;
			}
		}
	}
	return tnew;
}
void relationunion()
{
	clrscr();
	char mn[MAX_TNAME];
	table tnew;	//�����
	int i,j;
	gotoxy(33,7);setcolor(0,14);
	printf("�����������R��S");setcolor(0,15);
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("�����ϵR�ı���:");
	//gets(mn);
	cin>>mn;
	//��Ӵ�����
	for(i=0;i<t_num;i++)
		if(strcmp(mn,t[i].tname)==0)
		{
			//i��ֵ���Ǳ��λ��.
			goto eodrp;
		}
	gotoxy(1,20);printf("δ�ҵ���,��ȷ������.\a");
	getchar();
	return;
eodrp:;//�ҵ�������λ��,��t[i]�ǲ鵽�ı�.
	gotoxy(27,10); printf("�����ϵS�ı���:");
	//gets(mn);
	cin>>mn;
	//��Ӵ�����
	for(j=0;j<t_num;j++)
		if(strcmp(mn,t[j].tname)==0)		//j��ֵ���Ǳ��λ��.
			goto eodrp1;
	gotoxy(1,20);printf("δ�ҵ���,��ȷ������.\a");
	getchar();
	return;
eodrp1:;//�ҵ�������λ��,��t[j]�ǲ鵽�ı�.
	//��S���R������������R i  ��S j
//���濪ʼ����������Ĳ�����********************************************
	//������룬�����������ͬ���Ĵ�����
	if((tnew=yununion(&t[i],&t[j])).key_no==-1)
		return ;
	gotoxy(1,20);printf("\n�����ϵ��:");

	tableprint(&tnew);		//�����~~
	printf("\n����˽��,�س�������һ���˵�.");
	getchar();
	getchar();
}
void relationjiao()
{
	clrscr();
	char mn[MAX_TNAME];
	int i,j;
//	int m,n;
	table tnew;		//�����
	gotoxy(33,7);setcolor(0,14);
	printf("�����������R��S");setcolor(0,15);
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("�����ϵR�ı���:");
	//gets(mn);
	cin>>mn;

	//��Ӵ�����
	for(i=0;i<t_num;i++)
		if(strcmp(mn,t[i].tname)==0)
		{
			//i��ֵ���Ǳ��λ��.
			goto eodrp;
		}
	gotoxy(1,20);printf("δ�ҵ���,��ȷ������.\a");
	getchar();
	return;
eodrp:;//�ҵ�������λ��,��t[i]�ǲ鵽�ı�.
	gotoxy(27,10); printf("�����ϵS�ı���:");
	//gets(mn);
	cin>>mn;
	//��Ӵ�����
	for(j=0;j<t_num;j++)
		if(strcmp(mn,t[j].tname)==0)		//j��ֵ���Ǳ��λ��.
			goto eodrp1;
	gotoxy(1,20);printf("δ�ҵ���,��ȷ������.\a");
	getchar();
	return;
eodrp1:;//�ҵ�������λ��,��t[j]�ǲ鵽�ı�.
	//��S���R������������R i  ��S j
	if((tnew=yunjiao(&t[i],&t[j])).key_no==-1)
		return;

	gotoxy(1,20);printf("\n�����ϵ��:");
	tableprint(&tnew);		//�����~~
	printf("\n����˽��,�س�������һ���˵�.");
	getchar();
	getchar();
}
void relationdiff()
{
	clrscr();
	char mn[MAX_TNAME];
	int i,j;
//	int m,n;
	table tnew;
	gotoxy(33,7);setcolor(0,14);
	printf("�����������R-S");setcolor(0,15);
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("�����ϵR�ı���:");
	//gets(mn);
	cin>>mn;
	//��Ӵ�����
	for(i=0;i<t_num;i++)
		if(strcmp(mn,t[i].tname)==0)
		{
			//i��ֵ���Ǳ��λ��.
			goto eodrp;
		}
	gotoxy(1,20);printf("δ�ҵ���,��ȷ������.\a");
	getchar();
	return;
eodrp:;//�ҵ�������λ��,��t[i]�ǲ鵽�ı�.
	gotoxy(27,10); printf("�����ϵS�ı���:");
	//gets(mn);
	cin>>mn;
	//��Ӵ�����
	for(j=0;j<t_num;j++)
		if(strcmp(mn,t[j].tname)==0)		//j��ֵ���Ǳ��λ��.
			goto eodrp1;
	gotoxy(1,20);printf("δ�ҵ���,��ȷ������.\a");
	getchar();
	return;
eodrp1:;//�ҵ�������λ��,��t[j]�ǲ鵽�ı�.
	//��S���R������������R i  ��S j
	if((tnew=yundiff(&t[i],&t[j])).key_no==-1)
		return;
	gotoxy(1,20);printf("\n�����ϵ��:");
//*************************************************************
	tableprint(&tnew);		//�����~~
	printf("\n����˽��,�س�������һ���˵�.");
	getchar();
	getchar();
}
void relationshadow()
{
	clrscr();
	char mn[MAX_TNAME];
	char sx[MAX_PROLEN];
	char pro[MAX_PRONUM][MAX_PROLEN];
	table tnew;
	int i,k;
	int p,q;
	gotoxy(35,7);setcolor(0,14);
	printf("ͶӰ����");setcolor(0,15);
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("�����ϵR�ı���:");
	//gets(mn);
	cin>>mn;
	//��Ӵ�����
	for(i=0;i<t_num;i++)
		if(strcmp(mn,t[i].tname)==0)
		{
			//i��ֵ���Ǳ��λ��.
			goto eodrp;
		}
	gotoxy(1,20);printf("δ�ҵ���,��ȷ������.\a");
	getchar();
	return;
eodrp:;//�ҵ�������λ��,��t[i]�ǲ鵽�ı�.
	gotoxy(1,20);
	int y=0;
	for(p=0;p<MAX_PRONUM;p++)
		for(q=0;q<MAX_PROLEN;q++)
			pro[p][q]='\0';
	k=0;
	char ch;
	while(y!=1)
	{
		printf("\n����ͶӰ������:");
		//gets(sx);
		cin>>sx;

		//������Ӵ�����������������
		strcpy(pro[k++],sx);
		printf("    �Ƿ��������?[y/n]");
		cin>>ch;
		if(ch=='y')
			continue;
		else
			break;
	}//���ﱣ����pro[MAX_PRONUM][MAX_PROLEN]
	tnew=yunshadow(&t[i],pro,k);		//���ε�k��pro�±�
	printf("\nͶӰ�����:");
	tableprint(&tnew);		//�����~~

	printf("\nͶӰ����������س�������һ���˵���");
	getchar();
	getchar();
}
void relationsel0()
{
	clrscr();
	char mn[MAX_TNAME];
	char sx[MAX_PROLEN];
	char str[MAX_PROLEN];
	int i,ysf,shu;		//�����ͱ���������������
	char ch;
	table tnew;		//�����
	gotoxy(35,7);setcolor(0,14);
	printf("ѡ��һ����");setcolor(0,15);
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("�����ϵR�ı���:");
	//gets(mn);
	cin>>mn;
	//��Ӵ�����
	for(i=0;i<t_num;i++)
		if(strcmp(mn,t[i].tname)==0)
		{
			//i��ֵ���Ǳ��λ��.
			goto eodrp;
		}
	gotoxy(1,20);printf("δ�ҵ���,��ȷ������.\a");
	getchar();
	return;
eodrp:;//�ҵ�������λ��,��t[i]�ǲ鵽�ı�.    �����١��ܡ��ݡ�������
	gotoxy(1,20);printf("������ѡ�������:");
	//gets(sx);
	cin>>sx;
	//��Ӵ�����

as:	printf("\n�������ϵ�����(1.��  2.��  3.��  4.��  5.��  6.��)  [1-6]:");
	cin>>ch;
	if(ch>48 && ch<55)
		ysf=ch-48;
	else
		goto as;
	printf("\n����������������ֵ:");
	if(ysf>2)
		cin>>shu;	//3.��  4.��  5.��  6.������������ֵ�Ƚ�
	else
		//gets(str);	//1.��  2.��    ���������ַ����Ƚ�
		cin>>str;
	tnew=yunsel0(&t[i],sx,ysf,shu,str);

	printf("\nѡ������:");
	tableprint(&tnew);		//�����~~
	printf("\nѡ������������س�������һ���˵���");
	getchar();
	getchar();
}

void relationcalculate()
{
	table seland(table *t,char *pro1,char o1,char *shu1,char *pro2,char o2,char *shu2);
	table selor(table *t,char *pro1,char o1,char *shu1,char *pro2,char o2,char *shu2);
	clrscr();
	char large[100];	//�������ʷ�����֮ǰ�Ķ���������һ�����������
	char tname[MAX_TNAME];
	char pro[MAX_PRONUM][MAX_PROLEN];	//��ҪͶӰ���������Ƽ���
	char pro1[MAX_PROLEN];
	char pro2[MAX_PROLEN];
	char shu1[8],shu2[8];
	char o1,o2;
	int i,j,k=0;	//�����ͱ���������������
	gotoxy(33,7);setcolor(0,14);
	printf("��ϵ�����ѯ����");setcolor(0,15);
	DrawFrame(20,60,5,14);

	gotoxy(1,16);printf("\n����sQL���: ");
	printf("����select sno,sage from stu where sno<0003 and sage!=22;\n\n\n>sQL:");
	fflush(stdin);//���������
	gets(large);
	//*******************************������select
	if(strcmp(substr(large,0,6),"select")!=0)
	{
		printf("��ʽ����,û�з���select�ؼ���!!!���������...\a");
		getch();
		return;
	}
	//*******************************������from
	//�Ժ������������������from where��
	int p;
	for(p=0;p<(int)strlen(large);p++)
	{
		if(strcmp(substr(large,p,4),"from")==0)
			break;
		if(strcmp(substr(large,p,4),"from")!=0 && p>(int)strlen(large)-4)
		{
			printf("��ʽ����,û�з���from�ؼ���!!!���������...\a");
			getch();
			return;
		}
	}
	//******************************������where*********
	for(p=0;p<(int)strlen(large);p++)
	{
		if(strcmp(substr(large,p,5),"where")==0)
			break;
		if(strcmp(substr(large,p,5),"where")!=0 && p>(int)strlen(large)-5)
		{
			printf("��ʽ����,û�з���where�ؼ���!!!���������...\a");
			getch();
			return;
		}
	}
	if(large[(int)strlen(large)-1]!=';')
	{
		printf("��ʽ����,û�з���';'�ؼ���!!!���������...\a");
		getch();
		return;
	}
	//******************************
	for(j=0;j<MAX_PRONUM;j++)
		strcpy(pro[j]," ");
	i=7;		//��select ���һ����' '��ʼ
	j=0;
	//select sno,sage from stu where sno=0001 and sage=22;
	//select sno,sage from stu where sno!=0003 and sage!=22;
	//select * from stu
//************************���￪ʼ����************************************
	int secret=0;
	if(large[i]=='*')
	{
		secret=1;
		i+=7;
	}
	else
	{
		while(large[i]!=' ')
		{
			while(large[i]!=',' && large[i]!=' ')
				pro[k][j++]=large[i++];	//��pro1��ֵ
			pro[k][j]='\0';
			j=0;
			if(large[i]==',')
				i++;
			k++;
		}
		i+=6;
	}
	//Ŀǰi��λ����tname��0�±���
	while(large[i]!=' ')
		tname[j++]=large[i++];	//��tname��ֵ ֻ��ȡһ����
	tname[j]='\0';
	if(secret==1)
	{
		int p,q;
		for(p=0;p<t_num;p++)
		{
			if(strcmp(tname,t[p].tname)==0)
				break;
		}
		k = t[p].pro_num;
		for(q=0;q<t[p].pro_num;q++)
			strcpy(pro[q],t[p].proname[q]);
	}
	j=0;
	i+=7;		//Ŀǰi��λ����pro1��0�±���
	while(large[i]!='>' && large[i]!='=' && large[i]!='<'&& large[i]!='!')
		pro1[j++]=large[i++];	//��pro1��ֵ
	pro1[j]='\0';
	j=0;
	o1=large[i++];	//Ŀǰi��o1��,��o1��ֵ
	if(large[i]=='=')
		i++;
	while(large[i]!=' ')
		shu1[j++]=large[i++];	//��shu1��ֵ
	shu1[j]='\0';
	j=0;			//i��' '��
	i++;
	char andor[4];
	while(large[i]!=' ')
		andor[j++]=large[i++];
	andor[j]='\0';
	j=0;
	i++;
	while(large[i]!='>' && large[i]!='=' && large[i]!='<' && large[i]!='!')
		pro2[j++]=large[i++];	//��pro2��ֵ
	pro2[j]='\0';
	j=0;
	//i��o2��
	o2=large[i++];	//Ŀǰi��o2��,��o2��ֵ
	if(large[i]=='=')
		i++;
	while(large[i]!=';')
		shu2[j++]=large[i++];	//��shu2��ֵ
	shu2[j]='\0';
	j=0;			//i��';'��
//************************************************************
	//�����Ͱ�һ�����large�ַ�����ֿ���.
	table t1;
	for(i=0;i<t_num;i++)
		if(strcmp(tname,t[i].tname)==0)
		{
			//i��ֵ���Ǳ��λ��.
			goto eodrp;
		}
	gotoxy(1,20);printf("δ�ҵ���,��ȷ������.\a");
	getchar();
	return;
eodrp:;//�ҵ�������λ��,��t[i]�ǲ鵽�ı�.

	if(strcmp(andor,"and")==0)
		t1=seland(&t[i],pro1,o1,shu1,pro2,o2,shu2);
	else if(strcmp(andor,"or")==0)
		t1=selor(&t[i],pro1,o1,shu1,pro2,o2,shu2);
	else
		printf("\n\n�����error!!!\a\a");
	t1=yunshadow(&t1,pro,k);
	tableprint(&t1);
//**************************���Եص�*******

//*****************************************
	getch();
}
void relationcalculate1()
{
    table condition(table *t,char *pro1,char o1,char o2,char *shu1);
    table relationnaturalconnect(table *t1,table *t2);
	table selor(table *t,char *pro1,char o1,char *shu1,char *pro2,char o2,char *shu2);
	clrscr();
	char large[100];	//�������ʷ�����֮ǰ�Ķ���������һ�����������
	char tname[MAX_TNAME];
	char pro[MAX_PRONUM][MAX_PROLEN];	//��ҪͶӰ���������Ƽ���
	char pro1[MAX_PROLEN];
	char pro2[MAX_PROLEN];
	char shu1[8],shu2[8];
	char o1,o2;
	int i,j,k=0;	//�����ͱ���������������
	gotoxy(33,7);setcolor(0,14);
	printf("��ϵ�����ѯ����");setcolor(0,15);
	DrawFrame(20,60,5,14);

	gotoxy(1,16);printf("\n����sQL���: ");
	printf("����select sno,sage from stu where sno<0003 and sage!=22;\n\n\n>sQL:");
	fflush(stdin);//���������
	gets(large);
	//*******************************������select
	if(strcmp(substr(large,0,6),"select")!=0)
	{
		printf("��ʽ����,û�з���select�ؼ���!!!���������...\a");
		getch();
		return;
	}
	//*******************************������from
	//�Ժ������������������from where��
	int p;
	for(p=0;p<(int)strlen(large);p++)
	{
		if(strcmp(substr(large,p,4),"from")==0)
			break;
		if(strcmp(substr(large,p,4),"from")!=0 && p>(int)strlen(large)-4)
		{
			printf("��ʽ����,û�з���from�ؼ���!!!���������...\a");
			getch();
			return;
		}
	}
	//******************************������where*********
	int flag = 0;
	for(p=0;p<(int)strlen(large);p++)
	{
		if(strcmp(substr(large,p,5),"where")==0)
		{
		    flag = 1;
		    break;
		}
		if(strcmp(substr(large,p,5),"where")!=0 && p>(int)strlen(large)-5)
		{
			//printf("��ʽ����,û�з���where�ؼ���!!!���������...\a");
			flag = 0;
			break;
		}
	}
	if(large[(int)strlen(large)-1]!=';')
	{
		printf("��ʽ����,û�з���';'�ؼ���!!!���������...\a");
		getch();
		return;
	}
	//******************************
	for(j=0;j<MAX_PRONUM;j++)
		strcpy(pro[j]," ");
	i=7;		//��select ���һ����' '��ʼ
	j=0;
	//select sno,sage from stu where sno=0001 and sage=22;
	//select sno,sage from stu where sno!=0003 and sage!=22;
	//select * from stu;
	//************************���￪ʼ����************************************
	int secret=0;
	if(large[i]=='*')
	{
		secret=1;
		i+=7;
	}
	else
	{
		while(large[i]!=' ')
		{
			while(large[i]!=',' && large[i]!=' ')
				pro[k][j++]=large[i++];	//��pro1��ֵ
			pro[k][j]='\0';
			j=0;
			if(large[i]==',')
				i++;
			k++;
		}
		i+=6;
	}
	//Ŀǰi��λ����tname��0�±���
	//ʵ�ְѶ�����ӳ�Ϊ����洢��tnew��
	table tn;
	while(large[i]!=','&&large[i]!=' '&&large[i]!=';')
        tname[j++]=large[i++];
    tname[j]='\0';
    j=0;
    int m = 0;
    for(m;m<t_num;m++)
		if(strcmp(tname,t[m].tname)==0)
		{
			//k��ֵ���Ǳ��λ��.
			goto eodrp1;
		}
	gotoxy(1,20);printf("����ı����󣬲�����.\a");
	getchar();
	return;
    eodrp1:;//�ҵ�������λ��,��t[i]�ǲ鵽�ı�.
    tn = t[m];
    if(large[i]==',')
        i++;
	while(large[i]!=' '&&large[i]!=';')
    {
        while(large[i]!=','&&large[i]!=' '&&large[i]!=';')
            tname[j++]=large[i++];//��tname��ֵ��tname�������
        tname[j]='\0';
        j=0;
        if(large[i]==',')
            i++;
        for(m=0;m<t_num;m++)
            if(strcmp(tname,t[m].tname)==0)
            {
                //k��ֵ���Ǳ��λ��.
                goto eodrp2;
            }
		gotoxy(1,20);printf("����ı����󣬲�����1.\a");
        getchar();
        return;
    eodrp2:;//�ҵ�������λ��,��t[i]�ǲ鵽�ı�.
        tn = relationnaturalconnect(&tn,&t[m]);
    }
    if(secret==1)
	{
		int q;
		k=tn.pro_num;
		for(q=0;q<tn.pro_num;q++)
			strcpy(pro[q],tn.proname[q]);
	}
    if(flag==0)//��where���
    {
        tn = yunshadow(&tn,pro,k);
        tableprint(&tn);
    }
    else//��where���
    {
        table t1 = tn; //t1��Ϊ�����޸ĺ�Ĵ洢
        table t2 = tn; //t2��Ϊԭ��ļ�¼�Ĵ洢
        j=0;
        i+=7;//Ŀǰi��λ����pro1��0�±���
        while(large[i]!=';')
        {
            while(large[i]!='>' && large[i]!='=' && large[i]!='<'&& large[i]!='!')
                pro1[j++]=large[i++];//pro1���ڴ洢�Ƚϵ�����
            pro1[j]='\0';
            j=0;
            o1=large[i];//o1�洢�ȽϷ�
            i++;
            if(large[i]=='=')//�ж��Ƿ�Ϊ==
            {
                o2=large[i];
                i++;
            }
            else
            {
                o2 = '\0';
            }
            while(large[i]!=' '&&large[i]!=';')
                shu1[j++]=large[i++]; //shu1Ϊ�ȽϷ��ұߵ�ֵ
            shu1[j]='\0';
            j=0;
            t1 = condition(&t1,pro1,o1,o2,shu1);
            if(large[i]==' ')
            {
                i++;
                char andor[4];
                while(large[i]!=' ')
                    andor[j++]=large[i++];
                andor[j]='\0';
                j = 0;
                i++;
                if(strcmp(andor,"or")==0)
                {
                    if(tn.re_num == t2.re_num)
                    {
                        tn = t1;
                        t1 = t2;
                    }
                    else
                    {
                        tn = yununion(&tn,&t1);
                        t1 = t2;
                    }
                }
                else if(strcmp(andor,"and")!=0)
                {
                    printf("\n\n�����error!!!\a\a");
                }
            }
            else if(large[i]==';')
            {
                if(tn.re_num == t2.re_num)
                    tn = t1;
                else
                {
                    tn = yununion(&tn,&t1);
                }
                break;
            }
        }
        tn = yunshadow(&tn,pro,k);
        tableprint(&tn);
    }
    getch();
}
table condition(table *t,char *pro1,char o1,char o2,char *shu1)
{
	int shu,ysf;	//�����ͱ���������������
	table tnew1;		//�����
	table *tnull=NULL;

	//��Ӵ������ڵ��ñ�����֮ǰ
	//�ҵ�������λ��,��t[i]�ǲ鵽�ı�.    �����١��ܡ��ݡ�������
	if(o2=='=')
    {
        switch(o1)
        {
            case '>':ysf=4;break;
            case '<':ysf=3;break;
            case '=':ysf=1;break;
            case '!':ysf=2;break;
        }
    }
	else
    {
        switch(o1)
        {
            case '>':ysf=6;break;
            case '<':ysf=5;break;
            case '=':ysf=1;break;
        }
    }
	if(ysf>2)
		shu=strtonum(shu1);	//3.��  4.��  5.��  6.������������ֵ�Ƚ�
	tnew1=yunsel0(t,pro1,ysf,shu,shu1);
//***************************************************************
	return tnew1;
}

void relationseland()
{
	clrscr();
	char mn[MAX_TNAME];
	char sx[MAX_PROLEN];
	char str[MAX_PROLEN];
	char ch;
	int i,shu,ysf;	//�����ͱ���������������
	table tnew1,tnew2;		//�����
	gotoxy(35,7);setcolor(0,14);
	printf("ѡ�����AND");setcolor(0,15);
	DrawFrame(20,60,5,14);

	gotoxy(27,9); printf("�����ϵR�ı���:");
	//gets(mn);
	cin>>mn;
	//��Ӵ�����
	for(i=0;i<t_num;i++)
		if(strcmp(mn,t[i].tname)==0)
		{
			//i��ֵ���Ǳ��λ��.
			goto eodrp;
		}
	gotoxy(1,20);printf("δ�ҵ���,��ȷ������.\a");
	getchar();
	return;
eodrp:;//�ҵ�������λ��,��t[i]�ǲ鵽�ı�.    �����١��ܡ��ݡ�������
	gotoxy(1,20);printf("�������һ��ѡ�������:");
	//gets(sx);
	cin>>sx;
	//��Ӵ�����

as:	printf("\n�������ϵ�����(1.��  2.��  3.��  4.��  5.��  6.��)  [1-6]:");
	cin>>ch;
	if(ch>48 && ch<55)
		ysf=ch-48;
	else
		goto as;
	printf("\n����������������ֵ:");
	if(ysf>2)
		cin>>shu;	//3.��  4.��  5.��  6.������������ֵ�Ƚ�
	else
        {fflush(stdin);
		gets(str);}	//1.��  2.��    ���������ַ����Ƚ�
	tnew1=yunsel0(&t[i],sx,ysf,shu,str);
//****************************************************************
	printf("\n������ڶ���ѡ�������:");
	//gets(sx);
	cin>>sx;
	//��Ӵ�����

as1:	printf("\n�������ϵ�����(1.��  2.��  3.��  4.��  5.��  6.��)  [1-6]:");
	cin>>ch;
	if(ch>48 && ch<55)
		ysf=ch-48;
	else
		goto as1;
	printf("\n����������������ֵ:");
	if(ysf>2)
		cin>>shu;	//3.��  4.��  5.��  6.������������ֵ�Ƚ�
	else
        {fflush(stdin);
		gets(str);}	//1.��  2.��    ���������ַ����Ƚ�
	tnew2=yunsel0(&t[i],sx,ysf,shu,str);
//***************************************************************
	tnew1=yunjiao(&tnew1,&tnew2);
	printf("\nѡ��AND�����:");
	tableprint(&tnew1);		//�����~~
	printf("\nѡ������������س�������һ���˵���");
	getchar();
	getchar();
}
void relationselor()
{
	clrscr();
	char mn[MAX_TNAME];
	char sx[MAX_PROLEN];
	char str[MAX_PROLEN];
	char ch;
	int i,shu,ysf;	//�����ͱ���������������
	table tnew1,tnew2;		//�����
	gotoxy(35,7);setcolor(0,14);
	printf("ѡ�����OR");setcolor(0,15);
	DrawFrame(20,60,5,14);

	gotoxy(27,9); printf("�����ϵR�ı���:");
	fflush(stdin);
	gets(mn);
	//��Ӵ�����
	for(i=0;i<t_num;i++)
		if(strcmp(mn,t[i].tname)==0)
		{
			//i��ֵ���Ǳ��λ��.
			goto eodrp;
		}
	gotoxy(1,20);printf("δ�ҵ���,��ȷ������.\a");
	getchar();
	return;
eodrp:;//�ҵ�������λ��,��t[i]�ǲ鵽�ı�.    �����١��ܡ��ݡ�������
	gotoxy(1,20);printf("�������һ��ѡ�������:");
	fflush(stdin);
	gets(sx);
	//��Ӵ�����

as:	printf("\n�������ϵ�����(1.��  2.��  3.��  4.��  5.��  6.��)  [1-6]:");
	cin>>ch;
	if(ch>48 && ch<55)
		ysf=ch-48;
	else
		goto as;
	printf("\n����������������ֵ:");
	if(ysf>2)
		cin>>shu;	//3.��  4.��  5.��  6.������������ֵ�Ƚ�
	else
    {fflush(stdin);
		gets(str);}	//1.��  2.��    ���������ַ����Ƚ�
	tnew1=yunsel0(&t[i],sx,ysf,shu,str);
//****************************************************************
	printf("\n������ڶ���ѡ�������:");
	fflush(stdin);
	gets(sx);
	//��Ӵ�����

as1:	printf("\n�������ϵ�����(1.��  2.��  3.��  4.��  5.��  6.��)  [1-6]:");
	cin>>ch;
	if(ch>48 && ch<55)
		ysf=ch-48;
	else
		goto as1;
	printf("\n����������������ֵ:");
	if(ysf>2)
		cin>>shu;	//3.��  4.��  5.��  6.������������ֵ�Ƚ�
	else
    {fflush(stdin);
		gets(str);}	//1.��  2.��    ���������ַ����Ƚ�
	tnew2=yunsel0(&t[i],sx,ysf,shu,str);
//***************************************************************
	tnew1=yununion(&tnew1,&tnew2);
	printf("\nѡ��OR�����:");
	tableprint(&tnew1);		//�����~~
	printf("\nѡ������������س�������һ���˵���");
	getchar();
	getchar();
}
void relationselwhere()
{
		clrscr();
	char mn[MAX_TNAME];
	char sx[MAX_PROLEN],sx1[MAX_PROLEN];
	char str[MAX_PROLEN];
	char pro[MAX_PRONUM][MAX_PROLEN];
	int i,ysf,shu;		//�����ͱ���������������
	int q,p,k;
	char ch;
	table tnew;		//�����
	gotoxy(32,7);setcolor(0,14);
	printf("ͶӰѡ���ۺϲ���");setcolor(0,15);
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("�����ϵR�ı���:");
	fflush(stdin);
	gets(mn);
	//��Ӵ�����
	for(i=0;i<t_num;i++)
		if(strcmp(mn,t[i].tname)==0)
		{
			//i��ֵ���Ǳ��λ��.
			goto eodrp;
		}
	gotoxy(1,20);printf("δ�ҵ���,��ȷ������.\a");
	getchar();
	return;
eodrp:;//�ҵ�������λ��,��t[i]�ǲ鵽�ı�.    �����١��ܡ��ݡ�������
	gotoxy(1,20);printf("������ѡ�������:");
	fflush(stdin);
	gets(sx);
	//��Ӵ�����
	strcpy(sx1,sx);
as:	printf("\n�������ϵ�����(1.��  2.��  3.��  4.��  5.��  6.��)  [1-6]:");
	cin>>ch;
	if(ch>48 && ch<55)
		ysf=ch-48;
	else
		goto as;
	printf("\n����������������ֵ:");
	if(ysf>2)
		cin>>shu;	//3.��  4.��  5.��  6.������������ֵ�Ƚ�
	else
    {
        fflush(stdin);
		gets(str);}	//1.��  2.��    ���������ַ����Ƚ�
	tnew=yunsel0(&t[i],sx,ysf,shu,str);		//ѡ�����������ͶӰ��
//*********************************************************************
	int y=0;
	for(p=0;p<MAX_PRONUM;p++)
		for(q=0;q<MAX_PROLEN;q++)
			pro[p][q]='\0';
	k=0;
	while(y!=1)
	{
		printf("\n����ͶӰ������:");
		fflush(stdin);
		gets(sx);
		//������Ӵ�����������������
		strcpy(pro[k++],sx);
		printf("    �Ƿ��������?[y/n]");
		cin>>ch;
		if(ch=='y')
			continue;
		else
			break;
	}//���ﱣ����pro[MAX_PRONUM][MAX_PROLEN]
	tnew=yunshadow(&tnew,pro,k);		//���ε�k��pro�±�
	printf("\n��ѯ�Ľ����:select %s from %s where %s��",sx1,t[i].tname,sx1);
	tableprint(&tnew);		//�����~~
	printf("\nѡ������������س�������һ���˵���");
	getchar();
	getchar();
}
//*********************************************************************************************
//����ֵ����������,����5
table seland(table *t,char *pro1,char o1,char *shu1,char *pro2,char o2,char *shu2)
{
	int shu,ysf;	//�����ͱ���������������
	table tnew1,tnew2;		//�����
	table *tnull=NULL;

	//��Ӵ������ڵ��ñ�����֮ǰ
	//�ҵ�������λ��,��t[i]�ǲ鵽�ı�.    �����١��ܡ��ݡ�������
	switch(o1)
	{
		case '>':ysf=6;break;
		case '<':ysf=5;break;
		case '=':ysf=1;break;
		case '!':ysf=2;break;
	}
	if(ysf>2)
		shu=strtonum(shu1);	//3.��  4.��  5.��  6.������������ֵ�Ƚ�
	tnew1=yunsel0(t,pro1,ysf,shu,shu1);
//****************************************************************
//��Ӵ�����
	switch(o2)
	{
		case '>':ysf=6;break;
		case '<':ysf=5;break;
		case '=':ysf=1;break;
		case '!':ysf=2;break;
	}
	if(ysf>2)
		shu=strtonum(shu2);	//3.��  4.��  5.��  6.������������ֵ�Ƚ�

	tnew2=yunsel0(t,pro2,ysf,shu,shu2);
//***************************************************************
	tnew1=yunjiao(&tnew1,&tnew2);
	return tnew1;
}
table selor(table *t,char *pro1,char o1,char *shu1,char *pro2,char o2,char *shu2)
{
	int shu,ysf;	//�����ͱ���������������
	table tnew1,tnew2;		//�����
	table *tnull=NULL;

	//��Ӵ������ڵ��ñ�����֮ǰ
	//�ҵ�������λ��,��t[i]�ǲ鵽�ı�.    �����١��ܡ��ݡ�������
	switch(o1)
	{
		case '>':ysf=6;break;
		case '<':ysf=5;break;
		case '=':ysf=1;break;
		case '!':ysf=2;break;
	}
	if(ysf>2)
		shu=strtonum(shu1);	//3.��  4.��  5.��  6.������������ֵ�Ƚ�
	tnew1=yunsel0(t,pro1,ysf,shu,shu1);
//****************************************************************
//��Ӵ�����
	switch(o2)
	{
		case '>':ysf=6;break;
		case '<':ysf=5;break;
		case '=':ysf=1;break;
		case '!':ysf=2;break;
	}
	if(ysf>2)
		shu=strtonum(shu2);	//3.��  4.��  5.��  6.������������ֵ�Ƚ�

	tnew2=yunsel0(t,pro2,ysf,shu,shu2);
//***************************************************************
	tnew1=yununion(&tnew1,&tnew2);
	return tnew1;
}

//*********************************************************************************************
void relationconnect()
{
	table relationequalconnect(table *t1,table *t2,int x1,int x2);
	table relationnaturalconnect(table *t1,table *t2);
	//��������������ӣ������������ӵĽ����Ҫ���������
ab:	clrscr();
	char mn[MAX_TNAME];
	table tnew;	//�����
	int i,j,x1,x2;
	gotoxy(33,7);setcolor(0,14);
	printf("���Ӳ���");setcolor(0,15);
	DrawFrame(20,60,5,14);
	gotoxy(27,9);printf("ѡ����������: 1.��ֵ 2.��Ȼ[ ]\b\b");
	char ch;
	cin>>ch;
	if(ch=='1')
		goto ab1;
	else if(ch=='2')
		goto ab2;
	else
		goto ab;
//**********���´����ֵ���Ӻ���Ȼ����*************************************************
//**********�����ǵ�ֵ����*************************************************
ab1:gotoxy(1,20); printf("�����ϵR�ı���:");
	//gets(mn);
	cin>>mn;

	//��Ӵ�����
	for(i=0;i<t_num;i++)
		if(strcmp(mn,t[i].tname)==0)
		{
			//i��ֵ���Ǳ��λ��.
			goto eodrp;
		}
	gotoxy(1,20);printf("δ�ҵ���,��ȷ������.\a");
	getchar();
	return;
eodrp:;//�ҵ�������λ��,��t[i]�ǲ鵽�ı�.
	printf("\n�����ϵR�е�һ������������ֵ����:");
	//gets(mn);
	cin>>mn;
	for(x1=0;x1<t[i].pro_num;x1++)
		if(strcmp(t[i].proname[x1],mn)==0)
			break;
	//���ﲻ��Ӵ�����������Ǽ���goto����ʹ�ã������ڽṹ����������У�
	//GOTO�����һ���ܺ��õĶ���!!!......
	printf("\n�����ϵS�ı���:");
	//gets(mn);
	cin>>mn;
	//��Ӵ�����
	for(j=0;j<t_num;j++)
		if(strcmp(mn,t[j].tname)==0)		//j��ֵ���Ǳ��λ��.
			goto eodrp1;
	gotoxy(1,20);printf("δ�ҵ���,��ȷ������.\a");
	getchar();
	return;
eodrp1:;//�ҵ�������λ��,��t[j]�ǲ鵽�ı�.
	   	printf("\n�����ϵR�е�һ������������ֵ����:");
	//gets(mn);
	cin>>mn;
	for(x2=0;x2<t[j].pro_num;x2++)
		if(strcmp(t[j].proname[x2],mn)==0)
			break;
	//��S���R������������R i  ��S j
	tnew=relationequalconnect(&t[i],&t[j],x1,x2);
	goto enda;
//*********�ٴ�����Ȼ����*******************************************
//***********************************������*************************
ab2:gotoxy(1,20); printf("�����ϵR�ı���:");
	//gets(mn);
	cin>>mn;

	//��Ӵ�����
	for(i=0;i<t_num;i++)
		if(strcmp(mn,t[i].tname)==0)
		{
			//i��ֵ���Ǳ��λ��.
			goto eodrp2;
		}
	gotoxy(1,20);printf("δ�ҵ���,��ȷ������.\a");
	getchar();
	return;
eodrp2:;//�ҵ�������λ��,��t[i]�ǲ鵽�ı�.
	printf("\n�����ϵS�ı���:");
	//gets(mn);
	cin>>mn;
	//��Ӵ�����
	for(j=0;j<t_num;j++)
		if(strcmp(mn,t[j].tname)==0)		//j��ֵ���Ǳ��λ��.
			goto eodrp3;
	gotoxy(1,20);printf("δ�ҵ���,��ȷ������.\a");
	getchar();
	return;
eodrp3:;//�ҵ�������λ��,��t[j]�ǲ鵽�ı�.
	//��S���R������������R i  ��S j
	tnew=relationnaturalconnect(&t[i],&t[j]);
	goto enda;


enda:tableprint(&tnew);
	printf("\n���Ӳ����ɹ����س�������һ���˵�.");
	getchar();
	getchar();
}
table relationequalconnect(table *t1,table *t2,int x1,int x2)
{
	int m,n,i,j;
	table tnew;
	tnew.pro_num=t1->pro_num+t2->pro_num;
	strcpy(tnew.tname,"T");
	tnew.re_num=0;
	tnew.key_no=0;
	for(m=0;m<t1->pro_num;m++)
		strcpy(tnew.proname[m],t1->proname[m]);
	n=m;
	for(m=0;m<t2->pro_num;m++)
		strcpy(tnew.proname[m+n],t2->proname[m]);
//���ϳ�ʼ�������tnew
	for(i=0;i<t1->re_num;i++)
		for(j=0;j<t2->re_num;j++)
		{
			if(strcmp(t1->record[i][x1],t2->record[j][x2])==0)	//��ֵ
			{
				for(m=0;m<t1->pro_num;m++)
					strcpy(tnew.record[tnew.re_num][m],t1->record[i][m]);
				n=m;
				for(m=0;m<t2->pro_num;m++)
					strcpy(tnew.record[tnew.re_num][m+n],t2->record[j][m]);
				tnew.re_num++;
			}
		}
	return tnew;
}
int prosubscript(table *t1,char *str)		//����һ��������һ�����е�λ�ã��±꣩
{
	int i;
	for(i=0;i<t1->pro_num;i++)
		if(strcmp(str,t1->proname[i])==0)
			return i;
	return -1;
}
table relationnaturalconnect(table *t1,table *t2)
{
	table tnew;
	char prosame[MAX_PRONUM][MAX_PROLEN];
	int i,j,m,n,k,pronum=0;
	//��tnew�ĳ�ʼ��
	tnew.pro_num=t1->pro_num+t2->pro_num;
	tnew.key_no=0;
	tnew.re_num=0;
	strcpy(tnew.tname,"T");
		for(m=0;m<t1->pro_num;m++)
		strcpy(tnew.proname[m],t1->proname[m]);
	n=m;
	for(m=0;m<t2->pro_num;m++)
		strcpy(tnew.proname[m+n],t2->proname[m]);
//��ʼ�����
	for(i=0;i<MAX_PRONUM;i++)
		for(j=0;j<MAX_PROLEN;j++)
			prosame[i][j]='\0';		//��ʼ��������������
	for(j=0;j<t2->pro_num;j++)		//t2 j    t1 i
		for(i=0;i<t1->pro_num;i++)
			if(strcmp(t1->proname[i],t2->proname[j])==0)
				strcpy(prosame[pronum++],t1->proname[i]);
	//*****************************************
//	printf("******%s  %s******",prosame[0],prosame[1]);
//	getchar();
	//*****************************************���е�������ȷ
	//�������Լ����Ѿ�ȷ��,׼�������м����Ȼ���ӣ��������ظ����ԣ�
	for(j=0;j<t2->re_num;j++)
		for(i=0;i<t1->re_num;i++)
		{
			for(k=0;k<pronum;k++)
			{
				if(strcmp(t1->record[i][prosubscript(t1,prosame[k])],
					t2->record[j][prosubscript(t2,prosame[k])])==0)
					continue;
	 			else
					break;
			}
			if(k==pronum)	//�ѷ�����Ȼ���������ļ�¼��д����
			{
				for(m=0;m<t1->pro_num;m++)
					strcpy(tnew.record[tnew.re_num][m],t1->record[i][m]);
				n=m;
				printf("\n\nRUNS here\n");
				for(m=0;m<t2->pro_num;m++)
					strcpy(tnew.record[tnew.re_num][m+n],t2->record[i][m]);
				tnew.re_num++;
			}
		}
	//*****************************************�������м��,׼��ɾ����������
	for(i=0;i<tnew.pro_num;i++)
	{
		for(j=0;j<i;j++)
		{
			if(strcmp(tnew.proname[i],tnew.proname[j])==0)	//ɾ��i�е����м�¼
			{
				for(m=i;m<tnew.pro_num-1;m++)
				{
					for(n=0;n<tnew.re_num;n++)
						strcpy(tnew.record[n][m],tnew.record[n][m+1]);
				}
				for(m=i;m<tnew.pro_num-1;m++)
					strcpy(tnew.proname[m],tnew.proname[m+1]);
				tnew.pro_num--;
			}
		}
	}
	return tnew;

}

void test()			//�˺�����Ϊ����֮��
{
	clrscr();
	char chr[6];
	strcpy(chr,numtostr(1234));
	printf("\n\n\nnumtostr(1234)={%s}",chr);
	getch();
}
void StronglySetLock0()
{
	int databaselock=0;	//ǿ���趨����״̬Ϊ��
	FILE *fp;
	if((fp=fopen("dblock.db","wb"))==NULL)
		fp=fopen("dblock.db","wb+");
	fwrite(&databaselock,sizeof(int),1,fp);
	fclose(fp);
	PrintStringXY(1,24,"\a�Ѿ�������Ϊ��,���������...");
	getch();
}
void SetSpeedFactor()
{
	clrscr();
	printf("�����ٶ�����,�ٶ�����Խ��,������ٶ�Խ��.");
	printf("\n�������ٶ�����:(1--30)");
	cin>>speedfactor;
//**************************
//��Ӵ�����
//**************************
	printf("\n\n���óɹ�.");
}
void exit0()
{
	clrscr();				//����  �� ����  �� �� �� �� �� ��
	setcolor(0,10);
	gotoxy(22,9); printf("������������������������������������");
	gotoxy(22,10);printf("��                                ��");
	gotoxy(22,11);printf("��                                ��");
	gotoxy(22,12);printf("��                                ��");
	gotoxy(22,13);printf("��           ллʹ�� !!          ��");
	gotoxy(22,14);printf("��                                ��");
	gotoxy(22,15);printf("��                                ��");
	gotoxy(22,16);printf("��                                ��");
	gotoxy(22,17);printf("��                                ��");
	gotoxy(22,18);printf("������������������������������������");
	gotoxy(5,23);printf("\n\n");setcolor(0,6);
	exit(-1);
}
void login()
{
	u_num=4;
	strcpy(u[0].username,"0");
	strcpy(u[0].pwd,"0");
	u[0].revoke=0;
	strcpy(u[1].username,"guest");
	strcpy(u[1].pwd,"guest");
	u[1].revoke=0;
	strcpy(u[2].username,"admin");
	strcpy(u[2].pwd,"admin");
	u[2].revoke=1;
	strcpy(u[3].username,"1");
	strcpy(u[3].pwd,"1");
	u[3].revoke=1;
	int t=1;
ag:	clrscr();
	PrintStringXY(24,7, "��������������������������������");
	PrintStringXY(24,8, "��                            ��");
	PrintStringXY(24,9, "�ǩ�����������������������������");
	PrintStringXY(24,10,"��  �û���:                   ��");
	PrintStringXY(24,11,"�ǩ�����������������������������");
	PrintStringXY(24,12,"��  ����  :                   ��");
	PrintStringXY(24,13,"��������������������������������");
	PrintColorStringXY(36,8,0,14,0,15,"�û���½");

	int i;
	char username[MAX_USERLEN];
	char pwd[MAX_PWDLEN];
	gotoxy(37,10);
	gets(username);
	//***************************
	for(i=0;i<u_num;i++)
	{//acc
		if(strcmp(username,u[i].username)==0)
			break;
		if((strcmp(username,u[i].username)!=0)&&(i==u_num-1))
		{
			if(t==3)
			{
				PrintStringXY(3,24,"��������ﵽ����,ϵͳǿ���˳�.\a��������˳�.");
				getch();
				exit0();
			}
			PrintStringXY(3,24,"û�и��û���,��˶Ժ�������.\a�����������.");
			getch();
			t++;
			goto ag;
		}
	}
	//***************************
	gotoxy(37,12);
	strcpy(pwd,InputStarString());
//***********************************************************
	for(i=0;i<u_num;i++)
	{
		if(strcmp(username,u[i].username)==0)
		{
			if(strcmp(pwd,u[i].pwd)==0)
			{
				printf("\n\n\n��½�ɹ��������������...");
				CURRENTREVOKE=u[i].revoke;
				getch();
				return;
			}
		}
	}
	if(t==3)
	{
		PrintStringXY(3,24,"��������ﵽ����,ϵͳǿ���˳�.\a��������˳�.");
		getch();
		exit0();
	}
	PrintStringXY(3,24,"\a���������У�Ժ���ʹ�ã������������...");
	getch();
	t++;
	goto ag;
//***********************************************************
}
void menutable()
{
bha:;
	clrscr();
	char chr;
	gotoxy(37,7);setcolor(0,14);
	printf("��Ĳ���");setcolor(0,15);
	DrawFrame(20,60,5,15);
	gotoxy(27,9); printf("1.��Ľ���     CREATE TABLE");
	gotoxy(27,10);printf("2.���ɾ��     DROP TABLE");
	gotoxy(27,11);printf("3.������Ϣ��ʾ SHOW TABLE");
	gotoxy(27,12);printf("4.�����Ը���   PRO CHANGE");
	gotoxy(27,13);printf("0.������һ��   RETURN UPPER");
	gotoxy(27,16);setcolor(0,11);printf("��ѡ��:");setcolor(0,15);printf("[ ]");
	gotoxy(35,16);
	cin>>chr;
	switch(chr)
	{
		case '1' :tablecreate();break;
		case '2' :tabledrop();break;
		case '3' :outputts();break;
		case '4' :tablechange();break;
		case '0' :return;
		default:	gotoxy(1,16);
					printf("��������,����������!\a");
					getchar();
					goto bha;
	}
	goto bha;
}
void menusuper()
{
	if(CURRENTREVOKE==0)
	{
		gotoxy(1,24);printf("\a�Բ���,����Ȩ�޲������������.�����������...");
		getch();
		return;
	}
bha:;
	clrscr();
	char chr;
	gotoxy(37,7);setcolor(0,14);
	printf("��������");setcolor(0,15);
	DrawFrame(20,60,5,15);
	gotoxy(27,9); printf("1.ǿ�н������ļ�,����Ϊ0");
	gotoxy(27,10);printf("2.����ģʽ");
	gotoxy(27,11);printf("3.ȫ��ģʽ");
	gotoxy(27,12);printf("4.�����Ƿ��������");
	gotoxy(27,13);printf("0.������һ��   RETURN UPPER");
	gotoxy(27,14);setcolor(0,11);printf("��ѡ��:");setcolor(0,15);printf("[ ]");
	gotoxy(35,14);
	cin>>chr;
	switch(chr)
	{
		case '1' :StronglySetLock0();break;
		case '2' :NT_SetConsoleDisplayMode(GetStdHandle(STD_OUTPUT_HANDLE),0);break;
		case '3' :NT_SetConsoleDisplayMode(GetStdHandle(STD_OUTPUT_HANDLE),1);break;
		case '4' :SetSpeedFactor();break;
		case '0' :return;
		default:	gotoxy(1,16);
					printf("��������,����������!\a");
					getchar();
					goto bha;
	}
	goto bha;
}
void menurec()
{
bha:;
	clrscr();
	char chr;
	gotoxy(37,7);setcolor(0,14);
	printf("��¼����");setcolor(0,15);
	DrawFrame(20,60,5,15);
	gotoxy(27,9); printf("1.��¼����     RECORD INSERT");
	gotoxy(27,10);printf("2.��¼ɾ��     RECORD DELETE");
	gotoxy(27,11);printf("3.��¼��ѯ     RECORD QUERY");
	gotoxy(27,12);printf("4.��¼����     RECORD UPDATE");
	gotoxy(27,13);printf("0.������һ��   RETURN UPPER");
	gotoxy(27,14);setcolor(0,11);printf("��ѡ��:");setcolor(0,15);printf("[ ]");
	gotoxy(35,14);
	cin>>chr;
	switch(chr)
	{
		case '1' :recordinsert();break;
		case '2' :recorddel();break;
		case '3' :outputrec();break;
		case '4' :recordupdate();break;
		case '0' :return;
		default:	gotoxy(1,16);
					printf("��������,����������!\a");
					getchar();
					goto bha;
	}
	goto bha;
}
void menuiv()		//menu of index and view
{
bha:;
	clrscr();
	char chr;
	gotoxy(34,7);setcolor(0,14);
	printf("������ͼ����");setcolor(0,15);
	DrawFrame(20,60,5,16);
	gotoxy(27,9); printf("1.��������     INDEX CREATE");
	gotoxy(27,10);printf("2.ɾ������     INDEX DROP");
	gotoxy(27,11);printf("3.��ʾ����     INDEX SHOW");
	gotoxy(27,12);printf("4.������ͼ     VIEW CREATE");
	gotoxy(27,13);printf("5.ɾ����ͼ     VIEW DROP");
	gotoxy(27,14);printf("6.��ʾ��ͼ     VIEW SHOW");
	gotoxy(27,15);printf("0.������һ��   RETURN UPPER");
	gotoxy(27,17);setcolor(0,11);printf("��ѡ��:");setcolor(0,15);printf("[ ]");
	gotoxy(35,17);
	cin>>chr;
	switch(chr)
	{
		case '1' :indexcreate();break;
		case '2' :indexdrop();break;
		case '3' :indexshow();break;
		case '4' :viewcreate();break;
		case '5' :viewdrop();break;
		case '6' :viewtable();break;
		case '0' :return;
		default:	gotoxy(1,24);
					printf("��������,����������!\a");
					getchar();
					goto bha;
	}
	goto bha;
}
void menurelation()
{
bha:;
	clrscr();
	char chr;
	gotoxy(37,7);setcolor(0,14);
	printf("��ϵ����");setcolor(0,15);
	DrawFrame(20,60,5,21);
	gotoxy(27,9); printf("1.������          Union");
	gotoxy(27,10);printf("2.������          Jiao");
	gotoxy(27,11);printf("3.�����          DIFFERENCE");
	gotoxy(27,12);printf("4.ͶӰ����        SHADOW");
	gotoxy(27,13);printf("5.ѡ������ȵ�һ  SELECT");
	gotoxy(27,14);printf("6.ѡ�����AND     SELECT AND");
	gotoxy(27,15);printf("7.ѡ�����OR      SELECT OR");
	gotoxy(27,16);printf("8.ѡ��ͶӰ�ۺ�    SELECT WHERE");
	gotoxy(27,17);printf("9.��Ȼ/��ֵ����   NATRUAL/EQUAL");
	gotoxy(27,18);printf("0.������һ��      RETURN UPPER");
	gotoxy(27,19);setcolor(0,11);printf("��ѡ��:");setcolor(0,15);printf("[ ]");
	gotoxy(35,19);
	cin>>chr;
	switch(chr)
	{
		case '1' :relationunion();break;
		case '2' :relationjiao();break;
		case '3' :relationdiff();break;
		case '4' :relationshadow();break;
		case '5' :relationsel0();break;
		case '6' :relationseland();break;
		case '7' :relationselor();break;
		case '8' :relationselwhere();break;
		case '9' :relationconnect();break;
		case '0' :return;
		default:	gotoxy(1,24);
					printf("��������,����������!\a");
					getchar();
					goto bha;
	}
	goto bha;
}
void mainmenu()
{
	void login();
bha:;
	clrscr();
	char chr;
	gotoxy(37,7);setcolor(0,14);
	printf("���˵�");setcolor(0,15);
	DrawFrame(20,60,5,21);
	gotoxy(27,9); printf("1.��Ĳ���  ����ɾ�����");
	gotoxy(27,10);printf("2.��¼����  ����ɾ����");
	gotoxy(27,11);printf("3.������ͼ����");
	gotoxy(27,12);printf("4.��ϵ��������");
	gotoxy(27,13);printf("5.��ϵ�����ѯ����");
	gotoxy(27,14);printf("6.ϵͳ�ĳ�������");
	gotoxy(27,15);printf("7.����ȫ������");
	gotoxy(27,16);printf("8.�ָ�ָ����ĳ������");
	gotoxy(27,17);printf("9.�û��ص�½");
	gotoxy(27,18);printf("0.�˳�");
	gotoxy(27,19);setcolor(0,11);printf("��ѡ��:");setcolor(0,15);printf("[ ]");
	gotoxy(35,19);
	cin>>chr;
	switch(chr)
	{
		case '1' :menutable();break;
		case '2' :menurec();break;
		case '3' :menuiv();break;
		case '4' :menurelation();break;
//		case '5' :relationcalculate();break;
        case '5' :relationcalculate1();break;
		case '6' :menusuper();break;
		case '7' :BackUp();break;
		case '8' :ReCovery();break;
		case '9' :login();break;
		case '0' :exit0();break;
		case 'a' :test();break;
		default:	gotoxy(1,24);
					printf("��������,����������!\a");
					getchar();
					goto bha;
	}
	goto bha;
}

int main()
{
	if((alfp=fopen("log.txt","a"))==NULL)
		alfp=fopen("log.txt","w+");

	NT_SetConsoleDisplayMode(GetStdHandle(STD_OUTPUT_HANDLE),0);	//����dosȫ������
	Sleep(400);
	fprintf(alfp,"*******************************************");fputc('\n',alfp);

	fprintf(alfp,"\n[%d��%d��%d�� %d:%d:%d]ִ�����²���:\n",GetCurrentYear(),GetCurrentMonth(),GetCurrentDay(),
									GetCurrentHour(),GetCurrentMinute(),GetCurrentSecond() );
	fprintf(alfp,"����DOSȫ������.");fputc('\n',alfp);
	fprintf(alfp,"��ȡ���ݲ���ʼ��������Ϣ.");fputc('\n',alfp);
	initialize();
	fileread();
	login();
	mainmenu();
	fprintf(alfp,"\n[%d��%d��%d�� %d:%d:%d]ִ�����²���:\n",GetCurrentYear(),GetCurrentMonth(),GetCurrentDay(),
									GetCurrentHour(),GetCurrentMinute(),GetCurrentSecond() );
	fprintf(alfp,"�û������˳�����.");fputc('\n',alfp);
	fclose(alfp);
	return 0;
}

/*
ʵ��� �������ݿ⣨DDL������ģ��**************************************************************

  ��һ��ʵ��Ŀ�ģ�

  ��1���˽��ϵ���ݿⶨ�����ԵĹ��ܡ�
   ��2����Ϥ��ϵ���ݿ⽨������Ĺ��ܡ�
   ��3�������ø߼�����������Դ������ݿ�ķ�����
   ��4�������Ա����ʽ��ʾ�������ķ�����

 ������ʵ�������벽�裺

   1����C���ԣ����������ԣ�����"����"���ܵ�����˵��������˵����ı���ʽ����
	��������������Ľṹ���������ݡ���ʾ���ݿ��л�����Ľṹ�����ݡ��˳��ȹ��ܡ�
   2 ����C����ʵ�ֽ������ݿ��Ľṹ�Ĺ��ܡ�Ҫ��
    ��1�������������ϵ����
��  ��2���ֶθ�����������������������ֶ������ֶ����͡��������⣩��
    ��3��Ҫ�����ļ���ʽ������������洢�ṹҲ��������ƣ�

   3����C����ʵ���������ݿ��¼�Ĺ��ܣ�����λ�����⣩��
   4����C����ʵ��ɾ�����ݿ��¼�Ĺ��ܣ�ɾ��λ�����⣩��
   5����C����ʵ����ʾ���ݿ�ṹ�����ݣ�����Ա����ʽ��ʾ��
   6����C����ʵ�ֶ����ؼ��ֵ��������ܡ�
   7����C����ʵ�ֶ������ؼ��ֵ��������ܡ�
8����C����ʵ�ֽ�����ͼ�Ĺ��ܡ�
   9����C���Խ��������ֵ䱣������ģʽ���й���ͼ���������洢�ļ��Ķ��塣
   10�����ÿ�����ɺ���������˵��Ĺ��ܡ�

ʵ���� ��ϵ�������Եļ������㹦��ģ��**************************************************************
  ��һ��ʵ��Ŀ�ģ�

   ��1���˽⼯������Ĺ��ܡ�
   ��2�����չ�ϵ�������ԵĲ������������㡣
   ��3������C����ʵ�ֲ����������������Ʒ�����
   ��4�������Ա����ʽ��ʾ�����������������ķ�����

��������ʵ�������벽�裺

 1����Ƹ�ʵ��Ĺ��ܲ˵���
 2����C����ʵ�ֽ����������ݵĹ�ϵ��
 3����C����ʵ�ֹ�ϵ�������Ե�"��"���㹦�ܣ����Ա����ʽ��ʾ����Ľ����
 4����C����ʵ�ֹ�ϵ�������Ե�"��"���㹦�ܣ����Ա����ʽ��ʾ����Ľ����
 5����C����ʵ�ֹ�ϵ�������Ե�"��"���㹦�ܣ����Ա����ʽ��ʾ����Ľ����
 6�����ÿ�����ɺ���������˵��Ĺ��ܡ�

ʵ���� ��ϵ�������Ե�ר�����㹦��ģ��**************************************************************

��һ��ʵ��Ŀ�ģ�
��1����Ϥ�߼����Ա�������˼�롣
��2�����շ��Ŵ�����ķ����ͼ�����
��3�����չ�ϵ�������Ե�ѡȡ��ͶӰ����������Ĺ��ܡ�
��4�����չ�ϵ�������Ե�ѡȡ��ͶӰ�����������ʵ�ּ�����
��5�����չ�ϵ���ݿ�Ĳ�ѯ�Ż�������
��
������ʵ�������벽�裺

1������ϵ�������Ե�ѡȡ��ͶӰ���㹫ʽ���еȼ۱任���Ա���������������������ܺ�ʶ��
2����������һ��ϵ���ݿ⡣
3�����������ķ�ʽ����C����ʵ�ֶԸÿ��й�ϵ��ĳһ�ֶε�"��"��=���١��ܡ��ݡ�<��>��������ѡȡ���㣬
	���Ա����ʽ��ʾ����Ľ����

4�����������ķ�ʽ����C����ʵ�ֶԸÿ��й�ϵ�Ĵ���"��"�����Ĳ�ѯ�����Ա����ʽ��ʾ����Ľ����
5�����������ķ�ʽ����C����ʵ�ֶԸÿ��й�ϵ�Ĵ���"��"�����Ĳ�ѯ�����Ա����ʽ��ʾ����Ľ����

6����C����ʵ�ֶԸÿ�������ֶε�ͶӰ���㣬���Ա����ʽ��ʾ����Ľ����

7����C����ʵ�ֶԸÿ��ѡȡ��ͶӰ�ۺ����㣬���Ա����ʽ��ʾ����Ľ����

8�����������ķ�ʽ����C����ʵ�ֶ�������ϵ����Ȼ���Ӻ͵�ֵ�������㣬���Ա����ʽ��ʾ����Ľ����

ʵ���� �򵥵Ĺ�ϵ���㣨DML������ģ��                10************************************************

��һ��ʵ��Ŀ�ģ�

��1�����չ�ϵ�������ԵĲ�ѯ���ܡ�
��2����Ϥ���Ŵ�ʶ��ʹ�������
��3�����չ�ϵ�������ԵĲ�ѯ���ܵ�ʵ�ַ����ͼ�����

������ʵ�������벽�裺

��1��ϵͳ����DBMS�й�ϵ�������ԵĹ��ܣ���QUEL����Ϊ��������C����ʵ�ֶ��ù涨���ű���
     �򵥲�ѯ�����н���ɨ���ʶ�𣬲��Ա����ʽ��ʾ��ѯ�������
��2����ϤQUEL���ԵĲ�ѯ���ܣ��ø߼�����ʵ�ֶԺ��С��롱�����򡱡����ǡ�
     ���ŵ������н���ɨ���ʶ�𣬲��Ա����ʽ��ʾ��ѯ�����

ʵ���� ��ϵ���ݿ�Ĳ�ѯ�Ż�                        5**********************************************************
��һ��ʵ��Ŀ�ģ�
��1�����չ�ϵϵͳ�Ķ��弰���ࡣ
��2���˽�ȫ��ϵϵͳ�ĸ��
��3���˽��Ż���һ����ԡ���
��4�����չ�ϵ�����ȼ۱任���򡣪�
��5�����չ�ϵ���ݿ�ϵͳ�Ĳ�ѯ�Ż�������
��6�����չ�ϵ�������ʽ���Ż��㷨����

������ʵ�������벽�裺
 1����������ʽ��ϵ�����Ż��㷨��
(1)�Ѳ�ѯת����ĳ���ڲ���ʾ����
(2)���﷨��ת���ɱ�׼��ʽ��
(3)ѡ��Ͳ�Ĵ�ȡ·����
(4) �ø߼�����ʵ�ֹ�ϵ�������ʽ�Ż��㷨������ԭʼ���﷨�������Ż��������ɲ�ѯ�ƻ���ѡ�������С�ġ�����
2���ø߼�����ʵ������ʽ��ϵ�����Ż��㷨��

ʵ����  ���ݿⱣ����DCL������ģ��**********************************************************

��һ��ʵ��Ŀ�ģ�

��1���������ݿ�ϵͳ�İ�ȫ�Ա������ơ�		2*************************
��2���������ݿ�ϵͳ��������Լ�����ơ�		2*************************
��3���˽����ݿ�ϵͳ�Ĳ������Ƽ�����			8*************************
��4���������ݿ�ϵͳ�Ļָ�������				8*************************

������ʵ�������벽�裺

��1��Ҫ����������ֵ䣨DD��ʵ�ֶ����ݿ�İ�ȫ�Լ�顣�������ÿ�������Ȩ�޵ȡ�
��2�����������ֵ䣨DD��ʵ�ֶ����ݿ�ϵͳ��������Լ�����ơ�
��3����������ķ�������ʵ�ֶ�������Ĳ������ƣ�
��4�����ý�����־�ļ��������Ա��ݵĻ���ʵ�ֶ����ݿ�ϵͳ�Ļָ���

ʵ���   �ۺ����**************************************************************************

��һ��ʵ��Ŀ�ģ�

��1��ϵͳ�����չ�ϵ�������Ե����㹦�ܡ�
��2���˽�DBMS��DDL��DML��DCL�ȹ��ܵ�ʵ�ʺ��塣
��3�����սṹ����Ʒ�����
��4���˽�����������Ƽ�����


������ʵ�������벽�裺
��1������������̵Ľṹ����Ʒ�������ǰ����ʵ�����ýṹ����Ʒ����ۺ�Ϊһ��������DBMSԭ��ϵͳ���Դﵽϵͳ���տ������ϵͳ�ķ����ͼ�����Ŀ�ġ�
��2����Ƹ�ϵͳ������˵���
��3��ͨ������˵�ѡ��ĳһ���ܽ����Ӳ˵���
��4��ͨ��ĳһ�����Ӳ˵���������Ӧ������ϵͳ����ɾ���Ĺ��ܡ�
��5��ͨ���������ߣ���һ���ḻ�������������򴰿ڡ��˵��Ľ�����ơ�
*/
