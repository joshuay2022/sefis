#ifndef _PRINT_SHOW_H_
#define _PRINT_SHOW_H_

#include <stdio.h>

#include "../String/ConvertString.h"

static const int PASSWORD_MAX_LEN=73;
static const UInt64 MAX_BLOCK_NUM=35;

static const char * empty_buffer="                                                                                              ";

void Print(const char *s);
void Print(const AString &s);
void Print(const UString &s);
void Print(const wchar_t *s);
void PrintNewLine();
void PrintStringLn(const char *s);
void PrintError(const char *message);
void PrintError(const char *message, const FString &name);

int get_password(AString &usr_password);
void get_string(AString &astr);

void show_progress(const UInt64 &_cur,const UInt64 &_total);
void show_progress(const UInt64 &_cur,const UInt64 &_total,const AString &str);

void show_progress_file(const UString &file_name,const AString &str);
void show_progress_file_over(const AString &str);

void show_progress_count(const UInt64 &_count,const AString &str);
void show_progress_count_over(const AString &str);

#endif