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
    int a,i;	//a代表的是num的位数
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
char *substr(char s[],int n1,int len)		//从已知字符串s中取从n1(下标),共n2长度的部分字串
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
	return s;	//确实要返回一个char型指针
}
void setcolor(unsigned short ForeColor=4,unsigned short BackGroundColor=0) 
{ 
	HANDLE hCon=GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hCon,BackGroundColor);
	//para2:15亮白,14亮黄,11亮青色,7正常白
	/*
			NR-16   NR-8    颜色名 ????????????????
            0       0       Black (黑)
            1       4       DarkBlue (深蓝)
            2       2       DarkGreen (深绿)
            3       6       DarkCyan (深青)
            4       1       DarkRed (深红)
            5       5       DarkMagenta (深品红)
            6       3       Brown，DarkYellow (褐、深黄)
            7       7       LightGray, LightGrey，Gray, Grey (浅灰)
            8       0*      DarkGray，DarkGrey (深灰)
            9       4*      Blue，LightBlue (浅蓝)
            10      2*      Green，LightGreen (浅绿)
            11      6*      Cyan，LightCyan (浅青)
            12      1*      Red，LightRed (浅品红)
            13      5*      Magenta，LightMagenta (浅红)
            14      3*      Yellow，LightYellow (浅黄)
            15      7*      White (白)
	*/
};
struct tm *getcurrenttime()
{

	time_t current_time;
	struct tm *current_tm;
	time(&current_time);
	current_time+=3600*8;
	current_tm=gmtime(&current_time);
/*	current_tm->    tm_hour小时  tm_sec秒   tm_min分  tm_mday日  tm_mon月  tm_year年   tm_wday星期
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
		return "上午";
	else
		return "下午";
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
int GetCurrentMonth()		//返回1-12,则是1-12月
{
	char si[4];
	strcpy(si,substr(asctime(getcurrenttime()),4,3));
	si[3]='\0';
	//si是月份的字符串
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
int GetCurrentWeek(int i)	//符合中国人的习惯，周一是1，周日是7
{
	char si[4];
	strcpy(si,substr(asctime(getcurrenttime()),0,3));
	si[3]='\0';
	//si是星期的字符串
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
char *GetCurrentWeek(char *i)	//符合中国人的习惯，周一是"一"，周日是"日"
{
	char si[4];
	strcpy(si,substr(asctime(getcurrenttime()),0,3));
	si[3]='\0';
	//si是星期的字符串
	if(strcmp(si,"Mon")==0)
		return "一";
	else if(strcmp(si,"Tue")==0)
		return "二";
	else if(strcmp(si,"Wed")==0)
		return "三";
	else if(strcmp(si,"Thu")==0)
		return "四";
	else if(strcmp(si,"Fri")==0)
		return "五";
	else if(strcmp(si,"Sat")==0)
		return "六";
	else if(strcmp(si,"Sun")==0)
		return "日";
	else 
		return "零";
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
	gotoxy(il,ju);printf("┏");
	gotoxy(il,jd);printf("┗");
	gotoxy(ir,ju);printf("┓");
	gotoxy(ir,jd);printf("┛");
	if((ir-il>2)&&(jd-ju>1))		//如果上下超过2行并且左右超过4个字符的可以，并且最好是成2倍关系
	{
		for(zbi=il+2;zbi<ir;zbi+=2)
		{
			gotoxy(zbi,ju);printf("━");
		}
		for(zbi=il+2;zbi<ir;zbi+=2)
		{
			gotoxy(zbi,jd);printf("━");
		}
		for(zbj=ju+1;zbj<jd;zbj++)
		{
			gotoxy(il,zbj);printf("┃");
		}
		for(zbj=ju+1;zbj<jd;zbj++)
		{
			gotoxy(ir,zbj);printf("┃");
		}
	}
}
void DrawWindow(int il,int ir,int ju,int jd)
{
	int zbi,zbj;	//坐标i,j
	for(zbi=il;zbi<=ir;zbi++)
		for(zbj=ju;zbj<=jd;zbj++)
		{
			gotoxy(zbi,zbj);printf(" ");
		}
	DrawFrame(il,ir,ju,jd);
}
//退出函数，这里以后要修改的
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
{		//x,y坐标    ForeColor1是这次的颜色，ForeColor0是还原上一次的颜色。
	setcolor(ForeColor1,BackGroundColor1);	//设置屏幕颜色
	gotoxy(x,y);
	printf("%s",str);
	setcolor(ForeColor0,BackGroundColor0);	//还原屏幕颜色
}
void PrintColorString(unsigned short ForeColor1,unsigned short BackGroundColor1,
						unsigned short ForeColor0,unsigned short BackGroundColor0,char *str)
{
    setcolor(ForeColor1,BackGroundColor1);	//设置屏幕颜色 
    printf("%s",str);
	setcolor(ForeColor0,BackGroundColor0);	//还原屏幕颜色
}
void exit()
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
	gotoxy(22,16);printf("┃              Microsko★段金晟  ┃");
	gotoxy(22,17);printf("┃              05计算机20050675  ┃");
	gotoxy(22,18);printf("┗━━━━━━━━━━━━━━━━┛");
	gotoxy(5,23);printf("\n\n");setcolor(0,7);
	exit(-1);
}
