/*
* LZMA SDK is placed in the public domain.
*
* Anyone is free to copy, modify, publish, use, compile, sell, or distribute the original 
* LZMA SDK code, either in source code form or as a compiled binary, for any purpose, commercial 
* or non-commercial, and by any means.
*/

#include "StdAfx.h"

#include <stdio.h>

#include "../../7zip/CPP/Common/MyWindows.h"

#include "../../7zip/CPP/Common/Defs.h"
#include "../../7zip/CPP/Common/MyInitGuid.h"

#include "../../7zip/CPP/Common/IntToString.h"
#include "../../7zip/CPP/Common/StringToInt.h"
#include "../../7zip/CPP/Common/StringConvert.h"

#include "../../7zip/CPP/Windows/DLL.h"
#include "../../7zip/CPP/Windows/FileDir.h"
#include "../../7zip/CPP/Windows/FileFind.h"
#include "../../7zip/CPP/Windows/FileName.h"
#include "../../7zip/CPP/Windows/NtCheck.h"
#include "../../7zip/CPP/Windows/PropVariant.h"
#include "../../7zip/CPP/Windows/PropVariantConv.h"

#include "../../7zip/CPP/7zip/Common/FileStreams.h"

#include "../../7zip/CPP/7zip/Archive/IArchive.h"

#include "../../7zip/CPP/7zip/IPassword.h"
//#include "../7zip/C/7zVersion.h"

#include "../String/ConvertString.h"

#include "../Print/Print_Show.h"

// Tou can find the list of all GUIDs in Guid.txt file.
// use another CLSIDs, if you want to support other formats (zip, rar, ...).
// {23170F69-40C1-278A-1000-000110070000}

/*
#ifdef _WIN32
HINSTANCE g_hInstance = 0;
#endif
*/

DEFINE_GUID(CLSID_CFormat7z,
  0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x07, 0x00, 0x00);
DEFINE_GUID(CLSID_CFormatXz,
  0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x0C, 0x00, 0x00);

#define CLSID_Format CLSID_CFormat7z
// #define CLSID_Format CLSID_CFormatXz

using namespace NWindows;
using namespace NFile;
using namespace NDir;

#define kDllName "sefis_7z.dll"

static const char * const kHelpString =
"Usage: sefis.exe [ e | d ]  [-j thread_number] input_file [output_file...]\n"
"Examples:\n"
"  sefis.exe e filename.file enc_file.jy        : encrypt filename.file into enc_file.jy\n"
"  sefis.exe e -j 12 filename.file enc_file.jy  : encrypt filename.file into enc_file.jy using 12 threads\n"
"  sefis.exe d filename.jy dec_file.file        : Decrypt filename.jy into dec_file.file\n"
"  sefis.exe d -j 12 filename.jy                : Decrypt filename.jy into a file with original name using 12 threads\n";

static const char * const kInfoString =
"  sefis 1.0.0 2021-11-12\n"
"  Copyright (c) 2021 Joshua Yao\n";

static HRESULT IsArchiveItemProp(IInArchive *archive, UInt32 index, PROPID propID, bool &result);

static HRESULT IsArchiveItemFolder(IInArchive *archive, UInt32 index, bool &result);

static const wchar_t * const kEmptyFileAlias = L"[Content]";


//////////////////////////////////////////////////////////////
// Archive Open callback class


class CArchiveOpenCallback:
  public IArchiveOpenCallback,
  public ICryptoGetTextPassword,
  public CMyUnknownImp
{
public:
  MY_UNKNOWN_IMP1(ICryptoGetTextPassword)

  STDMETHOD(SetTotal)(const UInt64 *files, const UInt64 *bytes);
  STDMETHOD(SetCompleted)(const UInt64 *files, const UInt64 *bytes);

  STDMETHOD(CryptoGetTextPassword)(BSTR *password);

  bool PasswordIsDefined;
  UString Password;

  CArchiveOpenCallback() : PasswordIsDefined(false) {}
};



/*  
STDMETHODIMP CArchiveOpenCallback::CryptoGetTextPassword(BSTR *password)
{
  if (!PasswordIsDefined)
  {
    // You can ask real password here from user
    // Password = GetPassword(OutStream);
    // PasswordIsDefined = true;
    PrintError("Password is not defined");
    return E_ABORT;
  }
  return StringToBstr(Password, password);
}
*/



static const char * const kIncorrectCommand = "incorrect command";

//////////////////////////////////////////////////////////////
// Archive Extracting callback class

static const char * const kTestingString    =  "Testing     ";
static const char * const kExtractingString =  "Extracting  ";
static const char * const kSkippingString   =  "Skipping    ";

static const char * const kUnsupportedMethod = "Unsupported Method";
static const char * const kCRCFailed = "CRC Failed";
static const char * const kDataError = "Data Error";
static const char * const kUnavailableData = "Unavailable data";
static const char * const kUnexpectedEnd = "Unexpected end of data";
static const char * const kDataAfterEnd = "There are some data after the end of the payload data";
static const char * const kIsNotArc = "Is not archive";
static const char * const kHeadersError = "Headers Error";


class CArchiveExtractCallback:
  public IArchiveExtractCallback,
  public ICryptoGetTextPassword,
  public CMyUnknownImp
{
public:
  MY_UNKNOWN_IMP1(ICryptoGetTextPassword)

  // IProgress
  STDMETHOD(SetTotal)(UInt64 size);
  STDMETHOD(SetCompleted)(const UInt64 *completeValue);

  // IArchiveExtractCallback
  STDMETHOD(GetStream)(UInt32 index, ISequentialOutStream **outStream, Int32 askExtractMode);
  STDMETHOD(PrepareOperation)(Int32 askExtractMode);
  STDMETHOD(SetOperationResult)(Int32 resultEOperationResult);

  // ICryptoGetTextPassword
  STDMETHOD(CryptoGetTextPassword)(BSTR *aPassword);

private:
  CMyComPtr<IInArchive> _archiveHandler;
  FString _directoryPath;  // Output directory
  UString _filePath;       // name inside arcvhive
  FString _diskFilePath;   // full path to file on disk
  bool _extractMode;
  struct CProcessedFileInfo
  {
    FILETIME MTime;
    UInt32 Attrib;
    bool isDir;
    bool AttribDefined;
    bool MTimeDefined;
  } _processedFileInfo;

  COutFileStream *_outFileStreamSpec;
  CMyComPtr<ISequentialOutStream> _outFileStream;

public:
  void Init(IInArchive *archiveHandler, const FString &directoryPath);

  UInt64 NumErrors;
  bool PasswordIsDefined;
  UString Password;

  CArchiveExtractCallback() : PasswordIsDefined(false) {}
};



/*
STDMETHODIMP CArchiveExtractCallback::CryptoGetTextPassword(BSTR *password)
{
  if (!PasswordIsDefined)
  {
    // You can ask real password here from user
    // Password = GetPassword(OutStream);
    // PasswordIsDefined = true;
    PrintError("Password is not defined");
    return E_ABORT;
  }
  return StringToBstr(Password, password);
}
*/



//////////////////////////////////////////////////////////////
// Archive Creating callback class

struct CDirItem
{
  UInt64 Size;
  FILETIME CTime;
  FILETIME ATime;
  FILETIME MTime;
  UString Name;
  FString FullPath;
  UInt32 Attrib;

  bool isDir() const { return (Attrib & FILE_ATTRIBUTE_DIRECTORY) != 0 ; }
};

class CArchiveUpdateCallback:
  public IArchiveUpdateCallback2,
  public ICryptoGetTextPassword2,
  public CMyUnknownImp
{
public:
  MY_UNKNOWN_IMP2(IArchiveUpdateCallback2, ICryptoGetTextPassword2)

  // IProgress
  STDMETHOD(SetTotal)(UInt64 size);
  STDMETHOD(SetCompleted)(const UInt64 *completeValue);

  // IUpdateCallback2
  STDMETHOD(GetUpdateItemInfo)(UInt32 index,
      Int32 *newData, Int32 *newProperties, UInt32 *indexInArchive);
  STDMETHOD(GetProperty)(UInt32 index, PROPID propID, PROPVARIANT *value);
  STDMETHOD(GetStream)(UInt32 index, ISequentialInStream **inStream);
  STDMETHOD(SetOperationResult)(Int32 operationResult);
  STDMETHOD(GetVolumeSize)(UInt32 index, UInt64 *size);
  STDMETHOD(GetVolumeStream)(UInt32 index, ISequentialOutStream **volumeStream);

  STDMETHOD(CryptoGetTextPassword2)(Int32 *passwordIsDefined, BSTR *password);

public:
  CRecordVector<UInt64> VolumesSizes;
  UString VolName;
  UString VolExt;

  FString DirPrefix;
  const CObjectVector<CDirItem> *DirItems;

  bool PasswordIsDefined;
  UString Password;
  bool AskPassword;

  bool m_NeedBeClosed;

  FStringVector FailedFiles;
  CRecordVector<HRESULT> FailedCodes;

  CArchiveUpdateCallback(): PasswordIsDefined(false), AskPassword(false), DirItems(0) {};

  ~CArchiveUpdateCallback() { Finilize(); }
  HRESULT Finilize();

  void Init(const CObjectVector<CDirItem> *dirItems);
};

static void GetStream2(const wchar_t *name);


/*
STDMETHODIMP CArchiveUpdateCallback::CryptoGetTextPassword2(Int32 *passwordIsDefined, BSTR *password)
{
  if (!PasswordIsDefined)
  {
    if (AskPassword)
    {
      // You can ask real password here from user
      // Password = GetPassword(OutStream);
      // PasswordIsDefined = true;
      PrintError("Password is not defined");
      return E_ABORT;
    }
  }
  *passwordIsDefined = BoolToInt(PasswordIsDefined);
  return StringToBstr(Password, password);
}
*/

#define NT_CHECK_FAIL_ACTION PrintError("Unsupported Windows version"); return 1;

int add_dir_items(const FString &fs,CObjectVector<CDirItem> &dirItems);