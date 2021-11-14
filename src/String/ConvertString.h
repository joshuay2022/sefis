#ifndef _CONVERTSTRING_H_
#define _CONVERTSTRING_H_


#include "../../7zip/CPP/Common/StringConvert.h"

void Convert_UString_to_AString(const UString &s, AString &temp);

void Convert_AString_to_UString(const AString &s, UString &temp);

FString CmdStringToFString(const char *s);

#endif