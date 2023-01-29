/*
	POWER.H,WHICH INCLUDES MANY FUNCTION OF THE OS.
*/

#include "windows.h"
#include <time.h>
#include <ctype.h>



//********************video function************************************
void gotoxy(int x, int y)    //gotoxy().
{
	COORD c;
	c.X = x - 1;
	c.Y = y - 1;
	SetConsoleCursorPosition (GetStdHandle(STD_OUTPUT_HANDLE), c);
} 
void clrscr()
{
	system("cls");
}
int strtonum(char *str)
{
	int i=0,sum=0;
	while(str[i]!='\0')
	{
		sum=sum*10;
		sum=sum+str[i]-48;
		i++;
	}
	return sum;
}
char *numtostr(int num)
{
    int a,i;	//a�������num��λ��
    int sum=1;
    char r[6];
    for(i=0;i<6;i++)
		r[i]='\0';
	if(num/10000>0)
		a=5;
    else if(num/1000>0)
        a=4;
    else if(num/100>0)
        a=3;
    else if(num/10>0)
        a=2;
    else
        a=1;
    for(i=a-1;i>=0;i--)
    {
        r[i]=num%10+48;
        num/=10;                    
    }
    return r;    
}
char *substr(char s[],int n1,int len)		//����֪�ַ���s��ȡ��n1(�±�),��n2���ȵĲ����ִ�
{ 
	char ret[10];
	int i=0; 
	for(;i<len;i++) 
		ret[i]=s[n1+i]; 
	ret[i]='\0';
	char *p=ret;
	return p; 
}
char *InputStarString()
{
	char str[50],*s;
	char ch;
	int i=0;
	ch=getch();
	while(1)
	{
		str[i++]=ch;
		printf("*");
		ch=getch();
		if(ch==13) break;
	}
	for(;i<50;i++)
		str[i]='\0';
	s=str;
	return s;	//ȷʵҪ����һ��char��ָ��
}
void setcolor(unsigned short ForeColor=4,unsigned short BackGroundColor=0) 
{ 
	HANDLE hCon=GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hCon,BackGroundColor);
	//para2:15����,14����,11����ɫ,7������
	/*
			NR-16   NR-8    ��ɫ�� ????????????????
            0       0       Black (��)
            1       4       DarkBlue (����)
            2       2       DarkGreen (����)
            3       6       DarkCyan (����)
            4       1       DarkRed (���)
            5       5       DarkMagenta (��Ʒ��)
            6       3       Brown��DarkYellow (�֡����)
            7       7       LightGray, LightGrey��Gray, Grey (ǳ��)
            8       0*      DarkGray��DarkGrey (���)
            9       4*      Blue��LightBlue (ǳ��)
            10      2*      Green��LightGreen (ǳ��)
            11      6*      Cyan��LightCyan (ǳ��)
            12      1*      Red��LightRed (ǳƷ��)
            13      5*      Magenta��LightMagenta (ǳ��)
            14      3*      Yellow��LightYellow (ǳ��)
            15      7*      White (��)
	*/
};
struct tm *getcurrenttime()
{

	time_t current_time;
	struct tm *current_tm;
	time(&current_time);
	current_time+=3600*8;
	current_tm=gmtime(&current_time);
/*	current_tm->    tm_hourСʱ  tm_sec��   tm_min��  tm_mday��  tm_mon��  tm_year��   tm_wday����
 	printf("%s",asctime(current_tm));
	Wed Jul 09 11:10:28 2008    */
	return current_tm;	    
}
int GetCurrentHour()
{
	char si[3];
	strcpy(si,substr(asctime(getcurrenttime()),11,2));
	si[2]='\0';
	return strtonum(si);
}
char *AMORPM()
{
	if(GetCurrentHour()<12)
		return "����";
	else
		return "����";
}
int GetCurrentMinute()
{
	char si[3];
	strcpy(si,substr(asctime(getcurrenttime()),14,2));
	si[2]='\0';
	return strtonum(si);
}
int GetCurrentSecond()
{
	char si[3];
	strcpy(si,substr(asctime(getcurrenttime()),17,2));
	si[2]='\0';
	return strtonum(si);
}
int GetCurrentYear()
{
	char si[5];
	strcpy(si,substr(asctime(getcurrenttime()),20,4));
	si[5]='\0';
	return strtonum(si);
}
int GetCurrentMonth()		//����1-12,����1-12��
{
	char si[4];
	strcpy(si,substr(asctime(getcurrenttime()),4,3));
	si[3]='\0';
	//si���·ݵ��ַ���
	if(strcmp(si,"Jan")==0)
		return 1;
	else if(strcmp(si,"Feb")==0)
		return 2;
	else if(strcmp(si,"Mar")==0)
		return 3;
	else if(strcmp(si,"Apr")==0)
		return 4;
	else if(strcmp(si,"May")==0)
		return 5;
	else if(strcmp(si,"Jun")==0)
		return 6;
	else if(strcmp(si,"Jul")==0)
		return 7;
	else if(strcmp(si,"Aug")==0)
		return 8;
	else if(strcmp(si,"Sep")==0)
		return 9;
	else if(strcmp(si,"Oct")==0)
		return 10;
	else if(strcmp(si,"Nov")==0)
		return 11;
	else if(strcmp(si,"Dec")==0)
		return 12;
	else 
		return 0;
}
int GetCurrentWeek(int i)	//�����й��˵�ϰ�ߣ���һ��1��������7
{
	char si[4];
	strcpy(si,substr(asctime(getcurrenttime()),0,3));
	si[3]='\0';
	//si�����ڵ��ַ���
	if(strcmp(si,"Mon")==0)
		return 1;
	else if(strcmp(si,"Tue")==0)
		return 2;
	else if(strcmp(si,"Wed")==0)
		return 3;
	else if(strcmp(si,"Thu")==0)
		return 4;
	else if(strcmp(si,"Fri")==0)
		return 5;
	else if(strcmp(si,"Sat")==0)
		return 6;
	else if(strcmp(si,"Sun")==0)
		return 7;
	else 
		return 0;
}
char *GetCurrentWeek(char *i)	//�����й��˵�ϰ�ߣ���һ��"һ"��������"��"
{
	char si[4];
	strcpy(si,substr(asctime(getcurrenttime()),0,3));
	si[3]='\0';
	//si�����ڵ��ַ���
	if(strcmp(si,"Mon")==0)
		return "һ";
	else if(strcmp(si,"Tue")==0)
		return "��";
	else if(strcmp(si,"Wed")==0)
		return "��";
	else if(strcmp(si,"Thu")==0)
		return "��";
	else if(strcmp(si,"Fri")==0)
		return "��";
	else if(strcmp(si,"Sat")==0)
		return "��";
	else if(strcmp(si,"Sun")==0)
		return "��";
	else 
		return "��";
}
int GetCurrentDay()
{
	char si[3];
	strcpy(si,substr(asctime(getcurrenttime()),8,2));
	si[3]='\0';
	return strtonum(si);
}


BOOL NT_SetConsoleDisplayMode(HANDLE hOutputHandle, DWORD dwNewMode) 
{ 
	typedef BOOL (WINAPI *SCDMProc_t) (HANDLE, DWORD, LPDWORD); 
	SCDMProc_t SetConsoleDisplayMode; 
	HMODULE hKernel32; 
	BOOL bFreeLib = FALSE, ret; 
	const char KERNEL32_NAME[] = "kernel32.dll"; 

	hKernel32 = GetModuleHandleA(KERNEL32_NAME); 
	if (hKernel32 == NULL) 
	{ 
		hKernel32 = LoadLibraryA(KERNEL32_NAME); 
		if (hKernel32 == NULL) 
			return FALSE; 

		bFreeLib = TRUE; 
	} 

	SetConsoleDisplayMode = (SCDMProc_t)GetProcAddress(hKernel32, "SetConsoleDisplayMode"); 
	if (SetConsoleDisplayMode == NULL) 
	{ 
		SetLastError(ERROR_CALL_NOT_IMPLEMENTED); 
		ret = FALSE; 
	} 
	else 
	{ 
		DWORD dummy; 
		ret = SetConsoleDisplayMode(hOutputHandle, dwNewMode, &dummy); 
	} 

	if (bFreeLib) 
		FreeLibrary(hKernel32); 
	return ret; 
}//NT_SetConsoleDisplayMode

void DrawFrame(int il,int ir,int ju,int jd)
{
	int zbi,zbj;
	gotoxy(il,ju);printf("��");
	gotoxy(il,jd);printf("��");
	gotoxy(ir,ju);printf("��");
	gotoxy(ir,jd);printf("��");
	if((ir-il>2)&&(jd-ju>1))		//������³���2�в������ҳ���4���ַ��Ŀ��ԣ���������ǳ�2����ϵ
	{
		for(zbi=il+2;zbi<ir;zbi+=2)
		{
			gotoxy(zbi,ju);printf("��");
		}
		for(zbi=il+2;zbi<ir;zbi+=2)
		{
			gotoxy(zbi,jd);printf("��");
		}
		for(zbj=ju+1;zbj<jd;zbj++)
		{
			gotoxy(il,zbj);printf("��");
		}
		for(zbj=ju+1;zbj<jd;zbj++)
		{
			gotoxy(ir,zbj);printf("��");
		}
	}
}
void DrawWindow(int il,int ir,int ju,int jd)
{
	int zbi,zbj;	//����i,j
	for(zbi=il;zbi<=ir;zbi++)
		for(zbj=ju;zbj<=jd;zbj++)
		{
			gotoxy(zbi,zbj);printf(" ");
		}
	DrawFrame(il,ir,ju,jd);
}
//�˳������������Ժ�Ҫ�޸ĵ�
void PrintCharXY(int x,int y,char ch)
{
	gotoxy(x,y);
	printf("%c",ch);
}
void PrintStringXY(int x,int y,char *str)
{
	gotoxy(x,y);
	printf("%s",str);
}
void PrintColorStringXY(int x,int y,unsigned short ForeColor1,unsigned short BackGroundColor1,
						unsigned short ForeColor0,unsigned short BackGroundColor0,char *str)
{		//x,y����    ForeColor1����ε���ɫ��ForeColor0�ǻ�ԭ��һ�ε���ɫ��
	setcolor(ForeColor1,BackGroundColor1);	//������Ļ��ɫ
	gotoxy(x,y);
	printf("%s",str);
	setcolor(ForeColor0,BackGroundColor0);	//��ԭ��Ļ��ɫ
}
void PrintColorString(unsigned short ForeColor1,unsigned short BackGroundColor1,
						unsigned short ForeColor0,unsigned short BackGroundColor0,char *str)
{
    setcolor(ForeColor1,BackGroundColor1);	//������Ļ��ɫ 
    printf("%s",str);
	setcolor(ForeColor0,BackGroundColor0);	//��ԭ��Ļ��ɫ
}
void exit()
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
	gotoxy(22,16);printf("��              Microsko��ν���  ��");
	gotoxy(22,17);printf("��              05�����20050675  ��");
	gotoxy(22,18);printf("������������������������������������");
	gotoxy(5,23);printf("\n\n");setcolor(0,7);
	exit(-1);
}
