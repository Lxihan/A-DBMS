#include "stdio.h"
#include "stdlib.h"
#include <bits/stdc++.h>
#include "string.h"
#include "conio.h"
#include "windows.h"
#include <iostream>
#include "power.h"

using namespace std;

#define MAX_PROLEN 10	//最大属性字符长度
#define MAX_PRONUM 8	//最大属性个数
#define MAX_RECNUM 30	//最大记录个数
#define MAX_TNAME 15	//最长表名
#define MAX_TNUM 15		//最大表个数
#define MAX_VIEWNUM 15		//最大表个数
#define MAX_USERLEN 10	//用户名长度
#define MAX_PWDLEN 18	//密码最大长度
#define MAX_USERNUM 10	//用户名最大个数

//***************************
typedef struct
{
	int re_num;		//记录的个数
	int pro_num;	//属性个数
	int key_no;		//规定关键字是第几个属性,使用的是下标从0开始
	//int pro_len[MAX_PRONUM];	//属性长度
	char key[MAX_PROLEN];		//定义关键字,关键字也是一个属性
	char tname[MAX_TNAME];		//表名
	char proname[MAX_PRONUM][MAX_PROLEN];	//属性的名字
	char protype[MAX_PRONUM];	//属性的类型
	char record[MAX_RECNUM][MAX_PRONUM][MAX_PROLEN];		//三维表,保存的内容是记录的值
}table;

typedef struct
{
	char proname[MAX_PRONUM][MAX_PROLEN];	//可见的属性
	int prono[MAX_PRONUM];		//可见属性的下标
	char tname[MAX_TNAME];		//对应的表名
	char vname[MAX_TNAME];		//视图名
	int pro_num;
}view;		//视图，该试图对应一个表，一个表可以对应多个视图

typedef struct
{
	char iname[MAX_TNAME];		//一个索引对应一个表
	int key_num;
	char key[MAX_RECNUM][MAX_PROLEN];	//
}index;
typedef struct
{
	char username[MAX_USERLEN];
	char pwd[MAX_PWDLEN];
	int revoke;		//0是glz，1是nb
}user;

table NULLTABLE;	//空表，初始化的时候定义NULLTABLE.key_no=-1
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
		printf("不存在锁文件,请检查后再使用.\a");
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
		printf("不存在锁文件,请检查后再使用.\a");
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
		printf("不存在锁文件,请检查后再使用.\a");
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
		gotoxy(1,24);printf("\a对不起,您的权限不允许这项操作.按任意键返回...");
		getch();
		return;
	}
	if(!dbislock())		//没有锁，那就加锁@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		dblock();	//dblock();     dbunlock();
	else
	{
		clrscr();
		printf("数据库处于锁状态，即使超级用户也不能进行修改!\a");
		printf("\n按任意键返回...");
		getch();
		return;
	}
	char filename[20];
	clrscr();
	PrintColorStringXY(36,7,0,14,0,15,"数据的备份");
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("输入备份数据的文件名:");
	gotoxy(27,10);
	gets(filename);
	FILE *fp;
	if((fp=fopen(filename,"wb"))==NULL)
		fp=fopen(filename,"wb+");
	//以下共6个文件提供全体打包保存dblock.db,table.db,index.db,view.db,dic.db,log.txt
//*****************对dblock.db的备份********************************************
	int databaselock;
	if(dbislock())
		databaselock=1;
	else
		databaselock=0;
	fwrite(&databaselock,sizeof(int),1,fp);
	PrintStringXY(1,24,"备份dblock.db\a..   ");
	Sleep(300);
//******************对table.db的备份********************************************
	fwrite(&t_num,sizeof(int),1,fp);
	fwrite(t,sizeof(table),MAX_TNUM,fp);
	PrintStringXY(1,24,"备份table.db\a..   ");
	Sleep(300);
//******************对index.db的备份********************************************
	fwrite(&i_num,sizeof(int),1,fp);
	fwrite(ind,sizeof(index),MAX_TNUM,fp);
	PrintStringXY(1,24,"备份index.db\a..   ");
	Sleep(300);
//******************对view.db的备份**********************************************
	fwrite(&v_num,sizeof(int),1,fp);
	fwrite(vi,sizeof(view),MAX_VIEWNUM,fp);
	PrintStringXY(1,24,"备份view.db\a..   ");
	Sleep(300);
//******************对dic.db的备份***********************************************
	//dic.db是数据字典，不需要备份，只是在恢复的时候从table.db中分解出来即可.
//******************对log.txt的备份***********************************************
	FILE *fp1;
	char ch;
	if((fp1=fopen("log.txt","rb"))==NULL)
	{
		printf("不存在日志文件log.txt,本次备份未完全成功.\a");
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
	PrintStringXY(1,24,"备份log.txt\a..   ");
	Sleep(300);
	PrintStringXY(1,24,"恭喜，所有数据已成功备份于");
	printf("%s",filename);
	printf("\n按任意键返回...");
	getch();
	if(dbislock())
		dbunlock();
}

void ReCovery()
{
	//对某个指定的备份进行恢复
		if(CURRENTREVOKE==0)
	{
		gotoxy(1,24);printf("\a对不起,您的权限不允许这项操作.按任意键返回...");
		getch();
		return;
	}
	if(!dbislock())		//没有锁，那就加锁@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		dblock();	//dblock();     dbunlock();
	else
	{
		clrscr();
		printf("数据库处于锁状态，即使超级用户也不能进行修改!\a");
		printf("\n按任意键返回...");
		getch();
		return;
	}
	char filename[20];
	clrscr();
	PrintColorStringXY(36,7,0,14,0,15,"数据的恢复");
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("输入恢复数据的文件名:");
	gotoxy(27,10);
	gets(filename);
	FILE *fp;
	FILE *fp2;
	if((fp=fopen(filename,"rb"))==NULL)
	{
		PrintStringXY(1,24,"未发现此备份文件!按任意键返回...\a");
		getch();
		if(dbislock())
		dbunlock();
		return;
	}
	//以下文件提供全体读取dblock.db,table.db,index.db,view.db,log.txt
//*****************对dblock.db的恢复********************************************
	int databaselock;
	fread(&databaselock,sizeof(int),1,fp);
	if((fp2=fopen("dblock.db","wb"))==NULL)
		fp2=fopen("dblock.db","wb+");
	fwrite(&databaselock,sizeof(int),1,fp2);
	fclose(fp2);
	PrintStringXY(1,24,"恢复dblock.db\a..   ");
	Sleep(300);
//******************对table.db的恢复********************************************
	fread(&t_num,sizeof(int),1,fp);
	fread(t,sizeof(table),MAX_TNUM,fp);
	if((fp2=fopen("table.db","wb"))==NULL)
		fp2=fopen("table.db","wb+");
	fwrite(&t_num,sizeof(int),1,fp2);
	fwrite(t,sizeof(table),MAX_TNUM,fp2);
	fclose(fp2);
	PrintStringXY(1,24,"恢复table.db\a..   ");
	Sleep(300);
//******************对index.db的恢复********************************************
	fread(&i_num,sizeof(int),1,fp);
	fread(ind,sizeof(index),MAX_TNUM,fp);
	if((fp2=fopen("index.db","wb"))==NULL)
		fp2=fopen("index.db","wb+");
	fwrite(&i_num,sizeof(int),1,fp2);
	fwrite(ind,sizeof(index),MAX_TNUM,fp2);
	fclose(fp2);
	PrintStringXY(1,24,"恢复index.db\a..   ");
	Sleep(300);
//******************对view.db的恢复**********************************************
	fread(&v_num,sizeof(int),1,fp);
	fread(vi,sizeof(view),MAX_VIEWNUM,fp);
	if((fp2=fopen("view.db","wb"))==NULL)
		fp2=fopen("view.db","wb+");
	fwrite(&v_num,sizeof(int),1,fp2);
	fwrite(vi,sizeof(view),MAX_VIEWNUM,fp2);
	fclose(fp2);
	PrintStringXY(1,24,"恢复view.db\a..   ");
	Sleep(300);
//******************对dic.db的恢复***********************************************
	//dic.db是数据字典，不需要备份，只是在恢复的时候从table.db中分解出来即可.
	int i,j;
	if((fp2=fopen("dic.db","wb"))==NULL)
		fp2=fopen("dic.db","wb+");
	for(i=0;i<t_num;i++)
	{
		fprintf(fp2,"表名:%s   属性数量:%d   主键:%s",t[i].tname,t[i].pro_num,t[i].key);
		fprintf(fp2,"\n----------------------------------------------------\n");
		for(j=0;j<t[i].pro_num;j++)
			fprintf(fp2,"%s(%c)    ",t[i].proname[j],t[i].protype[j]);
		fprintf(fp,"\n\n\n");
	}
	fclose(fp2);
	PrintStringXY(1,24,"恢复dic.db\a..   ");
	Sleep(300);
//******************对log.txt的备份***********************************************
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
	PrintStringXY(1,24,"恢复log.txt\a..   ");
	Sleep(300);
	PrintStringXY(1,24,"恭喜，所有数据已成功恢复自");
	printf("%s",filename);
	printf("\n按任意键返回...");
	getch();
	if(dbislock())
		dbunlock();
}
void fileread()
{
	clrscr();
	FILE *fp;

	//*****************读取config.db内容
	if((fp=fopen("config.db","rb"))==NULL)
	{
		PrintStringXY(1,24,"没有发现系统配制文件config.db");
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
	printf("\n\n正在解析table.db...");
	Sleep(800/speedfactor);
	printf("\n正在载入t_num...");
	Sleep(500/speedfactor);
//这里可以做出动画来!!!!!!时间太太太有限了,这里就现不做了.
	fread(t,sizeof(table),MAX_TNUM,fp);
	fclose(fp);
	for(ip=0;ip<t_num;ip++)
	{
		printf("OK\n\n正在载入表%s...",t[ip].tname);
		Sleep(250/speedfactor);
		for(jp=0;jp<t[ip].re_num;jp++)
		{
			printf("OK\n正在载入记录%s...",t[ip].record[jp][t[ip].key_no]);
			Sleep(100/speedfactor);
		}
	}
//下面读取view.db的内容。
	if((fp=fopen("view.db","rb"))==NULL)
	{
		fp=fopen("dic.db","wb+");
		fclose(fp);
	}
	fread(&v_num,sizeof(int),1,fp);
	fread(vi,sizeof(view),MAX_VIEWNUM,fp);
	fclose(fp);
	printf("OK\n\n正在解析view.db...");
	Sleep(800/speedfactor);
	printf("\n正在载入v_num...");
	Sleep(500/speedfactor);
	for(ip=0;ip<v_num;ip++)
	{
		printf("OK\n正在载入视图%s...OK",vi[ip].vname);
		Sleep(100/speedfactor);
	}
//下面添加读取index.db的代码
	if((fp=fopen("index.db","rb"))==NULL)
	{
		fp=fopen("dic.db","wb+");
		fclose(fp);
	}
	fread(&i_num,sizeof(int),1,fp);
	fread(ind,sizeof(index),MAX_TNUM,fp);
	fclose(fp);
	printf("\n\n正在解析index.db...");
	Sleep(800/speedfactor);
	printf("\n正在载入i_num...");
	Sleep(100/speedfactor);
	for(ip=0;ip<i_num;ip++)
	{
		printf("OK\n正在载入视图%s...",ind[ip].iname);
		Sleep(100/speedfactor);
	}
	printf("OK\n\n成功载入所有数据!");
	printf("\n\n加载用户名于密码...");
	Sleep(200/speedfactor);
	printf("OK\n\n解析用户的权限...");
	Sleep(200/speedfactor);
	printf("OK\n\n加载虚拟可视化表格效果与颜色渲染");
	for(ip=0;ip<9;ip++)
	{
		Sleep(200/speedfactor);
		printf(".");
	}
	printf("OK\n\n加载进度完成!\a等待进入系统...");
	Sleep(1000/speedfactor);
}
void filewrite()
{
	FILE *fp;
//*****************读取config.db内容
	if((fp=fopen("config.db","wb"))==NULL)
		fp=fopen("config.db","wb+");
	fwrite(&speedfactor,sizeof(int),1,fp);
	fclose(fp);
//写入table.db的代码
	if((fp=fopen("table.db","wb"))==NULL)
	{
		printf("不存在table.db文件,请检查后再使用.\a");
		getchar();
		exit(-1);
	}
	fwrite(&t_num,sizeof(int),1,fp);
	fwrite(t,sizeof(table),MAX_TNUM,fp);
	fclose(fp);
//以下是写入dic.db的代码
	int i,j;
	if((fp=fopen("dic.db","wb"))==NULL)
	{
		printf("不存在dic.db文件,请检查后再使用.\a");
		getchar();
		exit(-1);
	}
	for(i=0;i<t_num;i++)
	{
		fprintf(fp,"表名:%s   属性数量:%d   主键:%s",t[i].tname,t[i].pro_num,t[i].key);
		fprintf(fp,"\n----------------------------------------------------\n");
		for(j=0;j<t[i].pro_num;j++)
			fprintf(fp,"%s(%c)    ",t[i].proname[j],t[i].protype[j]);
		fprintf(fp,"\n\n\n");
	}
	fclose(fp);
//以下是写入view.db的代码
	if((fp=fopen("view.db","wb"))==NULL)
	{
		printf("不存在view.db文件,请检查后再使用.\a");
		getchar();
		exit(-1);
	}
	fwrite(&v_num,sizeof(int),1,fp);
	fwrite(vi,sizeof(view),MAX_VIEWNUM,fp);
	fclose(fp);
//以下是写入index.db的代码
	if((fp=fopen("index.db","wb"))==NULL)
	{
		printf("不存在index.db文件,请检查后再使用.\a");
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
	if(r->pro_num!=s->pro_num)		//属性个数不匹配
		return false;
	for(p=0;p<s->pro_num;p++)		//属性名字不匹配
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
		gotoxy(1,24);printf("\a对不起,您的权限不允许这项操作.按任意键返回...");
		getch();
		return;
	}
	if(!dbislock())		//没有锁，那就加锁@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		dblock();	//dblock();     dbunlock();
	else
	{
		clrscr();
		printf("数据库处于锁状态，即使超级用户也不能进行修改!\a");
		printf("\n按任意键返回...");
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
	printf("建表操作");setcolor(0,15);
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("输入新建表的名称:");
	//gets(mn);
	cin>>mn;
	//添加重名处理与错误处理
	strcpy(tnew.tname,mn);	//新表表名
	gotoxy(27,10); printf("输入新建表的属性个数:");
	cin>>nnum;
	//添加错误处理
	if(nnum>=MAX_TNUM)
	{
		gotoxy(1,20);printf("已经达到表的最大个数,无法建表\a!");
		goto eotc1;
	}
	tnew.pro_num=nnum;
	//开始循环输入属性名称;
	//注意重名的属性
	gotoxy(1,15);
	i=0;
	while(i<tnew.pro_num)
	{
		printf("\n请输入第%d个属性名称:",i+1);
		//gets(mn);
		cin>>mn;
		//重名处理!!!!!
		strcpy(tnew.proname[i],mn);
		printf("\n请输入第%d个属性类型n=num  c=char:",i+1);
		//错误处理,不符合的类型要排除掉!
		cin>>tnew.protype[i];
		i++;
	}

	//定义主键,主键是唯一表示一个记录的属性,必须要有主键.主键的位置是tnew.key,主键在属性中的下标.
	printf("\n主键对于一个表是必不可少的.\n");
	printf("请输入主键的名称,输入错误则自动选择第一个属性:");
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
	printf("\n输入错误,主键自动选择为第一个属性:\a%s",tnew.proname[0]);
	strcpy(tnew.key,tnew.proname[0]);
	tnew.key_no=0;
nex1:;
	tnew.re_num=0;		//记录表设置为空
	for(i=0;i<MAX_RECNUM;i++)
		for(j=0;j<MAX_PRONUM;j++)
			for(k=0;k<MAX_PROLEN;k++)
				tnew.record[i][j][k]='\0';	//初始化记录表
	t[t_num++]=tnew;
	printf("\n建表成功,回车返回上一级菜单.");
	getch();
eotc1:;
	dbunlock();		//解锁@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	fprintf(alfp,"\n[%d年%d月%d日 %d:%d:%d]执行如下操作:\n",GetCurrentYear(),GetCurrentMonth(),GetCurrentDay(),
									GetCurrentHour(),GetCurrentMinute(),GetCurrentSecond() );
	fprintf(alfp,"用户建立表成功,表的内容是:表名:%s,表属性个数:%d,主键:%s.",tnew.tname,
								tnew.pro_num,tnew.key);fputc('\n',alfp);
}
void tabledrop()
{
	if(CURRENTREVOKE==0)
	{
		gotoxy(1,24);printf("\a对不起,您的权限不允许这项操作.按任意键返回...");
		getch();
		return;
	}
	if(!dbislock())		//没有锁，那就加锁@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		dblock();	//dblock();     dbunlock();
	else
	{
		clrscr();
		printf("数据库处于锁状态，即使超级用户也不能进行修改!\a");
		printf("\n按任意键返回...");
		getch();
		return;
	}
	clrscr();
	char mn[MAX_TNAME];
	int i,j;
	table tdel;
	gotoxy(37,7);setcolor(0,14);
	printf("表删除操作");setcolor(0,15);
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("输入删除的表名:");
	//gets(mn);
	cin>>mn;
	//添加错误处理
	for(i=0;i<t_num;i++)
		if(strcmp(mn,t[i].tname)==0)
		{
			//i的值就是要删除的表的位置.
			tdel=t[i];
			for(j=i;j<t_num-1;j++)
				t[j]=t[j+1];
			t_num--;
			goto eodrp;
		}
	gotoxy(1,20);printf("未找到要删除的表,请确认输入.\a");
	getchar();
	dbunlock();
	return;
eodrp:;
	gotoxy(1,20);
	printf("表%s已成功删除!回车返回上一级菜单.",mn);
	getch();
	dbunlock();
		fprintf(alfp,"\n[%d年%d月%d日 %d:%d:%d]执行如下操作:\n",GetCurrentYear(),GetCurrentMonth(),GetCurrentDay(),
									GetCurrentHour(),GetCurrentMinute(),GetCurrentSecond() );
	fprintf(alfp,"用户成功删除一个表,表的内容是:表名:%s,表属性个数:%d,主键:%s.",tdel.tname,
								tdel.pro_num,tdel.key);fputc('\n',alfp);
}


void tablechange() //功能4、表属性更改
{
    getchar();
	if(CURRENTREVOKE==0)
	{
		gotoxy(1,24);printf("\a对不起,您的权限不允许这项操作.按任意键返回...");
		getch();
		return;
	}
	if(!dbislock())		//没有锁，那就加锁@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		dblock();	//dblock();     dbunlock();
	else
	{
		clrscr();
		printf("数据库处于锁状态，即使超级用户也不能进行修改!\a");
		printf("\n按任意键返回...");
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
	printf("增/删/改表项信息操作");setcolor(0,15);
	DrawFrame(20,60,5,12);
	gotoxy(27,9); printf("输入想要操作的表名称:");
	cin>>mn;
	getchar();
    gotoxy(1,13);

    //-----------------显示现有表项信息-------------------
    for(i=0;i<t_num;i++)
    {
        if(strcmp(mn,t[i].tname)==0)
        {
            printf("该表当前的表项信息如下：\n");
            printf("\n表名:%s   属性个数:%d    主键:%s",t[i].tname,t[i].pro_num,t[i].key);
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
        printf("\n未找到要查询的表,按回车返回上一级菜单.");
        getchar();
        return;
    }

    //-----------------选择增/删/改表项信息-------------------
    int flag=-1;
    printf("请选择你想执行的操作：\n");
	printf("0、修改表名；1、修改字段；2、新增字段；3、删除字段；4、重选主键；\n");
	cin>>flag;
	getchar();

    if(flag==0)  //0、修改表名；（一次仅修改一个字段）
    {
        printf("请输入新的表名:");
        cin>>mn_new;
        getchar();

        for(i=0;i<t_num;i++)
        {
            if(strcmp(mn,t[i].tname)==0)//确定表
            {
                strcpy(t[i].tname,mn_new);
                strcpy(mn,mn_new);
                printf("\n\n修改成功！");
                break;
            }
        }

    }
    else if(flag==1)  //1、修改字段名及字段类型；（一次仅修改一个字段）
    {
        printf("请输入想要修改的字段名:");
	    cin>>wn;
	    getchar();

        for(i=0;i<t_num;i++)
        {
            if(strcmp(mn,t[i].tname)==0)//确定表
            {
                for(j=0;j<t[i].pro_num;j++)
                {
                    if(strcmp(wn,t[i].proname[j])==0)//确定字段
                    {
                        printf("请输入新的字段名:");
                        cin>>t[i].proname[j];
                        getchar();
                        printf("请重新选择该字段的类型：n=num，c=char：");
                        cin>>t[i].protype[j];
                        getchar();
                        break;
                    }
                }
                if(j==t[i].pro_num)
                {
                    printf("\n未找到要修改的字段名,按回车返回上一级菜单.");
                    getchar();
                    return;
                }
                else
                    printf("\n\n修改成功！");
                break;
            }
        }

    }
    else if(flag==2)  //2、新增字段；（一次可新增多个字段，默认初始值为空）
    {
        printf("请输入想要新增的字段个数:");
        cin>>nnum;
        getchar();

        for(i=0;i<t_num;i++)
        {
            if(strcmp(mn,t[i].tname)==0)//确定表
            {
                t[i].pro_num = t[i].pro_num + nnum;//更改属性总个数
                for(j=t[i].pro_num-nnum;j<t[i].pro_num;j++)
                {
                    printf("请输入新增的第%d个字段名:",j-(t[i].pro_num-nnum)+1);
                    cin>>wn;
                    getchar();
                    strcpy(t[i].proname[j],wn);
                    printf("请输入新增的第%d个字段的类型：n=num，c=char：",j-(t[i].pro_num-nnum)+1);
                    cin>>t[i].protype[j];
                    getchar();
                }
                printf("\n\n新增成功！");
                break;
            }
        }

    }
    else if(flag==3)  //3、删除字段；（一次仅删除一个字段）
    {
        printf("请输入想要删除的字段名:");
        cin>>wn;
	    getchar();

        for(i=0;i<t_num;i++)
        {
            if(strcmp(mn,t[i].tname)==0)//确定表
            {
                for(j=0;j<t[i].pro_num;j++)
                {
                    if(strcmp(wn,t[i].proname[j])==0)//确定字段
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
                    printf("\n未找到想要删除的字段名,按回车返回上一级菜单.");
                    getchar();
                    return;
                }
                else
                {
                    t[i].pro_num = t[i].pro_num - 1;
                    printf("\n\n删除成功！");
                }
                break;
            }
        }

    }
    else  //4、重选主键
    {
        printf("（注：请确认你所选的字段满足作为主键的要求！）\n");
        printf("请输入重新选定的主键:");
        cin>>ck;
	    getchar();

        for(i=0;i<t_num;i++)
        {
            if(strcmp(mn,t[i].tname)==0)//确定表
            {
                for(j=0;j<t[i].pro_num;j++)
                {
                    if(strcmp(ck,t[i].proname[j])==0)//确定字段
                    {
                        t[i].key_no=j;
                        strcpy(t[i].key,ck);
                        break;
                    }
                }
                if(j==t[i].pro_num)
                {
                    printf("\n未找到重新选定的主键名,按回车返回上一级菜单.");
                    getchar();
                    return;
                }
                else
                    printf("\n\n重选成功！");
                break;
            }
        }

    }

    //-----------------显示修改以后的表项信息-------------------
    for(i=0;i<t_num;i++)
    {
        if(strcmp(mn,t[i].tname)==0)
        {
            printf("修改后的表项信息如下：\n");
            printf("\n表名:%s   属性个数:%d    主键:%s",t[i].tname,t[i].pro_num,t[i].key);
            printf("\n----------------------------------------------------------------\n");
            for(j=0;j<t[i].pro_num;j++)
            {
                printf("%s(%c)\t",t[i].proname[j],t[i].protype[j]);
            }
            printf("\n\n");
            break;
        }
    }

    //-----------------解锁-------------------
	if(dbislock())
		dbunlock();

    printf("\n表信息输出结束，按回车返回上一级菜单.");
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
		gotoxy(1,24);printf("\a对不起,您的权限不允许这项操作.按任意键返回...");
		getch();
		return;
	}
	if(!dbislock())		//没有锁，那就加锁@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		dblock();	//dblock();     dbunlock();
	else
	{
		clrscr();
		printf("数据库处于锁状态，即使超级用户也不能进行修改!\a");
		printf("\n按任意键返回...");
		getch();
		return;
	}
	clrscr();
	char mn[MAX_TNAME];
	char mq[MAX_TNAME];
	int i,j;
	gotoxy(37,7);setcolor(0,14);
	printf("记录插入操作");setcolor(0,15);
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("输入插入的表名:");
	//gets(mn);
    cin>>mn;

	//添加错误处理
	for(i=0;i<t_num;i++)
		if(strcmp(mn,t[i].tname)==0)
		{
			//i的值就是要删除的表的位置.
			goto eodrp;
		}
	gotoxy(1,20);printf("未找到要删除的表,请确认输入.\a");
	getch();
	if(dbislock())
		dbunlock();
	return;
eodrp:;//找到表所在位置,接下来是插入记录动作,表t[i]是查到的表.
	//添加判断,是否可以继续添加记录而不达到满值.错误处理
	strcpy(mq,mn);		//本函数到最后用到mq变量
	if(t[i].re_num>=MAX_RECNUM)
	{
		gotoxy(1,20);printf("该表已经达到记录的最大值!无法插入.\a");
		getch();
		if(dbislock())
		dbunlock();
		return;
	}
	gotoxy(1,20);
	for(j=0;j<t[i].pro_num;j++)
	{
		printf("\n请输入属性%s的值:",t[i].proname[j]);
		//gets(mn);
		cin>>mn;
		//添加错误处理,包括主键不可以重复等等.
		strcpy(t[i].record[t[i].re_num][j],mn);		//把属性值赋给相应的记录属性位置.
				//re_num++
		//////////////////////////////////////////////////////////////////////////////////////////////////////////
	}
	t[i].re_num++;

	printf("\n表%s已经被成功地插入了一个新记录!回车返回上一级菜单.",mq);
	getch();
	if(dbislock())
		dbunlock();
}

void recorddel()
{
	if(CURRENTREVOKE==0)
	{
		gotoxy(1,24);printf("\a对不起,您的权限不允许这项操作.按任意键返回...");
		getch();
		return;
	}
	if(!dbislock())		//没有锁，那就加锁@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		dblock();	//dblock();     dbunlock();
	else
	{
		clrscr();
		printf("数据库处于锁状态，即使超级用户也不能进行修改!\a");
		printf("\n按任意键返回...");
		getch();
		return;
	}
	clrscr();
	char mn[MAX_TNAME];
	char mq[MAX_TNAME];
	char zj[MAX_PROLEN];
	int i,j,k;
	gotoxy(37,7);setcolor(0,14);
	printf("记录删除操作");setcolor(0,15);
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("输入删除记录的表名:");
	//gets(mn);
    cin>>mn;
	//添加错误处理
	for(i=0;i<t_num;i++)
		if(strcmp(mn,t[i].tname)==0)
		{
			//i的值就是要删除的表的位置.
			goto eodrd;
		}
	gotoxy(1,20);printf("未找到要删除记录的表,请确认输入.\a");
	getch();
	if(dbislock())
		dbunlock();
	return;
eodrd:;
	//找到表所在位置,接下来是插入记录动作,表t[i]是查到的表.
	//添加判断,是否可以继续添加记录而不达到满值.错误处理
	strcpy(mq,mn);		//本函数到最后用到mq变量
	if(t[i].re_num<=0)
	{
		gotoxy(1,20);printf("该表无记录!无法删除.\a");
		getch();
		if(dbislock())
			dbunlock();
		return;
	}
	gotoxy(27,10); printf("输入要删除记录的主键值:");
	//gets(zj);	//输入主键的值
	cin>>zj;

	//添加错误处理,判断是否有词记录
	for(j=0;j<t[i].re_num;j++)
	{
		if( strcmp( t[i].record[j][t[i].key_no] , zj ) ==0)
			goto findkey1;
	}
	gotoxy(1,20);printf("未找到该记录,请确认输入.\a");
	getch();
	if(dbislock())
		dbunlock();
	return;
findkey1:;
	//找到该记录,开始进行删除操作.j就是要删除的记录的下标
	for(;j<t[i].re_num-1;j++)
	{
		for(k=0;k<t[i].pro_num;k++)
			strcpy(t[i].record[j][k],t[i].record[j+1][k]);
	}
	t[i].re_num--;
	printf("\n表%s已经被成功地删除了一个记录!回车返回上一级菜单.",mq);
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
		gotoxy(1,24);printf("\a对不起,您的权限不允许这项操作.按任意键返回...");
		getch();
		return;
	}
	if(!dbislock())		//没有锁，那就加锁@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		dblock();	//dblock();     dbunlock();
	else
	{
		clrscr();
		printf("数据库处于锁状态，即使超级用户也不能进行修改!\a");
		printf("\n按任意键返回...");
		getch();
		return;
	}
	clrscr();
	char mn[MAX_TNAME];	//为了错误处理所建立的变量
	char zj[MAX_PROLEN];
	int pnum;
	int i,j,k;
	view vnew;
	gotoxy(37,7);setcolor(0,14);
	printf("建立视图");setcolor(0,15);
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("输入视图名:");
	//gets(vnew.vname);	//输入视图名
	cin>>vnew.vname;

	//添加错误处理，重名的视图等等
	gotoxy(27,10); printf("输入视图对应的表名:");
	//gets(mn);
	cin>>mn;
	for(i=0;i<t_num;i++)
		if(strcmp(t[i].tname,mn)==0)
			goto sse;
	gotoxy(1,20);printf("没有找到该表,请确认输入\a");
	getch();
	if(dbislock())
		dbunlock();
	return;
sse:;
	//找到 该表，i就是表的下标
	strcpy(vnew.tname,mn);
	gotoxy(1,20);
	printf("请输入视图显示的属性个数:");
	cin>>pnum;
	vnew.pro_num=pnum;
	//加入错误处理，在这里 不多说了。
	k=0;
	while(k<pnum)
	{
		printf("\n请输入第%个属性名:",k+1);
		//gets(zj);
		cin>>zj;

		strcpy(vnew.proname[k],zj);
		for(j=0;j<t[i].pro_num;j++)
			if(strcmp(t[i].proname[j],zj)==0)
				goto asqq;
		printf("\a\n没有找到该属性.");
		getch();
		if(dbislock())
			dbunlock();
		return;
asqq:;
		vnew.prono[k]=j;
		k++;
	}
	vi[v_num++]=vnew;
	printf("成功建立了视图%s",vnew.vname);
	getch();
	if(dbislock())
			dbunlock();
}
void viewdrop()
{
	if(CURRENTREVOKE==0)
	{
		gotoxy(1,24);printf("\a对不起,您的权限不允许这项操作.按任意键返回...");
		getch();
		return;
	}
	if(!dbislock())		//没有锁，那就加锁@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		dblock();	//dblock();     dbunlock();
	else
	{
		clrscr();
		printf("数据库处于锁状态，即使超级用户也不能进行修改!\a");
		printf("\n按任意键返回...");
		getch();
		return;
	}
	clrscr();
	char mn[MAX_TNAME];	//为了错误处理所建立的变量
	int i;
	gotoxy(37,7);setcolor(0,14);
	printf("放弃视图");setcolor(0,15);
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("输入视图名:");
	//gets(mn);	//输入视图名
	cin>>mn;
	//添加错误处理，重名的视图等等
	if(v_num<=0)
	{
		gotoxy(1,20);
		printf("视图数为0，无法删除视图\a");
	}
	for(i=0;i<v_num;i++)
	{
		if(strcmp(mn,vi[i].vname)==0)
			goto ae;
	}
	printf("\n没有找到该视图%s\a",mn);
	getch();
	if(dbislock())
		dbunlock();
	return;
ae:;for(;i<v_num-1;i++)
		vi[i]=vi[i+1];
	v_num--;
	printf("\n删除视图成功! 按回车返回上一级菜单。");
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
	printf("视图查看表单");setcolor(0,15);
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("输入视图名:");
	//gets(mn);	//输入视图名
	cin>>mn;
	//判断视图是否存在
	for(i=0;i<v_num;i++)
		if(strcmp(vi[i].vname,mn)==0)
			goto val1;
	gotoxy(1,20);printf("不存在的视图名!\a");
	getch();
	return;
val1:;//发现该视图，编号为i
	for(j=0;j<t_num;j++)
		if(strcmp(vi[i].tname,t[j].tname)==0)
			break;
	//j为表名的下标
	gotoxy(1,20);
	printf("\n表名:%s   属性个数:%d    主键:%s",t[j].tname,t[j].pro_num,t[j].key);
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
		gotoxy(1,24);printf("\a对不起,您的权限不允许这项操作.按任意键返回...");
		getch();
		return;
	}
	if(!dbislock())		//没有锁，那就加锁@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		dblock();	//dblock();     dbunlock();
	else
	{
		clrscr();
		printf("数据库处于锁状态，即使超级用户也不能进行修改!\a");
		printf("\n按任意键返回...");
		getch();
		return;
	}
	clrscr();
	char mn[MAX_TNAME];	//为了错误处理所建立的变量
	int i,j;
	index inew;
	setcolor(0,14);
	gotoxy(37,7);setcolor(0,14);
	printf("建立索引");setcolor(0,15);
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("输入索引表名:");
	//gets(mn);
	cin>>mn;

	//添加错误处理
	for(i=0;i<t_num;i++)
		if(strcmp(mn,t[i].tname)==0)
		{
			//i的值就是要删除的表的位置.
			goto eodrp;
		}
	gotoxy(1,20);printf("未找到该表,请确认输入.\a");
	getch();
	if(dbislock())
		dbunlock();
	return;
eodrp:;//找到表所在位置,接下来是建立索引动作,表t[i]是查到的表.
	//添加判断,是否可以继续添加记录而不达到满值.错误处理
	strcpy(inew.iname,mn);
	inew.key_num=t[i].key_no;
	for(j=0;j<t[i].re_num;j++)
		strcpy(inew.key[j],t[i].record[j][t[i].key_no]);
	gotoxy(1,20);printf("在表%s上成功建立了索引,回车返回上一级菜单.",mn);
	ind[i_num++]=inew;
	getch();
	if(dbislock())
		dbunlock();
}
void indexdrop()
{
	if(CURRENTREVOKE==0)
	{
		gotoxy(1,24);printf("\a对不起,您的权限不允许这项操作.按任意键返回...");
		getch();
		return;
	}
	if(!dbislock())		//没有锁，那就加锁@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		dblock();	//dblock();     dbunlock();
	else
	{
		clrscr();
		printf("数据库处于锁状态，即使超级用户也不能进行修改!\a");
		printf("\n按任意键返回...");
		getch();
		return;
	}
	if(dbislock())
		dbunlock();
}
void indexshow()			//显示索引内容，并且输入索引项，显示该项的具体内容!!
{
	clrscr();
	char mn[MAX_TNAME];
	int i,j,k;
	gotoxy(37,7);setcolor(0,14);
	printf("显示索引");setcolor(0,15);
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("输入索引表名:");
	//gets(mn);
	cin>>mn;
	for(i=0;i<v_num;i++)
		if(strcmp(mn,ind[i].iname)==0)
			goto aas;
	printf("\a\n没有找到该索引!");
	getchar();
	return;
aas:;
	for(j=0;j<t_num;j++)
		if(strcmp(mn,t[j].tname)==0)
			break;
	gotoxy(1,20);
	printf("\n\n索引表名:%s   索引的主键为:%s",ind[i].iname,t[j].key);
	printf("\n-------------------------------------");
	for(k=0;k<t[j].re_num;k++)
		printf("\n%s",t[j].record[k][t[j].key_no]);
	printf("\n\n显示了输入索引的记录主键内容.回车返回上一级菜单.");
	getchar();
}
void recordupdate()
{
	if(CURRENTREVOKE==0)
	{
		gotoxy(1,24);printf("\a对不起,您的权限不允许这项操作.按任意键返回...");
		getch();
		return;
	}
	if(!dbislock())		//没有锁，那就加锁@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		dblock();	//dblock();     dbunlock();
	else
	{
		clrscr();
		printf("数据库处于锁状态，即使超级用户也不能进行修改!\a");
		printf("\n按任意键返回...");
		getch();
		return;
	}
	clrscr();
	char mn[MAX_TNAME];
	char mq[MAX_TNAME];
	char zj[MAX_PROLEN];
	int i,j,k;
	gotoxy(37,7);setcolor(0,14);
	printf("记录更改操作");setcolor(0,15);
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("输入记录所在的表名:");
	//gets(mn);
	cin>>mn;

	//添加错误处理
	for(i=0;i<t_num;i++)
		if(strcmp(mn,t[i].tname)==0)
		{
			//i的值就是要删除的表的位置.
			goto eodrp;
		}
	gotoxy(1,20);printf("未找到要删除的表,请确认输入.\a");
	getch();
	if(dbislock())
		dbunlock();
	return;
eodrp:;//找到表所在位置,接下来是插入记录动作,表t[i]是查到的表.
	//添加判断,是否可以继续添加记录而不达到满值.错误处理
	strcpy(mq,mn);		//本函数到最后用到mq变量
	if(t[i].re_num<=0)
	{
		gotoxy(1,20);printf("该表记录值为0!无法更改.\a");
		getch();
		if(dbislock())
			dbunlock();
		return;
	}
	//从这里开始搜寻主键的值
	gotoxy(27,10); printf("输入记录主键值:");
	//gets(zj);	//输入主键的值
	cin>>zj;
	//添加错误处理,判断是否有词记录
	printf("   key_no=%d   ",t[i].key_no);
	for(j=0;j<t[i].re_num;j++)
	{
		if( strcmp( t[i].record[j][t[i].key_no] , zj ) ==0)
			goto findkey1;
	}
	gotoxy(1,20);printf("未找到该记录,请确认输入.\a");
	getch();
	if(dbislock())
		dbunlock();
	return;
	//找到该记录,开始进行删除操作.j就是记录的下标,i是表的下标
findkey1:;
	gotoxy(1,20);
	for(k=0;k<t[i].pro_num;k++)
	{
		printf("\n请输入属性%s的值:",t[i].proname[k]);
		//gets(mn);
		cin>>mn;
		//添加错误处理,包括主键不可以重复等等.
		strcpy(t[i].record[j][k],mn);		//把属性值赋给相应的记录属性位置.
				//re_num++
	}

	printf("\n表%s成功修改了一条记录!回车返回上一级菜单.",mq);
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
		printf("\n表名:%s   属性个数:%d    主键:%s",t[i].tname,t[i].pro_num,t[i].key);
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
		printf("\n\n按回车键输出下一张表...\n");
		getchar();
	}
	printf("\n表输出结束，按回车返回上一级菜单.");
	getchar();
}

void outputts()		//表的基本信息（3、表项信息显示）
{
	clrscr();
	getchar();
	char mn[MAX_TNAME];

	int flag=0;
	printf("\n1、查询所有表的表结构；2、查询单个表的表结构；\n");
	cin>>flag;
	getchar();
	if(flag==1)
    {
        int i,j;
        for(i=0;i<t_num;i++)
        {
            printf("\n表名:%s   属性个数:%d    主键:%s",t[i].tname,t[i].pro_num,t[i].key);
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
        printf("\n请输入想要查询的表名：");
        cin>>mn;
        getchar();
        int i,j;
        for(i=0;i<t_num;i++)
        {
            if(strcmp(mn,t[i].tname)==0)
            {
                printf("\n表名:%s   属性个数:%d    主键:%s",t[i].tname,t[i].pro_num,t[i].key);
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
            printf("\n未找到要查询的表,按回车返回上一级菜单.");
            getchar();
            return;
        }
    }
	printf("\n表信息输出结束，按回车返回上一级菜单.");
	getchar();
}


void initialize()
{
	int i,j,k,p;
	t_num=0;
	NULLTABLE.key_no=-1;	//空表的定义就是key_no是-1的表，也是一种标识
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
	table tnew;	//结果集
	int k,p,m,q;
		if(!IsTableSameType(t1,t2))
	{
		printf("\n所输入的两个表并不是同一个类型的!\a");
		getchar();
		return NULLTABLE;
	}
//并操作，把T1表赋值给结果集
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
			strcpy(tnew.record[k][p],t1->record[k][p]);	//表T就是结果集

	for(p=0;p<t2->re_num;p++)
		for(q=0;q<t1->re_num;q++)
		{
			if(IsRecEqu(t1,q,t2,p))	//记录相等。
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
	//这里加入，如果两个表不是同类表的错误处理
	if(!IsTableSameType(t1,t2))
	{
		gotoxy(1,20);
		printf("所输入的两个表并不是同一个类型的!\a");
		getchar();
		return NULLTABLE;
	}
	tnew.key_no=t1->key_no;
	tnew.pro_num=t1->pro_num;	//使结果集T的表类型和R,S相同
	tnew.re_num=0;		//把空集Φ给结果集T
	strcpy(tnew.key,t1->key);
	strcpy(tnew.tname,"T");
	for(k=0;k<t1->pro_num;k++)
	{
		strcpy(tnew.proname[k],t1->proname[k]);
		tnew.protype[k]=t1->protype[k];
	}
//交操作开始了

	for(p=0;p<t2->re_num;p++)
		for(q=0;q<t1->re_num;q++)
		{
			if(IsRecEqu(t1,q,t2,p))	//记录相等。
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
		//这里加入，如果两个表不是同类表的错误处理
	if(!IsTableSameType(t1,t2))
	{
		gotoxy(1,20);
		printf("所输入的两个表并不是同一个类型的!\a");
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
			strcpy(tnew.record[k][p],t1->record[k][p]);	//表T就是结果集,结果集的内容就是R
//开始差操作了！！！
	for(p=0;p<t2->re_num;p++)
		for(q=0;q<tnew.re_num;q++)
		{
			if(IsRecEqu(&tnew,q,t2,p))	//记录相等。
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
	table *t1			是表的指针
	char pro[][]		是投影属性的名称，二维，第一维是属性序号，第二维是属性名
	int k				是投影出的属性的个数
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	table tnew;
	int m,n,p;
	for(m=0;m<=k;m++)
	{
		strcpy(tnew.proname[m],pro[m]);
		//这里tnew.protype就不多做了，因为实用性不强，并且操作比价复杂。
	}
	tnew.pro_num=k;
	tnew.key_no=0;
	tnew.re_num=t1->re_num;
	strcpy(tnew.tname,"T");
	for(m=0;m<t1->re_num;m++)		//记录的变化
	{
		for(n=0;n<t1->pro_num;n++)	//属性的变化
		{
			for(p=0;p<tnew.pro_num;p++)
				if(strcmp(t1->proname[n],tnew.proname[p])==0)
				{
					strcpy(tnew.record[m][p],t1->record[m][n]);
				}	//找到对应项，加入到新表中去
		}
	}
	return tnew;
}
table yunsel0(table *t1,char *sx,int ysf,int shu,char *str)	//检查好了再往函数里传
{
	table tnew;
	int m,n,k;
	//先初始化tnew表，使得tnew表与t1表同类型
	tnew.pro_num=t1->pro_num;
	tnew.key_no=t1->key_no;
	tnew.re_num=0;
	strcpy(tnew.tname,"T");
	for(m=0;m<t1->pro_num;m++)
		strcpy(tnew.proname[m],t1->proname[m]);
	//再判断sx区域的下标是多少。
	for(k=0;k<tnew.pro_num;k++)
		if(strcmp(tnew.proname[k],sx)==0)
			break;
	//k就是sx的下标，后面用着方便!!!
	for(m=0;m<t1->re_num;m++)
	{
		//1.＝  2.≠  3.≤  4.≥  5.＜  6.＞
		if(ysf==1)		//1.＝
		{
			if(strcmp(t1->record[m][k],str)==0)
			{
				for(n=0;n<t1->pro_num;n++)
					strcpy(tnew.record[tnew.re_num][n],t1->record[m][n]);
				tnew.re_num++;
			}
		}
		else if(ysf==2)	//2.≠
		{
			if(strcmp(t1->record[m][k],str)!=0)
			{
				for(n=0;n<t1->pro_num;n++)
					strcpy(tnew.record[tnew.re_num][n],t1->record[m][n]);
				tnew.re_num++;
			}
		}
		else if(ysf==3)	//3.≤
		{
			if(strtonum(t1->record[m][k])<=shu)
			{
				for(n=0;n<t1->pro_num;n++)
					strcpy(tnew.record[tnew.re_num][n],t1->record[m][n]);
				tnew.re_num++;
			}
		}
		else if(ysf==4)	//4.≥
		{
			if(strtonum(t1->record[m][k])>=shu)
			{
				for(n=0;n<t1->pro_num;n++)
					strcpy(tnew.record[tnew.re_num][n],t1->record[m][n]);
				tnew.re_num++;
			}
		}
		else if(ysf==5)	//5.＜
		{
			if(strtonum(t1->record[m][k])<shu)
			{
				for(n=0;n<t1->pro_num;n++)
					strcpy(tnew.record[tnew.re_num][n],t1->record[m][n]);
				tnew.re_num++;
			}
		}
		else	//ysf==6//6.＞
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
	table tnew;	//结果集
	int i,j;
	gotoxy(33,7);setcolor(0,14);
	printf("并运算操作：R∪S");setcolor(0,15);
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("输入关系R的表名:");
	//gets(mn);
	cin>>mn;
	//添加错误处理
	for(i=0;i<t_num;i++)
		if(strcmp(mn,t[i].tname)==0)
		{
			//i的值就是表的位置.
			goto eodrp;
		}
	gotoxy(1,20);printf("未找到表,请确认输入.\a");
	getchar();
	return;
eodrp:;//找到表所在位置,表t[i]是查到的表.
	gotoxy(27,10); printf("输入关系S的表名:");
	//gets(mn);
	cin>>mn;
	//添加错误处理
	for(j=0;j<t_num;j++)
		if(strcmp(mn,t[j].tname)==0)		//j的值就是表的位置.
			goto eodrp1;
	gotoxy(1,20);printf("未找到表,请确认输入.\a");
	getchar();
	return;
eodrp1:;//找到表所在位置,表t[j]是查到的表.
	//表S与表R。。。。。表R i  表S j
//下面开始进行两个表的并操作********************************************
	//这里加入，如果两个表不是同类表的错误处理
	if((tnew=yununion(&t[i],&t[j])).key_no==-1)
		return ;
	gotoxy(1,20);printf("\n结果关系是:");

	tableprint(&tnew);		//输出表咯~~
	printf("\n输出了结果,回车返回上一级菜单.");
	getchar();
	getchar();
}
void relationjiao()
{
	clrscr();
	char mn[MAX_TNAME];
	int i,j;
//	int m,n;
	table tnew;		//结果集
	gotoxy(33,7);setcolor(0,14);
	printf("交运算操作：R∩S");setcolor(0,15);
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("输入关系R的表名:");
	//gets(mn);
	cin>>mn;

	//添加错误处理
	for(i=0;i<t_num;i++)
		if(strcmp(mn,t[i].tname)==0)
		{
			//i的值就是表的位置.
			goto eodrp;
		}
	gotoxy(1,20);printf("未找到表,请确认输入.\a");
	getchar();
	return;
eodrp:;//找到表所在位置,表t[i]是查到的表.
	gotoxy(27,10); printf("输入关系S的表名:");
	//gets(mn);
	cin>>mn;
	//添加错误处理
	for(j=0;j<t_num;j++)
		if(strcmp(mn,t[j].tname)==0)		//j的值就是表的位置.
			goto eodrp1;
	gotoxy(1,20);printf("未找到表,请确认输入.\a");
	getchar();
	return;
eodrp1:;//找到表所在位置,表t[j]是查到的表.
	//表S与表R。。。。。表R i  表S j
	if((tnew=yunjiao(&t[i],&t[j])).key_no==-1)
		return;

	gotoxy(1,20);printf("\n结果关系是:");
	tableprint(&tnew);		//输出表咯~~
	printf("\n输出了结果,回车返回上一级菜单.");
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
	printf("差运算操作：R-S");setcolor(0,15);
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("输入关系R的表名:");
	//gets(mn);
	cin>>mn;
	//添加错误处理
	for(i=0;i<t_num;i++)
		if(strcmp(mn,t[i].tname)==0)
		{
			//i的值就是表的位置.
			goto eodrp;
		}
	gotoxy(1,20);printf("未找到表,请确认输入.\a");
	getchar();
	return;
eodrp:;//找到表所在位置,表t[i]是查到的表.
	gotoxy(27,10); printf("输入关系S的表名:");
	//gets(mn);
	cin>>mn;
	//添加错误处理
	for(j=0;j<t_num;j++)
		if(strcmp(mn,t[j].tname)==0)		//j的值就是表的位置.
			goto eodrp1;
	gotoxy(1,20);printf("未找到表,请确认输入.\a");
	getchar();
	return;
eodrp1:;//找到表所在位置,表t[j]是查到的表.
	//表S与表R。。。。。表R i  表S j
	if((tnew=yundiff(&t[i],&t[j])).key_no==-1)
		return;
	gotoxy(1,20);printf("\n结果关系是:");
//*************************************************************
	tableprint(&tnew);		//输出表咯~~
	printf("\n输出了结果,回车返回上一级菜单.");
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
	printf("投影操作");setcolor(0,15);
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("输入关系R的表名:");
	//gets(mn);
	cin>>mn;
	//添加错误处理
	for(i=0;i<t_num;i++)
		if(strcmp(mn,t[i].tname)==0)
		{
			//i的值就是表的位置.
			goto eodrp;
		}
	gotoxy(1,20);printf("未找到表,请确认输入.\a");
	getchar();
	return;
eodrp:;//找到表所在位置,表t[i]是查到的表.
	gotoxy(1,20);
	int y=0;
	for(p=0;p<MAX_PRONUM;p++)
		for(q=0;q<MAX_PROLEN;q++)
			pro[p][q]='\0';
	k=0;
	char ch;
	while(y!=1)
	{
		printf("\n输入投影的属性:");
		//gets(sx);
		cin>>sx;

		//这里添加错误处理，重名或者其他
		strcpy(pro[k++],sx);
		printf("    是否继续输入?[y/n]");
		cin>>ch;
		if(ch=='y')
			continue;
		else
			break;
	}//这里保存了pro[MAX_PRONUM][MAX_PROLEN]
	tnew=yunshadow(&t[i],pro,k);		//传参的k是pro下标
	printf("\n投影结果是:");
	tableprint(&tnew);		//输出表咯~~

	printf("\n投影输出结束，回车返回上一级菜单。");
	getchar();
	getchar();
}
void relationsel0()
{
	clrscr();
	char mn[MAX_TNAME];
	char sx[MAX_PROLEN];
	char str[MAX_PROLEN];
	int i,ysf,shu;		//用整型变量保存运算符类别。
	char ch;
	table tnew;		//结果集
	gotoxy(35,7);setcolor(0,14);
	printf("选择单一操作");setcolor(0,15);
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("输入关系R的表名:");
	//gets(mn);
	cin>>mn;
	//添加错误处理
	for(i=0;i<t_num;i++)
		if(strcmp(mn,t[i].tname)==0)
		{
			//i的值就是表的位置.
			goto eodrp;
		}
	gotoxy(1,20);printf("未找到表,请确认输入.\a");
	getchar();
	return;
eodrp:;//找到表所在位置,表t[i]是查到的表.    ＝、≠、≤、≥、＜、＞
	gotoxy(1,20);printf("请输入选择的属性:");
	//gets(sx);
	cin>>sx;
	//添加错误处理

as:	printf("\n请输入关系运算符(1.＝  2.≠  3.≤  4.≥  5.＜  6.＞)  [1-6]:");
	cin>>ch;
	if(ch>48 && ch<55)
		ysf=ch-48;
	else
		goto as;
	printf("\n请输入运算符后的数值:");
	if(ysf>2)
		cin>>shu;	//3.≤  4.≥  5.＜  6.＞这四种是数值比较
	else
		//gets(str);	//1.＝  2.≠    这两种是字符串比较
		cin>>str;
	tnew=yunsel0(&t[i],sx,ysf,shu,str);

	printf("\n选择结果是:");
	tableprint(&tnew);		//输出表咯~~
	printf("\n选择输出结束，回车返回上一级菜单。");
	getchar();
	getchar();
}

void relationcalculate()
{
	table seland(table *t,char *pro1,char o1,char *shu1,char *pro2,char o2,char *shu2);
	table selor(table *t,char *pro1,char o1,char *shu1,char *pro2,char o2,char *shu2);
	clrscr();
	char large[100];	//这个保存词法分析之前的东东，就是一个大的输入流
	char tname[MAX_TNAME];
	char pro[MAX_PRONUM][MAX_PROLEN];	//将要投影的属性名称集合
	char pro1[MAX_PROLEN];
	char pro2[MAX_PROLEN];
	char shu1[8],shu2[8];
	char o1,o2;
	int i,j,k=0;	//用整型变量保存运算符类别。
	gotoxy(33,7);setcolor(0,14);
	printf("关系演算查询功能");setcolor(0,15);
	DrawFrame(20,60,5,14);

	gotoxy(1,16);printf("\n输入sQL语句: ");
	printf("例如select sno,sage from stu where sno<0003 and sage!=22;\n\n\n>sQL:");
	fflush(stdin);//清除缓冲区
	gets(large);
	//*******************************错误处理select
	if(strcmp(substr(large,0,6),"select")!=0)
	{
		printf("格式不对,没有发现select关键字!!!任意键返回...\a");
		getch();
		return;
	}
	//*******************************错误处理from
	//以后添加其他出错处理，比如from where。
	int p;
	for(p=0;p<(int)strlen(large);p++)
	{
		if(strcmp(substr(large,p,4),"from")==0)
			break;
		if(strcmp(substr(large,p,4),"from")!=0 && p>(int)strlen(large)-4)
		{
			printf("格式不对,没有发现from关键字!!!任意键返回...\a");
			getch();
			return;
		}
	}
	//******************************错误处理where*********
	for(p=0;p<(int)strlen(large);p++)
	{
		if(strcmp(substr(large,p,5),"where")==0)
			break;
		if(strcmp(substr(large,p,5),"where")!=0 && p>(int)strlen(large)-5)
		{
			printf("格式不对,没有发现where关键字!!!任意键返回...\a");
			getch();
			return;
		}
	}
	if(large[(int)strlen(large)-1]!=';')
	{
		printf("格式不对,没有发现';'关键字!!!任意键返回...\a");
		getch();
		return;
	}
	//******************************
	for(j=0;j<MAX_PRONUM;j++)
		strcpy(pro[j]," ");
	i=7;		//从select 后第一个非' '开始
	j=0;
	//select sno,sage from stu where sno=0001 and sage=22;
	//select sno,sage from stu where sno!=0003 and sage!=22;
	//select * from stu
//************************这里开始分析************************************
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
				pro[k][j++]=large[i++];	//给pro1赋值
			pro[k][j]='\0';
			j=0;
			if(large[i]==',')
				i++;
			k++;
		}
		i+=6;
	}
	//目前i的位置在tname的0下标上
	while(large[i]!=' ')
		tname[j++]=large[i++];	//给tname赋值 只能取一个表
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
	i+=7;		//目前i的位置在pro1的0下标上
	while(large[i]!='>' && large[i]!='=' && large[i]!='<'&& large[i]!='!')
		pro1[j++]=large[i++];	//给pro1赋值
	pro1[j]='\0';
	j=0;
	o1=large[i++];	//目前i在o1上,给o1赋值
	if(large[i]=='=')
		i++;
	while(large[i]!=' ')
		shu1[j++]=large[i++];	//给shu1赋值
	shu1[j]='\0';
	j=0;			//i在' '上
	i++;
	char andor[4];
	while(large[i]!=' ')
		andor[j++]=large[i++];
	andor[j]='\0';
	j=0;
	i++;
	while(large[i]!='>' && large[i]!='=' && large[i]!='<' && large[i]!='!')
		pro2[j++]=large[i++];	//给pro2赋值
	pro2[j]='\0';
	j=0;
	//i在o2上
	o2=large[i++];	//目前i在o2上,给o2赋值
	if(large[i]=='=')
		i++;
	while(large[i]!=';')
		shu2[j++]=large[i++];	//给shu2赋值
	shu2[j]='\0';
	j=0;			//i在';'上
//************************************************************
	//这样就把一个大的large字符串拆分开来.
	table t1;
	for(i=0;i<t_num;i++)
		if(strcmp(tname,t[i].tname)==0)
		{
			//i的值就是表的位置.
			goto eodrp;
		}
	gotoxy(1,20);printf("未找到表,请确认输入.\a");
	getchar();
	return;
eodrp:;//找到表所在位置,表t[i]是查到的表.

	if(strcmp(andor,"and")==0)
		t1=seland(&t[i],pro1,o1,shu1,pro2,o2,shu2);
	else if(strcmp(andor,"or")==0)
		t1=selor(&t[i],pro1,o1,shu1,pro2,o2,shu2);
	else
		printf("\n\n这个是error!!!\a\a");
	t1=yunshadow(&t1,pro,k);
	tableprint(&t1);
//**************************测试地点*******

//*****************************************
	getch();
}
void relationcalculate1()
{
    table condition(table *t,char *pro1,char o1,char o2,char *shu1);
    table relationnaturalconnect(table *t1,table *t2);
	table selor(table *t,char *pro1,char o1,char *shu1,char *pro2,char o2,char *shu2);
	clrscr();
	char large[100];	//这个保存词法分析之前的东东，就是一个大的输入流
	char tname[MAX_TNAME];
	char pro[MAX_PRONUM][MAX_PROLEN];	//将要投影的属性名称集合
	char pro1[MAX_PROLEN];
	char pro2[MAX_PROLEN];
	char shu1[8],shu2[8];
	char o1,o2;
	int i,j,k=0;	//用整型变量保存运算符类别。
	gotoxy(33,7);setcolor(0,14);
	printf("关系演算查询功能");setcolor(0,15);
	DrawFrame(20,60,5,14);

	gotoxy(1,16);printf("\n输入sQL语句: ");
	printf("例如select sno,sage from stu where sno<0003 and sage!=22;\n\n\n>sQL:");
	fflush(stdin);//清除缓冲区
	gets(large);
	//*******************************错误处理select
	if(strcmp(substr(large,0,6),"select")!=0)
	{
		printf("格式不对,没有发现select关键字!!!任意键返回...\a");
		getch();
		return;
	}
	//*******************************错误处理from
	//以后添加其他出错处理，比如from where。
	int p;
	for(p=0;p<(int)strlen(large);p++)
	{
		if(strcmp(substr(large,p,4),"from")==0)
			break;
		if(strcmp(substr(large,p,4),"from")!=0 && p>(int)strlen(large)-4)
		{
			printf("格式不对,没有发现from关键字!!!任意键返回...\a");
			getch();
			return;
		}
	}
	//******************************错误处理where*********
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
			//printf("格式不对,没有发现where关键字!!!任意键返回...\a");
			flag = 0;
			break;
		}
	}
	if(large[(int)strlen(large)-1]!=';')
	{
		printf("格式不对,没有发现';'关键字!!!任意键返回...\a");
		getch();
		return;
	}
	//******************************
	for(j=0;j<MAX_PRONUM;j++)
		strcpy(pro[j]," ");
	i=7;		//从select 后第一个非' '开始
	j=0;
	//select sno,sage from stu where sno=0001 and sage=22;
	//select sno,sage from stu where sno!=0003 and sage!=22;
	//select * from stu;
	//************************这里开始分析************************************
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
				pro[k][j++]=large[i++];	//给pro1赋值
			pro[k][j]='\0';
			j=0;
			if(large[i]==',')
				i++;
			k++;
		}
		i+=6;
	}
	//目前i的位置在tname的0下标上
	//实现把多表连接成为单表存储在tnew中
	table tn;
	while(large[i]!=','&&large[i]!=' '&&large[i]!=';')
        tname[j++]=large[i++];
    tname[j]='\0';
    j=0;
    int m = 0;
    for(m;m<t_num;m++)
		if(strcmp(tname,t[m].tname)==0)
		{
			//k的值就是表的位置.
			goto eodrp1;
		}
	gotoxy(1,20);printf("输入的表有误，不存在.\a");
	getchar();
	return;
    eodrp1:;//找到表所在位置,表t[i]是查到的表.
    tn = t[m];
    if(large[i]==',')
        i++;
	while(large[i]!=' '&&large[i]!=';')
    {
        while(large[i]!=','&&large[i]!=' '&&large[i]!=';')
            tname[j++]=large[i++];//给tname赋值，tname代表表名
        tname[j]='\0';
        j=0;
        if(large[i]==',')
            i++;
        for(m=0;m<t_num;m++)
            if(strcmp(tname,t[m].tname)==0)
            {
                //k的值就是表的位置.
                goto eodrp2;
            }
		gotoxy(1,20);printf("输入的表有误，不存在1.\a");
        getchar();
        return;
    eodrp2:;//找到表所在位置,表t[i]是查到的表.
        tn = relationnaturalconnect(&tn,&t[m]);
    }
    if(secret==1)
	{
		int q;
		k=tn.pro_num;
		for(q=0;q<tn.pro_num;q++)
			strcpy(pro[q],tn.proname[q]);
	}
    if(flag==0)//无where语句
    {
        tn = yunshadow(&tn,pro,k);
        tableprint(&tn);
    }
    else//有where语句
    {
        table t1 = tn; //t1作为条件修改后的存储
        table t2 = tn; //t2作为原表的记录的存储
        j=0;
        i+=7;//目前i的位置在pro1的0下标上
        while(large[i]!=';')
        {
            while(large[i]!='>' && large[i]!='=' && large[i]!='<'&& large[i]!='!')
                pro1[j++]=large[i++];//pro1用于存储比较的属性
            pro1[j]='\0';
            j=0;
            o1=large[i];//o1存储比较符
            i++;
            if(large[i]=='=')//判断是否为==
            {
                o2=large[i];
                i++;
            }
            else
            {
                o2 = '\0';
            }
            while(large[i]!=' '&&large[i]!=';')
                shu1[j++]=large[i++]; //shu1为比较符右边的值
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
                    printf("\n\n这个是error!!!\a\a");
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
	int shu,ysf;	//用整型变量保存运算符类别。
	table tnew1;		//结果集
	table *tnull=NULL;

	//添加错误处理在调用本函数之前
	//找到表所在位置,表t[i]是查到的表.    ＝、≠、≤、≥、＜、＞
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
		shu=strtonum(shu1);	//3.≤  4.≥  5.＜  6.＞这四种是数值比较
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
	int i,shu,ysf;	//用整型变量保存运算符类别。
	table tnew1,tnew2;		//结果集
	gotoxy(35,7);setcolor(0,14);
	printf("选择操作AND");setcolor(0,15);
	DrawFrame(20,60,5,14);

	gotoxy(27,9); printf("输入关系R的表名:");
	//gets(mn);
	cin>>mn;
	//添加错误处理
	for(i=0;i<t_num;i++)
		if(strcmp(mn,t[i].tname)==0)
		{
			//i的值就是表的位置.
			goto eodrp;
		}
	gotoxy(1,20);printf("未找到表,请确认输入.\a");
	getchar();
	return;
eodrp:;//找到表所在位置,表t[i]是查到的表.    ＝、≠、≤、≥、＜、＞
	gotoxy(1,20);printf("请输入第一个选择的属性:");
	//gets(sx);
	cin>>sx;
	//添加错误处理

as:	printf("\n请输入关系运算符(1.＝  2.≠  3.≤  4.≥  5.＜  6.＞)  [1-6]:");
	cin>>ch;
	if(ch>48 && ch<55)
		ysf=ch-48;
	else
		goto as;
	printf("\n请输入运算符后的数值:");
	if(ysf>2)
		cin>>shu;	//3.≤  4.≥  5.＜  6.＞这四种是数值比较
	else
        {fflush(stdin);
		gets(str);}	//1.＝  2.≠    这两种是字符串比较
	tnew1=yunsel0(&t[i],sx,ysf,shu,str);
//****************************************************************
	printf("\n请输入第二个选择的属性:");
	//gets(sx);
	cin>>sx;
	//添加错误处理

as1:	printf("\n请输入关系运算符(1.＝  2.≠  3.≤  4.≥  5.＜  6.＞)  [1-6]:");
	cin>>ch;
	if(ch>48 && ch<55)
		ysf=ch-48;
	else
		goto as1;
	printf("\n请输入运算符后的数值:");
	if(ysf>2)
		cin>>shu;	//3.≤  4.≥  5.＜  6.＞这四种是数值比较
	else
        {fflush(stdin);
		gets(str);}	//1.＝  2.≠    这两种是字符串比较
	tnew2=yunsel0(&t[i],sx,ysf,shu,str);
//***************************************************************
	tnew1=yunjiao(&tnew1,&tnew2);
	printf("\n选择AND结果是:");
	tableprint(&tnew1);		//输出表咯~~
	printf("\n选择输出结束，回车返回上一级菜单。");
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
	int i,shu,ysf;	//用整型变量保存运算符类别。
	table tnew1,tnew2;		//结果集
	gotoxy(35,7);setcolor(0,14);
	printf("选择操作OR");setcolor(0,15);
	DrawFrame(20,60,5,14);

	gotoxy(27,9); printf("输入关系R的表名:");
	fflush(stdin);
	gets(mn);
	//添加错误处理
	for(i=0;i<t_num;i++)
		if(strcmp(mn,t[i].tname)==0)
		{
			//i的值就是表的位置.
			goto eodrp;
		}
	gotoxy(1,20);printf("未找到表,请确认输入.\a");
	getchar();
	return;
eodrp:;//找到表所在位置,表t[i]是查到的表.    ＝、≠、≤、≥、＜、＞
	gotoxy(1,20);printf("请输入第一个选择的属性:");
	fflush(stdin);
	gets(sx);
	//添加错误处理

as:	printf("\n请输入关系运算符(1.＝  2.≠  3.≤  4.≥  5.＜  6.＞)  [1-6]:");
	cin>>ch;
	if(ch>48 && ch<55)
		ysf=ch-48;
	else
		goto as;
	printf("\n请输入运算符后的数值:");
	if(ysf>2)
		cin>>shu;	//3.≤  4.≥  5.＜  6.＞这四种是数值比较
	else
    {fflush(stdin);
		gets(str);}	//1.＝  2.≠    这两种是字符串比较
	tnew1=yunsel0(&t[i],sx,ysf,shu,str);
//****************************************************************
	printf("\n请输入第二个选择的属性:");
	fflush(stdin);
	gets(sx);
	//添加错误处理

as1:	printf("\n请输入关系运算符(1.＝  2.≠  3.≤  4.≥  5.＜  6.＞)  [1-6]:");
	cin>>ch;
	if(ch>48 && ch<55)
		ysf=ch-48;
	else
		goto as1;
	printf("\n请输入运算符后的数值:");
	if(ysf>2)
		cin>>shu;	//3.≤  4.≥  5.＜  6.＞这四种是数值比较
	else
    {fflush(stdin);
		gets(str);}	//1.＝  2.≠    这两种是字符串比较
	tnew2=yunsel0(&t[i],sx,ysf,shu,str);
//***************************************************************
	tnew1=yununion(&tnew1,&tnew2);
	printf("\n选择OR结果是:");
	tableprint(&tnew1);		//输出表咯~~
	printf("\n选择输出结束，回车返回上一级菜单。");
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
	int i,ysf,shu;		//用整型变量保存运算符类别。
	int q,p,k;
	char ch;
	table tnew;		//结果集
	gotoxy(32,7);setcolor(0,14);
	printf("投影选择综合操作");setcolor(0,15);
	DrawFrame(20,60,5,14);
	gotoxy(27,9); printf("输入关系R的表名:");
	fflush(stdin);
	gets(mn);
	//添加错误处理
	for(i=0;i<t_num;i++)
		if(strcmp(mn,t[i].tname)==0)
		{
			//i的值就是表的位置.
			goto eodrp;
		}
	gotoxy(1,20);printf("未找到表,请确认输入.\a");
	getchar();
	return;
eodrp:;//找到表所在位置,表t[i]是查到的表.    ＝、≠、≤、≥、＜、＞
	gotoxy(1,20);printf("请输入选择的属性:");
	fflush(stdin);
	gets(sx);
	//添加错误处理
	strcpy(sx1,sx);
as:	printf("\n请输入关系运算符(1.＝  2.≠  3.≤  4.≥  5.＜  6.＞)  [1-6]:");
	cin>>ch;
	if(ch>48 && ch<55)
		ysf=ch-48;
	else
		goto as;
	printf("\n请输入运算符后的数值:");
	if(ysf>2)
		cin>>shu;	//3.≤  4.≥  5.＜  6.＞这四种是数值比较
	else
    {
        fflush(stdin);
		gets(str);}	//1.＝  2.≠    这两种是字符串比较
	tnew=yunsel0(&t[i],sx,ysf,shu,str);		//选择结束，即将投影。
//*********************************************************************
	int y=0;
	for(p=0;p<MAX_PRONUM;p++)
		for(q=0;q<MAX_PROLEN;q++)
			pro[p][q]='\0';
	k=0;
	while(y!=1)
	{
		printf("\n输入投影的属性:");
		fflush(stdin);
		gets(sx);
		//这里添加错误处理，重名或者其他
		strcpy(pro[k++],sx);
		printf("    是否继续输入?[y/n]");
		cin>>ch;
		if(ch=='y')
			continue;
		else
			break;
	}//这里保存了pro[MAX_PRONUM][MAX_PROLEN]
	tnew=yunshadow(&tnew,pro,k);		//传参的k是pro下标
	printf("\n查询的结果是:select %s from %s where %sθ",sx1,t[i].tname,sx1);
	tableprint(&tnew);		//输出表咯~~
	printf("\n选择输出结束，回车返回上一级菜单。");
	getchar();
	getchar();
}
//*********************************************************************************************
//返回值的三个函数,试验5
table seland(table *t,char *pro1,char o1,char *shu1,char *pro2,char o2,char *shu2)
{
	int shu,ysf;	//用整型变量保存运算符类别。
	table tnew1,tnew2;		//结果集
	table *tnull=NULL;

	//添加错误处理在调用本函数之前
	//找到表所在位置,表t[i]是查到的表.    ＝、≠、≤、≥、＜、＞
	switch(o1)
	{
		case '>':ysf=6;break;
		case '<':ysf=5;break;
		case '=':ysf=1;break;
		case '!':ysf=2;break;
	}
	if(ysf>2)
		shu=strtonum(shu1);	//3.≤  4.≥  5.＜  6.＞这四种是数值比较
	tnew1=yunsel0(t,pro1,ysf,shu,shu1);
//****************************************************************
//添加错误处理
	switch(o2)
	{
		case '>':ysf=6;break;
		case '<':ysf=5;break;
		case '=':ysf=1;break;
		case '!':ysf=2;break;
	}
	if(ysf>2)
		shu=strtonum(shu2);	//3.≤  4.≥  5.＜  6.＞这四种是数值比较

	tnew2=yunsel0(t,pro2,ysf,shu,shu2);
//***************************************************************
	tnew1=yunjiao(&tnew1,&tnew2);
	return tnew1;
}
table selor(table *t,char *pro1,char o1,char *shu1,char *pro2,char o2,char *shu2)
{
	int shu,ysf;	//用整型变量保存运算符类别。
	table tnew1,tnew2;		//结果集
	table *tnull=NULL;

	//添加错误处理在调用本函数之前
	//找到表所在位置,表t[i]是查到的表.    ＝、≠、≤、≥、＜、＞
	switch(o1)
	{
		case '>':ysf=6;break;
		case '<':ysf=5;break;
		case '=':ysf=1;break;
		case '!':ysf=2;break;
	}
	if(ysf>2)
		shu=strtonum(shu1);	//3.≤  4.≥  5.＜  6.＞这四种是数值比较
	tnew1=yunsel0(t,pro1,ysf,shu,shu1);
//****************************************************************
//添加错误处理
	switch(o2)
	{
		case '>':ysf=6;break;
		case '<':ysf=5;break;
		case '=':ysf=1;break;
		case '!':ysf=2;break;
	}
	if(ysf>2)
		shu=strtonum(shu2);	//3.≤  4.≥  5.＜  6.＞这四种是数值比较

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
	//这里包含两种连接，将这两种连接的结果都要输出出来。
ab:	clrscr();
	char mn[MAX_TNAME];
	table tnew;	//结果集
	int i,j,x1,x2;
	gotoxy(33,7);setcolor(0,14);
	printf("连接操作");setcolor(0,15);
	DrawFrame(20,60,5,14);
	gotoxy(27,9);printf("选择连接类型: 1.等值 2.自然[ ]\b\b");
	char ch;
	cin>>ch;
	if(ch=='1')
		goto ab1;
	else if(ch=='2')
		goto ab2;
	else
		goto ab;
//**********以下处理等值连接和自然连接*************************************************
//**********首先是等值连接*************************************************
ab1:gotoxy(1,20); printf("输入关系R的表名:");
	//gets(mn);
	cin>>mn;

	//添加错误处理
	for(i=0;i<t_num;i++)
		if(strcmp(mn,t[i].tname)==0)
		{
			//i的值就是表的位置.
			goto eodrp;
		}
	gotoxy(1,20);printf("未找到表,请确认输入.\a");
	getchar();
	return;
eodrp:;//找到表所在位置,表t[i]是查到的表.
	printf("\n输入关系R中的一个属性用做等值连接:");
	//gets(mn);
	cin>>mn;
	for(x1=0;x1<t[i].pro_num;x1++)
		if(strcmp(t[i].proname[x1],mn)==0)
			break;
	//这里不添加错误处理的理由是减少goto语句的使用，但是在结构化程序设计中，
	//GOTO语句是一个很好用的东东!!!......
	printf("\n输入关系S的表名:");
	//gets(mn);
	cin>>mn;
	//添加错误处理
	for(j=0;j<t_num;j++)
		if(strcmp(mn,t[j].tname)==0)		//j的值就是表的位置.
			goto eodrp1;
	gotoxy(1,20);printf("未找到表,请确认输入.\a");
	getchar();
	return;
eodrp1:;//找到表所在位置,表t[j]是查到的表.
	   	printf("\n输入关系R中的一个属性用做等值连接:");
	//gets(mn);
	cin>>mn;
	for(x2=0;x2<t[j].pro_num;x2++)
		if(strcmp(t[j].proname[x2],mn)==0)
			break;
	//表S与表R。。。。。表R i  表S j
	tnew=relationequalconnect(&t[i],&t[j],x1,x2);
	goto enda;
//*********再次是自然连接*******************************************
//***********************************容易乱*************************
ab2:gotoxy(1,20); printf("输入关系R的表名:");
	//gets(mn);
	cin>>mn;

	//添加错误处理
	for(i=0;i<t_num;i++)
		if(strcmp(mn,t[i].tname)==0)
		{
			//i的值就是表的位置.
			goto eodrp2;
		}
	gotoxy(1,20);printf("未找到表,请确认输入.\a");
	getchar();
	return;
eodrp2:;//找到表所在位置,表t[i]是查到的表.
	printf("\n输入关系S的表名:");
	//gets(mn);
	cin>>mn;
	//添加错误处理
	for(j=0;j<t_num;j++)
		if(strcmp(mn,t[j].tname)==0)		//j的值就是表的位置.
			goto eodrp3;
	gotoxy(1,20);printf("未找到表,请确认输入.\a");
	getchar();
	return;
eodrp3:;//找到表所在位置,表t[j]是查到的表.
	//表S与表R。。。。。表R i  表S j
	tnew=relationnaturalconnect(&t[i],&t[j]);
	goto enda;


enda:tableprint(&tnew);
	printf("\n连接操作成功，回车返回上一级菜单.");
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
//以上初始化结果集tnew
	for(i=0;i<t1->re_num;i++)
		for(j=0;j<t2->re_num;j++)
		{
			if(strcmp(t1->record[i][x1],t2->record[j][x2])==0)	//等值
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
int prosubscript(table *t1,char *str)		//返回一个属性在一个表中的位置（下标）
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
	//对tnew的初始化
	tnew.pro_num=t1->pro_num+t2->pro_num;
	tnew.key_no=0;
	tnew.re_num=0;
	strcpy(tnew.tname,"T");
		for(m=0;m<t1->pro_num;m++)
		strcpy(tnew.proname[m],t1->proname[m]);
	n=m;
	for(m=0;m<t2->pro_num;m++)
		strcpy(tnew.proname[m+n],t2->proname[m]);
//初始化完成
	for(i=0;i<MAX_PRONUM;i++)
		for(j=0;j<MAX_PROLEN;j++)
			prosame[i][j]='\0';		//初始化公共属性数组
	for(j=0;j<t2->pro_num;j++)		//t2 j    t1 i
		for(i=0;i<t1->pro_num;i++)
			if(strcmp(t1->proname[i],t2->proname[j])==0)
				strcpy(prosame[pronum++],t1->proname[i]);
	//*****************************************
//	printf("******%s  %s******",prosame[0],prosame[1]);
//	getchar();
	//*****************************************运行到这里正确
	//公共属性集合已经确定,准备生成中间表（自然连接，但是有重复属性）
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
			if(k==pronum)	//把符合自然连接条件的记录都写进来
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
	//*****************************************建立了中间表,准备删除公共属性
	for(i=0;i<tnew.pro_num;i++)
	{
		for(j=0;j<i;j++)
		{
			if(strcmp(tnew.proname[i],tnew.proname[j])==0)	//删除i列的所有记录
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

void test()			//此函数作为测试之用
{
	clrscr();
	char chr[6];
	strcpy(chr,numtostr(1234));
	printf("\n\n\nnumtostr(1234)={%s}",chr);
	getch();
}
void StronglySetLock0()
{
	int databaselock=0;	//强制设定锁的状态为开
	FILE *fp;
	if((fp=fopen("dblock.db","wb"))==NULL)
		fp=fopen("dblock.db","wb+");
	fwrite(&databaselock,sizeof(int),1,fp);
	fclose(fp);
	PrintStringXY(1,24,"\a已经将锁置为开,任意键返回...");
	getch();
}
void SetSpeedFactor()
{
	clrscr();
	printf("设置速度因子,速度因子越大,则加载速度越快.");
	printf("\n请输入速度因子:(1--30)");
	cin>>speedfactor;
//**************************
//添加错误处理
//**************************
	printf("\n\n设置成功.");
}
void exit0()
{
	clrscr();				//┏┗  ┓ ┛┣  ┫ ╋ ┳ ┻ ━ ┃
	setcolor(0,10);
	gotoxy(22,9); printf("┏━━━━━━━━━━━━━━━━┓");
	gotoxy(22,10);printf("┃                                ┃");
	gotoxy(22,11);printf("┃                                ┃");
	gotoxy(22,12);printf("┃                                ┃");
	gotoxy(22,13);printf("┃           谢谢使用 !!          ┃");
	gotoxy(22,14);printf("┃                                ┃");
	gotoxy(22,15);printf("┃                                ┃");
	gotoxy(22,16);printf("┃                                ┃");
	gotoxy(22,17);printf("┃                                ┃");
	gotoxy(22,18);printf("┗━━━━━━━━━━━━━━━━┛");
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
	PrintStringXY(24,7, "┏━━━━━━━━━━━━━━┓");
	PrintStringXY(24,8, "┃                            ┃");
	PrintStringXY(24,9, "┣━━━━━━━━━━━━━━┫");
	PrintStringXY(24,10,"┃  用户名:                   ┃");
	PrintStringXY(24,11,"┣━━━━━━━━━━━━━━┫");
	PrintStringXY(24,12,"┃  密码  :                   ┃");
	PrintStringXY(24,13,"┗━━━━━━━━━━━━━━┛");
	PrintColorStringXY(36,8,0,14,0,15,"用户登陆");

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
				PrintStringXY(3,24,"错误次数达到三次,系统强行退出.\a按任意键退出.");
				getch();
				exit0();
			}
			PrintStringXY(3,24,"没有该用户名,请核对后再输入.\a按任意键返回.");
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
				printf("\n\n\n登陆成功，按任意键进入...");
				CURRENTREVOKE=u[i].revoke;
				getch();
				return;
			}
		}
	}
	if(t==3)
	{
		PrintStringXY(3,24,"错误次数达到三次,系统强行退出.\a按任意键退出.");
		getch();
		exit0();
	}
	PrintStringXY(3,24,"\a密码错误，请校对后再使用，按任意键返回...");
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
	printf("表的操作");setcolor(0,15);
	DrawFrame(20,60,5,15);
	gotoxy(27,9); printf("1.表的建立     CREATE TABLE");
	gotoxy(27,10);printf("2.表的删除     DROP TABLE");
	gotoxy(27,11);printf("3.表项信息显示 SHOW TABLE");
	gotoxy(27,12);printf("4.表属性更改   PRO CHANGE");
	gotoxy(27,13);printf("0.返回上一级   RETURN UPPER");
	gotoxy(27,16);setcolor(0,11);printf("请选择:");setcolor(0,15);printf("[ ]");
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
					printf("输入有误,请重新输入!\a");
					getchar();
					goto bha;
	}
	goto bha;
}
void menusuper()
{
	if(CURRENTREVOKE==0)
	{
		gotoxy(1,24);printf("\a对不起,您的权限不允许这项操作.按任意键返回...");
		getch();
		return;
	}
bha:;
	clrscr();
	char chr;
	gotoxy(37,7);setcolor(0,14);
	printf("超级管理");setcolor(0,15);
	DrawFrame(20,60,5,15);
	gotoxy(27,9); printf("1.强行建立锁文件,并设为0");
	gotoxy(27,10);printf("2.窗口模式");
	gotoxy(27,11);printf("3.全屏模式");
	gotoxy(27,12);printf("4.设置是否快速载入");
	gotoxy(27,13);printf("0.返回上一级   RETURN UPPER");
	gotoxy(27,14);setcolor(0,11);printf("请选择:");setcolor(0,15);printf("[ ]");
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
					printf("输入有误,请重新输入!\a");
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
	printf("记录操作");setcolor(0,15);
	DrawFrame(20,60,5,15);
	gotoxy(27,9); printf("1.记录插入     RECORD INSERT");
	gotoxy(27,10);printf("2.记录删除     RECORD DELETE");
	gotoxy(27,11);printf("3.记录查询     RECORD QUERY");
	gotoxy(27,12);printf("4.记录更改     RECORD UPDATE");
	gotoxy(27,13);printf("0.返回上一级   RETURN UPPER");
	gotoxy(27,14);setcolor(0,11);printf("请选择:");setcolor(0,15);printf("[ ]");
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
					printf("输入有误,请重新输入!\a");
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
	printf("索引视图操作");setcolor(0,15);
	DrawFrame(20,60,5,16);
	gotoxy(27,9); printf("1.建立索引     INDEX CREATE");
	gotoxy(27,10);printf("2.删除索引     INDEX DROP");
	gotoxy(27,11);printf("3.显示索引     INDEX SHOW");
	gotoxy(27,12);printf("4.建立视图     VIEW CREATE");
	gotoxy(27,13);printf("5.删除视图     VIEW DROP");
	gotoxy(27,14);printf("6.显示视图     VIEW SHOW");
	gotoxy(27,15);printf("0.返回上一级   RETURN UPPER");
	gotoxy(27,17);setcolor(0,11);printf("请选择:");setcolor(0,15);printf("[ ]");
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
					printf("输入有误,请重新输入!\a");
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
	printf("关系代数");setcolor(0,15);
	DrawFrame(20,60,5,21);
	gotoxy(27,9); printf("1.并操作          Union");
	gotoxy(27,10);printf("2.交操作          Jiao");
	gotoxy(27,11);printf("3.差操作          DIFFERENCE");
	gotoxy(27,12);printf("4.投影操作        SHADOW");
	gotoxy(27,13);printf("5.选择操作θ单一  SELECT");
	gotoxy(27,14);printf("6.选择操作AND     SELECT AND");
	gotoxy(27,15);printf("7.选择操作OR      SELECT OR");
	gotoxy(27,16);printf("8.选择投影综合    SELECT WHERE");
	gotoxy(27,17);printf("9.自然/等值连接   NATRUAL/EQUAL");
	gotoxy(27,18);printf("0.返回上一级      RETURN UPPER");
	gotoxy(27,19);setcolor(0,11);printf("请选择:");setcolor(0,15);printf("[ ]");
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
					printf("输入有误,请重新输入!\a");
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
	printf("主菜单");setcolor(0,15);
	DrawFrame(20,60,5,21);
	gotoxy(27,9); printf("1.表的操作  建表，删表，查表");
	gotoxy(27,10);printf("2.记录操作  增，删，改");
	gotoxy(27,11);printf("3.索引视图操作");
	gotoxy(27,12);printf("4.关系代数操作");
	gotoxy(27,13);printf("5.关系演算查询功能");
	gotoxy(27,14);printf("6.系统的超级管理");
	gotoxy(27,15);printf("7.备份全体数据");
	gotoxy(27,16);printf("8.恢复指定的某个备份");
	gotoxy(27,17);printf("9.用户重登陆");
	gotoxy(27,18);printf("0.退出");
	gotoxy(27,19);setcolor(0,11);printf("请选择:");setcolor(0,15);printf("[ ]");
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
					printf("输入有误,请重新输入!\a");
					getchar();
					goto bha;
	}
	goto bha;
}

int main()
{
	if((alfp=fopen("log.txt","a"))==NULL)
		alfp=fopen("log.txt","w+");

	NT_SetConsoleDisplayMode(GetStdHandle(STD_OUTPUT_HANDLE),0);	//调用dos全屏函数
	Sleep(400);
	fprintf(alfp,"*******************************************");fputc('\n',alfp);

	fprintf(alfp,"\n[%d年%d月%d日 %d:%d:%d]执行如下操作:\n",GetCurrentYear(),GetCurrentMonth(),GetCurrentDay(),
									GetCurrentHour(),GetCurrentMinute(),GetCurrentSecond() );
	fprintf(alfp,"调用DOS全屏函数.");fputc('\n',alfp);
	fprintf(alfp,"读取数据并初始化各项信息.");fputc('\n',alfp);
	initialize();
	fileread();
	login();
	mainmenu();
	fprintf(alfp,"\n[%d年%d月%d日 %d:%d:%d]执行如下操作:\n",GetCurrentYear(),GetCurrentMonth(),GetCurrentDay(),
									GetCurrentHour(),GetCurrentMinute(),GetCurrentSecond() );
	fprintf(alfp,"用户正常退出操作.");fputc('\n',alfp);
	fclose(alfp);
	return 0;
}

/*
实验二 创建数据库（DDL）功能模拟**************************************************************

  （一）实验目的：

  （1）了解关系数据库定义语言的功能。
   （2）熟悉关系数据库建库命令的功能。
   （3）掌握用高级程序设计语言创建数据库的方法。
   （4）掌握以表格形式显示运算结果的方法。

 （二）实验内容与步骤：

   1、用C语言（或其它语言）建立"建库"功能的总体菜单（下拉菜单或文本方式），
	包括建立基本表的结构、输入数据、显示数据库中基本表的结构和内容、退出等功能。
   2 、用C语言实现建立数据库表的结构的功能。要求：
    （1）能输入任意关系名。
  （2）字段个数、名称任意给定（包括字段名、字段类型、长度任意）。
    （3）要求以文件形式保存基本表。（存储结构也可自行设计）

   3、用C语言实现输入数据库记录的功能（插入位置任意）。
   4、用C语言实现删除数据库记录的功能（删除位置任意）。
   5、用C语言实现显示数据库结构和内容（最好以表格形式显示）
   6、用C语言实现对主关键字的索引功能。
   7、用C语言实现对其它关键字的索引功能。
8、用C语言实现建立视图的功能。
   9、用C语言建立数据字典保存三级模式中有关视图、基本表、存储文件的定义。
   10、设计每项功能完成后均返回主菜单的功能。

实验三 关系代数语言的集合运算功能模拟**************************************************************
  （一）实验目的：

   （1）了解集合运算的功能。
   （2）掌握关系代数语言的并、交、差运算。
   （3）掌握C语言实现并、交、差运算的设计方法。
   （4）掌握以表格形式显示并、交、差运算结果的方法。

（二）实验内容与步骤：

 1、设计该实验的功能菜单。
 2、用C语言实现建立两个相容的关系。
 3、用C语言实现关系代数语言的"并"运算功能，并以表格形式显示运算的结果。
 4、用C语言实现关系代数语言的"交"运算功能，并以表格形式显示运算的结果。
 5、用C语言实现关系代数语言的"差"运算功能，并以表格形式显示运算的结果。
 6、设计每项功能完成后均返回主菜单的功能。

实验四 关系代数语言的专门运算功能模拟**************************************************************

（一）实验目的：
（1）熟悉高级语言编译程序的思想。
（2）掌握符号串处理的方法和技术。
（3）掌握关系代数语言的选取、投影、连接运算的功能。
（4）掌握关系代数语言的选取、投影、连接运算的实现技术。
（5）掌握关系数据库的查询优化技术。

（二）实验内容与步骤：

1、将关系代数语言的选取、投影运算公式进行等价变换，以便输入计算机，被计算机接受和识别。
2、建立任意一关系数据库。
3、采用索引的方式，用C语言实现对该库中关系的某一字段的"θ"（=、≠、≤、≥、<、>）条件的选取运算，
	并以表格形式显示运算的结果。

4、采用索引的方式，用C语言实现对该库中关系的带有"与"条件的查询，并以表格形式显示运算的结果。
5、采用索引的方式，用C语言实现对该库中关系的带有"或"条件的查询，并以表格形式显示运算的结果。

6、用C语言实现对该库的任意字段的投影运算，并以表格形式显示运算的结果。

7、用C语言实现对该库的选取、投影综合运算，并以表格形式显示运算的结果。

8、采用索引的方式，用C语言实现对两个关系的自然连接和等值连接运算，并以表格形式显示运算的结果。

实验五 简单的关系演算（DML）功能模拟                10************************************************

（一）实验目的：

（1）掌握关系演算语言的查询功能。
（2）熟悉符号串识别和处理技术。
（3）掌握关系演算语言的查询功能的实现方法和技术。

（二）实验内容与步骤：

（1）系统掌握DBMS中关系演算语言的功能，以QUEL语言为样本，用C语言实现对用规定符号表达的
     简单查询命令行进行扫描和识别，并以表格形式显示查询结果。
（2）熟悉QUEL语言的查询功能，用高级语言实现对含有“与”、“或”、“非”
     符号的命令行进行扫描和识别，并以表格形式显示查询结果。

实验六 关系数据库的查询优化                        5**********************************************************
（一）实验目的：
（1）掌握关系系统的定义及分类。
（2）了解全关系系统的概念。
（3）了解优化的一般策略。
（4）掌握关系代数等价变换规则。
（5）掌握关系数据库系统的查询优化技术。
（6）掌握关系代数表达式的优化算法。

（二）实验内容与步骤：
 1、采用启发式关系代数优化算法：
(1)把查询转换成某种内部表示。
(2)把语法树转换成标准形式。
(3)选择低层的存取路径。
(4) 用高级语言实现关系代数表达式优化算法。（对原始的语法树进行优化处理，生成查询计划，选择代价最小的。）
2、用高级语言实现启发式关系演算优化算法。

实验七  数据库保护（DCL）功能模拟**********************************************************

（一）实验目的：

（1）掌握数据库系统的安全性保护机制。		2*************************
（2）掌握数据库系统的完整性约束机制。		2*************************
（3）了解数据库系统的并发控制技术。			8*************************
（4）掌握数据库系统的恢复技术。				8*************************

（二）实验内容与步骤：

（1）要求根据数据字典（DD）实现对数据库的安全性检查。包括设置口令、密码和权限等。
（2）根据数据字典（DD）实现对数据库系统的完整性约束机制。
（3）利用事务的封锁机制实现对事务处理的并发控制；
（4）利用建立日志文件及周期性备份的机制实现对数据库系统的恢复。

实验八   综合设计**************************************************************************

（一）实验目的：

（1）系统地掌握关系代数语言的运算功能。
（2）了解DBMS的DDL、DML、DCL等功能的实质含义。
（3）掌握结构化设计方法。
（4）了解大型软件的设计技术。


（二）实验内容与步骤：
（1）采用软件工程的结构化设计方法，将前几个实验利用结构化设计方法综合为一个完整的DBMS原型系统，以达到系统掌握开发软件系统的方法和技术的目的。
（2）设计该系统的总体菜单。
（3）通过总体菜单选择某一功能进入子菜单。
（4）通过某一功能子菜单，进入相应各个子系统，完成具体的功能。
（5）通过辅助工具，进一步丰富和完善上述程序窗口、菜单的界面设计。
*/
