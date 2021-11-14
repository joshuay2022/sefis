#include "StdAfx.h"

#include "ConvertString.h"

void Convert_UString_to_AString(const UString &s, AString &temp)
{
    int codePage = CP_OEMCP;
    /*
    int g_CodePage = -1;
    int codePage = g_CodePage;
    if (codePage == -1)
    codePage = CP_OEMCP;
    if (codePage == CP_UTF8)
    ConvertUnicodeToUTF8(s, temp);
    else
    */
    UnicodeStringToMultiByte2(temp, s, (UINT)codePage);
}

void Convert_AString_to_UString(const AString &s, UString &temp)
{
    MultiByteToUnicodeString2(temp,s);
}

FString CmdStringToFString(const char *s)
{
    return us2fs(GetUnicodeString(s));
}