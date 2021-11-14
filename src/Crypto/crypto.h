#ifndef _CRYPTO_H_
#define _CRYPTO_H_

#include "StdAfx.h"

//#include <iostream>

//#include <string>
//#include "../../7zip/CPP/Common/IntToString.h"
#include "../String/ConvertString.h"

#include "../Print/Print_Show.h"

//#include "../../7zip/CPP/Windows/DLL.h"
#include "../../7zip/CPP/Windows/FileDir.h"
#include "../../7zip/CPP/Windows/FileFind.h"
#include "../../7zip/CPP/Windows/FileIO.h"
#include "../../7zip/CPP/Windows/FileName.h"

using namespace std;
using namespace NWindows;
using namespace NFile;
using namespace NIO;
using namespace NFind;
using namespace NDir;

static const int BLOCK=16;
static const int header_size=8;
static const int msg_size=4*BLOCK;
static const Byte HEADER[header_size]={'L','Z','Y','&','&','Y','S','J'};
static const UString TEMP_DIR_NAME=L"TEMP_DIR-LY_FOREVER_LOVE-";
static const UString TLOG_NAME=L"tlog.txt";
static const int MAX_ITER_NUM=6;
static const int hashed_key_size=60;
static const int hashed_salt_size=12;
static const int sha_key_size=32;

static const size_t hashed_iter_num=2000;
static const size_t sha_iter_num=1500;

extern UInt64 *cur_proc_size;

class crypto
{    
private:
    
protected:
    CInFile _in_file;
    COutFile _out_file;

    UString _in_file_path;
    UString _out_file_path;
    UString _in_file_name;

    UString _out_file_temp_dir_path;

    //CInFile* _in_file_array;
    int THREAD_NUMBER;
    UString* _out_file_array;

    Byte sha_key[sha_key_size];
    Byte hashed_key[hashed_key_size];

    UInt64 _in_file_size;

    int find_pos() const;
    int find_pos2() const;
    int sub_path(int pos);
    int get_out_temp_dir_path();

    //int copy_byte(Byte *b1,const Byte *b2,int size);
public:
    int set_thread_number(const int &_thread_number);
    int set_thread_number();

    bool exe_system(const UString &ustr);

    /*
    virtual int crypto_process_simple();
    virtual int crypto_process_complex();

    virtual int crypto_init(const UString &in_file_path,const UString &out_file_path);
    virtual int crypto_process();
    */
    
    bool cmp_uca(const unsigned char *a1,const unsigned char *a2,const int &size);
    bool cmp_uca(const char *a1,const unsigned char *a2,const int &size);
    bool cmp_uca(const unsigned char *a1,const char *a2,const int &size);
    bool cmp_uca(const char *a1,const char *a2,const int &size);
};

class ENcrypto:public crypto
{
private:
    int crypto_process_simple();
    int crypto_process_complex();

    //int write_i_file(const UString &_in_i_file);
    
public:
    int crypto_verify(const UString &in_file_path,const UString &out_file_path);
    int cmd_get_hashed_key(const AString &usr_password);
    int crypto_init(const UString &in_file_path,const UString &out_file_path,const UString &ori_name);
    int crypto_process();
};

class DEcrypto:public crypto
{
private:
    UString final_out_file_path;
    size_t all_header_size;

    int crypto_process_simple();
    int crypto_process_complex();
public:
    int crypto_init(const UString &mid_file,UString &final_output_path);
    int crypto_process();
    int verify_file(const UString &in_file_path);
    bool verify_key(const AString &usr_password);
    int verify_name();
};

extern unsigned int *thrdid;

struct para_data_encrypt
{
    UString in_file_path;
    UString in_file_name;
    UString out_dir_path;
    UInt64 block_num;
    Byte hashed_key[hashed_key_size];
    Byte sha_key[sha_key_size];
    int thread_number;
};

struct para_data_decrypt
{
    UString in_file_path;
    UString out_dir_path;
    UInt64 block_num;
    Byte sha_key[sha_key_size];
    int thread_number;
    size_t header_size;
};

struct para_data_show
{
    UInt64 file_size;
    int thread_number;
    bool en_flag;
};


#endif