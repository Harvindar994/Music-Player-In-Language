#define WINVER 0x0501 // WinXP and UP
#include<stdio.h>
#include<conio.h>
#include<windows.h>
#include<mmsystem.h>
#include<stdlib.h>
#include<time.h>
#include<dir.h>
#include<string.h>
void remove_console_buttons()
{
  LONG style;
  HWND ConsoleWindow;

  ConsoleWindow = GetConsoleWindow();

  style = GetWindowLong(ConsoleWindow, GWL_STYLE);
  style &= ~( WS_MINIMIZEBOX | WS_SYSMENU );
  SetWindowLongPtr(ConsoleWindow, GWL_STYLE, style);

  SetWindowPos(ConsoleWindow, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED |SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
  //Resize
     //system ("mode con cols=75 lines=50");
}
int detect_windows_drive(char *ch)
{
    char a='a';
    FILE *fp;
    char path[]="a:\\Windows\\System32\\control.exe";
    while(a<='z')
    {
        fp=fopen(path,"r");
        if(fp!=NULL)
        {
            *ch=a;
            fclose(fp);
            return 1;
        }
        a++;
        path[0]=a;
    }
    return 0;
}
void setcolor(int ForgC)
{
    WORD wColor;
    HANDLE hStdOut=GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if(GetConsoleScreenBufferInfo(hStdOut,&csbi))
    {
    wColor=(csbi.wAttributes & 0xF0)+(ForgC & 0x0F);
    // SetConsoleTextAttributes(hStdOut,wColor);
    SetConsoleTextAttribute(hStdOut,wColor);
    }
}
int input_password(char *a,int lenth,int x,int y)
{
    int i=0,j;
    for(i=0;i<lenth;i++)
    {

        abc:
        gotoxy(x,y);
        a[i]=getche();
        if(a[i]==-32)
        {
            a[i]=getche();
            gotoxy(x,y);
            printf("%c",256);
            goto abc;
        }
		if(a[i]==27)
		{
			a[i]='\0';
			return 202;
		}
        if(a[i]!=8)
        {
            gotoxy(x,y);
            printf("*");
        }
        if(a[i]==8)
        {
            if(i==0)
            {
                goto abc;
            }
            else
            {
            i--;
            x--;
            gotoxy(x,y);
            printf("%c",256);
            goto abc;
            }
        }
        if((a[i]>='a' && a[i]<='z') || (a[i]>='A' && a[i]<='Z') || (a[i]>=48 && a[i]<=57))
        {

        }
        else if(a[i]==33 || a[i]==64 || a[i]==35 || a[i]==36 ||a[i]==37 || a[i]==94 ||a[i]==38 || a[i]==42)
        {

        }
        else if(a[i]==13 && i>=8)
        {
            a[i]='\0';
            return;
        }
        else
        {

            gotoxy(x,y);
            printf("%c",256);
            goto abc;
        }
        x++;
    }
    a[i]='\0';
    return a;
}
void make_frame(int x,int x1,int y,int y1)
{
    int i;
    for(i=x+1;i<=x1-1;i++)
    {
        gotoxy(i,y);
        printf("%c",196);
        gotoxy(i,y1);
        printf("%c",196);
    }
    for(i=y+1;i<=y1-1;i++)
    {
        gotoxy(x,i);
        printf("%c",179);
        gotoxy(x1,i);
        printf("%c",179);
    }
    gotoxy(x,y);
    printf("%c",218);
    gotoxy(x,y1);
    printf("%c",192);
    gotoxy(x1,y1);
    printf("%c",217);
    gotoxy(x1,y);
    printf("%c",191);
}
void rec_MSGinM(char *string,int x,int x1,int y)
{
    int H_lenth,middile_p;
    H_lenth=(strlen(string))/2;
    middile_p=x1-((x1-x)/2);
    gotoxy(middile_p-H_lenth+1,y);
    printf("%s",string);
}
void line_V(int y,int y1,int x,int bands_type,int direction,int plus_y)
{
    int i;
    for(i=y;i<=y1;i++)
    {
        gotoxy(x,i);
        printf("%c",179);
    }
    if(direction==117)
    {
        gotoxy(x,y);
        if(bands_type==116)
            printf("%c",194);
        if(bands_type==112)
            printf("%c",197);
    }
    else if(direction==100)
    {
        gotoxy(x,y1);
        if(bands_type==116)
            printf("%c",193);
        if(bands_type==112)
            printf("%c",197);
    }
    else if(direction==97)
    {
        gotoxy(x,y);
        if(bands_type==116)
            printf("%c",194);
        if(bands_type==112)
            printf("%c",197);
        gotoxy(x,y1);
        if(bands_type==116)
            printf("%c",193);
        if(bands_type==112)
            printf("%c",197);
    }
    if(plus_y!=0)
    {
        gotoxy(x,plus_y);
        printf("%c",197);
    }
}
void line_H(int x,int x1,int y,int bands_type,int direction)
{
    int i;
    for(i=x;i<=x1;i++)
    {
        gotoxy(i,y);
        printf("%c",196);
    }
    if(direction==108)
    {
        gotoxy(x,y);
        if(bands_type==116)
            printf("%c",195);
        if(bands_type==112)
            printf("%c",197);
    }
    else if(direction==114)
    {
        gotoxy(x1,y);
        if(bands_type==116)
            printf("%c",180);
        if(bands_type==112)
            printf("%c",197);
    }
    else if(direction==97)
    {
        gotoxy(x,y);
        if(bands_type==116)
            printf("%c",195);
        if(bands_type==112)
            printf("%c",197);
        gotoxy(x1,y);
        if(bands_type==116)
            printf("%c",180);
        if(bands_type==112)
            printf("%c",197);
    }
}
int yes_or_no_choise(char *a)
{
    system("COLOR F9");
	setcolor(12);
    int i,j,y,x1,y1;
    char ch;
    for(i=8;i<=17;i++)
    {
        gotoxy(20,i);
        printf("%c",219);
        gotoxy(65,i);
        printf("%c",219);
    }
    for(i=20;i<=65;i++)
    {
        gotoxy(i,8);
        printf("%c",219);
        gotoxy(i,17);
        printf("%c",219);
    }
	setcolor(7);
	drowlineH(22,66,18,219);
	drowlineV(9,18,66,219);
	setcolor(9);
    x1=22;
    y1=10;
    for(i=0;a[i]!='\0';i++)
    {
        gotoxy(x1,y1);
        printf("%c",a[i]);
        if(x1==65 && y1==12)
            break;
        if(x1==65)
        {
            x1=21;
            y1++;
        }
        x1++;
    }
    gotoxy(40,14);
    printf("Yes");
    gotoxy(58,14);
    printf("No");
    y=53;               //53,36
    reprint:
    for(i=y;i<=y+10;i++)
    {
        gotoxy(i,13);
        printf("%c",220);
        gotoxy(i,15);
        printf("%c",223);
    }
    for(i=14;i<=14;i++)
    {
        gotoxy(y,i);
        printf("%c",219);
        gotoxy(y+10,i);
        printf("%c",219);
    }
    gotoxy(1,1);
    ch=getche();
    gotoxy(1,1);
    printf("%c",256);
    if(ch==-32)
    {
        ch=getche();
        if(ch==75 || ch==77)
        {
            switch(ch)
            {
            case 75:
                setcolor(12);
                if(y==53)
                {
                    for(i=y;i<=y+10;i++)
                    {
                        gotoxy(i,13);
                        printf("%c",256);
                        gotoxy(i,15);
                        printf("%c",256);
                    }
                    for(i=14;i<=14;i++)
                    {
                        gotoxy(y,i);
                        printf("%c",256);
                        gotoxy(y+10,i);
                        printf("%c",256);
                    }
                    y=y-17;
                }
                goto reprint;
            case 77:
                setcolor(9);
                if(y==36)
                {
                    for(i=y;i<=y+10;i++)
                    {
                        gotoxy(i,13);
                        printf("%c",256);
                        gotoxy(i,15);
                        printf("%c",256);
                    }
                    for(i=14;i<=14;i++)
                    {
                        gotoxy(y,i);
                        printf("%c",256);
                        gotoxy(y+10,i);
                        printf("%c",256);
                    }
                    y=y+17;
                }
                goto reprint;
            }
        }
        else
        {
            goto reprint;
        }
    }
    else if(ch==13)
    {
        switch(y)
        {
        case 53:
            return 0;
            break;
        case 36:
            return 1;
            break;
        }
    }
    else
    {
        goto reprint;
    }
}
int drow_search_box(char *p,int pos,int y,int y1)
{
    int i,j=6;
    j=j+strlen(p);
    for(i=1;i<=78;i++)
    {
        gotoxy(i,y);
        printf("%c",219);
        gotoxy(i,y1);
        printf("%c",219);
    }
    for(i=y;i<=y1;i++)
    {
        gotoxy(1,i);
        printf("%c",219);
        gotoxy(78,i);
        printf("%c",219);
        gotoxy(j,i);
        printf("%c",219);
    }
    gotoxy(3,pos);
    printf("%s",p);
    gotoxy(j+2,pos);
    return j+2;
}
void get_system_date(int *day,int *month,int *year)
{
    SYSTEMTIME st;
    GetSystemTime(&st);
    *day=st.wDay;
    *month=st.wMonth;
    *year=st.wYear;
}
int input_mobile(char *a,int lenth,int x,int y)
{
    int i=0;
    for(i=0;i<lenth;i++)
    {
        abc:
        gotoxy(x,y);
        a[i]=getche();
        if(a[i]==-32)
        {
            a[i]=getche();
            gotoxy(x,y);
            printf("%c",256);
            goto abc;
        }
		if(a[i]==27)
		{
			a[i]='\0';
			return 202;
		}
        if(a[i]==8)
        {
            if(i==0)
            {
                goto abc;
            }
            else
            {
            i--;
            x--;
            gotoxy(x,y);
            printf("%c",256);
            goto abc;
            }

        }
        if((a[i]>=48 && a[i]<=57))
        {

        }
        else if(a[i]==13 && i==10)
        {
            a[i]='\0';
            return 1;
        }
        else
        {
            gotoxy(x,y);
            printf("%c",256);
            goto abc;
        }
        x++;
    }
    a[i]='\0';
    return a;
}
int input_string(char *a,int lenth,int x,int y)
{
    int i=0;
    for(i=0;i<lenth;i++)
    {
        abc:
        gotoxy(x,y);
        a[i]=getche();
        if(a[i]==-32)
        {
            a[i]=getche();
            gotoxy(x,y);
            printf("%c",256);
            goto abc;
        }
		if(a[i]==27)
		{
			a[i]='\0';
			return 202;
		}
        if(a[i]==8)
        {
            if(i==0)
            {
                goto abc;
            }
            else
            {
            i--;
            x--;
            gotoxy(x,y);
            printf("%c",256);
            goto abc;
            }

        }
        if((a[i]>='a' && a[i]<='z') || (a[i]>='A' && a[i]<='Z'))
        {

        }
        else if(a[i]==32 && i!=0 && a[i-1]!=' ')
        {

        }
        else if(a[i]==13 && i>=5)
        {
            a[i]='\0';
            return 1;
        }
        else
        {
            gotoxy(x,y);
            printf("%c",256);
            goto abc;
        }
        x++;
    }
    a[i]='\0';
    return a;
}
int input_Number_char(char *a,int lenth,int x,int y)
{
    int i=0;
    for(i=0;i<lenth;i++)
    {
        abc:
        gotoxy(x,y);
        a[i]=getche();
        if(a[i]==-32)
        {
            a[i]=getche();
            gotoxy(x,y);
            printf("%c",256);
            goto abc;
        }
        if(a[i]==8)
        {
            if(i==0)
            {
                goto abc;
            }
            else
            {
            i--;
            x--;
            gotoxy(x,y);
            printf("%c",256);
            goto abc;
            }

        }
        if((a[i]>='a' && a[i]<='z') || (a[i]>='A' && a[i]<='Z'))
        {

        }
        else if(a[i]==32 && i!=0 && a[i-1]!=' ')
        {

        }
        else if(a[i]>=48 && a[i]<=57)
        {

        }
        else if(a[i]==13 && i>=5)
        {
            a[i]='\0';
            return 1;
        }
        else
        {
            gotoxy(x,y);
            printf("%c",256);
            goto abc;
        }
        x++;
    }
    a[i]='\0';
    return a;
}
int input_email(char *a,int lenth,int x,int y)
{
    int i=0;
    int count=0;
    int tempx,tempy;
    int dot=0,last_position=0,last_atrate=0;
    tempx=x;
    tempy=y;
    creset:
    for(i=0;i<lenth;i++)
    {
        abc:
        gotoxy(x,y);
        a[i]=getche();
		if(a[i]==27)
		{
			a[i]='\0';
			return 202;
		}
        if(a[i]==-32)
        {
            a[i]=getche();
            gotoxy(x,y);
            printf("%c",256);
            goto abc;
        }
        if(a[i]==8)
        {
            if(i==0)
            {
                goto abc;
            }
            else
            {
            i--;
            x--;
            if(a[i]=='@')
            {
                count=0;
                last_atrate=0;
            }
            if(a[i]=='.')
                last_position=0;
            gotoxy(x,y);
            printf("%c",256);
            goto abc;
            }

        }
        if((a[i]>='a' && a[i]<='z'))
        {

        }
        else if( a[i]=='.' && i!=0 && a[i-1]!='.' && a[i-1]!='@' && a[i-1]!='-')
        {
            dot++;
            last_position=i;
        }
        else if(a[i]=='@' && i!=0 && a[i-1]!='@' && a[i-1]!='.' && a[i-1]!='-' && count<1)
        {
            count++;
            last_atrate=i;
        }
        else if(a[i]>='0' && a[i]<='9')
        {

        }
        else if( a[i]=='-' && i!=0 && a[i-1]!='-' && a[i-1]!='@' && a[i-1]!='.')
        {

        }
        else if(a[i]==13 && i>=5 && dot!=0 && count!=0 && a[i-1]!='.' && a[i-1]!='@' && a[i-1]!='-' && last_position>last_atrate)
        {
            a[i]='\0';
            if((a[last_position+1]=='\0' || a[last_position+2]=='\0'))
            {
                gotoxy(x,y);
                printf("%c",256);
                goto abc;
            }
            return 1;
        }
        else
        {
            gotoxy(x,y);
            printf("%c",256);
            goto abc;
        }
        x++;
    }
    a[i]='\0';
    if( dot!=0 && count!=0 && a[i-1]!='.' && a[i-1]!='@' && a[i-1]!='-' && last_position>last_atrate)
    {
        if((a[last_position+1]!='\0' && a[last_position+2]!='\0'))
        {
            return a;
        }
        else
        {
            x=tempx;
            y=tempy;
            for(i=x;i<=x+lenth;i++)
            {
                gotoxy(i,y);
                printf("%c",256);
            }
            last_position=0;
            last_atrate=0;
            count=0;
            dot=0;
            goto creset;
        }
    }
    else
    {
        x=tempx;
        y=tempy;
        for(i=x;i<=x+lenth;i++)
        {
            gotoxy(i,y);
            printf("%c",256);
        }
        last_position=0;
        last_atrate=0;
        count=0;
        dot=0;
        goto creset;
    }
}
int input_date(int *day1,int *month1,int *year1,int x,int y,int ex,int ey)
{
    int ch;
    int a,day=0,month=0,year=0,count=1,m,loop=0,i,j;
    m=x;
    gotoxy(x,y);
    printf("dd/mm/yyyy");
    restart:
    gotoxy(x,y);
    ch=getche();
    systemback:
    if(ch==-32)
    {
        ch=getche();
        gotoxy(x,y);
        printf("%c",256);
    }
    if(ch>=48 && ch<=57)
    {
        switch(ch)
        {
        case 48:
            a=0;
            break;
        case 49:
            a=1;
            break;
        case 50:
            a=2;
            break;
        case 51:
            a=3;
            break;
        case 52:
            a=4;
            break;
        case 53:
            a=5;
            break;
        case 54:
            a=6;
            break;
        case 55:
            a=7;
            break;
        case 56:
            a=8;
            break;
        case 57:
            a=9;
            break;
        }
        if(count<=2)
        {
            day=(day*10)+a;
            count++;
            x++;
            if(count==3)
            {
                printf("/");
                x++;
            }
            if(count==3)
            {

                    if(day>31 || day<1)
                    {
                                ch=8;
                                loop=1;
                                gotoxy(ex,ey);
                                printf("Invalid Day");
                                for(i=0;i<=100000000;i++);
                                for(i=ex;i<=ex+12;i++)
                                {
                                    gotoxy(i,ey);
                                    printf("%c",256);
                                    for(j=0;j<=10000000;j++);
                                }
                                goto systemback;
                    }
            }
            goto restart;
        }
        else if(count<=4)
        {
            month=(month*10)+a;
            count++;
            x++;
            if(count==5)
            {
                x++;
                printf("/");
            }
            if(count==5)
            {
                if(month>12 || month<1)
                {
                            ch=8;
                            loop=1;
                            gotoxy(ex,ey);
                            printf("Invalid Month");
                            for(i=0;i<=100000000;i++);
                            for(i=ex;i<=ex+12;i++)
                            {
                                gotoxy(i,ey);
                                printf("%c",256);
                                for(j=0;j<=10000000;j++);
                            }
                            goto systemback;
                }
            }
            if(count==5)
            {
                if(day>28)
                {
                    if(month==2)
                    {
                            ch=8;
                            loop=1;
                            gotoxy(ex,ey);
                            printf("Invalid Month");
                            for(i=0;i<=100000000;i++);
                            for(i=ex;i<=ex+12;i++)
                            {
                                gotoxy(i,ey);
                                printf("%c",256);
                                for(j=0;j<=10000000;j++);
                            }
                            goto systemback;
                    }
                }
                if(day>30)
                {
                    if(month==4 || month==6 || month==9 || month==11)
                    {
                            ch=8;
                            loop=1;
                            gotoxy(ex,ey);
                            printf("Invalid Month");
                            for(i=0;i<=100000000;i++);
                            for(i=ex;i<=ex+12;i++)
                            {
                                gotoxy(i,ey);
                                printf("%c",256);
                                for(j=0;j<=10000000;j++);
                            }
                            goto systemback;
                    }
                }
            }
            goto restart;
        }
        else if(count<=8)
        {
            year=(year*10)+a;
            count++;
            x++;
            if(count==9)
            {
                if(year<2018)
                {
                            ch=8;
                            loop=3;
                            gotoxy(ex,ey);
                            printf("Invalid Year");
                            for(i=0;i<=100000000;i++);
                            for(i=ex;i<=ex+12;i++)
                            {
                                gotoxy(i,ey);
                                printf("%c",256);
                                for(j=0;j<=10000000;j++);
                            }
                            goto systemback;
                }
            }
            goto restart;
        }
        else
        {
            *day1=day;
            *month1=month;
            *year1=year;
            return 1;
        }
    }
    else if(ch==13)
    {
        if(day==0 || month==0 || year<2018)
        {
            gotoxy(x,y);
            printf("%c",256);
            goto restart;
        }
        *day1=day;
        *month1=month;
        *year1=year;
        return 1;
    }
    else if(ch==8)
    {
        gotoxy(x,y);
        printf("%c",256);
        if(count>1)
        {
            count--;
        }
        if(count>=5 && count<=8)
        {
            year=year/10;
        }
        if(count>=3 && count<=4)
        {
            month=month/10;
        }
        if(count>=1 && count<=2)
        {
            day=day/10;
        }
        if(count==4)
        {
            gotoxy(x,y);
            printf("%c",256);
            x--;
            gotoxy(x,y);
            printf("%c",256);
        }
        if(count==2)
        {
            gotoxy(x,y);
            printf("%c",256);
            x--;
            gotoxy(x,y);
            printf("%c",256);
        }
        if(x>m)
        {
            x--;
            gotoxy(x,y);
            printf("%c",256);
        }
        if(loop==0)
        goto restart;
        else
        {
            while(loop)
            {
                loop--;
                goto systemback;
            }
        }
    }
    else
    {
        gotoxy(x,y);
        printf("%c",256);
        goto restart;
    }
}
void encryption(char *a)
{
    int i;
    for(i=0;a[i]!='\0';i++)
    {
        if(a[i]>='a' && a[i]<='z')
        {
            switch(a[i])
            {
            case 97:
                a[i]=154;
                break;
            case 98:
                a[i]=155;
                break;
            case 99:
                a[i]=156;
                break;
            case 100:
                a[i]=157;
                break;
            case 101:
                a[i]=158;
                break;
            case 102:
                a[i]=159;
                break;
            case 103:
                a[i]=160;
                break;
            case 104:
                a[i]=161;
                break;
            case 105:
                a[i]=162;
                break;
            case 106:
                a[i]=163;
                break;
            case 107:
                a[i]=164;
                break;
            case 108:
                a[i]=165;
                break;
            case 109:
                a[i]=166;
                break;
            case 110:
                a[i]=167;
                break;
            case 111:
                a[i]=168;
                break;
            case 112:
                a[i]=169;
                break;
            case 113:
                a[i]=170;
                break;
            case 114:
                a[i]=171;
                break;
            case 115:
                a[i]=172;
                break;
            case 116:
                a[i]=173;
                break;
            case 117:
                a[i]=174;
                break;
            case 118:
                a[i]=175;
                break;
            case 119:
                a[i]=176;
                break;
            case 120:
                a[i]=177;
                break;
            case 121:
                a[i]=178;
                break;
            case 122:
                a[i]=179;
                break;
            }
        }
        else if(a[i]>='A' && a[i]<='Z')
        {
            switch(a[i])
            {
            case 65:
                a[i]=128;
                break;
            case 66:
                a[i]=129;
                break;
            case 67:
                a[i]=130;
                break;
            case 68:
                a[i]=131;
                break;
            case 69:
                a[i]=132;
                break;
            case 70:
                a[i]=133;
                break;
            case 71:
                a[i]=134;
                break;
            case 72:
                a[i]=135;
                break;
            case 73:
                a[i]=136;
                break;
            case 74:
                a[i]=137;
                break;
            case 75:
                a[i]=138;
                break;
            case 76:
                a[i]=139;
                break;
            case 77:
                a[i]=140;
                break;
            case 78:
                a[i]=141;
                break;
            case 79:
                a[i]=142;
                break;
            case 80:
                a[i]=143;
                break;
            case 81:
                a[i]=144;
                break;
            case 82:
                a[i]=145;
                break;
            case 83:
                a[i]=146;
                break;
            case 84:
                a[i]=147;
                break;
            case 85:
                a[i]=148;
                break;
            case 86:
                a[i]=149;
                break;
            case 87:
                a[i]=150;
                break;
            case 88:
                a[i]=151;
                break;
            case 89:
                a[i]=152;
                break;
            case 90:
                a[i]=153;
                break;
            }
        }
        else if(a[i]>=48 && a[i]<=57)
        {
            switch(a[i])
            {
            case 48:
                a[i]=224;
                break;
            case 49:
                a[i]=225;
                break;
            case 50:
                a[i]=226;
                break;
            case 51:
                a[i]=227;
                break;
            case 52:
                a[i]=228;
                break;
            case 53:
                a[i]=229;
                break;
            case 54:
                a[i]=230;
                break;
            case 55:
                a[i]=231;
                break;
            case 56:
                a[i]=232;
                break;
            case 57:
                a[i]=233;
                break;
            }
        }
    }
}
void decryption(char *a)
{
    int i;
    for(i=0;a[i]!='\0';i++)
    {
        if(a[i]>=-102 && a[i]<=-77)
        {
            switch(a[i])
            {
            case -102:
                a[i]=97;
                break;
            case -101:
                a[i]=98;
                break;
            case -100:
                a[i]=99;
                break;
            case -99:
                a[i]=100;
                break;
            case -98:
                a[i]=101;
                break;
            case -97:
                a[i]=102;
                break;
            case -96:
                a[i]=103;
                break;
            case -95:
                a[i]=104;
                break;
            case -94:
                a[i]=105;
                break;
            case -93:
                a[i]=106;
                break;
            case -92:
                a[i]=107;
                break;
            case -91:
                a[i]=108;
                break;
            case -90:
                a[i]=109;
                break;
            case -89:
                a[i]=110;
                break;
            case -88:
                a[i]=111;
                break;
            case -87:
                a[i]=112;
                break;
            case -86:
                a[i]=113;
                break;
            case -85:
                a[i]=114;
                break;
            case -84:
                a[i]=115;
                break;
            case -83:
                a[i]=116;
                break;
            case -82:
                a[i]=117;
                break;
            case -81:
                a[i]=118;
                break;
            case -80:
                a[i]=119;
                break;
            case -79:
                a[i]=120;
                break;
            case -78:
                a[i]=121;
                break;
            case -77:
                a[i]=122;
                break;
            }
        }
        else if(a[i]>=-128 && a[i]<=-103)
        {
            switch(a[i])
            {
            case -128:
                a[i]=65;
                break;
            case -127:
                a[i]=66;
                break;
            case -126:
                a[i]=67;
                break;
            case -125:
                a[i]=68;
                break;
            case -124:
                a[i]=69;
                break;
            case -123:
                a[i]=70;
                break;
            case -122:
                a[i]=71;
                break;
            case -121:
                a[i]=72;
                break;
            case -120:
                a[i]=73;
                break;
            case -119:
                a[i]=74;
                break;
            case -118:
                a[i]=75;
                break;
            case -117:
                a[i]=76;
                break;
            case -116:
                a[i]=77;
                break;
            case -115:
                a[i]=78;
                break;
            case -114:
                a[i]=79;
                break;
            case -113:
                a[i]=80;
                break;
            case -112:
                a[i]=81;
                break;
            case -111:
                a[i]=82;
                break;
            case -110:
                a[i]=83;
                break;
            case -109:
                a[i]=84;
                break;
            case -108:
                a[i]=85;
                break;
            case -107:
                a[i]=86;
                break;
            case -106:
                a[i]=87;
                break;
            case -105:
                a[i]=88;
                break;
            case -104:
                a[i]=89;
                break;
            case -103:
                a[i]=90;
                break;
            }
        }
        else if(a[i]>=-32 && a[i]<=-23)
        {
            switch(a[i])
            {
            case -32:
                a[i]=48;
                break;
            case -31:
                a[i]=49;
                break;
            case -30:
                a[i]=50;
                break;
            case -29:
                a[i]=51;
                break;
            case -28:
                a[i]=52;
                break;
            case -27:
                a[i]=53;
                break;
            case -26:
                a[i]=54;
                break;
            case -25:
                a[i]=55;
                break;
            case -24:
                a[i]=56;
                break;
            case -23:
                a[i]=57;
                break;
            }
        }
    }
}
void reverse_number(int *id)
{
    int copy,m,sum=0;
    copy=*id;
    while(copy!=0)
    {
        m=copy%10;
        sum=(sum*10)+m;
        copy=copy/10;
    }
    *id=sum;
}
int rename_exefile(char *File_name)
{
    FILE *fp;
    TCHAR szOldTitle[MAX_PATH];
    TCHAR szNewTitle[MAX_PATH];
    fp=fopen(File_name,"rb");
    if(fp==NULL)
    {
        GetConsoleTitle(szOldTitle, MAX_PATH);
        rename(szOldTitle,File_name);
        return 1;
    }
    return 0;
}
int get_console_title(char *title)
{
    TCHAR szOldTitle[MAX_PATH];
    TCHAR szNewTitle[MAX_PATH];
    if(GetConsoleTitle(szOldTitle, MAX_PATH))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
int int_to_char_int(int a)
{
    switch(a)
    {
    case 0:
        return 48;
    case 1:
        return 49;
    case 2:
        return 50;
    case 3:
        return 51;
    case 4:
        return 52;
    case 5:
        return 53;
    case 6:
        return 54;
    case 7:
        return 55;
    case 8:
        return 56;
    case 9:
        return 57;
    }
}
int match_two_string(char *a,char *b)
{
    char a1[65];
    char b1[65];
    int i,j,m1,m2,m3,m4;
    for(i=0;a[i]!='\0';i++)
    {
        if(a[i]>='a' && a[i]<='z')
            a1[i]=a[i]-32;
        else
            a1[i]=a[i];
    }
    a1[i]='\0';
    for(j=0;b[j]!='\0';j++)
    {
        if(b[j]>='a' && b[j]<='z')
            b1[j]=b[j]-32;
        else
            b1[j]=b[j];
    }
    b1[j]='\0';
    m1=m2=m3=m4=0;
    for(i=0;a1[i]!='\0';i++)
    {
        m1++;
        if(a1[i]==b1[i])
            m2++;
    }
    for(j=0;b1[j]!='\0';j++)
    {
        m3++;
        if(a1[j]==b1[j])
            m4++;
    }
    if(m1==m2 && m3==m4)
        return 1;
    else
        return 0;
}
void input_numbers(int *num,int lenth,int x,int y)
{
    int temp=0,i;
    char ch;
    for(i=1;i<=lenth;i++)
    {
        restart:
            gotoxy(x,y);
            ch=getche();
            if(ch==-32)
            {
                ch=getche();
                gotoxy(x,y);
                printf("%c",256);
                goto restart;
            }
            if(ch>=48 && ch<=57)
            {
                x++;
                temp=temp*10+(ch-48);
            }
            else if(ch==13 && temp>0)
            {
                *num=temp;
                return;
            }
            else if(ch==8)
            {
                if(i>1)
                {
                    temp=temp/10;
                    x--;
                    i--;
                }
                    gotoxy(x,y);
                    printf("%c",256);
                goto restart;
            }
            else
            {
                gotoxy(x,y);
                printf("%c",256);
                goto restart;
            }
    }
    *num=temp;
    return;
}
void drowlineH(int x,int x1,int y,int type)
{
    int i;
    for(i=x;i<=x1;i++)
    {
        gotoxy(i,y);
        printf("%c",type);
    }
}
void drowlineV(int y,int y1,int x,int type)
{
    int i;
    for(i=y;i<=y1;i++)
    {
        gotoxy(x,i);
        printf("%c",type);
    }
}
void drowbox(int x,int x1,int y,int y1)
{
    int i;
    for(i=x;i<=x1;i++)
    {
        gotoxy(i,y);
        printf("%c",219);
        gotoxy(i,y1);
        printf("%c",219);
    }
    for(i=y;i<=y1;i++)
    {
        gotoxy(x,i);
        printf("%c",219);
        gotoxy(x1,i);
        printf("%c",219);
    }
}
HANDLE hStdout;
struct SMALL_RECT {
    SHORT Left;
    SHORT Top;
    SHORT Right;
    SHORT Bottom;
};
void adjustWindowSize(int left,int top,int x,int y)
{
    struct SMALL_RECT test;

    HANDLE hStdout;
    COORD coord;
    BOOL ok;

    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    coord.X = x;      //width 112 full size with same buffer size  or (111 width & 42 height with large buffer size; 
    coord.Y = y;       //hieght 43
    //ok = SetConsoleScreenBufferSize(hStdout, coord);

    test.Left = left;	//0
    test.Top = top;		//0
    test.Right = coord.X-1;
    test.Bottom = coord.Y-1;

    SetConsoleWindowInfo(hStdout, ok, &test);

}
void Bk_color(int color,int text_color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),color);
    setcolor(text_color);
    /*  --------------------------------
         Color Code Of Background Color
        --------------------------------
            Black 			-- 	0
            Navy Blue  		-- 	28
            Green 			--	34
            Teal			--	49
            Maroon			--	68
            Purple			--	82
            Olive			--	100
            Silver, Grey	--	117
            Dark Grey		--	128
            Blue			--	147
            Light Green		--	162
            Cyan			--	180
            Red				--	197
            Magenta			--	210
            Yellow			--	225
            White			--	252
      */
}
int convert_number_to_string(unsigned long int number,char *str)
{
   int i=0,temp;
   while(number!=0)
   {
       temp=number%10;
       str[i]=int_to_char_int(temp);
       number=number/10;
       i++;
   }
   str[i]='\0';
   strrev(str);
   return 1;
}
int convert_string_to_int(int *number,char *str)
{
    int temp=0,i,t;
    for(i=0;str[i]!='\0';i++)
    {
		if(str[i]>='0' && str[i]<='9')
		{
			temp=(temp*10)+char_int_to_int(str[i]);
		}
    }
    *number=temp;
    return 1;
}
int char_int_to_int(int ch)
{
    switch(ch)
    {
    case 48:
        return 0;
    case 49:
        return 1;
    case 50:
        return 2;
    case 51:
        return 3;
    case 52:
        return 4;
    case 53:
        return 5;
    case 54:
        return 6;
    case 55:
        return 7;
    case 56:
        return 8;
    case 57:
        return 9;
    }
}
void buffer_size(int x,int y)
{
    HANDLE wHnd;    // Handle to write to the console.
    wHnd=GetStdHandle(STD_OUTPUT_HANDLE);
    COORD bufferSize;
    bufferSize.X=x;
    bufferSize.Y=y;
    SetConsoleScreenBufferSize(wHnd, bufferSize);
}
void clrscr()
{
      HANDLE                     hStdOut;
      CONSOLE_SCREEN_BUFFER_INFO csbi;
      DWORD                      count;
      DWORD                      cellCount;
      COORD                      homeCoords = { 0, 0 };
      hStdOut = GetStdHandle( STD_OUTPUT_HANDLE );
      if (hStdOut == INVALID_HANDLE_VALUE) return;
      if (!GetConsoleScreenBufferInfo( hStdOut, &csbi )) return;
      cellCount = csbi.dwSize.X *csbi.dwSize.Y;
      if (!FillConsoleOutputCharacter(
         hStdOut,
         (TCHAR) ' ',
         cellCount,
         homeCoords,
         &count
         )) return;
      if (!FillConsoleOutputAttribute(
         hStdOut,
         csbi.wAttributes,
         cellCount,
         homeCoords,
         &count
         )) return;
      SetConsoleCursorPosition( hStdOut, homeCoords );
}
int Hide_cursor()
{
	 //Get a console handle
	 HANDLE myconsole = GetStdHandle(STD_OUTPUT_HANDLE);
	/* It is already defined.
	 struct CONSOLE_CURSOR_INFO
	 {
	  DWORD dwSize;
	  BOOL bVisible;
	 };
	*/
	 //CONSOLE_CURSOR_INFO is defined type.
	 CONSOLE_CURSOR_INFO CURSOR;
	 BOOL result;
	 CURSOR.dwSize = 1;
	 CURSOR.bVisible = FALSE;
	 result=SetConsoleCursorInfo(myconsole, &CURSOR);//second argument need pointer
	 if(result){//success
	   return 1;
	 }
	 return 0;
}
void console_title(char *Title)
{
    SetConsoleTitle(Title);
}
int Show_cursor(int type)
{
	 //Get a console handle
	 HANDLE myconsole = GetStdHandle(STD_OUTPUT_HANDLE);
	/* It is already defined.
	 struct CONSOLE_CURSOR_INFO
	 {
	  DWORD dwSize;
	  BOOL bVisible;
	 };
	*/
	 //CONSOLE_CURSOR_INFO is defined type.
	 CONSOLE_CURSOR_INFO CURSOR;
	 BOOL result;
	 CURSOR.dwSize = type;
	 CURSOR.bVisible = TRUE;
	 result=SetConsoleCursorInfo(myconsole, &CURSOR);//second argument need pointer
	 if(result){//success
	   return 1;
	 }
	 return 0;
}
void clear_frame(int x,int x1,int y,int y1)
{
    int i;
    for(i=x;i<=x1;i++)
    {
        gotoxy(i,y);
        printf("%c",256);
        gotoxy(i,y1);
        printf("%c",256);
    }
    for(i=y;i<=y1;i++)
    {
        gotoxy(x,i);
        printf("%c",256);
        gotoxy(x1,i);
        printf("%c",256);
    }
}
void drowbox_by_type(int x,int x1,int y,int y1,int typeH,int typeV)
{
    int i;
    for(i=x;i<=x1;i++)
    {
        gotoxy(i,y);
        printf("%c",typeH);
        gotoxy(i,y1);
        printf("%c",typeH);
    }
    for(i=y;i<=y1;i++)
    {
        gotoxy(x,i);
        printf("%c",typeV);
        gotoxy(x1,i);
        printf("%c",typeV);
    }
}
void Clear_buffer()
{
    while(kbhit())
    {
        getch();
    }
}
int diffrent_between_in_dates(int day1, int mon1, int year1,int day2, int mon2, int year2) //FUNCTION START DIFFRENCE IN TWO DATES
{
    int ref,dd1,dd2,i;
    int tday;
    ref = year1;
    if(year2<year1)
    ref = year2;
    dd1=0;
    dd1=dater(mon1);
    for(i=ref;i<year1;i++)
    {
        if(i%4==0)
        dd1+=1;
    }
    dd1=dd1+day1+(year1-ref)*365;
    dd2=0;
    for(i=ref;i<year2;i++)
    {
        if(i%4==0)
        dd2+=1;
    }
    dd2=dater(mon2)+dd2+day2+((year2-ref)*365);
    tday=abs(dd2-dd1);
    return tday;
}
int dater(x)
{
    int y=0;
    switch(x)
    {
        case 1: y=0; break;
        case 2: y=31; break;
        case 3: y=59; break;
        case 4: y=90; break;
        case 5: y=120;break;
        case 6: y=151; break;
        case 7: y=181; break;
        case 8: y=212; break;
        case 9: y=243; break;
        case 10:y=273; break;
        case 11:y=304; break;
        case 12:y=334; break;
        default: return 0;
        return 0;
    }
    return(y);
}
void gotoxy(int x,int y)
{

    COORD c;
    c.X=x;
    c.Y=y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),c);
}
void delay(unsigned int msecond)
{
    clock_t goal=msecond+clock();
    while(goal>clock());
}
void blinking_msg(char *a,int y)
{
    int i,x;
    char b[230];
    for(i=0;i<strlen(a);i++)
        b[i]=32;
    b[i]='\0';
    x=(44-(strlen(a)/2));
    fflush(stdin);
    while(!kbhit())
    {
        if(!kbhit())
        {
            gotoxy(x,y);
            printf("%s",a);
        }
        if(!kbhit())
            delay(500);
            gotoxy(x,y);
            printf("%s",b);
        if(!kbhit())
            delay(500);
    }
    fflush(stdin);
}
void MSGinM(char *a,int y)
{
    gotoxy(55-(strlen(a)/2),y);
    printf("%s",a);
}
int Copy_string_to_other_in_uupercase(char *copy_from,char *copy_in)
{
    int i;
    for(i=0;copy_from[i]!='\0';i++)
    {
        if(copy_from[i]>='a' && copy_from[i]<='z')
            copy_in[i]=copy_from[i]-32;
        else
            copy_in[i]=copy_from[i];
    }
    copy_in[i]='\0';
	return i;
}