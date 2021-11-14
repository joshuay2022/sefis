#include "StdAfx.h"

#include "Print_Show.h"

#include <conio.h>

void Print(const char *s)
{
  fputs(s, stdout);
}

void Print(const AString &s)
{
  Print(s.Ptr());
}

void Print(const UString &s)
{
  AString as;
  Convert_UString_to_AString(s, as);
  Print(as);
}

void Print(const wchar_t *s)
{
  Print(UString(s));
}

void PrintNewLine()
{
  Print("\n");
}

void PrintStringLn(const char *s)
{
  Print(s);
  PrintNewLine();
}

void PrintError(const char *message)
{
  Print("Error: ");
  PrintNewLine();
  Print(message);
  PrintNewLine();
}

void PrintError(const char *message, const FString &name)
{
  PrintError(message);
  Print(name);
}

int get_password(AString &usr_password)
{
    char ch;
    int i=0;
    char password[PASSWORD_MAX_LEN];
    
    while (((ch = (char)_getch()) != '\r')&&i<PASSWORD_MAX_LEN-1)
    {
        if (ch != 8)
        {
            password[i] = ch;
            putchar('*');
            i++;
        }
        else if(i>0)
        {
            putchar('\b');
            putchar(' ');
            putchar('\b');
            i--;
        }
        else {
            //putchar('\b');
        }
    }

    if(i>=PASSWORD_MAX_LEN) return 1;

    password[i] = '\0';
    printf("\n");

    usr_password=AString(password);

    return 0;
}

void get_string(AString &astr)
{
  char ch;
  int i=0;
  char char_str[PASSWORD_MAX_LEN];

  while(((ch=(char)getchar())!='\n')&&i<PASSWORD_MAX_LEN-1){
    char_str[i]=ch;
    ++i;
  }
  char_str[i]='\0';
  for(i=0;i<(int)strlen(char_str);++i){
    if(char_str[i]!=' ') break;
  }

  if(i==(int)strlen(char_str)){
    astr=AString("");
    return;
  }
  else{
    astr=AString(char_str+i);
    return;
  }
}

void show_progress(const UInt64 &_cur,const UInt64 &_total)
{
    static unsigned long p = 0;
    static const unsigned char w[] = "\\/-";
    double val;

    val = ((double)_cur * 100) / (double)_total;

    if (++p == 3)  p = 0;

    printf("\r Progress Bar: %.2f%c%c", val, '%', w[p]);
}

void show_progress(const UInt64 &_cur,const UInt64 &_total,const AString &str)
{
    double val=((double)_cur*100)/(double)_total;
    size_t block_num;

    //SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);  
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE);

    if(_cur<_total){
        printf("\r  %s[%.2lf%%]:",str.Ptr(),val);
        block_num=(size_t)(MAX_BLOCK_NUM*_cur/_total);
    }
    else{
        printf("\r  %s have been done[%.2lf%%]:",str.Ptr(),100.0);
        block_num=(size_t)MAX_BLOCK_NUM;
    }

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
    for (size_t j = 0; j < block_num; j++) printf("\001 ");
    
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    //Sleep(10);
}

void show_progress_file(const UString &file_name,const AString &str)
{
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE);

  printf("\r  %s",empty_buffer);
  printf("\r  %sfile: ",str.Ptr());

  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

  Print(file_name);
}

void show_progress_file_over(const AString &str)
{
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE);

  printf("\r  %s",empty_buffer);
  printf("\r  %sfile ",str.Ptr());

  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

  PrintStringLn("ends");
}

void show_progress_count(const UInt64 &_count,const AString &str)
{
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE);

  printf("\r  %sfile's order: ",str.Ptr());

  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

  printf("%llu",_count);
}

void show_progress_count_over(const AString &str)
{
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE);

  printf("\r  %s files ",str.Ptr());

  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

  PrintStringLn("ends                         ");
}