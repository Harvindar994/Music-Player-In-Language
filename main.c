#include "custom.h"
#include<pthread.h>
#include<stdio.h>
#include<windows.h>
#include<windows.h>
#include<dos.h>
#include<dirent.h>
#include<mmsystem.h>
#include<mciavi.h>
#include<pthread.h>
#pragma comment(lib, "Winmm.lib")
#define up 72
#define down 80
#define left 75
#define right 77
#define Cancel 2
#define Ok 1
#define TryAgain 10
#define Continue 11
#define Ignore 5
#define Retry 4
#define Abort 3
#define Yes 6
#define No 7
#define Folder_p 1
#define File_p 0
#define or ||
#define and &&
#define Play_Music 11
#define Close_Music 22
#define Resume_Music 33
#define Pause_Music 44
#define None 0
#define Play_Next 55
#define Play_Previous 66
#define Stop_Music 77
#define Restart_Music_Player 101
#define Close_Music_Player 102
#define Player_Loaded 103
#define Player_Loding 104
#define Stoped 105
#define Played 106
#define Paused 107
#define Paused_By_Exp 108


struct open_paths
{
    char f_path[500];
    int type;
    struct open_paths *back;
    struct open_paths *next;
};
struct path
{
    int f_type;
    char f_path[450];
    char f_name[65];
    struct path *back;
    struct path *next;
};
struct Music_Lib
{
    char f_path[450];
    char f_name[65];
    int day,month,year;
    struct Music_Lib *back;
    struct Music_Lib *next;
};
char Drives[50];

int Key_Stoke=None;
Total_Song_In_Play_List=None;
int Play_Status=None;
int Reloding_Player=None;

int System_Drive_Status;
int Remove_Music_from_Library=0;

struct Music_Lib *Play_list=NULL;
struct Music_Lib *Current_Playing_Song=NULL;
struct Music_Lib *Last_Song_of_PL=NULL;

struct open_paths *open_dir=NULL;
struct path *current_path=NULL; //This Variable are use for contain a list of file or Dir. Wich are open_dir path.

char Music_Library[]="Brightgoal_music_library.lib";
char Temporary_File[]="Temporary_File.lib";



struct path* new_node()
{
    struct path *temp;
    temp=(struct path*)malloc(sizeof(struct path));
};
int load_all_file_of_path(struct path **node,char *dir_path)
{
    int count=0,flag=0,re;
    struct path *temp,*head=NULL;
    DIR *dir;
    struct dirent *file;
    dir=opendir(dir_path);
    if(dir==NULL)
    {
        return -1;
    }
    flag=strlen(dir_path);
    if(dir_path[flag-1]==92 && dir_path[flag]=='\0')
    {
        flag=1;
    }
    else
    {
        flag=0;
    }
    while((file=readdir(dir))!=NULL)
    {
        if(strcmp(file->d_name,"..")==0 || strcmp(file->d_name,".")==0)
            continue;
        if(strcmp(file->d_name,"$RECYCLE.BIN")==0 || strcmp(file->d_name,"$Recycle.Bin")==0)
            continue;
        if(strcmp(file->d_name,"System Volume Information")==0)
            continue;
        if(strcmp(file->d_name,"PAV.QUAR")==0)
            continue;
        temp=new_node();
        temp->back=NULL;
        temp->next=NULL;
        temp->f_name[0]='\0';
        temp->f_path[0]='\0';

        copy_spcf_part_of_string_to_another(file->d_name,temp->f_name,51);
        //strcpy(temp->f_name,file->d_name);
        strcat(temp->f_path,dir_path);
        if(flag==0)
            strcat(temp->f_path,"\\");
        strcat(temp->f_path,file->d_name);
        temp->f_type=check_dir_or_not(temp->f_path);
        if(temp->f_type==File_p)
        {

            re=check_mp3_file_or_not(file->d_name);
            if(re==FALSE && re!=TRUE)
            {
                continue;
            }
        }

        if(head==NULL)
        {
            head=temp;
        }
        else
        {
            head->back=temp;
            temp->next=head;
            head=temp;
        }
        count++;
    }
    closedir(dir);
    *node=head;
    return count;
}
void copy_spcf_part_of_string_to_another(char *copy_from,char *copy_in,int lenth)
{
    int i;
    for(i=0;copy_from[i]!='\0' && i<lenth;i++)
    {
        copy_in[i]=copy_from[i];
    }
    if(copy_from[i]=='\0' && i<=lenth)
    {
        copy_in[i]='\0';
    }
    else
    {
        copy_in[i+1]='\0';
        copy_in[i]='.';
        copy_in[i-1]='.';
        copy_in[i-2]='.';
    }
}
struct open_paths* new_node_of_open_paths()
{
    struct open_paths *temp;
    temp=(struct open_paths*)malloc(sizeof(struct open_paths));
    temp->back=NULL;
    temp->next=NULL;
    temp->f_path[0]='\0';
    return temp;
};
int add_object_in_open_paths(struct open_paths **node,char *path,int type)
{
    struct open_paths *temp=NULL,*head=NULL,*t=NULL;
    temp=new_node_of_open_paths();
    head=*node;
    strcpy(temp->f_path,path);
    temp->type=type;
    if(head==NULL)
    {
        head=temp;
    }
    else
    {
        t=head;
        while(t->next!=NULL)
        {
            t=t->next;
        }
        temp->back=t;
        t->next=temp;
    }
    *node=head;
    return 1;
}
int delete_open_dir_list(struct open_paths **dir_list)
{
    struct open_paths *temp,*t;
    temp=*dir_list;
    t=*dir_list;
    if(temp==NULL)
    {
        return 0;
    }
    while(t!=NULL)
    {
        temp=temp->next;
        free(t);
        t=temp;
    }
    *dir_list=t;
    return 1;
}
int delete_list_of_path(struct path **dir_list)
{
    struct path *temp,*t;
    t=temp=*dir_list;
    if(temp==NULL)
    {
        return 0;
    }
    while(t!=NULL)
    {
        temp=temp->next;
        free(t);
        t=temp;
    }
    *dir_list=t;
    return 1;
}
int check_mp3_file_or_not(char *f_path)
{
    int lenth;
    lenth=strlen(f_path);
    if(f_path[lenth]=='\0' && f_path[lenth-1]=='3' && (f_path[lenth-2]=='p' || f_path[lenth-2]=='P') && (f_path[lenth-3]=='m' || f_path[lenth-3]=='M') && f_path[lenth-4]=='.')
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
int check_dir_or_not(char *path)
{
    FILE *fp;
    DIR *dir;
    fp=fopen(path,"rb");
    if(fp==NULL)
    {
        dir=opendir(path);
        if(dir!=NULL)
        {
            closedir(dir);
            return Folder_p;
        }
        else
        {
            return File_p;
        }
    }
    else
    {
        fclose(fp);
        return File_p;
    }
}
int main_menu()
{
    char ch;
    char a_type;
    int re,i;
    int x,y,x1,y1,point,pointx;
    restart_complete_mainmenu:
    system("cls");
    system("COLOR bC");
    console_title("Harvindar Singh - 9140417112, www.brightgoal.in");
    Hide_cursor();
    buffer_size(116,45);
    adjustWindowSize(0,0,111,42);
    Bk_color(28,1);
    back_freame(0,111,0,2);
    Bk_color(28,15);
    MSGinM("BRIGHTGOAL MP3 PLAYER POWERED BY HARVINDAR SINGH",1);
    Bk_color(180,17);
    drowlineH(0,110,42,220);
    Bk_color(117,17);
    drowlineV(3,42,0,219);
    drowlineV(3,42,111,219);
    ind_back_freame(1,110,40,42,28,180,1);
    buffer_size(112,43);
    adjustWindowSize(0,0,112,43);
    restart_menu:
    Bk_color(180,7);
    //window(2,38,6,41);
    //window(40,109,6,41);
    main_menu_background();
    working_area_background();
    ind_back_freame(2,38,3,5,117,180,12);
    ind_back_freame(40,109,3,5,117,180,12);
    Bk_color(28,15);
    gotoxy(2,41);
    printf("%c STATUS BAR",14);
    Bk_color(197,15);
    gotoxy(17,4);
    printf("MAIN MENU");
    x=3;
    x1=38;
    y=6;
    pointx=6;
    Update_Current_playing_Music();
    Notification();
    reset:
        y1=y+2;
        point=y+1;
        Bk_color(117,1);
        if(x==3)
        {
            gotoxy(12,7);
            printf("Music Library");
            gotoxy(12,9);
            printf("Add Music in Library");
            gotoxy(12,11);
            printf("Rules & Instructions");
            gotoxy(12,13);
            printf("About");
            gotoxy(12,15);
            printf("Exit");
            setcolor(14);
            menu_option(x,x1,y,y1);
            Bk_color(225,9);
            if(y+1==7 && x==3)
            {
                gotoxy(12,7);
                printf("Music Library");
            }
            if(y+1==9 && x==3)
            {
                gotoxy(12,9);
                printf("Add Music in Library");
            }
            if(y+1==11 && x==3)
            {
                gotoxy(12,11);
                printf("Rules & Instructions");
            }
            if(y+1==13 && x==3)
            {
                gotoxy(12,13);
                printf("About");
            }
            if(y+1==15 && x==3)
            {
                gotoxy(12,15);
                printf("Exit");
            }
        }
        gotoxy(pointx,point);
        printf("%c",249);
        Bk_color(117,14);
        rechoise:
        Bk_color(117,14);
        gotoxy(56,4);
        ch=getch();
        if(ch==-32)
        {
            ch=getch();
            if(ch==up || ch==down || ch==left || ch==right)
            {
                switch(ch)
                {
                case up:
                    if(y>6 && y<=14)
                    {
                        gotoxy(pointx,point);
                        printf("%c",256);
                        setcolor(7);
                        menu_option(x,x1,y,y1);
                        y=y-2;
                        goto reset;
                    }
                    goto rechoise;
                case down:
                    if(y>=6 && y<14)
                    {
                        gotoxy(pointx,point);
                        printf("%c",256);
                        setcolor(7);
                        menu_option(x,x1,y,y1);
                        y=y+2;
                        goto reset;
                    }
                    goto rechoise;
                case left:
                    Reloding_Player=Player_Loding;
                    Key_Stoke=Play_Previous;
                    while(Reloding_Player!=Player_Loaded);
                    Update_Current_playing_Music();
                    goto rechoise;
                case right:
                    Reloding_Player=Player_Loding;
                    Key_Stoke=Play_Next;
                    while(Reloding_Player!=Player_Loaded);
                    Update_Current_playing_Music();
                    goto rechoise;
                }
            }
            else
            {
                goto reset;
            }
        }
        else if(ch==13)
        {
            switch(y+1)
            {
            case 7:
                Explore_Music_Library();
                Notification();
                goto reset;
                break;
            case 9:
                detect_all_drives(Drives,&System_Drive_Status);
                File_Explorer(Drives);
                delete_play_list();
                Key_Stoke=Restart_Music_Player;
                Notification();
                goto reset;
                break;
            case 11:
                Intruction();
                Notification();
                goto reset;
                goto reset;
                break;
            case 13:
                about();
                Notification();
                goto reset;
                break;
            case 15:
                Key_Stoke=Close_Music;
                Exit_Now();
                break;
            }
        }
        else if(ch=='p' or ch=='P')
        {
            Reloding_Player=Player_Loding;
            if(Play_Status==Stoped)
            {
                Key_Stoke=Play_Music;
            }
            else if(Play_Status==Played)
            {
                Key_Stoke=Pause_Music;
            }
            else if(Play_Status==Paused)
            {
                Key_Stoke=Resume_Music;
            }
            while(Reloding_Player!=Player_Loaded);
            Update_Current_playing_Music();
            goto rechoise;
        }
        else if(ch=='s' or ch=='S')
        {
            Reloding_Player=Player_Loding;
            Key_Stoke=Stop_Music;
            while(Reloding_Player!=Player_Loaded);
            Update_Current_playing_Music();
            goto rechoise;
        }
        else
        {
            goto reset;
        }
}
int Update_Current_playing_Music()
{
    if(Play_list==NULL)
        return 0;
    Update_status_bar(1,41);
    Bk_color(28,15);
    gotoxy(1,41);
    if(Play_Status==Stoped)
        printf("Song : %s",Current_Playing_Song->f_name);
    else if(Play_Status==Played)
        printf("Playing : %s",Current_Playing_Song->f_name);
    else if(Play_Status==Paused)
        printf("Pause : %s",Current_Playing_Song->f_name);
    return 1;
}
void notification_strip(int x,int y)
{
    gotoxy(x,y);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196);
}
int total_music_in_library()
{
    int count = 0;
    FILE *fp;
    fp = fopen(Music_Library, "rb");
    struct Music_Lib data;
    if(fp == NULL)
    {
        return 0;
    }
    else
    {
        while(fread(&data, sizeof(struct Music_Lib), 1, fp)>0)
        {
            count++;
        }
    }
    return count;
}
void Notification()
{
    ind_back_freame(40,109,3,5,117,180,12);
    int Total_music = total_music_in_library();
    Bk_color(197,15);
    gotoxy(70,4);
    printf("NOTIFICATIONS");
    Bk_color(180,7);
    working_area_background();
    Bk_color(117,1);
    gotoxy(42,7);
    printf("Library Information");
    notification_strip(42,8);
    gotoxy(44,10);
    printf("%c Total Music In Libary                      : %d",249, Total_music);
}
void* Music_Player_Back_End(void *n)
{
    char current_song[450];
    current_song[0]='\0';
    int re=0;
    Play_Status=Stoped;
    Restart_Player:
        Last_Song_of_PL=NULL;
        Total_Song_In_Play_List=None;
        Current_Playing_Song=NULL;
        load_all_music_file_in_play_list(&Play_list);
        if(Play_list==NULL)
        {
            Total_Song_In_Play_List=None;
            Key_Stoke=None;
        }
        else
        {
            Total_Song_In_Play_List=Get_Lent_of_Play_List(Play_list);
            Current_Playing_Song=Play_list;
            if(strlen(current_song)>0)
            {
                re=Set_Current_Playing_Song(&Play_list,&Current_Playing_Song,current_song);
                if(re==0)
                {
                    Current_Playing_Song=Play_list;
                }
            }
            Last_Music_file_of_play_list(&Play_list,&Last_Song_of_PL);

        }
        Reset_Key_Stoke:
            Key_Stoke=0;
            Reloding_Player=Player_Loaded;
            while(Key_Stoke==0)
            {

            }

            switch(Key_Stoke)
            {
            case Play_Music:
                if(Total_Song_In_Play_List==0 || Current_Playing_Song==NULL)
                {
                    MessageBoxA(NULL," No Music File In Library"," Brightgoal Mp3 Player",48);
                    Reloding_Player=Player_Loaded;
                    goto Reset_Key_Stoke;
                }
                close_music_file();
                re=play_file(Current_Playing_Song->f_path,1);
                if(re!=1)
                {
                    MessageBoxA(NULL," Brightgoal Mp3 Player encountered a problem while playing the file."," Brightgoal Mp3 Player",16);
                    Play_Status=Stoped;
                    Reloding_Player=Player_Loaded;
                    goto Reset_Key_Stoke;
                }
                else
                {
                    Play_Status=Played;
                    Reloding_Player=Player_Loaded;
                    strcpy(current_song,Current_Playing_Song->f_path);
                }
                goto Reset_Key_Stoke;

            case Play_Next:
                if(Total_Song_In_Play_List==0 || Current_Playing_Song==NULL)
                {
                    MessageBoxA(NULL," No Music File In Library"," Brightgoal Mp3 Player",48);
                    Reloding_Player=Player_Loaded;
                    goto Reset_Key_Stoke;
                }

                close_music_file();
                if(Current_Playing_Song->next==NULL)
                    Current_Playing_Song=Play_list;
                else
                    Current_Playing_Song=Current_Playing_Song->next;

                re=play_file(Current_Playing_Song->f_path,1);
                if(re!=1)
                {
                    MessageBoxA(NULL," Brightgoal Mp3 Player encountered a problem while playing the file."," Brightgoal Mp3 Player",16);
                    Play_Status=Stoped;
                    Reloding_Player=Player_Loaded;
                    goto Reset_Key_Stoke;
                }
                else
                {
                    Play_Status=Played;
                    Reloding_Player=Player_Loaded;
                    strcpy(current_song,Current_Playing_Song->f_path);
                }
                goto Reset_Key_Stoke;

            case Play_Previous:
                if(Total_Song_In_Play_List==0 || Current_Playing_Song==NULL)
                {
                    MessageBoxA(NULL," No Music File In Library"," Brightgoal Mp3 Player",48);
                    Reloding_Player=Player_Loaded;
                    goto Reset_Key_Stoke;
                }
                close_music_file();
                if(Current_Playing_Song->back==NULL)
                {
                    if(Last_Song_of_PL!=NULL)
                    {
                        Current_Playing_Song=Last_Song_of_PL;
                    }
                    else
                    {
                        Current_Playing_Song=Play_list;
                    }
                }
                else
                    Current_Playing_Song=Current_Playing_Song->back;

                re=play_file(Current_Playing_Song->f_path,1);
                if(re!=1)
                {
                    MessageBoxA(NULL," Brightgoal Mp3 Player encountered a problem while playing the file."," Brightgoal Mp3 Player",16);
                    Play_Status=Stoped;
                    Reloding_Player=Player_Loaded;
                    goto Reset_Key_Stoke;
                }
                else
                {
                    Play_Status=Played;
                    Reloding_Player=Player_Loaded;
                    strcpy(current_song,Current_Playing_Song->f_path);
                }
                goto Reset_Key_Stoke;

            case Pause_Music:
                Pause_Playing();
                Play_Status=Paused;
                Reloding_Player=Player_Loaded;
                goto Reset_Key_Stoke;

            case Resume_Music:
                Resume_Playing();
                Play_Status=Played;
                Reloding_Player=Player_Loaded;
                goto Reset_Key_Stoke;

            case Stop_Music:
                stop_playing();
                Play_Status=Stoped;
                Reloding_Player=Player_Loaded;
                goto Reset_Key_Stoke;

            case Close_Music:
                close_music_file();
                Play_Status=Stoped;
                Reloding_Player=Player_Loaded;
                goto Reset_Key_Stoke;

            case Restart_Music_Player:
                delete_play_list(&Play_list);
                Current_Playing_Song=NULL;
                Play_list=NULL;
                Total_Song_In_Play_List=0;
                Reloding_Player=Player_Loaded;
                goto Restart_Player;

            case Close_Music_Player:
                delete_play_list(&Play_list);
                Current_Playing_Song=NULL;
                close_music_file();
                Play_list=NULL;
                Total_Song_In_Play_List=0;
                return 1;
            }
}
int Last_Music_file_of_play_list(struct Music_Lib **Node,struct Music_Lib **Last_Song)
{
    struct Music_Lib *temp;
    temp=*Node;
    if(temp==NULL)
    {
        *Last_Song=NULL;
        return 0;
    }
    while(temp->next!=NULL)
    {
        temp=temp->next;
    }
    *Last_Song=temp;
}
int Set_Current_Playing_Song(struct Music_Lib **Node, struct Music_Lib **Current_song,char *song)
{
    struct Music_Lib *temp;
    temp=*Node;
    if(temp==NULL)
    {
        return 0;
    }
    while(temp!=NULL)
    {
        if(strcmp(song,temp->f_path)==0)
        {
            *Current_song=temp;
            return 1;
        }
        temp=temp->next;
    }
    *Current_song=NULL;
    return 0;
}
int Explore_Music_Library()
{

    int y,Lenth,Count,re;
    char ch;
    struct Music_Lib *point1,*point2,*point3,*point4,*point5,*point6,*point7,*point8,*point9,*point10,*point11,*point12,*point13,*point14,*point15,*point16;

    ind_back_freame(40,109,3,5,117,180,12);
    Bk_color(180,7);
    working_area_background();
    Bk_color(197,15);
    gotoxy(67,4);
    printf("NAME");
    gotoxy(101,4);
    printf("DATE");
    Bk_color(117,1);
    line_V(6,38,96,'n','n',0);

    Restart_Library:
    point1=point2=point3=point4=point5=point6=point7=point8=point9=point10=point11=point12=point13=point14=point15=point16=NULL;
    Lenth=Total_Song_In_Play_List;

    if(Lenth>=1)
        point1=Play_list;
    if(Lenth>=2)
        point2=point1->next;
    if(Lenth>=3)
        point3=point2->next;
    if(Lenth>=4)
        point4=point3->next;
    if(Lenth>=5)
        point5=point4->next;
    if(Lenth>=6)
        point6=point5->next;
    if(Lenth>=7)
        point7=point6->next;
    if(Lenth>=8)
        point8=point7->next;
    if(Lenth>=9)
        point9=point8->next;
    if(Lenth>=10)
        point10=point9->next;
    if(Lenth>=11)
        point11=point10->next;
    if(Lenth>=12)
        point12=point11->next;
    if(Lenth>=13)
        point13=point12->next;
    if(Lenth>=14)
        point14=point13->next;
    if(Lenth>=15)
        point15=point14->next;
    if(Lenth>=16)
        point16=point15->next;
    if(Lenth==0)
    {
        Bk_color(117,1);
        gotoxy(60,21);
        printf("NO FILE IN LIBRARY");
        getch();
        return 404;
    }

    y=7;
    Count=1;
    reset:
        if(Lenth==0)
        {
            Bk_color(117,1);
            gotoxy(60,21);
            printf("NO FILE IN LIBRARY");
            getch();
            return 404;
        }
        Bk_color(117,14);
        Explore_Directories_strip(41,y);
        if(Lenth>=1)
        {
            if(y==7)
                Bk_color(225,9);
            else
                Bk_color(117,1);
            if(point1==Current_Playing_Song)
            {
                setcolor(12);
            }
            gotoxy(42,7);
            printf("%s",point1->f_name);
            Bk_color(117,1);
            gotoxy(98,7);
            printf("%d/%d/%d",point1->day,point1->month,point1->year);
        }
        if(Lenth>=2)
        {
            if(y==9)
                Bk_color(225,9);
            else
                Bk_color(117,1);
            if(point2==Current_Playing_Song)
            {
                setcolor(12);
            }
            gotoxy(42,9);
            printf("%s",point2->f_name);
            Bk_color(117,1);
            gotoxy(98,9);
            printf("%d/%d/%d",point2->day,point2->month,point2->year);
        }

        if(Lenth>=3)
        {
            if(y==11)
                Bk_color(225,9);
            else
                Bk_color(117,1);
            if(point3==Current_Playing_Song)
            {
                setcolor(12);
            }
            gotoxy(42,11);
            printf("%s",point3->f_name);
            Bk_color(117,1);
            gotoxy(98,11);
            printf("%d/%d/%d",point3->day,point3->month,point3->year);
        }

        if(Lenth>=4)
        {
            if(y==13)
                Bk_color(225,9);
            else
                Bk_color(117,1);
            if(point4==Current_Playing_Song)
            {
                setcolor(12);
            }
            gotoxy(42,13);
            printf("%s",point4->f_name);
            Bk_color(117,1);
            gotoxy(98,13);
            printf("%d/%d/%d",point4->day,point4->month,point4->year);
        }

        if(Lenth>=5)
        {
            if(y==15)
                Bk_color(225,9);
            else
                Bk_color(117,1);
            if(point5==Current_Playing_Song)
            {
                setcolor(12);
            }
            gotoxy(42,15);
            printf("%s",point5->f_name);
            Bk_color(117,1);
            gotoxy(98,15);
            printf("%d/%d/%d",point5->day,point5->month,point5->year);
        }

        if(Lenth>=6)
        {
            if(y==17)
                Bk_color(225,9);
            else
                Bk_color(117,1);
            if(point6==Current_Playing_Song)
            {
                setcolor(12);
            }
            gotoxy(42,17);
            printf("%s",point6->f_name);
            Bk_color(117,1);
            gotoxy(98,17);
            printf("%d/%d/%d",point6->day,point6->month,point6->year);
        }

        if(Lenth>=7)
        {
            if(y==19)
                Bk_color(225,9);
            else
                Bk_color(117,1);
            if(point7==Current_Playing_Song)
            {
                setcolor(12);
            }
            gotoxy(42,19);
            printf("%s",point7->f_name);
            Bk_color(117,1);
            gotoxy(98,19);
            printf("%d/%d/%d",point7->day,point7->month,point7->year);
        }

        if(Lenth>=8)
        {
            if(y==21)
                Bk_color(225,9);
            else
                Bk_color(117,1);
            if(point8==Current_Playing_Song)
            {
                setcolor(12);
            }
            gotoxy(42,21);
            printf("%s",point8->f_name);
            Bk_color(117,1);
            gotoxy(98,21);
            printf("%d/%d/%d",point8->day,point8->month,point8->year);
        }

        if(Lenth>=9)
        {
            if(y==23)
                Bk_color(225,9);
            else
                Bk_color(117,1);
            if(point9==Current_Playing_Song)
            {
                setcolor(12);
            }
            gotoxy(42,23);
            printf("%s",point9->f_name);
            Bk_color(117,1);
            gotoxy(98,23);
            printf("%d/%d/%d",point9->day,point9->month,point9->year);
        }

        if(Lenth>=10)
        {
            if(y==25)
                Bk_color(225,9);
            else
                Bk_color(117,1);
            if(point10==Current_Playing_Song)
            {
                setcolor(12);
            }
            gotoxy(42,25);
            printf("%s",point10->f_name);
            Bk_color(117,1);
            gotoxy(98,25);
            printf("%d/%d/%d",point10->day,point10->month,point10->year);
        }

        if(Lenth>=11)
        {
            if(y==27)
                Bk_color(225,9);
            else
                Bk_color(117,1);
            if(point11==Current_Playing_Song)
            {
                setcolor(12);
            }
            gotoxy(42,27);
            printf("%s",point11->f_name);
            Bk_color(117,1);
            gotoxy(98,27);
            printf("%d/%d/%d",point11->day,point11->month,point11->year);
        }

        if(Lenth>=12)
        {
            if(y==29)
                Bk_color(225,9);
            else
                Bk_color(117,1);
            if(point12==Current_Playing_Song)
            {
                setcolor(12);
            }
            gotoxy(42,29);
            printf("%s",point12->f_name);
            Bk_color(117,1);
            gotoxy(98,29);
            printf("%d/%d/%d",point12->day,point12->month,point12->year);
        }

        if(Lenth>=13)
        {
            if(y==31)
                Bk_color(225,9);
            else
                Bk_color(117,1);
            if(point13==Current_Playing_Song)
            {
                setcolor(12);
            }
            gotoxy(42,31);
            printf("%s",point13->f_name);
            Bk_color(117,1);
            gotoxy(98,31);
            printf("%d/%d/%d",point13->day,point13->month,point13->year);
        }

        if(Lenth>=14)
        {
            if(y==33)
                Bk_color(225,9);
            else
                Bk_color(117,1);
            if(point14==Current_Playing_Song)
            {
                setcolor(12);
            }
            gotoxy(42,33);
            printf("%s",point14->f_name);
            Bk_color(117,1);
            gotoxy(98,33);
            printf("%d/%d/%d",point14->day,point14->month,point14->year);
        }

        if(Lenth>=15)
        {
            if(y==35)
                Bk_color(225,9);
            else
                Bk_color(117,1);
            if(point15==Current_Playing_Song)
            {
                setcolor(12);
            }
            gotoxy(42,35);
            printf("%s",point15->f_name);
            Bk_color(117,1);
            gotoxy(98,35);
            printf("%d/%d/%d",point15->day,point15->month,point15->year);
        }

        if(Lenth>=16)
        {
            if(y==37)
                Bk_color(225,9);
            else
                Bk_color(117,1);
            if(point16==Current_Playing_Song)
            {
                setcolor(12);
            }
            gotoxy(42,37);
            printf("%s",point16->f_name);
            Bk_color(117,1);
            gotoxy(98,37);
            printf("%d/%d/%d",point16->day,point16->month,point16->year);
        }

        re_choice:
            ch=getch();
            if(ch==-32)
            {
                ch=getch();
                if(ch==up || ch==down || ch==left || ch==right)
                {
                    switch(ch)
                    {
                    case up:
                        if(y>7 && Count>1)
                        {
                            Bk_color(117,7);
                            Explore_Directories_strip(41,y);
                            y=y-2;
                            Count--;
                            goto reset;
                        }
                        else
                        {
                            if(point1->back!=NULL)
                            {
                                point1=point1->back;
                                point2=point2->back;
                                point3=point3->back;
                                point4=point4->back;
                                point5=point5->back;
                                point6=point6->back;
                                point7=point7->back;
                                point8=point8->back;
                                point9=point9->back;
                                point10=point10->back;
                                point11=point11->back;
                                point12=point12->back;
                                point13=point13->back;
                                point14=point14->back;
                                point15=point15->back;
                                point16=point16->back;
                                Explore_Directories_updater();
                                goto reset;
                            }
                            goto re_choice;
                        }
                    case down:
                        if(y<37 && Count<Lenth)
                        {
                            Bk_color(117,7);
                            Explore_Directories_strip(41,y);
                            y=y+2;
                            Count++;
                            goto reset;
                        }
                        else
                        {
                            if(Lenth>16 && point16->next!=NULL)
                            {
                                point1=point1->next;
                                point2=point2->next;
                                point3=point3->next;
                                point4=point4->next;
                                point5=point5->next;
                                point6=point6->next;
                                point7=point7->next;
                                point8=point8->next;
                                point9=point9->next;
                                point10=point10->next;
                                point11=point11->next;
                                point12=point12->next;
                                point13=point13->next;
                                point14=point14->next;
                                point15=point15->next;
                                point16=point16->next;
                                Explore_Directories_updater();
                                goto reset;
                            }
                            goto re_choice;
                        }
                    case left:
                        Reloding_Player=Player_Loding;
                        Key_Stoke=Play_Previous;
                        while(Reloding_Player!=Player_Loaded);
                        Update_Current_playing_Music();
                        goto reset;
                    case right:
                        Reloding_Player=Player_Loding;
                        Key_Stoke=Play_Next;
                        while(Reloding_Player!=Player_Loaded);
                        Update_Current_playing_Music();
                        goto reset;
                    }
                }
                else if(ch==83)
                {
                    if(y==7)
                    {
                        if(point1==Current_Playing_Song)
                        {
                            Reloding_Player=Player_Loding;
                            if(Total_Song_In_Play_List==1)
                                Key_Stoke=Close_Music;
                            else
                                Key_Stoke=Play_Next;
                            while(Reloding_Player!=Player_Loaded)
                            {

                            }
                        }
                        re=remove_song_from_play_list(point1->f_name);
                    }
                    else if(y==9)
                    {
                        if(point2==Current_Playing_Song)
                        {
                            Reloding_Player=Player_Loding;
                            Key_Stoke=Play_Next;
                            while(Reloding_Player!=Player_Loaded)
                            {

                            }
                        }
                        re=remove_song_from_play_list(point2->f_name);
                    }
                    else if(y==11)
                    {
                        re=remove_song_from_play_list(point3->f_name);
                        if(point3==Current_Playing_Song)
                        {
                            Reloding_Player=Player_Loding;
                            Key_Stoke=Play_Next;
                            while(Reloding_Player!=Player_Loaded)
                            {

                            }
                        }
                    }
                    else if(y==13)
                    {
                        re=remove_song_from_play_list(point4->f_name);
                        if(point4==Current_Playing_Song)
                        {
                            Reloding_Player=Player_Loding;
                            Key_Stoke=Play_Next;
                            while(Reloding_Player!=Player_Loaded)
                            {

                            }
                        }
                    }
                    else if(y==15)
                    {
                        re=remove_song_from_play_list(point5->f_name);
                        if(point5==Current_Playing_Song)
                        {
                            Reloding_Player=Player_Loding;
                            Key_Stoke=Play_Next;
                            while(Reloding_Player!=Player_Loaded)
                            {

                            }
                        }
                    }
                    else if(y==17)
                    {
                        re=remove_song_from_play_list(point6->f_name);
                        if(point6==Current_Playing_Song)
                        {
                            Reloding_Player=Player_Loding;
                            Key_Stoke=Play_Next;
                            while(Reloding_Player!=Player_Loaded)
                            {

                            }
                        }
                    }
                    else if(y==19)
                    {
                        re=remove_song_from_play_list(point7->f_name);
                        if(point7==Current_Playing_Song)
                        {
                            Reloding_Player=Player_Loding;
                            Key_Stoke=Play_Next;
                            while(Reloding_Player!=Player_Loaded)
                            {

                            }
                        }
                    }
                    else if(y==21)
                    {
                        re=remove_song_from_play_list(point8->f_name);
                        if(point8==Current_Playing_Song)
                        {
                            Reloding_Player=Player_Loding;
                            Key_Stoke=Play_Next;
                            while(Reloding_Player!=Player_Loaded)
                            {

                            }
                        }
                    }
                    else if(y==23)
                    {
                        re=remove_song_from_play_list(point9->f_name);
                        if(point9==Current_Playing_Song)
                        {
                            Reloding_Player=Player_Loding;
                            Key_Stoke=Play_Next;
                            while(Reloding_Player!=Player_Loaded)
                            {

                            }
                        }
                    }
                    else if(y==25)
                    {
                        re=remove_song_from_play_list(point10->f_name);
                        if(point10==Current_Playing_Song)
                        {
                            Reloding_Player=Player_Loding;
                            Key_Stoke=Play_Next;
                            while(Reloding_Player!=Player_Loaded)
                            {

                            }
                        }
                    }
                    else if(y==27)
                    {
                        re=remove_song_from_play_list(point11->f_name);
                        if(point11==Current_Playing_Song)
                        {
                            Reloding_Player=Player_Loding;
                            Key_Stoke=Play_Next;
                            while(Reloding_Player!=Player_Loaded)
                            {

                            }
                        }
                    }
                    else if(y==29)
                    {
                        re=remove_song_from_play_list(point12->f_name);
                        if(point12==Current_Playing_Song)
                        {
                            Reloding_Player=Player_Loding;
                            Key_Stoke=Play_Next;
                            while(Reloding_Player!=Player_Loaded)
                            {

                            }
                        }
                    }
                    else if(y==31)
                    {
                        re=remove_song_from_play_list(point13->f_name);
                        if(point13==Current_Playing_Song)
                        {
                            Reloding_Player=Player_Loding;
                            Key_Stoke=Play_Next;
                            while(Reloding_Player!=Player_Loaded)
                            {

                            }
                        }
                    }
                    else if(y==33)
                    {
                        if(point14==Current_Playing_Song)
                        {
                            Reloding_Player=Player_Loding;
                            Key_Stoke=Play_Next;
                            while(Reloding_Player!=Player_Loaded)
                            {

                            }
                        }
                        re=remove_song_from_play_list(point14->f_name);
                    }
                    else if(y==35)
                    {
                        if(point15==Current_Playing_Song)
                        {
                            Reloding_Player=Player_Loding;
                            Key_Stoke=Play_Next;
                            while(Reloding_Player!=Player_Loaded)
                            {

                            }
                        }
                        re=remove_song_from_play_list(point15->f_name);
                    }
                    else if(y==37)
                    {
                        if(point16==Current_Playing_Song)
                        {
                            Reloding_Player=Player_Loding;
                            Key_Stoke=Play_Next;
                            while(Reloding_Player!=Player_Loaded)
                            {

                            }
                        }
                        re=remove_song_from_play_list(point16->f_name);
                    }

                    if(re!=1)
                    {
                        MessageBoxA(NULL," Brightgoal Mp3 Player encountered a problem to Remove Music File From Library"," Brightgoal Mp3 Player",16);
                        Clear_buffer();
                    }
                    else
                    {
                        Reloding_Player=Player_Loding;
                        Key_Stoke=Restart_Music_Player;
                        MessageBoxA(NULL,"Music File Successfully Removed From Music library"," Brightgoal Mp3 Player",64);
                        while(Reloding_Player!=Player_Loaded)
                        {

                        }
                        Explore_Directories_updater();
                        if(Total_Song_In_Play_List > 0)
                            Update_Current_playing_Music();
                        else
                            Update_status_bar(1, 41);
                        Clear_buffer();
                        goto Restart_Library;
                    }
                    goto reset;
                }
                else
                {
                    goto re_choice;
                }
            }
            else if(ch==13 && Lenth>0)
            {
                if(y==7)
                    Current_Playing_Song=point1;
                if(y==9)
                    Current_Playing_Song=point2;
                if(y==11)
                    Current_Playing_Song=point3;
                if(y==13)
                    Current_Playing_Song=point4;
                if(y==15)
                    Current_Playing_Song=point5;
                if(y==17)
                    Current_Playing_Song=point6;
                if(y==19)
                    Current_Playing_Song=point7;
                if(y==21)
                    Current_Playing_Song=point8;
                if(y==23)
                    Current_Playing_Song=point9;
                if(y==25)
                    Current_Playing_Song=point10;
                if(y==27)
                    Current_Playing_Song=point11;
                if(y==29)
                    Current_Playing_Song=point12;
                if(y==31)
                    Current_Playing_Song=point13;
                if(y==33)
                    Current_Playing_Song=point14;
                if(y==35)
                    Current_Playing_Song=point15;
                if(y==37)
                    Current_Playing_Song=point16;
                Reloding_Player=Player_Loding;
                Key_Stoke=Play_Music;
                while(Reloding_Player!=Player_Loaded);
                Update_Current_playing_Music();
                goto reset;
            }
            else if(ch=='s' || ch=='S' || ch=='p' || ch=='P')
            {
                Handle_Back_End_Player(ch);
                Update_Current_playing_Music();
                goto re_choice;
            }
            else if(ch==27)
            {
                return 1;
            }
            else
            {
                goto re_choice;
            }
}
void Handle_Back_End_Player(char *ch)
{
    if(ch=='p' || ch=='P')
    {
        Reloding_Player=Player_Loding;
        if(Play_Status==Stoped)
        {
            Key_Stoke=Play_Music;
        }
        else if(Play_Status==Played)
        {
            Key_Stoke=Pause_Music;
        }
        else if(Play_Status==Paused)
        {
            Key_Stoke=Resume_Music;
        }
        while(Reloding_Player!=Player_Loaded);
    }
    else if(ch=='s' || ch=='S')
    {
        Reloding_Player=Player_Loding;
        Key_Stoke=Stop_Music;
        while(Reloding_Player!=Player_Loaded);
    }
    else if(ch==right)
    {
        Reloding_Player=Player_Loding;
        Key_Stoke=Play_Next;
        while(Reloding_Player!=Player_Loaded);
    }
}
int remove_song_from_play_list(char *f_name)
{
    FILE *fp;
    int flag=0;
    FILE *temp;
    struct Music_Lib data;
    fp=fopen(Music_Library,"rb");
    temp=fopen(Temporary_File,"wb");
    if(fp==NULL || temp==NULL)
    {
        fclose(fp);
        fclose(temp);
        return 0;
    }
    while(fread(&data,sizeof(struct Music_Lib),1,fp)>0)
    {
        if(flag==0)
        {
            if(strcmp(f_name,data.f_name)!=0)
            {
                fwrite(&data,sizeof(struct Music_Lib),1,temp);
            }
            else
            {
                flag=1;
            }
        }
        else
        {
            fwrite(&data,sizeof(struct Music_Lib),1,temp);
        }
    }
    fclose(fp);
    fclose(temp);
    fp=fopen(Music_Library,"wb");
    temp=fopen(Temporary_File,"rb");
    if(fp==NULL || temp==NULL)
    {
        fclose(fp);
        fclose(temp);
        return 0;
    }
    while(fread(&data,sizeof(struct Music_Lib),1,temp)>0)
    {
        fwrite(&data,sizeof(struct Music_Lib),1,fp);
    }
    fclose(fp);
    fclose(temp);
    remove(Temporary_File);
    return 1;
}
int Verify_Mp3_File(char *file_name)
{
    char temp[250],invated_coma;
    int lenth=0,re1;
    temp[0]='\0';

    invated_coma=34;
    strcat(temp,"open ");
    lenth=strlen(temp);
    temp[lenth]=invated_coma;
    lenth++;
    temp[lenth]='\0';
    strcat(temp,file_name);
    lenth=strlen(temp);
    temp[lenth]=invated_coma;
    lenth++;
    temp[lenth]='\0';
    strcat(temp," type mpegvideo alias mp3");

    re1=mciSendStringA(temp,NULL,0,NULL);

    if(re1==0)
        return TRUE;
    else
        return FALSE;
}
int add_music_in_Library(char *f_path,char *f_name,int f_type)
{
    struct Music_Lib temp; //51
    struct dirent *data;
    DIR *dir;
    int re;
    FILE *fp;
    int type,flag;
    temp.back=NULL;
    temp.next=NULL;
    flag=0;
    get_system_date(&temp.day,&temp.month,&temp.year);
    close_music_file();
    fp=fopen(Music_Library,"ab");
    if(fp==NULL)
    {
        MessageBoxA(NULL," Please try Again. Music Files are not added in Music library."," Error",16);
        Clear_buffer();
        return 404;
    }
    if(f_type==Folder_p)
    {
        dir=opendir(f_path);
        if(dir==NULL)
        {
            MessageBoxA(NULL," Please try Again. File are not added in Music library."," Error",16);
            Clear_buffer();
            return 404;
        }
        close_music_file();
        while((data=readdir(dir))!=NULL)
        {
            type=check_mp3_file_or_not(data->d_name);
            if(type==TRUE)
            {
                flag=1;
                load_null_in_array(temp.f_name,64);
                load_null_in_array(temp.f_path,449);
                copy_spcf_part_of_string_to_another(data->d_name,temp.f_name,51);
                strcpy(temp.f_path,f_path);
                strcat(temp.f_path,"\\");
                strcat(temp.f_path,data->d_name);
                re=Verify_Mp3_File(temp.f_path);
                if(Music_file_already_exit_in_Music_Library(temp.f_name)!=1 && re==TRUE)
                    fwrite(&temp,sizeof(struct Music_Lib),1,fp);
                close_music_file();
            }
        }
        fclose(fp);
        closedir(dir);
        if(flag==1)
            MessageBoxA(NULL," All Music files of this folder or Drive have been added in Music Library","Music Files Added",64);
        else
            MessageBoxA(NULL," No Music Files in This Drive or Folder."," No Music Files",32);
        Clear_buffer();
        return 1;
    }
    else if(f_type==File_p)
    {
        load_null_in_array(temp.f_name,64);
        load_null_in_array(temp.f_path,449);
        strcpy(temp.f_name,f_name);
        strcpy(temp.f_path,f_path);
        if(Music_file_already_exit_in_Music_Library(temp.f_name)==1)
        {
            fclose(fp);
            MessageBoxA(NULL," Music file already exist in Music Library","File Already Exist",48);
            Clear_buffer();
            return 0;
        }
        close_music_file();
        if(Verify_Mp3_File(temp.f_path)!=TRUE)
        {
            MessageBoxA(NULL," Unsupportable Mp3 File","Unsupportable File",64);
            close_music_file();
            Clear_buffer();
            return 0;
        }
        fwrite(&temp,sizeof(struct Music_Lib),1,fp);
        fclose(fp);
        MessageBoxA(NULL," Music file have been added in Music Library","Music Files Added",64);
        Clear_buffer();
        return 1;
    }
    else
    {
        fclose(fp);
        MessageBoxA(NULL," Please try Again. File are not added in Music library."," Error",16);
        Clear_buffer();
        return 0;
    }
}
struct Music_Lib* new_node_of_music_lib()
{
    struct Music_Lib *temp;
    temp=(struct Music_Lib*)malloc(sizeof(struct Music_Lib));
    return temp;
};
int load_all_music_file_in_play_list(struct Music_Lib **Node)
{
    int count=0;
    struct Music_Lib *temp=NULL,*t=NULL;
    FILE *fp;
    fp=fopen(Music_Library,"rb");
    if(fp==NULL)
    {
        return -404;
    }
    temp=*Node;
    t=new_node_of_music_lib();
    while(fread(t,sizeof(struct Music_Lib),1,fp)>0)
    {
        t->next=NULL;
        t->back=NULL;
        if(temp==NULL)
        {
            temp=t;
        }
        else
        {
            t->next=temp;
            temp->back=t;
            temp=t;
        }
        t=new_node_of_music_lib();
        count++;
    }
    *Node=temp;
    free(t);
    fclose(fp);
    return count;
}
int Music_file_already_exit_in_Music_Library(char *f_name)
{
    FILE *fp;
    struct Music_Lib temp;
    fp=fopen(Music_Library,"rb");
    if(fp==NULL)
    {
        fclose(fp);
        return 0;
    }
    while(fread(&temp,sizeof(struct Music_Lib),1,fp)>0)
    {
        if(strcmp(f_name,temp.f_name)==0)
        {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}
int delete_play_list(struct Music_Lib **Node)
{
    struct Music_Lib *temp,*t;
    temp=*Node;
    t=*Node;
    if(temp==NULL)
    {
        return 0;
    }
    while(temp!=NULL)
    {
        t=temp;
        temp=temp->next;
        free(t);
    }
    *Node=NULL;
    return 1;
}
int Get_Lent_of_Play_List(struct Music_Lib *Node)
{
    int count=0;
    struct Music_Lib *temp;
    temp=Node;
    if(temp==NULL)
        return 0;
    while(temp!=NULL)
    {
        count++;
        temp=temp->next;
    }
    return count;
}
int delete_one_element_of_playlits(struct Music_Lib *element,struct Music_Lib **Node)
{
    struct Music_Lib *temp,*t;
    temp=*Node;
    if(temp==NULL)
        return 0;
    if(temp==element)
    {
        temp=element->next;
        temp->back=NULL;
        free(element);
        *Node=temp;
        return 1;
    }
    else
    {
        t=element->back;
        t->next=element->next;
        element->next->back=t;
        free(element);
        return 1;
    }
}
int play_file(char *file_name,int repeat)
{
    char temp[250],invated_coma;
    int lenth=0,re1,re2;
    temp[0]='\0';

    invated_coma=34;
    strcat(temp,"open ");
    lenth=strlen(temp);
    temp[lenth]=invated_coma;
    lenth++;
    temp[lenth]='\0';
    strcat(temp,file_name);
    lenth=strlen(temp);
    temp[lenth]=invated_coma;
    lenth++;
    temp[lenth]='\0';
    strcat(temp," type mpegvideo alias mp3");

    re1=mciSendStringA(temp,NULL,0,NULL);
    if(repeat==1)
    {
        re2=mciSendStringA("play mp3 repeat",NULL,0,NULL);
    }
    else
    {
        re2=mciSendStringA("play mp3",NULL,0,NULL);
    }
    if(re1==0 && re2==0)
        return 1;
    else
        return 0;
}
int stop_playing()
{
    int re1,re2;
    re1=mciSendStringA("stop mp3",NULL,0,NULL); //for stop music
    if(re1==0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
int close_music_file()
{
    int re;
    re=mciSendStringA("close mp3",NULL,0,NULL); //for close music file
    if(re==1)
        return 1;
    else
        return 0;
}
int Pause_Playing(int play)
{
    int re;
    re=mciSendStringA("pause mp3",NULL,0,NULL);
    if(re==0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
int Resume_Playing()
{
    int re;
    re=mciSendStringA("resume mp3",NULL,0,NULL);
    if(re==0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
int play_from_scratch()
{
    int re;
    re=mciSendString("play mp3 from 0", NULL, 0, NULL);
    if(re==0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
int File_Explorer(char *Disk)
{
    int x,y,a,total_drv,count,lenth,re;
    char ch,drv[50],current_drv[25];
    ind_back_freame(40,109,3,5,117,180,12);
    Bk_color(180,7);
    working_area_background();
    Bk_color(197,15);
    rec_MSGinM("Explorer",40,109,4);
    Copy_string_to_other_in_uupercase(Disk,drv);
    a=0;
    count=1;
    y=7;
    lenth=strlen(drv);
    total_drv=(lenth-4);
    if(lenth==0)
    {
        Bk_color(117,1);
        gotoxy(66,23);
        printf("NO DRIVES");
        getch();
        return 404;
    }
    else if(lenth==1 && System_Drive_Status==0)
    {
        Bk_color(117,1);
        gotoxy(72,21);
        printf("NO DRIVES");
        getch();
        return 404;
    }
    DS_reset:
    if(lenth>=1)
    {
        Bk_color(117,1);
        explorer_drive_strip(45,7);
    }
    if(lenth>=2)
    {
        Bk_color(117,5);
        explorer_drive_strip(45,15);
    }
    if(lenth>=3)
    {
        Bk_color(117,2);
        explorer_drive_strip(45,23);
    }
    if(lenth>=4)
    {
        Bk_color(117,20);
        explorer_drive_strip(45,31);
    }

    reset:
        explorer_drive_selector(41,y,12,197,15);
        if(lenth>=1)
        {
            Bk_color(28,15);
            gotoxy(66,10);
            if(a==0 && System_Drive_Status==1)
                printf("%c :  System Drive  %c",drv[a],240);
            else
                printf("%c :  Local Drive  %c",drv[a],14);
        }
        if(lenth>=2)
        {
            Bk_color(82,15);
            gotoxy(66,18);
            printf("%c :  Local Drive  %c",drv[a+1],14);
        }
        if(lenth>=3)
        {
            Bk_color(34,15);
            gotoxy(66,26);
            printf("%c :  Local Drive  %c",drv[a+2],14);
        }
        if(lenth>=4)
        {
            Bk_color(68,15);
            gotoxy(66,34);
            printf("%c :  Local Drive  %c",drv[a+3],14);
        }
        re_choice:
            ch=getch();
            if(ch==-32)
            {
                ch=getch();
                if(ch==up || ch==down || ch==right || ch==left)
                {
                    switch(ch)
                    {
                    case up:
                        if(y>7 && count>1)
                        {
                            explorer_drive_selector(41,y,7,117,7);
                            y=y-8;
                            count--;
                            goto reset;
                        }
                        else
                        {
                            if(a>0 && lenth>4)
                            {
                                Bk_color(28,1);
                                drive_strip_updater(64,10);
                                Bk_color(82,5);
                                drive_strip_updater(63,18);
                                Bk_color(34,2);
                                drive_strip_updater(63,26);
                                Bk_color(66,20);
                                drive_strip_updater(63,34);
                                a--;
                            }
                            goto reset;
                        }
                    case down:
                        if(y<31 && count<lenth)
                        {
                            explorer_drive_selector(41,y,7,117,7);
                            y=y+8;
                            count++;
                            goto reset;
                        }
                        else
                        {
                            if(a<total_drv && lenth>4)
                            {
                                Bk_color(28,1);
                                drive_strip_updater(64,10);
                                Bk_color(82,5);
                                drive_strip_updater(63,18);
                                Bk_color(34,2);
                                drive_strip_updater(63,26);
                                Bk_color(66,20);
                                drive_strip_updater(63,34);
                                a++;
                            }
                            goto reset;
                        }
                    case right:
                        Reloding_Player=Player_Loding;
                        Key_Stoke=Play_Next;
                        while(Reloding_Player!=Player_Loaded);
                        Update_Current_playing_Music();
                        goto re_choice;
                    case left:
                        Reloding_Player=Player_Loding;
                        Key_Stoke=Play_Previous;
                        while(Reloding_Player!=Player_Loaded);
                        Update_Current_playing_Music();
                        goto re_choice;
                    }
                }
                else
                {
                    goto re_choice;
                }
            }
            else if(ch==13)
            {
                switch(y)
                {
                case 7:
                    load_null_in_array(current_drv,24);
                    current_drv[0]=Disk[a];
                    strcat(current_drv,":\\");
                    break;
                case 15:
                    load_null_in_array(current_drv,24);
                    current_drv[0]=Disk[a+1];
                    strcat(current_drv,":\\");
                    break;
                case 23:
                    load_null_in_array(current_drv,24);
                    current_drv[0]=Disk[a+2];
                    strcat(current_drv,":\\");
                    break;
                case 31:
                    load_null_in_array(current_drv,24);
                    current_drv[0]=Disk[a+3];
                    strcat(current_drv,":\\");
                    break;
                }
                re=Explore_Directories(current_drv);
                Bk_color(180,7);
                working_area_background();
                ind_back_freame(40,109,3,5,117,180,12);
                Bk_color(197,15);
                if(re==27)
                {
                    load_null_in_array(Drives,50);
                    delete_list_of_path(&current_path);
                    delete_open_dir_list(&open_dir);
                    open_dir=NULL;
                    current_path=NULL;
                    return 27;
                }
                delete_list_of_path(&current_path);
                current_path=NULL;
                rec_MSGinM("Explorer",40,109,4);
                goto DS_reset;
            }
            else if(ch==27)
            {
                load_null_in_array(Drives,50);
                delete_open_dir_list(&open_dir);
                delete_list_of_path(&current_path);
                open_dir=NULL;
                current_path=NULL;
                return 27;
            }
            else if(ch=='+')
            {
                switch(y)
                {
                case 7:
                    load_null_in_array(current_drv,24);
                    current_drv[0]=Disk[a];
                    strcat(current_drv,":\\");
                    break;
                case 15:
                    load_null_in_array(current_drv,24);
                    current_drv[0]=Disk[a+1];
                    strcat(current_drv,":\\");
                    break;
                case 23:
                    load_null_in_array(current_drv,24);
                    current_drv[0]=Disk[a+2];
                    strcat(current_drv,":\\");
                    break;
                case 31:
                    load_null_in_array(current_drv,24);
                    current_drv[0]=Disk[a+3];
                    strcat(current_drv,":\\");
                    break;
                }
                add_music_in_Library(current_drv,"Music Of Disk Drive",Folder_p);
                goto re_choice;
            }
            else if(ch=='p' || ch=='P' || ch=='s' || ch=='S')
            {
                Handle_Back_End_Player(ch);
                Update_Current_playing_Music();
                goto re_choice;
            }
            else
            {
                goto re_choice;
            }
}
int Explore_Directories(char *current_dir_path)
{

    int y,Lenth,Count,re;
    char ch;
    struct open_paths *opened;
    struct path *point1,*point2,*point3,*point4,*point5,*point6,*point7,*point8,*point9,*point10,*point11,*point12,*point13,*point14,*point15,*point16;

    add_object_in_open_paths(&open_dir,current_dir_path,Folder_p);
    opened=open_dir;
    load_all_file_of_path(&current_path,current_dir_path);

    Lenth=get_lenth_of_current_path(&current_path);

    if(Lenth>=1)
        point1=current_path;
    if(Lenth>=2)
        point2=point1->next;
    if(Lenth>=3)
        point3=point2->next;
    if(Lenth>=4)
        point4=point3->next;
    if(Lenth>=5)
        point5=point4->next;
    if(Lenth>=6)
        point6=point5->next;
    if(Lenth>=7)
        point7=point6->next;
    if(Lenth>=8)
        point8=point7->next;
    if(Lenth>=9)
        point9=point8->next;
    if(Lenth>=10)
        point10=point9->next;
    if(Lenth>=11)
        point11=point10->next;
    if(Lenth>=12)
        point12=point11->next;
    if(Lenth>=13)
        point13=point12->next;
    if(Lenth>=14)
        point14=point13->next;
    if(Lenth>=15)
        point15=point14->next;
    if(Lenth>=16)
        point16=point15->next;

    ind_back_freame(40,109,3,5,117,180,12);
    Bk_color(180,7);
    working_area_background();
    Bk_color(197,15);
    gotoxy(67,4);
    printf("NAME");
    gotoxy(101,4);
    printf("TYPE");
    Bk_color(117,1);
    if(Lenth==0)
    {
        Bk_color(117,1);
        gotoxy(72,21);
        printf("No File / Folder");
        getch();
        return 404;
    }
    line_V(6,38,96,'n','n',0);
    y=7;
    Count=1;
    reset:
        if(Lenth==0)
        {
            Bk_color(117,1);
            gotoxy(62,21);
            printf("No File / Folder");
            goto re_choice;
        }
        Bk_color(117,14);
        Explore_Directories_strip(41,y);

        if(Lenth>=1)
        {
            if(y==7)
                Bk_color(225,9);
            else
                Bk_color(117,1);
            gotoxy(42,7);
            printf("%s",point1->f_name);
            Bk_color(117,1);
            gotoxy(98,7);
            if(point1->f_type==Folder_p)
                printf("File Folder");
            else
                printf("Mp3 Sound");
        }
        if(Lenth>=2)
        {
            if(y==9)
                Bk_color(225,9);
            else
                Bk_color(117,1);
            gotoxy(42,9);
            printf("%s",point2->f_name);
            Bk_color(117,1);
            gotoxy(98,9);
            if(point2->f_type==Folder_p)
                printf("File Folder");
            else
                printf("Mp3 Sound");
        }

        if(Lenth>=3)
        {
            if(y==11)
                Bk_color(225,9);
            else
                Bk_color(117,1);
            gotoxy(42,11);
            printf("%s",point3->f_name);
            Bk_color(117,1);
            gotoxy(98,11);
            if(point3->f_type==Folder_p)
                printf("File Folder");
            else
                printf("Mp3 Sound");
        }

        if(Lenth>=4)
        {
            if(y==13)
                Bk_color(225,9);
            else
                Bk_color(117,1);
            gotoxy(42,13);
            printf("%s",point4->f_name);
            Bk_color(117,1);
            gotoxy(98,13);
            if(point4->f_type==Folder_p)
                printf("File Folder");
            else
                printf("Mp3 Sound");
        }

        if(Lenth>=5)
        {
            if(y==15)
                Bk_color(225,9);
            else
                Bk_color(117,1);
            gotoxy(42,15);
            printf("%s",point5->f_name);
            Bk_color(117,1);
            gotoxy(98,15);
            if(point5->f_type==Folder_p)
                printf("File Folder");
            else
                printf("Mp3 Sound");
        }

        if(Lenth>=6)
        {
            if(y==17)
                Bk_color(225,9);
            else
                Bk_color(117,1);
            gotoxy(42,17);
            printf("%s",point6->f_name);
            Bk_color(117,1);
            gotoxy(98,17);
            if(point6->f_type==Folder_p)
                printf("File Folder");
            else
                printf("Mp3 Sound");
        }

        if(Lenth>=7)
        {
            if(y==19)
                Bk_color(225,9);
            else
                Bk_color(117,1);
            gotoxy(42,19);
            printf("%s",point7->f_name);
            Bk_color(117,1);
            gotoxy(98,19);
            if(point7->f_type==Folder_p)
                printf("File Folder");
            else
                printf("Mp3 Sound");
        }

        if(Lenth>=8)
        {
            if(y==21)
                Bk_color(225,9);
            else
                Bk_color(117,1);
            gotoxy(42,21);
            printf("%s",point8->f_name);
            Bk_color(117,1);
            gotoxy(98,21);
            if(point8->f_type==Folder_p)
                printf("File Folder");
            else
                printf("Mp3 Sound");
        }

        if(Lenth>=9)
        {
            if(y==23)
                Bk_color(225,9);
            else
                Bk_color(117,1);
            gotoxy(42,23);
            printf("%s",point9->f_name);
            Bk_color(117,1);
            gotoxy(98,23);
            if(point9->f_type==Folder_p)
                printf("File Folder");
            else
                printf("Mp3 Sound");
        }

        if(Lenth>=10)
        {
            if(y==25)
                Bk_color(225,9);
            else
                Bk_color(117,1);
            gotoxy(42,25);
            printf("%s",point10->f_name);
            Bk_color(117,1);
            gotoxy(98,25);
            if(point10->f_type==Folder_p)
                printf("File Folder");
            else
                printf("Mp3 Sound");
        }

        if(Lenth>=11)
        {
            if(y==27)
                Bk_color(225,9);
            else
                Bk_color(117,1);
            gotoxy(42,27);
            printf("%s",point11->f_name);
            Bk_color(117,1);
            gotoxy(98,27);
            if(point11->f_type==Folder_p)
                printf("File Folder");
            else
                printf("Mp3 Sound");
        }

        if(Lenth>=12)
        {
            if(y==29)
                Bk_color(225,9);
            else
                Bk_color(117,1);
            gotoxy(42,29);
            printf("%s",point12->f_name);
            Bk_color(117,1);
            gotoxy(98,29);
            if(point12->f_type==Folder_p)
                printf("File Folder");
            else
                printf("Mp3 Sound");
        }

        if(Lenth>=13)
        {
            if(y==31)
                Bk_color(225,9);
            else
                Bk_color(117,1);
            gotoxy(42,31);
            printf("%s",point13->f_name);
            Bk_color(117,1);
            gotoxy(98,31);
            if(point13->f_type==Folder_p)
                printf("File Folder");
            else
                printf("Mp3 Sound");
        }

        if(Lenth>=14)
        {
            if(y==33)
                Bk_color(225,9);
            else
                Bk_color(117,1);
            gotoxy(42,33);
            printf("%s",point14->f_name);
            Bk_color(117,1);
            gotoxy(98,33);
            if(point14->f_type==Folder_p)
                printf("File Folder");
            else
                printf("Mp3 Sound");
        }

        if(Lenth>=15)
        {
            if(y==35)
                Bk_color(225,9);
            else
                Bk_color(117,1);
            gotoxy(42,35);
            printf("%s",point15->f_name);
            Bk_color(117,1);
            gotoxy(98,35);
            if(point15->f_type==Folder_p)
                printf("File Folder");
            else
                printf("Mp3 Sound");
        }

        if(Lenth>=16)
        {
            if(y==37)
                Bk_color(225,9);
            else
                Bk_color(117,1);
            gotoxy(42,37);
            printf("%s",point16->f_name);
            Bk_color(117,1);
            gotoxy(98,37);
            if(point16->f_type==Folder_p)
                printf("File Folder");
            else
                printf("Mp3 Sound");
        }

        re_choice:
            ch=getch();
            if(ch==-32)
            {
                ch=getch();
                if(ch==up || ch==down || ch==right || ch==left)
                {
                    switch(ch)
                    {
                    case up:
                        if(y>7 && Count>1)
                        {
                            Bk_color(117,7);
                            Explore_Directories_strip(41,y);
                            y=y-2;
                            Count--;
                            goto reset;
                        }
                        else
                        {
                            if(point1->back!=NULL)
                            {
                                point1=point1->back;
                                point2=point2->back;
                                point3=point3->back;
                                point4=point4->back;
                                point5=point5->back;
                                point6=point6->back;
                                point7=point7->back;
                                point8=point8->back;
                                point9=point9->back;
                                point10=point10->back;
                                point11=point11->back;
                                point12=point12->back;
                                point13=point13->back;
                                point14=point14->back;
                                point15=point15->back;
                                point16=point16->back;
                                Explore_Directories_updater();
                                goto reset;
                            }
                            goto re_choice;
                        }
                    case down:
                        if(y<37 && Count<Lenth)
                        {
                            Bk_color(117,7);
                            Explore_Directories_strip(41,y);
                            y=y+2;
                            Count++;
                            goto reset;
                        }
                        else
                        {
                            if(Lenth>16 && point16->next!=NULL)
                            {
                                point1=point1->next;
                                point2=point2->next;
                                point3=point3->next;
                                point4=point4->next;
                                point5=point5->next;
                                point6=point6->next;
                                point7=point7->next;
                                point8=point8->next;
                                point9=point9->next;
                                point10=point10->next;
                                point11=point11->next;
                                point12=point12->next;
                                point13=point13->next;
                                point14=point14->next;
                                point15=point15->next;
                                point16=point16->next;
                                Explore_Directories_updater();
                                goto reset;
                            }
                            goto re_choice;
                        }
                    case right:
                        close_music_file();
                        Reloding_Player=Player_Loding;
                        Key_Stoke=Play_Next;
                        while(Reloding_Player!=Player_Loaded);
                        Update_Current_playing_Music();
                        goto re_choice;
                    case left:
                        close_music_file();
                        Reloding_Player=Player_Loding;
                        Key_Stoke=Play_Previous;
                        while(Reloding_Player!=Player_Loaded);
                        Update_Current_playing_Music();
                        goto re_choice;
                    }
                }
                else
                {
                    goto re_choice;
                }
            }
            else if(ch==13 && Lenth>=1)
            {
                if(y==7 && point1->f_type==Folder_p)
                {
                    add_object_in_open_paths(&open_dir,point1->f_path,Folder_p);
                    opened=opened->next;
                    delete_list_of_path(&current_path);
                    load_all_file_of_path(&current_path,opened->f_path);
                    Lenth=get_lenth_of_current_path(&current_path);
                    y=7;
                    Count=1;
                    if(Lenth>=1)
                        point1=current_path;
                    if(Lenth>=2)
                        point2=point1->next;
                    if(Lenth>=3)
                        point3=point2->next;
                    if(Lenth>=4)
                        point4=point3->next;
                    if(Lenth>=5)
                        point5=point4->next;
                    if(Lenth>=6)
                        point6=point5->next;
                    if(Lenth>=7)
                        point7=point6->next;
                    if(Lenth>=8)
                        point8=point7->next;
                    if(Lenth>=9)
                        point9=point8->next;
                    if(Lenth>=10)
                        point10=point9->next;
                    if(Lenth>=11)
                        point11=point10->next;
                    if(Lenth>=12)
                        point12=point11->next;
                    if(Lenth>=13)
                        point13=point12->next;
                    if(Lenth>=14)
                        point14=point13->next;
                    if(Lenth>=15)
                        point15=point14->next;
                    if(Lenth>=16)
                        point16=point15->next;
                    Explore_Directories_updater();
                    goto reset;
                }
                if(y==9 && point2->f_type==Folder_p)
                {
                    add_object_in_open_paths(&open_dir,point2->f_path,Folder_p);
                    opened=opened->next;
                    delete_list_of_path(&current_path);
                    load_all_file_of_path(&current_path,opened->f_path);
                    Lenth=get_lenth_of_current_path(&current_path);
                    y=7;
                    Count=1;
                    if(Lenth>=1)
                        point1=current_path;
                    if(Lenth>=2)
                        point2=point1->next;
                    if(Lenth>=3)
                        point3=point2->next;
                    if(Lenth>=4)
                        point4=point3->next;
                    if(Lenth>=5)
                        point5=point4->next;
                    if(Lenth>=6)
                        point6=point5->next;
                    if(Lenth>=7)
                        point7=point6->next;
                    if(Lenth>=8)
                        point8=point7->next;
                    if(Lenth>=9)
                        point9=point8->next;
                    if(Lenth>=10)
                        point10=point9->next;
                    if(Lenth>=11)
                        point11=point10->next;
                    if(Lenth>=12)
                        point12=point11->next;
                    if(Lenth>=13)
                        point13=point12->next;
                    if(Lenth>=14)
                        point14=point13->next;
                    if(Lenth>=15)
                        point15=point14->next;
                    if(Lenth>=16)
                        point16=point15->next;
                    Explore_Directories_updater();
                    goto reset;
                }
                if(y==11 && point3->f_type==Folder_p)
                {
                    add_object_in_open_paths(&open_dir,point3->f_path,Folder_p);
                    opened=opened->next;
                    delete_list_of_path(&current_path);
                    load_all_file_of_path(&current_path,opened->f_path);
                    Lenth=get_lenth_of_current_path(&current_path);
                    y=7;
                    Count=1;
                    if(Lenth>=1)
                        point1=current_path;
                    if(Lenth>=2)
                        point2=point1->next;
                    if(Lenth>=3)
                        point3=point2->next;
                    if(Lenth>=4)
                        point4=point3->next;
                    if(Lenth>=5)
                        point5=point4->next;
                    if(Lenth>=6)
                        point6=point5->next;
                    if(Lenth>=7)
                        point7=point6->next;
                    if(Lenth>=8)
                        point8=point7->next;
                    if(Lenth>=9)
                        point9=point8->next;
                    if(Lenth>=10)
                        point10=point9->next;
                    if(Lenth>=11)
                        point11=point10->next;
                    if(Lenth>=12)
                        point12=point11->next;
                    if(Lenth>=13)
                        point13=point12->next;
                    if(Lenth>=14)
                        point14=point13->next;
                    if(Lenth>=15)
                        point15=point14->next;
                    if(Lenth>=16)
                        point16=point15->next;

                    Explore_Directories_updater();
                    goto reset;
                }
                if(y==13 && point4->f_type==Folder_p)
                {
                    add_object_in_open_paths(&open_dir,point4->f_path,Folder_p);
                    opened=opened->next;
                    delete_list_of_path(&current_path);
                    load_all_file_of_path(&current_path,opened->f_path);
                    Lenth=get_lenth_of_current_path(&current_path);
                    y=7;
                    Count=1;
                    if(Lenth>=1)
                        point1=current_path;
                    if(Lenth>=2)
                        point2=point1->next;
                    if(Lenth>=3)
                        point3=point2->next;
                    if(Lenth>=4)
                        point4=point3->next;
                    if(Lenth>=5)
                        point5=point4->next;
                    if(Lenth>=6)
                        point6=point5->next;
                    if(Lenth>=7)
                        point7=point6->next;
                    if(Lenth>=8)
                        point8=point7->next;
                    if(Lenth>=9)
                        point9=point8->next;
                    if(Lenth>=10)
                        point10=point9->next;
                    if(Lenth>=11)
                        point11=point10->next;
                    if(Lenth>=12)
                        point12=point11->next;
                    if(Lenth>=13)
                        point13=point12->next;
                    if(Lenth>=14)
                        point14=point13->next;
                    if(Lenth>=15)
                        point15=point14->next;
                    if(Lenth>=16)
                        point16=point15->next;
                    Explore_Directories_updater();
                    goto reset;
                }
                if(y==15 && point5->f_type==Folder_p)
                {
                    add_object_in_open_paths(&open_dir,point5->f_path,Folder_p);
                    opened=opened->next;
                    delete_list_of_path(&current_path);
                    load_all_file_of_path(&current_path,opened->f_path);
                    Lenth=get_lenth_of_current_path(&current_path);
                    y=7;
                    Count=1;
                    if(Lenth>=1)
                        point1=current_path;
                    if(Lenth>=2)
                        point2=point1->next;
                    if(Lenth>=3)
                        point3=point2->next;
                    if(Lenth>=4)
                        point4=point3->next;
                    if(Lenth>=5)
                        point5=point4->next;
                    if(Lenth>=6)
                        point6=point5->next;
                    if(Lenth>=7)
                        point7=point6->next;
                    if(Lenth>=8)
                        point8=point7->next;
                    if(Lenth>=9)
                        point9=point8->next;
                    if(Lenth>=10)
                        point10=point9->next;
                    if(Lenth>=11)
                        point11=point10->next;
                    if(Lenth>=12)
                        point12=point11->next;
                    if(Lenth>=13)
                        point13=point12->next;
                    if(Lenth>=14)
                        point14=point13->next;
                    if(Lenth>=15)
                        point15=point14->next;
                    if(Lenth>=16)
                        point16=point15->next;
                    Explore_Directories_updater();
                    goto reset;
                }
                if(y==17 && point6->f_type==Folder_p)
                {
                    add_object_in_open_paths(&open_dir,point6->f_path,Folder_p);
                    opened=opened->next;
                    delete_list_of_path(&current_path);
                    load_all_file_of_path(&current_path,opened->f_path);
                    Lenth=get_lenth_of_current_path(&current_path);
                    y=7;
                    Count=1;
                    if(Lenth>=1)
                        point1=current_path;
                    if(Lenth>=2)
                        point2=point1->next;
                    if(Lenth>=3)
                        point3=point2->next;
                    if(Lenth>=4)
                        point4=point3->next;
                    if(Lenth>=5)
                        point5=point4->next;
                    if(Lenth>=6)
                        point6=point5->next;
                    if(Lenth>=7)
                        point7=point6->next;
                    if(Lenth>=8)
                        point8=point7->next;
                    if(Lenth>=9)
                        point9=point8->next;
                    if(Lenth>=10)
                        point10=point9->next;
                    if(Lenth>=11)
                        point11=point10->next;
                    if(Lenth>=12)
                        point12=point11->next;
                    if(Lenth>=13)
                        point13=point12->next;
                    if(Lenth>=14)
                        point14=point13->next;
                    if(Lenth>=15)
                        point15=point14->next;
                    if(Lenth>=16)
                        point16=point15->next;
                    Explore_Directories_updater();
                    goto reset;
                }
                if(y==19 && point7->f_type==Folder_p)
                {
                    add_object_in_open_paths(&open_dir,point7->f_path,Folder_p);
                    opened=opened->next;
                    delete_list_of_path(&current_path);
                    load_all_file_of_path(&current_path,opened->f_path);
                    Lenth=get_lenth_of_current_path(&current_path);
                    y=7;
                    Count=1;
                    if(Lenth>=1)
                        point1=current_path;
                    if(Lenth>=2)
                        point2=point1->next;
                    if(Lenth>=3)
                        point3=point2->next;
                    if(Lenth>=4)
                        point4=point3->next;
                    if(Lenth>=5)
                        point5=point4->next;
                    if(Lenth>=6)
                        point6=point5->next;
                    if(Lenth>=7)
                        point7=point6->next;
                    if(Lenth>=8)
                        point8=point7->next;
                    if(Lenth>=9)
                        point9=point8->next;
                    if(Lenth>=10)
                        point10=point9->next;
                    if(Lenth>=11)
                        point11=point10->next;
                    if(Lenth>=12)
                        point12=point11->next;
                    if(Lenth>=13)
                        point13=point12->next;
                    if(Lenth>=14)
                        point14=point13->next;
                    if(Lenth>=15)
                        point15=point14->next;
                    if(Lenth>=16)
                        point16=point15->next;
                    Explore_Directories_updater();
                    goto reset;
                }
                if(y==21 && point8->f_type==Folder_p)
                {
                    add_object_in_open_paths(&open_dir,point8->f_path,Folder_p);
                    opened=opened->next;
                    delete_list_of_path(&current_path);
                    load_all_file_of_path(&current_path,opened->f_path);
                    Lenth=get_lenth_of_current_path(&current_path);
                    y=7;
                    Count=1;
                    if(Lenth>=1)
                        point1=current_path;
                    if(Lenth>=2)
                        point2=point1->next;
                    if(Lenth>=3)
                        point3=point2->next;
                    if(Lenth>=4)
                        point4=point3->next;
                    if(Lenth>=5)
                        point5=point4->next;
                    if(Lenth>=6)
                        point6=point5->next;
                    if(Lenth>=7)
                        point7=point6->next;
                    if(Lenth>=8)
                        point8=point7->next;
                    if(Lenth>=9)
                        point9=point8->next;
                    if(Lenth>=10)
                        point10=point9->next;
                    if(Lenth>=11)
                        point11=point10->next;
                    if(Lenth>=12)
                        point12=point11->next;
                    if(Lenth>=13)
                        point13=point12->next;
                    if(Lenth>=14)
                        point14=point13->next;
                    if(Lenth>=15)
                        point15=point14->next;
                    if(Lenth>=16)
                        point16=point15->next;
                    Explore_Directories_updater();
                    goto reset;
                }
                if(y==23 && point9->f_type==Folder_p)
                {
                    add_object_in_open_paths(&open_dir,point9->f_path,Folder_p);
                    opened=opened->next;
                    delete_list_of_path(&current_path);
                    load_all_file_of_path(&current_path,opened->f_path);
                    Lenth=get_lenth_of_current_path(&current_path);
                    y=7;
                    Count=1;
                    if(Lenth>=1)
                        point1=current_path;
                    if(Lenth>=2)
                        point2=point1->next;
                    if(Lenth>=3)
                        point3=point2->next;
                    if(Lenth>=4)
                        point4=point3->next;
                    if(Lenth>=5)
                        point5=point4->next;
                    if(Lenth>=6)
                        point6=point5->next;
                    if(Lenth>=7)
                        point7=point6->next;
                    if(Lenth>=8)
                        point8=point7->next;
                    if(Lenth>=9)
                        point9=point8->next;
                    if(Lenth>=10)
                        point10=point9->next;
                    if(Lenth>=11)
                        point11=point10->next;
                    if(Lenth>=12)
                        point12=point11->next;
                    if(Lenth>=13)
                        point13=point12->next;
                    if(Lenth>=14)
                        point14=point13->next;
                    if(Lenth>=15)
                        point15=point14->next;
                    if(Lenth>=16)
                        point16=point15->next;
                    Explore_Directories_updater();
                    goto reset;
                }
                if(y==25 && point10->f_type==Folder_p)
                {
                    add_object_in_open_paths(&open_dir,point10->f_path,Folder_p);
                    opened=opened->next;
                    delete_list_of_path(&current_path);
                    load_all_file_of_path(&current_path,opened->f_path);
                    Lenth=get_lenth_of_current_path(&current_path);
                    y=7;
                    Count=1;
                    if(Lenth>=1)
                        point1=current_path;
                    if(Lenth>=2)
                        point2=point1->next;
                    if(Lenth>=3)
                        point3=point2->next;
                    if(Lenth>=4)
                        point4=point3->next;
                    if(Lenth>=5)
                        point5=point4->next;
                    if(Lenth>=6)
                        point6=point5->next;
                    if(Lenth>=7)
                        point7=point6->next;
                    if(Lenth>=8)
                        point8=point7->next;
                    if(Lenth>=9)
                        point9=point8->next;
                    if(Lenth>=10)
                        point10=point9->next;
                    if(Lenth>=11)
                        point11=point10->next;
                    if(Lenth>=12)
                        point12=point11->next;
                    if(Lenth>=13)
                        point13=point12->next;
                    if(Lenth>=14)
                        point14=point13->next;
                    if(Lenth>=15)
                        point15=point14->next;
                    if(Lenth>=16)
                        point16=point15->next;
                    Explore_Directories_updater();
                    goto reset;
                }
                if(y==27 && point11->f_type==Folder_p)
                {
                    add_object_in_open_paths(&open_dir,point11->f_path,Folder_p);
                    opened=opened->next;
                    delete_list_of_path(&current_path);
                    load_all_file_of_path(&current_path,opened->f_path);
                    Lenth=get_lenth_of_current_path(&current_path);
                    y=7;
                    Count=1;
                    if(Lenth>=1)
                        point1=current_path;
                    if(Lenth>=2)
                        point2=point1->next;
                    if(Lenth>=3)
                        point3=point2->next;
                    if(Lenth>=4)
                        point4=point3->next;
                    if(Lenth>=5)
                        point5=point4->next;
                    if(Lenth>=6)
                        point6=point5->next;
                    if(Lenth>=7)
                        point7=point6->next;
                    if(Lenth>=8)
                        point8=point7->next;
                    if(Lenth>=9)
                        point9=point8->next;
                    if(Lenth>=10)
                        point10=point9->next;
                    if(Lenth>=11)
                        point11=point10->next;
                    if(Lenth>=12)
                        point12=point11->next;
                    if(Lenth>=13)
                        point13=point12->next;
                    if(Lenth>=14)
                        point14=point13->next;
                    if(Lenth>=15)
                        point15=point14->next;
                    if(Lenth>=16)
                        point16=point15->next;
                    Explore_Directories_updater();
                    goto reset;
                }
                if(y==29 && point12->f_type==Folder_p)
                {
                    add_object_in_open_paths(&open_dir,point12->f_path,Folder_p);
                    opened=opened->next;
                    delete_list_of_path(&current_path);
                    load_all_file_of_path(&current_path,opened->f_path);
                    Lenth=get_lenth_of_current_path(&current_path);
                    y=7;
                    Count=1;
                    if(Lenth>=1)
                        point1=current_path;
                    if(Lenth>=2)
                        point2=point1->next;
                    if(Lenth>=3)
                        point3=point2->next;
                    if(Lenth>=4)
                        point4=point3->next;
                    if(Lenth>=5)
                        point5=point4->next;
                    if(Lenth>=6)
                        point6=point5->next;
                    if(Lenth>=7)
                        point7=point6->next;
                    if(Lenth>=8)
                        point8=point7->next;
                    if(Lenth>=9)
                        point9=point8->next;
                    if(Lenth>=10)
                        point10=point9->next;
                    if(Lenth>=11)
                        point11=point10->next;
                    if(Lenth>=12)
                        point12=point11->next;
                    if(Lenth>=13)
                        point13=point12->next;
                    if(Lenth>=14)
                        point14=point13->next;
                    if(Lenth>=15)
                        point15=point14->next;
                    if(Lenth>=16)
                        point16=point15->next;
                    Explore_Directories_updater();
                    goto reset;
                }
                if(y==31 && point13->f_type==Folder_p)
                {
                    add_object_in_open_paths(&open_dir,point13->f_path,Folder_p);
                    opened=opened->next;
                    delete_list_of_path(&current_path);
                    load_all_file_of_path(&current_path,opened->f_path);
                    Lenth=get_lenth_of_current_path(&current_path);
                    y=7;
                    Count=1;
                    if(Lenth>=1)
                        point1=current_path;
                    if(Lenth>=2)
                        point2=point1->next;
                    if(Lenth>=3)
                        point3=point2->next;
                    if(Lenth>=4)
                        point4=point3->next;
                    if(Lenth>=5)
                        point5=point4->next;
                    if(Lenth>=6)
                        point6=point5->next;
                    if(Lenth>=7)
                        point7=point6->next;
                    if(Lenth>=8)
                        point8=point7->next;
                    if(Lenth>=9)
                        point9=point8->next;
                    if(Lenth>=10)
                        point10=point9->next;
                    if(Lenth>=11)
                        point11=point10->next;
                    if(Lenth>=12)
                        point12=point11->next;
                    if(Lenth>=13)
                        point13=point12->next;
                    if(Lenth>=14)
                        point14=point13->next;
                    if(Lenth>=15)
                        point15=point14->next;
                    if(Lenth>=16)
                        point16=point15->next;
                    Explore_Directories_updater();
                    goto reset;
                }
                if(y==33 && point14->f_type==Folder_p)
                {
                    add_object_in_open_paths(&open_dir,point14->f_path,Folder_p);
                    opened=opened->next;
                    delete_list_of_path(&current_path);
                    load_all_file_of_path(&current_path,opened->f_path);
                    Lenth=get_lenth_of_current_path(&current_path);
                    y=7;
                    Count=1;
                    if(Lenth>=1)
                        point1=current_path;
                    if(Lenth>=2)
                        point2=point1->next;
                    if(Lenth>=3)
                        point3=point2->next;
                    if(Lenth>=4)
                        point4=point3->next;
                    if(Lenth>=5)
                        point5=point4->next;
                    if(Lenth>=6)
                        point6=point5->next;
                    if(Lenth>=7)
                        point7=point6->next;
                    if(Lenth>=8)
                        point8=point7->next;
                    if(Lenth>=9)
                        point9=point8->next;
                    if(Lenth>=10)
                        point10=point9->next;
                    if(Lenth>=11)
                        point11=point10->next;
                    if(Lenth>=12)
                        point12=point11->next;
                    if(Lenth>=13)
                        point13=point12->next;
                    if(Lenth>=14)
                        point14=point13->next;
                    if(Lenth>=15)
                        point15=point14->next;
                    if(Lenth>=16)
                        point16=point15->next;
                    Explore_Directories_updater();
                    goto reset;
                }
                if(y==35 && point15->f_type==Folder_p)
                {
                    add_object_in_open_paths(&open_dir,point15->f_path,Folder_p);
                    opened=opened->next;
                    delete_list_of_path(&current_path);
                    load_all_file_of_path(&current_path,opened->f_path);
                    Lenth=get_lenth_of_current_path(&current_path);
                    y=7;
                    Count=1;
                    if(Lenth>=1)
                        point1=current_path;
                    if(Lenth>=2)
                        point2=point1->next;
                    if(Lenth>=3)
                        point3=point2->next;
                    if(Lenth>=4)
                        point4=point3->next;
                    if(Lenth>=5)
                        point5=point4->next;
                    if(Lenth>=6)
                        point6=point5->next;
                    if(Lenth>=7)
                        point7=point6->next;
                    if(Lenth>=8)
                        point8=point7->next;
                    if(Lenth>=9)
                        point9=point8->next;
                    if(Lenth>=10)
                        point10=point9->next;
                    if(Lenth>=11)
                        point11=point10->next;
                    if(Lenth>=12)
                        point12=point11->next;
                    if(Lenth>=13)
                        point13=point12->next;
                    if(Lenth>=14)
                        point14=point13->next;
                    if(Lenth>=15)
                        point15=point14->next;
                    if(Lenth>=16)
                        point16=point15->next;
                    Explore_Directories_updater();
                    goto reset;
                }
                if(y==37 && point16->f_type==Folder_p)
                {
                    add_object_in_open_paths(&open_dir,point16->f_path,Folder_p);
                    opened=opened->next;
                    delete_list_of_path(&current_path);
                    load_all_file_of_path(&current_path,opened->f_path);
                    Lenth=get_lenth_of_current_path(&current_path);
                    y=7;
                    Count=1;
                    if(Lenth>=1)
                        point1=current_path;
                    if(Lenth>=2)
                        point2=point1->next;
                    if(Lenth>=3)
                        point3=point2->next;
                    if(Lenth>=4)
                        point4=point3->next;
                    if(Lenth>=5)
                        point5=point4->next;
                    if(Lenth>=6)
                        point6=point5->next;
                    if(Lenth>=7)
                        point7=point6->next;
                    if(Lenth>=8)
                        point8=point7->next;
                    if(Lenth>=9)
                        point9=point8->next;
                    if(Lenth>=10)
                        point10=point9->next;
                    if(Lenth>=11)
                        point11=point10->next;
                    if(Lenth>=12)
                        point12=point11->next;
                    if(Lenth>=13)
                        point13=point12->next;
                    if(Lenth>=14)
                        point14=point13->next;
                    if(Lenth>=15)
                        point15=point14->next;
                    if(Lenth>=16)
                        point16=point15->next;
                    Explore_Directories_updater();
                    goto reset;
                }
                goto re_choice;
            }
            else if(ch==8)
            {
                opened=opened->back;
                delete_last_node_of_open_paths(&open_dir);
                delete_list_of_path(&current_path);
                if(opened==NULL)
                {
                    if(Play_Status==Paused_By_Exp)
                    {
                        close_music_file();
                        Reloding_Player=Player_Loding;
                        Key_Stoke=Resume_Music;
                        while(Reloding_Player!=Player_Loaded);
                        Update_Current_playing_Music();
                    }
                    return 1;
                }
                load_all_file_of_path(&current_path,opened->f_path);
                Lenth=get_lenth_of_current_path(&current_path);
                y=7;
                Count=1;
                if(Lenth>=1)
                    point1=current_path;
                if(Lenth>=2)
                    point2=point1->next;
                if(Lenth>=3)
                    point3=point2->next;
                if(Lenth>=4)
                    point4=point3->next;
                if(Lenth>=5)
                    point5=point4->next;
                if(Lenth>=6)
                    point6=point5->next;
                if(Lenth>=7)
                    point7=point6->next;
                if(Lenth>=8)
                    point8=point7->next;
                if(Lenth>=9)
                    point9=point8->next;
                if(Lenth>=10)
                    point10=point9->next;
                if(Lenth>=11)
                    point11=point10->next;
                if(Lenth>=12)
                    point12=point11->next;
                if(Lenth>=13)
                    point13=point12->next;
                if(Lenth>=14)
                    point14=point13->next;
                if(Lenth>=15)
                    point15=point14->next;
                if(Lenth>=16)
                    point16=point15->next;
                Explore_Directories_updater();
                goto reset;
            }
            else if(ch==27)
            {
                delete_list_of_path(&current_path);
                delete_open_dir_list(&open_dir);
                if(Play_Status==Paused_By_Exp)
                {
                    Reloding_Player=Player_Loding;
                    Key_Stoke=Resume_Music;
                    while(Reloding_Player!=Player_Loaded);
                    Update_Current_playing_Music();
                }
                return 27;
            }
            else if(ch=='+' && Lenth!=0)
            {
                if(y==7)
                    add_music_in_Library(point1->f_path,point1->f_name,point1->f_type);
                if(y==9)
                    add_music_in_Library(point2->f_path,point2->f_name,point2->f_type);
                if(y==11)
                    add_music_in_Library(point3->f_path,point3->f_name,point3->f_type);
                if(y==13)
                    add_music_in_Library(point4->f_path,point4->f_name,point4->f_type);
                if(y==15)
                    add_music_in_Library(point5->f_path,point5->f_name,point5->f_type);
                if(y==17)
                    add_music_in_Library(point6->f_path,point6->f_name,point6->f_type);
                if(y==19)
                    add_music_in_Library(point7->f_path,point7->f_name,point7->f_type);
                if(y==21)
                    add_music_in_Library(point8->f_path,point8->f_name,point8->f_type);
                if(y==23)
                    add_music_in_Library(point9->f_path,point9->f_name,point9->f_type);
                if(y==25)
                    add_music_in_Library(point10->f_path,point10->f_name,point10->f_type);
                if(y==27)
                    add_music_in_Library(point11->f_path,point11->f_name,point11->f_type);
                if(y==29)
                    add_music_in_Library(point12->f_path,point12->f_name,point12->f_type);
                if(y==31)
                    add_music_in_Library(point13->f_path,point13->f_name,point13->f_type);
                if(y==33)
                    add_music_in_Library(point14->f_path,point14->f_name,point14->f_type);
                if(y==35)
                    add_music_in_Library(point15->f_path,point15->f_name,point15->f_type);
                if(y==37)
                    add_music_in_Library(point16->f_path,point16->f_name,point16->f_type);
                Clear_buffer();
                goto re_choice;
            }
            else if((ch=='p' || ch=='P') && Lenth>1)
            {
                re=0;
                close_music_file();
                if(y==7 && point1->f_type==File_p)
                {
                    re=play_file(point1->f_path,0);
                    if(re==0)
                    {
                        MessageBoxA(NULL," Brightgoal Mp3 Player encountered a problem while playing the file."," Brightgoal Mp3 Player",16);
                    }
                }
                else if(y==9 && point2->f_type==File_p)
                {
                    re=play_file(point2->f_path,0);
                    if(re==0)
                    {
                        MessageBoxA(NULL," Brightgoal Mp3 Player encountered a problem while playing the file."," Brightgoal Mp3 Player",16);
                    }
                }
                else if(y==11 && point3->f_type==File_p)
                {
                    re=play_file(point3->f_path,0);
                    if(re==0)
                    {
                        MessageBoxA(NULL," Brightgoal Mp3 Player encountered a problem while playing the file."," Brightgoal Mp3 Player",16);
                    }
                }
                else if(y==13 && point4->f_type==File_p)
                {
                    re=play_file(point4->f_path,0);
                    if(re==0)
                    {
                        MessageBoxA(NULL," Brightgoal Mp3 Player encountered a problem while playing the file."," Brightgoal Mp3 Player",16);
                    }
                }
                else if(y==15 && point5->f_type==File_p)
                {
                    re=play_file(point5->f_path,0);
                    if(re==0)
                    {
                        MessageBoxA(NULL," Brightgoal Mp3 Player encountered a problem while playing the file."," Brightgoal Mp3 Player",16);
                    }
                }
                else if(y==17 && point6->f_type==File_p)
                {
                    re=play_file(point6->f_path,0);
                    if(re==0)
                    {
                        MessageBoxA(NULL," Brightgoal Mp3 Player encountered a problem while playing the file."," Brightgoal Mp3 Player",16);
                    }
                }
                else if(y==19 && point7->f_type==File_p)
                {
                    re=play_file(point7->f_path,0);
                    if(re==0)
                    {
                        MessageBoxA(NULL," Brightgoal Mp3 Player encountered a problem while playing the file."," Brightgoal Mp3 Player",16);
                    }
                }
                else if(y==21 && point8->f_type==File_p)
                {
                    re=play_file(point8->f_path,0);
                    if(re==0)
                    {
                        MessageBoxA(NULL," Brightgoal Mp3 Player encountered a problem while playing the file."," Brightgoal Mp3 Player",16);
                    }
                }
                else if(y==23 && point9->f_type==File_p)
                {
                    re=play_file(point9->f_path,0);
                    if(re==0)
                    {
                        MessageBoxA(NULL," Brightgoal Mp3 Player encountered a problem while playing the file."," Brightgoal Mp3 Player",16);
                    }
                }
                else if(y==25 && point10->f_type==File_p)
                {
                    re=play_file(point10->f_path,0);
                    if(re==0)
                    {
                        MessageBoxA(NULL," Brightgoal Mp3 Player encountered a problem while playing the file."," Brightgoal Mp3 Player",16);
                    }
                }
                else if(y==27 && point11->f_type==File_p)
                {
                    re=play_file(point11->f_path,0);
                    if(re==0)
                    {
                        MessageBoxA(NULL," Brightgoal Mp3 Player encountered a problem while playing the file."," Brightgoal Mp3 Player",16);
                    }
                }
                else if(y==29 && point12->f_type==File_p)
                {
                    re=play_file(point12->f_path,0);
                    if(re==0)
                    {
                        MessageBoxA(NULL," Brightgoal Mp3 Player encountered a problem while playing the file."," Brightgoal Mp3 Player",16);
                    }
                }
                else if(y==31 && point13->f_type==File_p)
                {
                    re=play_file(point13->f_path,0);
                    if(re==0)
                    {
                        MessageBoxA(NULL," Brightgoal Mp3 Player encountered a problem while playing the file."," Brightgoal Mp3 Player",16);
                    }
                }
                else if(y==33 && point14->f_type==File_p)
                {
                    re=play_file(point14->f_path,0);
                    if(re==0)
                    {
                        MessageBoxA(NULL," Brightgoal Mp3 Player encountered a problem while playing the file."," Brightgoal Mp3 Player",16);
                    }
                }
                else if(y==35 && point15->f_type==File_p)
                {
                    re=play_file(point15->f_path,0);
                    if(re==0)
                    {
                        MessageBoxA(NULL," Brightgoal Mp3 Player encountered a problem while playing the file."," Brightgoal Mp3 Player",16);
                    }
                }
                else if(y==37 && point16->f_type==File_p)
                {
                    re=play_file(point16->f_path,0);
                    if(re==0)
                    {
                        MessageBoxA(NULL," Brightgoal Mp3 Player encountered a problem while playing the file."," Brightgoal Mp3 Player",16);
                    }
                }
                else
                {
                    Handle_Back_End_Player(ch);
                    Update_Current_playing_Music();
                    Clear_buffer();
                    goto re_choice;
                }
                if(re==1)
                {
                    if(Play_Status==Played)
                    {
                        Reloding_Player=Player_Loding;
                        Key_Stoke=Pause_Music;
                        while(Reloding_Player!=Player_Loaded);
                        {

                        }
                        Update_Current_playing_Music();
                        Play_Status=Paused_By_Exp;
                    }
                }
                Clear_buffer();
                goto re_choice;
            }
            else if(ch=='s' || ch=='S')
            {
                close_music_file();
                if(Play_Status==Paused_By_Exp)
                {
                    Reloding_Player=Player_Loding;
                    Key_Stoke=Resume_Music;
                    while(Reloding_Player!=Player_Loaded);
                    Update_Current_playing_Music();
                }
                goto re_choice;
            }
            else
            {
                goto re_choice;
            }
}
int delete_last_node_of_open_paths(struct open_paths **Node)
{
    struct open_paths *temp,*t;
    temp=*Node;
    if(temp==NULL)
    {
        return 0;
    }
    while(temp->next!=NULL)
    {
        temp=temp->next;
    }
    if(*Node==temp)
    {
        free(temp);
        *Node=NULL;
        return 1;
    }
    else
    {
        t=temp->back;
        free(temp);
        t->next=NULL;
        return 1;
    }
}
int get_lenth_of_current_path(struct path **node)
{
    struct path *temp;
    int count=0;
    temp=*node;
    if(temp==NULL)
    {
        return 0;
    }
    while(temp!=NULL)
    {
        count++;
        temp=temp->next;
    }
    return count;
}
void load_null_in_array(char *string,int lenth)
{
    int i;
    for(i=0;i<lenth;i++)
    {
        string[i]='\0';
    }
}
void Explore_Directories_updater()
{
    Bk_color(117,7);
    Explore_Directories_strip(41,7);
    Explore_Directories_strip(41,9);
    Explore_Directories_strip(41,11);
    Explore_Directories_strip(41,13);
    Explore_Directories_strip(41,15);
    Explore_Directories_strip(41,17);
    Explore_Directories_strip(41,19);
    Explore_Directories_strip(41,21);
    Explore_Directories_strip(41,23);
    Explore_Directories_strip(41,25);
    Explore_Directories_strip(41,27);
    Explore_Directories_strip(41,29);
    Explore_Directories_strip(41,31);
    Explore_Directories_strip(41,33);
    Explore_Directories_strip(41,35);
    Explore_Directories_strip(41,37);

    explorer_file_type_updater_strip(98,7);
    explorer_file_type_updater_strip(98,9);
    explorer_file_type_updater_strip(98,11);
    explorer_file_type_updater_strip(98,13);
    explorer_file_type_updater_strip(98,15);
    explorer_file_type_updater_strip(98,17);
    explorer_file_type_updater_strip(98,19);
    explorer_file_type_updater_strip(98,21);
    explorer_file_type_updater_strip(98,23);
    explorer_file_type_updater_strip(98,25);
    explorer_file_type_updater_strip(98,27);
    explorer_file_type_updater_strip(98,29);
    explorer_file_type_updater_strip(98,31);
    explorer_file_type_updater_strip(98,33);
    explorer_file_type_updater_strip(98,35);
    explorer_file_type_updater_strip(98,37);
}
void Explore_Directories_strip(int x,int y)
{
    gotoxy(x,y-1);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220);
    gotoxy(x,y);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(x,y+1);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223);
}
void explorer_file_type_updater_strip(int x,int y)
{
    gotoxy(x,y-1);
    printf("%c%c%c%c%c%c%c%c%c%c%c",220,220,220,220,220,220,220,220,220,220,220);
    gotoxy(x,y);
    printf("%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(x,y+1);
    printf("%c%c%c%c%c%c%c%c%c%c%c",223,223,223,223,223,223,223,223,223,223,223);
}
void drive_strip_updater(int x,int y)
{
    gotoxy(x,y);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
}
void explorer_drive_strip(int x,int y)
{
    gotoxy(x,y);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(x,y+1);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(x,y+2);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(x,y+3);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(x,y+4);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(x,y+5);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(x,y+6);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);

}
void explorer_drive_selector(int x,int y,int color,int point_bkcolor,int point_color)
{
    Bk_color(117,color);
    gotoxy(x,y);
    printf("%c%c%c",219,219,219);
    gotoxy(x,y+1);
    printf("%c%c%c",219,219,219);
    gotoxy(x,y+2);
    printf("%c%c%c",219,219,219);
    gotoxy(x,y+3);
    printf("%c%c%c",219,219,219);
    gotoxy(x,y+4);
    printf("%c%c%c",219,219,219);
    gotoxy(x,y+5);
    printf("%c%c%c",219,219,219);
    gotoxy(x,y+6);
    printf("%c%c%c",219,219,219);
    Bk_color(point_bkcolor,point_color);
    gotoxy(x+1,y+3);
    printf("%c",16);
}
void main_frame(int color_of_time_strip,char *massage)
{
    Hide_cursor();
    buffer_size(116,45);
    adjustWindowSize(0,0,111,42);
    Bk_color(28,1);
    back_freame(0,111,0,2);
    Bk_color(28,15);
    MSGinM(massage,1);
    Bk_color(180,17);
    drowlineH(0,110,42,220);
    Bk_color(117,17);
    drowlineV(3,42,0,219);
    drowlineV(3,42,111,219);
    ind_back_freame(1,110,40,42,28,color_of_time_strip,1);
    buffer_size(112,43);
    adjustWindowSize(0,0,112,43);
}
void main_menu_background()
{
    gotoxy(2,6);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(2,7);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(2,8);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(2,9);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(2,10);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(2,11);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(2,12);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(2,13);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(2,14);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(2,15);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(2,16);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(2,17);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(2,18);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(2,19);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(2,20);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(2,21);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(2,22);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(2,23);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(2,24);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(2,25);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(2,26);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(2,27);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(2,28);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(2,29);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(2,30);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(2,31);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(2,32);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(2,33);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(2,34);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(2,35);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(2,36);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(2,37);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(2,38);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(2,39);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);

}
void ind_back_freame(int x,int x1,int y, int y1,int bottomLineshadow_color,int topLineShadow_color,int line_color)
{
    int i;
    for(i=x;i<=x1;i++)
    {
        Bk_color(topLineShadow_color,line_color);
        gotoxy(i,y);
        printf("%c",220);
        gotoxy(i,y+1);
        printf("%c",219);
        Bk_color(bottomLineshadow_color,line_color);
        gotoxy(i,y1);
        printf("%c",223);
    }
}
void Update_status_bar(int x,int y)
{
    Bk_color(28,1);
    gotoxy(x,y);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
}
void working_area_background()
{
    gotoxy(40,6);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,7);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,8);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,9);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,10);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,11);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,12);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,13);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,14);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,15);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,16);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,17);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,18);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,19);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,20);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,21);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,22);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,23);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,24);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,25);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,26);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,27);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,28);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,29);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,30);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,31);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,32);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,33);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,34);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,35);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,36);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,37);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,38);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,39);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
}
void Intruction_background()
{
    gotoxy(40,6);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,7);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,8);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,9);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,10);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,11);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,12);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,13);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,14);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,15);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,16);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,17);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,18);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,19);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,20);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,21);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,22);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,23);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,24);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,25);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,26);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,27);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,28);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,29);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,30);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,31);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,32);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,33);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,34);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,35);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,36);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,37);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,38);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(40,39);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
}
void scroll_bar_strip_of_intruction(register int x,register int y)
{
    gotoxy(x,y);
    printf("%c",219);
    gotoxy(x,y+1);
    printf("%c",219);
    gotoxy(x,y+2);
    printf("%c",219);
    gotoxy(x,y+3);
    printf("%c",219);
    gotoxy(x,y+4);
    printf("%c",219);
    gotoxy(x,y+5);
    printf("%c",219);
    gotoxy(x,y+6);
    printf("%c",219);
    gotoxy(x,y+7);
    printf("%c",219);
    gotoxy(x,y+8);
    printf("%c",219);
    gotoxy(x,y+9);
    printf("%c",219);
    gotoxy(x,y+10);
    printf("%c",219);
    gotoxy(x,y+11);
    printf("%c",219);
    gotoxy(x,y+12);
    printf("%c",219);
    gotoxy(x,y+13);
    printf("%c",219);
    gotoxy(x,y+14);
    printf("%c",219);
    gotoxy(x,y+15);
    printf("%c",219);
    gotoxy(x,y+16);
    printf("%c",219);
    gotoxy(x,y+17);
    printf("%c",219);
    gotoxy(x,y+18);
    printf("%c",219);
    gotoxy(x,y+19);
    printf("%c",219);
    gotoxy(x,y+20);
    printf("%c",219);
    gotoxy(x,y+21);
    printf("%c",219);
    gotoxy(x,y+22);
    printf("%c",219);
    gotoxy(x,y+23);
    printf("%c",219);
    gotoxy(x,y+24);
    printf("%c",219);
    gotoxy(x,y+25);
    printf("%c",219);
    gotoxy(x,y+26);
    printf("%c",219);
    gotoxy(x,y+27);
    printf("%c",219);
    gotoxy(x,y+28);
    printf("%c",219);
    gotoxy(x,y+29);
    printf("%c",219);
}
void back_freame(int x,int x1,int y, int y1)
{
    int i;
    for(i=x;i<=x1;i++)
    {
        gotoxy(i,y);
        printf("%c",220);
        gotoxy(i,y+1);
        printf("%c",219);
        gotoxy(i,y1);
        printf("%c",223);
    }
}
void menu_option(int x,int x1,int y,int y1)
{
    gotoxy(x,y);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220);
    gotoxy(x,y+1);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(x,y1);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223,223);
}
int detect_all_drives(char *drives,int *sys_drive_status)
{
    int i=1,sys_drive=0;
    FILE *fp;
    char temp,drive[]="a:\\Brightgoal_dr_detection_404.txt",sys_drv;
    temp='#';
    detect_windows_drive(&temp);
    drives[0]=temp;
    sys_drv=temp;
    temp='a';
    if(drives[0]!='#' && ((drives[0]>='a' && drives[0]<='z') || (drives[0]>='A' && drives[0]<='Z')))
    {
        sys_drive=1;
        *sys_drive_status=1;
        i=1;
    }
    else
    {
        sys_drive=0;
        *sys_drive_status=0;
        load_null_in_array(drives,50);
        i=0;
    }
    while(temp<='z')
    {
        if(temp!=sys_drv)
        {
            drive[0]=temp;
            fp=fopen(drive,"wb");
            if(fp!=NULL)
            {
                drives[i]=temp;
                fclose(fp);
                remove(drive);
                i++;
            }
        }
        temp++;
    }
    drives[i]='\0';
    if(i==1 && drives[0]=='#')
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
void Exit_Now()
{
    if(open_dir!=NULL)
    {
        delete_open_dir_list(&open_dir);
        open_dir=NULL;
    }
    if(current_path!=NULL)
    {
        delete_list_of_path(&current_path);
        current_path=NULL;
    }
    Key_Stoke=Close_Music_Player;
    exit(0);
}
int about()
{
    char ch;
    Bk_color(180,1);
    working_area_background();
    ind_back_freame(40,109,3,5,28,180,12);
    Bk_color(197,15);
    rec_MSGinM("About",40,109,4);
    Bk_color(28,15);
    make_frame(42,108,8,21);
    gotoxy(44,8);
    printf(" Music Player  v1.0.0   ");
    make_frame(43,43+25,7,9);
    gotoxy(43,8);
    printf("%c",180);
    gotoxy(43+25,8);
    printf("%c",195);
    gotoxy(46,11);
    printf("Author Name : Harvindar Singh");
    gotoxy(46,13);
    printf("Contact No : 9140417112");
    gotoxy(46,15);
    printf("Home Page : www.brightgoal.in");
    gotoxy(46,17);
    printf("Operating System : Windows xp, 7, 8, 8.1, 10 (64 or 32 Bit)");
    gotoxy(46,19);
    printf("Build Time : 01:20:33 AM - 24/04/2019");
    gotoxy(46,23);
    printf("This Software is Build in C language by Harvindar Singh.");
    gotoxy(46,25);
    printf("This Software is Designed to play Music.");
    Bk_color(28,15);
    Clear_buffer();
    while(TRUE)
    {
        ch=getch();
        if(ch==27)
            return 1;
    }

}
int Intruction()
{
    char Intructions[10][75]={"\4 You Can change Music Using 'Left' or 'Right' Key.",
                              "\4 You Can 'Play' or 'Pause' Music Using 'P' key.",
                              "\4 Use '+' Under File Explorer to Add Msuic In Music Library.",
                              "\4 Use 'Delete' Key Under Music library to remove Music File",
                              "  From Library.",
                              "\4 Use 'S' to Stop Playing Music",
                              "\4 If you want to add all music of a folder in library",
                              "  Press '+' button on the Folder."};
    char ch;
    int point;
    point = 0;
    ind_back_freame(40,109,3,5,28,180,12);
    Bk_color(197,15);
    rec_MSGinM("RULES & INSTRUCTIONS",40,109,4);
    Bk_color(180,1);
    working_area_background();
    Bk_color(28,15);
    gotoxy(41,7);
    printf("%s",Intructions[point]);
    gotoxy(41,9);
    printf("%s",Intructions[point+1]);
    gotoxy(41,11);
    printf("%s",Intructions[point+2]);
    gotoxy(41,13);
    printf("%s",Intructions[point+3]);
    gotoxy(41,15);
    printf("%s",Intructions[point+4]);
    gotoxy(41,17);
    printf("%s",Intructions[point+5]);
    gotoxy(41,19);
    printf("%s",Intructions[point+6]);
    gotoxy(41,21);
    printf("%s",Intructions[point+7]);
    while(TRUE)
    {
        ch = getch();
        if(ch==27)
            return 1;
    }
}
void loder_box(int x,int y)
{
    gotoxy(x,y);
    printf("%c%c%c%c%c%c",219,219,219,219,219,219);
    gotoxy(x,y+1);
    printf("%c%c%c%c%c%c",219,219,219,219,219,219);
    gotoxy(x,y+2);
    printf("%c%c%c%c%c%c",219,219,219,219,219,219);
    gotoxy(x,y+3);
    printf("%c%c%c%c%c%c",219,219,219,219,219,219);

}
void window(int x,int x1,int y,int y1)
{
    int i,j;
    for(j=y;j<=y1;j++)
    {
        for(i=x;i<=x1;i++)
        {
            gotoxy(i,j);
            printf("%c",219);
        }
    }
}
void b_alpha(int x,int y)
{
    gotoxy(x,y);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c",223,219,219,219,219,219,219,219,219,219,219,220);
    gotoxy(x,y+1);
    printf("  %c%c%c     %c%c%c",219,219,219,219,219,219);
    gotoxy(x,y+2);
    printf("  %c%c%c     %c%c%c",219,219,219,219,219,219);
    gotoxy(x,y+3);
    printf("  %c%c%c    %c%c%c",219,219,219,220,219,223);
    gotoxy(x,y+4);
    printf("%c%c%c%c%c%c%c%c%c%c%c",220,219,219,219,219,219,219,219,219,219,219);
    gotoxy(x,y+5);
    printf("  %c%c%c    %c%c%c",219,219,219,223,219,220);
    gotoxy(x,y+6);
    printf("  %c%c%c     %c%c%c",219,219,219,219,219,219);
    gotoxy(x,y+7);
    printf("  %c%c%c     %c%c%c",219,219,219,219,219,219);
    gotoxy(x,y+8);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c",220,219,219,219,219,219,219,219,219,219,219,223);
    gotoxy(x,y+9);
}
void welcome_screen()
{
    int x,x1,y,y1,i;
    //adjustWindowSize(0, 0, 110, 41);
    //buffer_size(300, 300);
    buffer_size(112,43);
    adjustWindowSize(0,0,112,43);
    system("COLOR BC");
    Bk_color(28,1);
    window(0,111,6,37);
    Bk_color(28,15);
    welcome_box(40,8);
    welcome_box(64,8);
    welcome_box(40,28);
    welcome_box(64,28);
    x=40;
    x1=64;
    y=8;
    y1=28;
    Hide_cursor();
    for(i=1;i<=5;i++)
    {
        Bk_color(28,1);
        welcome_box(x,y);
        welcome_box(x1,y);
        welcome_box(x,y1);
        welcome_box(x1,y1);
        x++;
        x1--;
        y++;
        y1--;
        Bk_color(28,15);
        welcome_box(x,y);
        welcome_box(x1,y);
        welcome_box(x,y1);
        welcome_box(x1,y1);
        delay(40);
    }
    delay(500);
    x=45;
    y=13;
    Bk_color(28,15);
    welcome_box(x+14,y);
    welcome_box(x,y);
    welcome_box(x+14,y+10);
    welcome_box(x,y+10);
    while(x!=2)
    {
        Bk_color(28,1);
        welcome_box(x+14,y);
        welcome_box(x,y);
        welcome_box(x+14,y+10);
        welcome_box(x,y+10);
        x--;
        Bk_color(28,15);
        welcome_box(x+14,y);
        welcome_box(x,y);
        welcome_box(x+14,y+10);
        welcome_box(x,y+10);
        delay(40);
    }
    Bk_color(28,15);
    gotoxy(32,14);
    printf("888888b.           d8b          888      888                               888");
    gotoxy(32,15);
    printf("888  %c88b          Y8P          888      888                               888",34);
    gotoxy(32,16);
    printf("888  .88P                       888      888                               888 ");
    gotoxy(32,17);
    printf("8888888K.  888d888 888  .d88b.  88888b.  888888  .d88b.   .d88b.   8888b.  888 ");
    gotoxy(32,18);
    printf("888  %cY88b 888P%c   888 d88P%c88b 888 %c88b 888    d88P%c88b d88%c%c88b     %c88b 888 ",34,34,34,34,34,34,34,34);
    gotoxy(32,19);
    printf("888    888 888     888 888  888 888  888 888    888  888 888  888 .d888888 888 ");
    gotoxy(32,20);
    printf("888   d88P 888     888 Y88b 888 888  888 Y88b.  Y88b 888 Y88..88P 888  888 888 ");
    gotoxy(32,21);
    printf("8888888P%c  888     888  %cY88888 888  888  %cY888  %cY88888  %cY88P%c  %cY888888 888 ",34,34,34,34,34,34,34);
    gotoxy(32,22);
    printf("                            888                      888                       ");
    gotoxy(32,23);
    printf("                       Y8b d88P                 Y8b d88P                       ");
    gotoxy(32,24);
    printf("                       %cY88P%c                   %cY88P%c                        ",34,34,34,34);
    gotoxy(97,24);
    printf("BRIGHTGOAL.IN");
    gotoxy(87,26);
    //printf("For Windows,Version 1.0");
    gotoxy(92,26);
    printf("BY HARVINDAR SINGH");
    gotoxy(80,30);
    //printf("www.Brightgol.in (2016 - 2019)");
    delay(100);
    Bk_color(28,1);
    welcome_box(x+14,y);
    welcome_box(x,y);
    welcome_box(x+14,y+10);
    welcome_box(x,y+10);
    delay(60);
    Bk_color(255,15);
    welcome_box(x+13,y);
    welcome_box(x,y);
    welcome_box(x+13,y+9);
    welcome_box(x,y+9);
    Bk_color(255,1);
    b_alpha(9,17);
    make_frame(2,27,13,30);
    delay(3000);
}
void welcome_box(int x,int y)
{
    gotoxy(x,y);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(x,y+1);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(x,y+2);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(x,y+3);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(x,y+4);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(x,y+5);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(x,y+6);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(x,y+7);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219);
    gotoxy(x,y+8);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c",219,219,219,219,219,219,219,219,219,219,219,219,219);
}
void main()
{
    pthread_t BackEnd_Music_Player;
    pthread_create(&BackEnd_Music_Player,NULL,&Music_Player_Back_End,NULL);
    welcome_screen();
    main_menu();
}
