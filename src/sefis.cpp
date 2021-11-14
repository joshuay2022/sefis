/*
*
*/

#include "StdAfx.h"

#include "./Compress/com7z.h"
#include "./Crypto/crypto.h"

#ifdef _WIN32
HINSTANCE g_hInstance = 0;
#endif

bool isNum(const FString &num_str)
{
  if(num_str.Len()==0) return false;

  size_t i=0;
  if(num_str[0]=='+'||num_str[0]=='-'){
    ++i;
    if(num_str.Len()==1) return false;
  }
  for(;i<num_str.Len();++i){
    if(num_str[i]<'0'||num_str[i]>'9') return false;
  }

  return true;
}

bool isFilePath(const FString &path)
{
  AString astr;
  Convert_UString_to_AString(path,astr);

  for(size_t i=0;i<astr.Len();++i){
    if(astr[i]=='/') return false;
  }

  return true;
}

int MY_CDECL main(int numArgs, const char *args[])
{
  NT_CHECK

  //PrintStringLn(kCopyrightString);

  if (numArgs < 2)
  {
    PrintStringLn(kHelpString);
    return 0;
  }

  if (numArgs == 2)
  {
    FString option_info=CmdStringToFString(args[1]);

    if(option_info==L"-V"||option_info==L"-v"||option_info==L"--v"||option_info==L"--version"){
      PrintStringLn(kInfoString);
      return 0;
    }
    else {
      PrintError(kIncorrectCommand);
      return 1;
    }
  }

  if(numArgs>7){
    PrintError(kIncorrectCommand);
    return 1;
  }


  FString option_ed=CmdStringToFString(args[1]);
  int option_ED=-1;

  if(option_ed==L"e"||option_ed==L"E") option_ED=0;
  else if(option_ed==L"d"||option_ed==L"D") option_ED=1;
  else{
    PrintError(kIncorrectCommand);
    return 1;
  }

  bool IsUseDefaultThr=true;
  int thread_number=12;
  
  FString option_thr=CmdStringToFString(args[2]);
  int args_id=3;
  if(option_thr==L"-J"||option_thr==L"-j"){
    IsUseDefaultThr=false;

    if(numArgs<5){
      PrintError(kIncorrectCommand);
      return 1;
    }

    FString thr_num_str=CmdStringToFString(args[args_id]);
    if(!isNum(thr_num_str)){
      PrintError("Don't figure out the thread number");
      return 1;
    }

    const wchar_t **_end=nullptr;
    thread_number=ConvertStringToInt32(thr_num_str.Ptr(),_end);
    if(thread_number<=0||thread_number>40){
      PrintError("Incorrect thread number (must be positive and no more than 40");
      return 1;
    }

    ++args_id;
    option_thr=CmdStringToFString(args[args_id]);
    ++args_id;
  }

  bool IsDeleteOri=false;
  ///
  /*
  Print(option_thr);
  PrintNewLine();
  */
  if(option_thr==L"-d"||option_thr==L"-D"){
    IsDeleteOri=true;
    if(args_id>=numArgs){
      PrintError(kIncorrectCommand);
      return 1;
    }
    option_thr=CmdStringToFString(args[args_id]);
    ++args_id;
  }

  if(option_thr==L"-J"||option_thr==L"-j"){
    IsUseDefaultThr=false;

    if(numArgs<6){
      PrintError(kIncorrectCommand);
      return 1;
    }

    FString thr_num_str=CmdStringToFString(args[args_id]);
    if(!isNum(thr_num_str)){
      PrintError("Don't figure out the thread number");
      return 1;
    }

    const wchar_t **_end=nullptr;
    thread_number=ConvertStringToInt32(thr_num_str.Ptr(),_end);
    if(thread_number<=0||thread_number>40){
      PrintError("Incorrect thread number (must be positive and no more than 40");
      return 1;
    }

    ++args_id;
    option_thr=CmdStringToFString(args[args_id]);
    ++args_id;
  }

  UString input_path=option_thr;
  if(!isFilePath(input_path)){
    PrintError("Incorrect file path");
    return 1;
  }
  /*
  Print(input_path);
  PrintNewLine();
  */

  if(input_path[input_path.Len()-1]=='\\') input_path.DeleteBack();

  NFind::CFileInfo cfi;
  if(!cfi.Find(input_path.Ptr())){
    PrintError("Can't find the input file or wrong file path");
    return 1;
  }

  bool IsNoOutputName=false;
  UString output_path=L"";
  if(numArgs==args_id){
    if(option_ED==1) IsNoOutputName=true;
    else{
      PrintError("An output file path needed");
      return 1;
    }
  }
  else if(numArgs!=args_id+1){
    PrintError(kIncorrectCommand);
    return 1;
  }
  else{
    output_path=CmdStringToFString(args[args_id]);
    /*
    if(!cfi.Find(output_path.Ptr())){
      PrintError("Can't find the output file or wrong file path");
      return 1;
    }
    */
    if(!isFilePath(output_path)){
      PrintError("Incorrect file path");
      return 1;
    }
  }
  



  /***********************************/
  //Compress or decompress
  NDLL::CLibrary lib;
  if (!lib.Load(NDLL::GetModuleDirPrefix() + FTEXT(kDllName)))
  {
    PrintError("Can not load sefis 7-zip library");
    return 1;
  }

  Func_CreateObject createObjectFunc = (Func_CreateObject)lib.GetProc("CreateObject");
  if (!createObjectFunc)
  {
    PrintError("Can not get CreateObject");
    return 1;
  }

  UString mid_file_name;
  if(IsNoOutputName) GetOnlyDirPrefix(input_path,mid_file_name);
  else GetOnlyDirPrefix(output_path,mid_file_name);
  mid_file_name.DeleteBack();mid_file_name+=L"\\";
  mid_file_name+=L"mid_file";

  if(option_ED==0){
    /*************************************/
    //Compress
    NFind::CFileInfo ncfi;
    while(ncfi.Find(mid_file_name+L".7z")){
      mid_file_name+=L"_ly";
    }
    mid_file_name+=L".7z";
    FString archiveName = mid_file_name;

    CObjectVector<CDirItem> dirItems;
    CDirItem di;

    FString name = input_path;
    if(name[name.Len()-1]=='\\') name.DeleteBack();
        
    NFind::CFileInfo fi;
    fi.Find(name);
    di.Attrib = fi.Attrib;
    di.Size = fi.Size;
    di.CTime = fi.CTime;
    di.ATime = fi.ATime;
    di.MTime = fi.MTime;
    di.Name = fs2us(name);
    di.FullPath = name;
    dirItems.Add(di);

    if(fi.IsDir()){
      if(name[name.Len()-1]=='\\')name+=L"*";
      else name+=L"\\*";
      add_dir_items(name,dirItems);
    }

    /*
    AString astr;
    Convert_UString_to_AString(archiveName,astr);
    printf("%s\n",astr.Ptr());
    */

    COutFileStream *outFileStreamSpec = new COutFileStream;
    CMyComPtr<IOutStream> outFileStream = outFileStreamSpec;
    if (!outFileStreamSpec->Create(archiveName, false))
    {
      PrintError("can't create archive file");
      return 1;
    }

    CMyComPtr<IOutArchive> outArchive;
    if (createObjectFunc(&CLSID_Format, &IID_IOutArchive, (void **)&outArchive) != S_OK)
    {
      PrintError("Can not get class object");
      return 1;
    }

    CArchiveUpdateCallback *updateCallbackSpec = new CArchiveUpdateCallback;
    CMyComPtr<IArchiveUpdateCallback2> updateCallback(updateCallbackSpec);
    updateCallbackSpec->Init(&dirItems);

    HRESULT result = outArchive->UpdateItems(outFileStream, dirItems.Size(), updateCallback);
    
    updateCallbackSpec->Finilize();

    show_progress_count_over(AString("Compressing "));
    
    if (result != S_OK)
    {
      PrintError("Update Error");
      return 1;
    }
    
    FOR_VECTOR (i, updateCallbackSpec->FailedFiles)
    {
      PrintNewLine();
      PrintError("Error for file", updateCallbackSpec->FailedFiles[i]);
    }
    
    if (updateCallbackSpec->FailedFiles.Size() != 0) return 1;

    
    //Free memory
    //delete outFileStreamSpec;
    //delete updateCallbackSpec;
    updateCallback.~CMyComPtr();
    outFileStream.~CMyComPtr();


    /***************************/
    //Encrypt
    ENcrypto en_crypto;
    en_crypto.set_thread_number(thread_number);
    
    //get user key
    AString usr_password;
    PrintStringLn("Please enter your password:");
    get_password(usr_password);
    //printf("Here?\n");
    if(en_crypto.cmd_get_hashed_key(usr_password)!=0){
      PrintError("Can't generate hashed key");
      return 1;
    }

    //printf("Here?\n");
    int result1=en_crypto.crypto_init(mid_file_name,output_path,input_path);
    //printf("RESULT1:%d\n",result1);
    if(result1!=0){
      if(result1==1) PrintError("Unexpected error1");
      else if(result1==2) PrintError("Can't create destination file");
      return 1;
    }

    int result2=en_crypto.crypto_process();
    PrintNewLine();
    switch (result2)
    {
    case 0:
      break;
    case 1:
      PrintError("Too many iterations");
      break;
    case 2:
      PrintError("Can't create a temporary direction");
      break;
    case 3:
      PrintError("Can't open the file");
      break;
    case 4:
      PrintError("Can't read file");
      break;
    case 5:
      PrintError("Can't write file");
      break;
    default:
      PrintError("Unexpected error2");
      break;
    }

    if(!DeleteFileAlways(mid_file_name)){
      PrintError("Can't delete the middle file");
      return 1;
    }

    if(IsDeleteOri){
      cfi.Find(input_path);
      if(!cfi.IsDir()){
        if(!DeleteFileAlways(input_path.Ptr())){
          PrintError("Can't delete the input file");
          return 1;
        }
      }
      else{
        if(!RemoveDirWithSubItems(input_path.Ptr())){
          PrintError("Can't delete the input directory");
          return 1;
        }
      }
    }


    PrintStringLn("  Finished!");
  }
  else if(option_ED==1) //**********************************************************************
  {
    DEcrypto de_crypto;
    de_crypto.set_thread_number(thread_number);

    int result1=de_crypto.verify_file(input_path);
    switch (result1)
    {
    case 0:
      break;
    case 1:
      PrintError("Can't find the file");
      break;
    case 2:
      PrintError("Wrong File: wrong size");
      break;
    case 3:
      PrintError("Wrong File: incorrect file header");
      break;
    default:
      PrintError("Unexpected error");
      break;
    }

    AString usr_password;
    PrintStringLn("Please enter your password:");
    get_password(usr_password);
    bool result2=de_crypto.verify_key(usr_password);
    while(!result2){
      PrintStringLn("Sorry, please enter your password again:");
      get_password(usr_password);
      result2=de_crypto.verify_key(usr_password);
    }

    int result3=de_crypto.verify_name();
    if(result3!=0){
      PrintError("Output file already exists");
      return 1;
    }

    while(cfi.Find(mid_file_name+L".7z")){
      mid_file_name+=L"-ly";
    }
    mid_file_name+=L".7z";

    UString final_output_path;
    de_crypto.crypto_init(mid_file_name,final_output_path);

    int result4=de_crypto.crypto_process();
    PrintNewLine();
    switch (result4)
    {
    case 0:
      break;
    case 1:
      PrintError("Too many iterations");
      break;
    case 2:
      PrintError("Can't create temporary file directories");
      break;
    case 3:
      PrintError("Unexpected error");
      break;
    case 4:
      PrintError("Command error");
      break;
    default:
      PrintError("Unexpected error");
      break;
    }
    
    UString final_output_dir;
    if(!IsNoOutputName){
      int idd=0;
      for(idd=(int)final_output_path.Len()-1;idd>=0;--idd){
        if(final_output_path[idd]=='\\') break;
      }
      UString temp_final_path=L"";
      for(int i=idd+1;i<(int)final_output_path.Len();++i) temp_final_path+=final_output_path[i];
      UString temp_final_path2;
      GetOnlyDirPrefix(output_path,temp_final_path2);
      temp_final_path2.DeleteBack();
      final_output_dir=temp_final_path2;
      temp_final_path2+=L"\\";
      temp_final_path2+=temp_final_path;
      final_output_path=temp_final_path2;
    }
    else{
      UString temp_final_path;
      GetOnlyDirPrefix(input_path,temp_final_path);
      temp_final_path.DeleteBack();
      final_output_dir=temp_final_path;
    }


    if(cfi.Find(final_output_path.Ptr())){
      if(cfi.IsDir()) Print("Directory ");
      else Print("File ");
      Print(final_output_path);
      PrintStringLn(" already exists");
FINAL:Print("Do you want to replace it?( Y [Yes to All] | N [No] ");
      AString option_final;
      get_string(option_final);
      if(option_final[0]=='N'||option_final[0]=='n'){
        if(!DeleteFileAlways(mid_file_name.Ptr())){
          PrintError("Can't delete the middle file");
          return 1;
        }
        return 0;
      }
      else if(option_final[0]=='y'||option_final[0]=='Y'){}
      else{
        PrintError("Incorrect option");
        goto FINAL;
      }
    }

    FString archiveName=mid_file_name;
    
    CMyComPtr<IInArchive> archive;
    if (createObjectFunc(&CLSID_Format, &IID_IInArchive, (void **)&archive) != S_OK)
    {
      PrintError("Can not get class object");
      return 1;
    }
    
    CInFileStream *fileSpec = new CInFileStream;
    CMyComPtr<IInStream> file = fileSpec;
    
    if (!fileSpec->Open(archiveName))
    {
      PrintError("Can not open archive file", archiveName);
      return 1;
    }

    
    CArchiveOpenCallback *openCallbackSpec = new CArchiveOpenCallback;
    CMyComPtr<IArchiveOpenCallback> openCallback(openCallbackSpec);
    openCallbackSpec->PasswordIsDefined = false;
      
    const UInt64 scanSize = 1 << 23;
    if (archive->Open(file, &scanSize, openCallback) != S_OK)
    {
      PrintError("Can not open file as archive", archiveName);
      return 1;
    }
    
    PrintStringLn("OUTOUT:");
    Print(final_output_dir);
    PrintNewLine();
    // Extract command
    CArchiveExtractCallback *extractCallbackSpec = new CArchiveExtractCallback;
    CMyComPtr<IArchiveExtractCallback> extractCallback(extractCallbackSpec);
    extractCallbackSpec->Init(archive, FString()); // second parameter is output folder path
    extractCallbackSpec->PasswordIsDefined = false;

    HRESULT result = archive->Extract(NULL, (UInt32)(Int32)(-1), false, extractCallback);
  
    if (result != S_OK)
    {
      PrintError("Extract Error");
      return 1;
    }

    show_progress_count_over(AString(kExtractingString));

    archive.~CMyComPtr();
    file.~CMyComPtr();
    //delete fileSpec;
    openCallback.~CMyComPtr();
    //delete openCallbackSpec;
    extractCallback.~CMyComPtr();
    //delete extractCallbackSpec;

    if(!DeleteFileAlways(mid_file_name)){
      PrintError("Can't delete the middle file");
      return 1;
    }

    if(!IsNoOutputName){
      AString re_path1,re_path2;
      Convert_UString_to_AString(final_output_path,re_path1);
      Convert_UString_to_AString(output_path,re_path2);

      //printf("%s\n",re_path1.Ptr());
      //printf("%s\n",re_path2.Ptr());
      if(cfi.Find(final_output_path)){
        bool fl_re=false;
        fl_re=MoveFileA(re_path1.Ptr(),re_path2.Ptr());

        if(!fl_re){
          PrintError("Can't move the file");
          return 1;
        }
      }
      else{
        PrintError("Can't find the file");
        return 1;
      }
    }

    if(IsDeleteOri){
      if(!DeleteFileAlways(input_path.Ptr())){
        PrintError("Can't delete the input file");
        return 1;
      }
    }

    PrintStringLn("  Finished!");
    
  }

  return 0;
}