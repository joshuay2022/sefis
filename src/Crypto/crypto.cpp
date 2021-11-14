#include "StdAfx.h"

#include "crypto.h"
//#include <Windows.h>
#include <stdio.h>
#include <process.h>

#include "../SSL/aes.h"
#include "../SSL/sha.h"

unsigned int *thrdid;
UInt64 *cur_proc_size;

void print_hex(Byte *arr,size_t size){
    for(size_t i=0;i<size;++i){
        printf("0x%.2x, ",arr[i]);
        if((i+1)%16==0) printf("\n");
    }
    printf("\n");
}

bool crypto::cmp_uca(const unsigned char *a1,const unsigned char *a2,const int &size)
{
    for(int i=0;i<size;++i){
        if(a1[i]!=a2[i]) return false;
    }

    return true;
}

int crypto::set_thread_number(const int &_thread_number)
{
    if(_thread_number<=0&&_thread_number>40){
        return 1;
    }
    THREAD_NUMBER=_thread_number;

    return 0;
}

int crypto::set_thread_number()
{
    //To get the number of CPU cores
    THREAD_NUMBER=12;
    return 0;
}

int crypto::find_pos() const
{
    UString temp_str=_in_file_path;
    for(int i=(int)temp_str.Len() -1;i>=0;--i){
        if(temp_str[i]=='/'||temp_str[i]=='\\') return i;
    }

    return -1;
}

int crypto::find_pos2() const
{
    UString temp_str=_in_file_path;
    for(int i=(int)temp_str.Len() -1;i>=0;--i){
        if(temp_str[i]=='.') return i;
    }

    return -1;
}

int crypto::sub_path(int pos)
{
    wchar_t temp[BLOCK*BLOCK];
    if(pos<=0) {
        _out_file_temp_dir_path=L".\\";
        return (int)'\\';
    }
    for(int i=0;i<=pos;++i){
        temp[i]=_in_file_path[i];
    }
    temp[pos+1]='\0';
    _out_file_temp_dir_path=temp;

    return (int)temp[pos];
}

/*
* 0:TRUE
* 1:Too many iterations
*/
int crypto::get_out_temp_dir_path()
{
    char ch=(char)sub_path(find_pos());
    
    CFileInfo file_dir;
    UString temp_dir_path=_out_file_temp_dir_path+TEMP_DIR_NAME;

    bool _flag;
    int iter_num=0;
    while(true){
        _flag=file_dir.Find(temp_dir_path.Ptr());
        if(_flag){
            temp_dir_path+=L"_L";
            ++iter_num;
        }
        else break;

        if(iter_num>MAX_ITER_NUM) return 1;
    }

    _out_file_temp_dir_path=temp_dir_path+(wchar_t)ch;
    return 0;
}

/*
* 0:TRUE
* 1:Failed to generate hashed key
* 2:Can't load dll
*/
int ENcrypto::cmd_get_hashed_key(const AString &usr_password)
{
    //TODO bcrypt
    //copy(sha_key,x,hashed_key_size);
    
    Byte salt[hashed_salt_size];
	Byte hash[sha_key_size];

    srand((unsigned int)time(nullptr));
    
    //if(bcrypt_gensalt(12, salt)!=0) return 1;
    generate_salt(salt,hashed_salt_size);

    //if(bcrypt_hashpw(char_usr_password, salt, hash)!=0) return 1;
    pbkdf2((uint8_t*)usr_password.Ptr(),usr_password.Len(),salt,hashed_salt_size,hashed_iter_num,hash);

    /*
    for(int i=0;i<hashed_key_size;++i){
        hashed_key[i]=(Byte)hash[i];
    }
    */
    memcpy_s(hashed_key,hashed_salt_size,salt,hashed_salt_size);
    memcpy_s(hashed_key+hashed_salt_size,sha_key_size,hash,sha_key_size);
    //printf("222\n");

    //calc_sha_256(sha_key,char_usr_password,strlen(char_usr_password));

    Byte sha_salt[BLOCK];
    generate_salt(sha_salt,BLOCK);
    memcpy_s(hashed_key+hashed_salt_size+sha_key_size,BLOCK,sha_salt,BLOCK);
    pbkdf2((uint8_t*)usr_password.Ptr(),usr_password.Len(),sha_salt,BLOCK,sha_iter_num,sha_key);

    /**//**//**//**/
    /*
    printf("KEY:");
    print_hex(sha_key,sha_key_size);
    */

    //printf("1\n");

    return 0;
}

/*
* 0:TRUE
* 1:Unexpected error
* 2:Can't create destination file
*/
int ENcrypto::crypto_init(const UString &in_file_path,const UString &out_file_path,const UString &ori_name)
{
    //printf("Here1?\n");
    if(!_in_file.Open((CFSTR)in_file_path.Ptr(),FILE_SHARE_READ,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL)) return 1;
    _in_file_path=in_file_path;
    _in_file.GetLength(_in_file_size);
    _in_file.Close();

    //printf("Here2?\n");
    if(!_out_file.Open(out_file_path.Ptr(),FILE_SHARE_WRITE,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL)) return 2;
    _out_file_path=out_file_path;
    _out_file.Close();

    //printf("Here3?\n");
    int i=0;
    for(i=(int)ori_name.Len()-1;i>=0;--i){
        if(ori_name[i]=='\\') break;
    }

    //printf("Here4?\n");
    if(i<=0){
        _in_file_name=ori_name;
    }
    else{
        _in_file_name=L"";
        for(size_t j=i+1;j<ori_name.Len();++j){
            _in_file_name+=ori_name[j];
        }
    }

    //printf("Here5?\n");
    return 0;
}

/*
* 0:TRUE
* 1:Can't find file
*/
int DEcrypto::crypto_init(const UString &mid_file,UString &final_output_path)
{
    _out_file_path=mid_file;
    
    _out_file.Open(_out_file_path.Ptr(),FILE_SHARE_WRITE,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL);
    _out_file.Close();

    final_output_path=final_out_file_path;

    return 0;
}

/*
* 0:TRUE
* 1:Can't find the file
* 2:Wrong File: wrong size
* 3:Wrong File: incorrect file header
* 4:Unexpected error
*/
int DEcrypto::verify_file(const UString &in_file_path)
{
    CFileInfo cfi;
    if(!cfi.Find(in_file_path)){
        //cout<<"Can't find file \'"<<out_file_path.Ptr()<<"\'!"<<endl;
        return 1;
    }

    _in_file.Open((CFSTR)in_file_path.Ptr(),FILE_SHARE_READ,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL);
    _in_file.GetLength(_in_file_size);
    _in_file_path=in_file_path;

    if(_in_file_size<=(header_size+hashed_key_size+3*BLOCK)){
        //cout<<"Wrong file!"<<endl;
        _in_file.Close();
        return 2;
    }
    unsigned char tmp_array[header_size];
    UInt32 processedSize;
    _in_file.SeekToBegin();
    if(!_in_file.Read(tmp_array,header_size,processedSize)){
        //cout<<"Wrong file!"<<endl;
        _in_file.Close();
        return 4;
    }
    _in_file.Close();
    if(!cmp_uca((unsigned char*)HEADER,tmp_array,header_size)){
        //cout<<"Wrong file!"<<endl;
        return 3;
    }
    UInt64 rest_length=(_in_file_size-(UInt64)header_size-(UInt64)hashed_key_size)%(UInt64)BLOCK;
    if(rest_length!=0){
        //cout<<"Wrong file!"<<endl;
        return 2;
    }

    return 0;
}

bool DEcrypto::verify_key(const AString &usr_password)
{
    _in_file.Open((CFSTR)_in_file_path.Ptr(),FILE_SHARE_READ,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL);
    UInt64 newPos;
    _in_file.Seek(header_size,FILE_BEGIN,newPos);

    uint8_t salt[hashed_salt_size];UInt32 proccessd;
    _in_file.Read(salt,hashed_salt_size,proccessd);

    uint8_t hash[sha_key_size];
    _in_file.Read(hash,sha_key_size,proccessd);

    uint8_t usr_hash[sha_key_size];
    pbkdf2((uint8_t*)usr_password.Ptr(),usr_password.Len(),salt,hashed_salt_size,hashed_iter_num,usr_hash);

    bool flag=cmp_uca(hash,usr_hash,sha_key_size);

    if(flag){
        uint8_t key_salt[BLOCK];
        _in_file.Read(key_salt,BLOCK,proccessd);
        pbkdf2((uint8_t*)usr_password.Ptr(),usr_password.Len(),key_salt,BLOCK,sha_iter_num,sha_key);
        /***********************************************/
        /*
        printf("KEY:");
        print_hex(sha_key,sha_key_size);
        */
    }

    _in_file.Close();

    return flag;
}


/*
* 0:TRUE
* 1:Original file exists
* 2:File exists
*/
int DEcrypto::verify_name()
{
    
    _in_file.Open(_in_file_path.Ptr(),FILE_SHARE_READ,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL);

    UInt64 seekDis=(UInt64)(header_size+hashed_key_size);
    UInt64 newPos;
    _in_file.Seek(seekDis,FILE_BEGIN,newPos);

    uint8_t tmp_enc_length[BLOCK],tmp_dec_length[BLOCK];
    UInt32 proccessd;
    _in_file.Read(tmp_enc_length,BLOCK,proccessd);
    /*******************************************/
    /*
    printf("KEY:");print_hex(sha_key,sha_key_size);
    printf("LEN:");
    print_hex(tmp_enc_length,BLOCK);
    */
    decrypt_buffer(tmp_enc_length,tmp_dec_length,sha_key);
    UInt16 tmp_length=(((UInt16)tmp_dec_length[0])<<8)+((UInt16)tmp_dec_length[1]);
    //printf("LEN:%.2x %.2x %u\n",tmp_dec_length[0],tmp_dec_length[1],tmp_length);
    all_header_size=(size_t)header_size+(size_t)hashed_key_size+(size_t)BLOCK+(size_t)(BLOCK*tmp_length);
    
    uint8_t* tmp_enc_name=new uint8_t[tmp_length*(UInt16)BLOCK];
    uint8_t* tmp_dec_name=new uint8_t[tmp_length*(UInt16)BLOCK];
    _in_file.Read(tmp_enc_name,(UInt32)tmp_length*(UInt32)BLOCK,proccessd);
    _in_file.Close();
    for(UInt16 i=0;i<tmp_length;++i){
        decrypt_buffer(tmp_enc_name+(int)i*BLOCK,tmp_dec_name+(int)i*BLOCK,sha_key);
    }
    UInt16 id;
    for(id=tmp_length*(UInt16)BLOCK-1;id>=0;--id){
        if(tmp_dec_name[id]==(uint8_t)1) break;
    }
    char *tmp_char_name=new char[id+1];
    memset(tmp_char_name,0,sizeof(char)*(id+1));
    for(UInt16 i=0;i<id;++i) tmp_char_name[i]=(char)tmp_dec_name[i];
    //printf("TEST%s\n",tmp_char_name);

    UString out_file_name;
    Convert_AString_to_UString(AString(tmp_char_name),out_file_name);
        
    sub_path(find_pos());
    final_out_file_path=_out_file_temp_dir_path+out_file_name;

    /**/
    /*
    AString astr;
    Convert_UString_to_AString(final_out_file_path,astr);
    printf("FINAL:%s\n",astr.Ptr());
    */

    delete[]tmp_char_name;
    delete[]tmp_enc_name;
    delete[]tmp_dec_name;
    tmp_enc_name=tmp_dec_name=nullptr;

    /*
    CFileInfo cfi;
    if(cfi.Find(final_out_file_path)) return 1;
    */
    
    /*
    else{
        _in_file.Close();
        CFileInfo cfi;
        if(cfi.Find(out_file)) return 2;

        final_out_file_path=out_file;
    }
    */

    return 0;
}

bool crypto::exe_system(const UString &ustr)
{
    CFileInfo cfi;
    if(!cfi.Find(ustr.Ptr())) return false;

    CInFile cif;
    if(!cif.Open(ustr.Ptr(),FILE_SHARE_READ,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL)) return false;

    UInt64 fs;
    cif.GetLength(fs);
    UInt32 file_size=(UInt32)fs;

    char *file_data=new char[file_size];
    UInt32 proccessd;
    cif.Read(file_data,file_size,proccessd);
    cif.Close();

    AString astr(file_data);

    char temp_char[BLOCK];
    int cur_id=0;int pole=0;
    for(int i=0;i<THREAD_NUMBER;++i){
        memset(temp_char,'\0',BLOCK*sizeof(char));
        sprintf_s(temp_char,BLOCK,"id%.2d.jy",i);
        int pos=astr.Find(temp_char,cur_id);
        if(pos<0){
            delete[]file_data;
            return false;
        }
        if(i>1){
            if(pos-cur_id!=pole){
                delete[]file_data;
                return false;
            }
        }
        if(i>0) pole=pos-cur_id;
        cur_id=pos+7;
    }

    delete[]file_data;

    return true;
    
}

/*
* 0:TRUE
* 1:Failed to read file
* 2:Failed to write file
* 3:Failed to encrypt
*/
int ENcrypto::crypto_process_simple()
{
    if(!_in_file.Open((CFSTR)_in_file_path.Ptr(),FILE_SHARE_READ,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL)) return 1;
    if(!_out_file.Open(_out_file_path.Ptr(),FILE_SHARE_WRITE,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL)) return 2;

    UInt64 block_number=(_in_file_size/(UInt64)BLOCK);
    int rest_number;
    if(_in_file_size%(UInt64)BLOCK==0) rest_number=BLOCK;
    else{
        rest_number=(int)(_in_file_size%(UInt64)BLOCK);
        ++block_number;
    }


    // HEADER HEADER
    /* process HEADER
    * HEADER
    * hashed key
    * enc(name length)
    * enc(name)
    */
    UInt32 processd;
    // Write HEADER directly
    _out_file.Write(HEADER,header_size,processd);

    // Write hashed key
    _out_file.Write(hashed_key,hashed_key_size,processd);

    //Write encrypted name length and name
    AString tmp_in_file_name;
    Convert_UString_to_AString(_in_file_name,tmp_in_file_name);
    int tmp_name_size=tmp_in_file_name.Len();
    UInt16 tmp_name_length=(UInt16)tmp_name_size;
    tmp_name_length=tmp_name_length/(UInt16)BLOCK+1;
    Byte byte_ch1=(Byte)((tmp_name_length & 0xFF00)>>8);
    Byte byte_ch2=(Byte)(tmp_name_length & 0x00FF);
    Byte *tmp_byte_buf=new Byte[BLOCK];
    Byte *enc_tmp_buf=new Byte[BLOCK];
    memset(tmp_byte_buf,(Byte)'\0',BLOCK*sizeof(Byte));
    tmp_byte_buf[0]=byte_ch1;tmp_byte_buf[1]=byte_ch2;
        
    //TODO
    //encrypt
    if(encrypt_buffer(tmp_byte_buf,enc_tmp_buf,sha_key)!=0) return 3;
    _out_file.Write(enc_tmp_buf,BLOCK,processd);

    for(int i=0;i<(int)tmp_name_length;++i){
        if(i==(int)tmp_name_length-1){
            if(i*BLOCK<tmp_name_size){
                for(int j=0;j<tmp_name_size-i*BLOCK;++j) tmp_byte_buf[j]=(Byte)tmp_in_file_name[i*BLOCK+j];
                tmp_byte_buf[tmp_name_size-i*BLOCK]=(char)1;
                for(int j=tmp_name_size-i*BLOCK+1;j<BLOCK;++j) tmp_byte_buf[j]='\0';
            }
            else{
                memset(tmp_byte_buf,(Byte)'\0',sizeof(Byte)*BLOCK);
                tmp_byte_buf[0]=(Byte)1;
            }
        }
        else{
            for(int j=0;j<BLOCK;++j){
                tmp_byte_buf[j]=(Byte)tmp_in_file_name[i*BLOCK+j];
            }
        }

        //TODO
        //encrypt
        if(encrypt_buffer(tmp_byte_buf,enc_tmp_buf,sha_key)!=0) return 3;
        _out_file.Write(enc_tmp_buf,BLOCK,processd);
    }

    delete[]tmp_byte_buf;
    delete[]enc_tmp_buf;
    tmp_byte_buf=nullptr;
    enc_tmp_buf=nullptr;

    // HEADER HEADER

    /* Show Progress*/
    show_progress((UInt64)(header_size+hashed_key_size+BLOCK+BLOCK*tmp_name_size),
        _in_file_size+(UInt64)(header_size+hashed_key_size+BLOCK+BLOCK*tmp_name_size),
        AString("Encrypting")
    );

    //encrypt the body

    Byte before_cipher[BLOCK];
    Byte after_cipher[BLOCK];
    for(UInt64 i=0;i<block_number;++i){
        
        if(i==block_number-1){
            if(!_in_file.Read(before_cipher,rest_number,processd)) return 1;

            if(rest_number==BLOCK){
                encrypt_buffer(before_cipher,after_cipher,sha_key);
                if(!_out_file.Write(after_cipher,BLOCK,processd)) return 2;
                memset(before_cipher,(Byte)'\0',sizeof(Byte)*BLOCK);
                before_cipher[0]=(Byte)1;
            }
            else{
                before_cipher[rest_number]=(Byte)1;
                for(int j=rest_number+1;j<BLOCK;++j) before_cipher[j]=(Byte)0;
            }
            //TODO
            //for(int j=0;j<rest_number;++j) after_cipher[j]=before_cipher[j];
            //encrypt
            encrypt_buffer(before_cipher,after_cipher,sha_key);

            if(!_out_file.Write(after_cipher,BLOCK,processd)) return 2;
        }
        else{
        
            if(!_in_file.Read(before_cipher,BLOCK,processd)) return 1;

            //TODO
            //for(int j=0;j<BLOCK;++j) after_cipher[j]=before_cipher[j];
            encrypt_buffer(before_cipher,after_cipher,sha_key);

            if(!_out_file.Write(after_cipher,BLOCK,processd)) return 2;
        }

        /* Show Progress*/
        show_progress((UInt64)(header_size+hashed_key_size+BLOCK+BLOCK*tmp_name_size)+i*(UInt64)BLOCK,
            _in_file_size+(UInt64)(header_size+hashed_key_size+BLOCK+BLOCK*tmp_name_size),
            AString("Encrypting")
        );
    }

    /* Show Progress*/
    show_progress(_in_file_size,
        _in_file_size,
        AString("Encrypting")
    );

    _in_file.Close();
    _out_file.Close();

    return 0;
}



unsigned int __stdcall thread_show_progress(void *lpParameter)
{
    para_data_show *parameter=(para_data_show*)lpParameter;
    int thread_number=parameter->thread_number;
    UInt64 file_size=parameter->file_size;
    bool enflag=parameter->en_flag;

    while(true){
        UInt64 _cur_size=0;
        for(int i=0;i<thread_number;++i){
            _cur_size+=cur_proc_size[i];
        }
        if(enflag) show_progress(_cur_size,file_size,AString("Encrypting"));
        else show_progress(_cur_size,file_size,AString("Decrypting"));

        if(_cur_size>=file_size) break;

        //Sleep(1);
    }

    return 0;
} 


/*
*
*/
unsigned int __stdcall multi_thread_process_en(void *lpParameter)
{
    Sleep(100);
    para_data_encrypt* parameter=(para_data_encrypt*)lpParameter;

    int thread_number=parameter->thread_number;
    unsigned long thrdaddr=GetCurrentThreadId();
    int thread_id=0;
    for(int i=0;i<thread_number;++i){
        if(thrdid[i]==thrdaddr){
            thread_id=i;
            break;
        }
    }

    UInt64 block_number=parameter->block_num;
    
    /* Read file */
    CInFile in_file_in;
    in_file_in.Open(parameter->in_file_path,FILE_SHARE_READ,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL);

    /*out file name*/
    UString thread_tmp_file_path=parameter->out_dir_path;
    char *_buffer=new char[BLOCK];
    memset(_buffer,'\0',BLOCK);
    sprintf_s(_buffer,BLOCK,"id%.2d.jy",thread_id);
    UString string_buffer(_buffer);
    thread_tmp_file_path+=string_buffer;
    delete[]_buffer;

    /*handle of temp files*/
    COutFile tmp_out_file;
    tmp_out_file.Open(thread_tmp_file_path,FILE_SHARE_WRITE,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL);

    Byte before_cipher[BLOCK];
    Byte after_cipher[BLOCK];
    UInt32 processd;
    UInt64 move_distance=(UInt64)thread_id*block_number*(UInt64)BLOCK;
    UInt64 non_use_move;
    UInt64 file_size;
    in_file_in.GetLength(file_size);
    in_file_in.Seek(move_distance,non_use_move);

    /* encrypt or decrypt the block */

    // HEADER HEADER
    UInt64 tmp_header_size=100;
    if(thread_id==0){
        // Write HEADER directly
        tmp_out_file.Write(HEADER,header_size,processd);

        // Write hashed key
        tmp_out_file.Write(parameter->hashed_key,hashed_key_size,processd);

        //Write encrypted name length and name
        AString tmp_in_file_name;
        Convert_UString_to_AString(parameter->in_file_name,tmp_in_file_name);
        int tmp_name_size=tmp_in_file_name.Len();
        UInt16 tmp_name_length=(UInt16)tmp_name_size;
        tmp_name_length=tmp_name_length/(UInt16)BLOCK+1;
        Byte byte_ch1=(Byte)((tmp_name_length & 0xFF00)>>8);
        Byte byte_ch2=(Byte)(tmp_name_length & 0x00FF);
        Byte *tmp_byte_buf=new Byte[BLOCK];
        Byte *enc_tmp_buf=new Byte[BLOCK];
        memset(tmp_byte_buf,'\0',BLOCK*sizeof(Byte));
        tmp_byte_buf[0]=byte_ch1;tmp_byte_buf[1]=byte_ch2;

        tmp_header_size=(UInt64)(header_size+hashed_key_size+(tmp_name_length+1)*BLOCK);

        /**********************************/
        /*
        printf("LEN HEX:");
        print_hex(tmp_byte_buf,BLOCK);
        printf("KEY:");
        print_hex(parameter->sha_key,sha_key_size);
        */
        
        //TODO
        //encrypt
        encrypt_buffer(tmp_byte_buf,enc_tmp_buf,parameter->sha_key);
        tmp_out_file.Write(enc_tmp_buf,BLOCK,processd);

        /*************************************************/
        /*
        printf("LEN HEX ENC:");
        print_hex(enc_tmp_buf,BLOCK);
        */

        for(int i=0;i<(int)tmp_name_length;++i){
            if(i==(int)tmp_name_length-1){
                if(i*BLOCK<tmp_name_size){
                    for(int j=0;j<tmp_name_size-i*BLOCK;++j) tmp_byte_buf[j]=(Byte)tmp_in_file_name[i*BLOCK+j];
                    tmp_byte_buf[tmp_name_size-i*BLOCK]=(char)1;
                    for(int j=tmp_name_size-i*BLOCK+1;j<BLOCK;++j) tmp_byte_buf[j]=(Byte)'\0';
                }
                else{
                    memset(tmp_byte_buf,(Byte)'\0',sizeof(Byte)*BLOCK);
                    tmp_byte_buf[0]=(Byte)1;
                }
            }
            else{
                for(int j=0;j<BLOCK;++j){
                    tmp_byte_buf[j]=(Byte)tmp_in_file_name[i*BLOCK+j];
                }
            }

            //encrypt
            encrypt_buffer(tmp_byte_buf,enc_tmp_buf,parameter->sha_key);
            tmp_out_file.Write(enc_tmp_buf,BLOCK,processd);
        }

        delete[]tmp_byte_buf;
        delete[]enc_tmp_buf;
        tmp_byte_buf=nullptr;
        enc_tmp_buf=nullptr;

        /* Show Progress*/
        /*
        show_progress(tmp_header_size,
            file_size,
            AString("Encrypting")
        );
        */
        //cur_proc_size[0]+=tmp_header_size;
    }
    //HEADER HEADER


    //encrypt the body
    if(thread_id<thread_number-1){
        for(UInt64 i=0;i<block_number;++i){
            //if(!in_file_in.Read(before_cipher,BLOCK,processd)) return 4;
            in_file_in.Read(before_cipher,BLOCK,processd);
            
            //TODO
            //for(int j=0;j<BLOCK;++j) after_cipher[j]=before_cipher[j];
            encrypt_buffer(before_cipher,after_cipher,parameter->sha_key);

            //if(!tmp_out_file.Write(after_cipher,BLOCK,processd)) return 5;
            tmp_out_file.Write(after_cipher,BLOCK,processd);

            /*
            if(thread_id==0){
                /* Show Progress*//*
                show_progress(tmp_header_size+i*(UInt64)BLOCK*(UInt64)thread_number,
                    file_size,
                    AString("Encrypting")
                );
            }
            */
            cur_proc_size[thread_id]+=(UInt64)BLOCK;
        }
        in_file_in.Close();
        tmp_out_file.Close();
    }
    else{
        UInt64 last_array_number=file_size-move_distance;
        /*
        in_file_in.SeekToEnd(last_array_number);
        UInt64 non_use_number;
        in_file_in.Seek(-((Int64)last_array_number),FILE_END,non_use_number);
        */

        UInt64 last_array_block_number;
        int last_rest_number=(int)(last_array_number%(UInt64)BLOCK);
        if(last_rest_number==0){
            last_array_block_number=last_array_number/(UInt64)BLOCK;
            last_rest_number=BLOCK;
        }
        else last_array_block_number=last_array_number/(UInt64)BLOCK+1;

        for(UInt64 i=0;i<last_array_block_number;++i){
            if(i==last_array_block_number-1){
                //if(!in_file_in.Read(before_cipher,last_rest_number,processd)) return 4;
                in_file_in.Read(before_cipher,last_rest_number,processd);

                if(last_rest_number==BLOCK){
                    //TODO
                    //for(int j=0;j<last_rest_number;++j) after_cipher[j]=before_cipher[j];
                    //encrypt
                    encrypt_buffer(before_cipher,after_cipher,parameter->sha_key);
                    tmp_out_file.Write(after_cipher,BLOCK,processd);
                    memset(before_cipher,(Byte)'\0',sizeof(Byte)*BLOCK);
                    before_cipher[0]=(Byte)1;
                }
                else{
                    before_cipher[last_rest_number]=(Byte)1;
                    for(int j=last_rest_number+1;j<BLOCK;++j){
                        before_cipher[j]=(Byte)0;
                    }
                }

                //TODO
                //for(int j=0;j<last_rest_number;++j) after_cipher[j]=before_cipher[j];
                //encrypt
                encrypt_buffer(before_cipher,after_cipher,parameter->sha_key);

                //if(!tmp_out_file.Write(after_cipher,last_rest_number,processd)) return 5;
                tmp_out_file.Write(after_cipher,BLOCK,processd);
            }
            else{

                //if(!in_file_in.Read(before_cipher,BLOCK,processd)) return 4;
                in_file_in.Read(before_cipher,BLOCK,processd);

                //TODO
                //for(int j=0;j<BLOCK;++j) after_cipher[j]=before_cipher[j];
                //encrypt
                encrypt_buffer(before_cipher,after_cipher,parameter->sha_key);

                //if(!tmp_out_file.Write(after_cipher,BLOCK,processd)) return 5;
                tmp_out_file.Write(after_cipher,BLOCK,processd);
            }

            /* Show Progress*/
            cur_proc_size[thread_id]+=(UInt64)BLOCK;
        }

        in_file_in.Close();
        tmp_out_file.Close();
    }

    return 0;
}

/*
* 0:TRUE
* 1:Too many iterations
* 2:Failed to create a temporary direction
* 3:Failed to open the file
* 4:Failed to read file
* 5:Failed to write file
* 6:Unexpected error
*/
int ENcrypto::crypto_process_complex()
{
    /*get the directory path*/
    int result=get_out_temp_dir_path();
    if(result!=0) return result;

    if(!CreateComplexDir(_out_file_temp_dir_path)) return 2;

    //After set thread number

    UInt64 inter_temp=_in_file_size/(UInt64)BLOCK;
    if(_in_file_size%(UInt64)BLOCK!=0) inter_temp++;
    UInt64 block_number;
    if(inter_temp%(UInt64)THREAD_NUMBER==0) block_number=inter_temp/(UInt64)THREAD_NUMBER;
    else block_number=inter_temp/(UInt64)THREAD_NUMBER;

    thrdid=new unsigned[THREAD_NUMBER];
    HANDLE *handle=new HANDLE[THREAD_NUMBER+1];
    para_data_encrypt *para_array=new para_data_encrypt[THREAD_NUMBER];

    cur_proc_size=new UInt64[THREAD_NUMBER];
    memset(cur_proc_size,0,sizeof(UInt64)*THREAD_NUMBER);
    para_data_show *show_data=new para_data_show;
    show_data->file_size=_in_file_size;
    show_data->thread_number=THREAD_NUMBER;
    show_data->en_flag=true;

    for(int i=0;i<THREAD_NUMBER;++i){
        para_array[i].block_num=block_number;
        para_array[i].in_file_path=_in_file_path;
        para_array[i].in_file_name=_in_file_name;
        para_array[i].out_dir_path=_out_file_temp_dir_path;
        para_array[i].thread_number=THREAD_NUMBER;
        memcpy_s(para_array[i].hashed_key,hashed_key_size,hashed_key,hashed_key_size);
        memcpy_s(para_array[i].sha_key,sha_key_size,sha_key,sha_key_size);
    }
    for(int i=0;i<THREAD_NUMBER;++i){
        handle[i]=(HANDLE)_beginthreadex(nullptr,0,multi_thread_process_en,&para_array[i],0,&thrdid[i]);
    }

    handle[THREAD_NUMBER]=(HANDLE)_beginthreadex(nullptr,0,thread_show_progress,show_data,0,nullptr);

    WaitForMultipleObjects(THREAD_NUMBER+1,handle,TRUE,INFINITE);

    /* Show Progress*/
    /*
    show_progress(_in_file_size,
        _in_file_size,
        AString("Encrypting")
    );
    */

    for(int i=0;i<THREAD_NUMBER+1;++i) CloseHandle(handle[i]);

    delete[]para_array;
    delete[]handle;
    delete[]thrdid;
    delete[]cur_proc_size;
    delete show_data;


    // Write final output file
    // DOS command
    /*
    UString *temp_out_file_name=new UString[THREAD_NUMBER];
    char tmp_name[BLOCK];
    for(int i=0;i<THREAD_NUMBER;++i){
        memset(tmp_name,'\0',sizeof(char)*BLOCK);
        sprintf_s(tmp_name,BLOCK-1,"id%.2d.jy ",i);
        temp_out_file_name[i]=_out_file_temp_dir_path+UString(tmp_name);
    }

    UString command_string=L"copy /B ";
    for(int i=0;i<THREAD_NUMBER;++i){
        command_string+=temp_out_file_name[i];
    }
    delete[]temp_out_file_name;
    */

    UString command_string=L"copy /b ";
    command_string+=_out_file_temp_dir_path;
    command_string+=L"*.jy ";
    command_string+=_out_file_path;
    command_string+=L" > ";
    command_string+=_out_file_temp_dir_path;
    command_string+=TLOG_NAME;

    AString command_str;
    Convert_UString_to_AString(command_string,command_str);

    //printf("%s\n",command_str.Ptr());

    system(command_str.Ptr());

    bool fl=exe_system(_out_file_temp_dir_path+TLOG_NAME);

    UString _out_file_temp_dir_path2=_out_file_temp_dir_path;
    _out_file_temp_dir_path2.DeleteBack();
    if(!RemoveDirWithSubItems(_out_file_temp_dir_path2.Ptr())) return 6;

    return fl?0:4;
}

/*
* 0:TRUE
* 1:Too many iterations
* 2:Failed to create a temporary direction
* 3:Failed to open the file
* 4:Failed to read file
* 5:Failed to write file
* 6:Unknown error
*/
int ENcrypto::crypto_process()
{
    if(_in_file_size<=1024){
        int result=crypto_process_simple();

        if(result!=0) return result+3;
        else return 0;
    }
    else{
        int result=crypto_process_complex();

        return result;
    }

    //return 6;
}

/*
* 0:TRUE
* 1:Unexpected error
*/

int DEcrypto::crypto_process_simple()
{
    if(!_in_file.Open(_in_file_path.Ptr(),FILE_SHARE_READ,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL)) return 1;
    if(!_out_file.Open(_out_file_path.Ptr(),FILE_SHARE_WRITE,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL)) return 1;

    UInt64 seekDis=(UInt64)all_header_size;
    UInt64 newPos;
    if(!_in_file.Seek(seekDis,FILE_BEGIN,newPos)) return 1;

    /* Show Progress*/
    show_progress(all_header_size,
        _in_file_size,
        AString("Decrypting")
    );

    UInt64 block_number=(_in_file_size-(UInt64)all_header_size)/BLOCK;
    uint8_t before_dec[BLOCK];
    uint8_t after_dec[BLOCK];
    UInt32 proccessd;
    for(UInt64 i=0;i<block_number;++i){
        _in_file.Read(before_dec,BLOCK,proccessd);
        decrypt_buffer(before_dec,after_dec,sha_key);
        if(i==block_number-1){
            int j=BLOCK-1;
            for(j=BLOCK-1;j>=0;--j){
                if(after_dec[j]==(uint8_t)1) break;
            }
            if(j>0){
                _out_file.Write(after_dec,j,proccessd);
            }
        }
        else{
            _out_file.Write(after_dec,BLOCK,proccessd);
        }

        /* Show Progress*/
        show_progress(all_header_size+i*(UInt64)BLOCK,
            _in_file_size,
            AString("Decrypting")
        );
    }

    /* Show Progress*/
    show_progress(_in_file_size,
        _in_file_size,
        AString("Decrypting")
    );

    _in_file.Close();
    _out_file.Close();

    return 0;
}

unsigned int __stdcall multi_thread_process_de(void *lpParameter)
{
    Sleep(100);
    para_data_decrypt* parameter=(para_data_decrypt*)lpParameter;

    int thread_number=parameter->thread_number;
    unsigned long thrdaddr=GetCurrentThreadId();
    int thread_id=0;
    for(int i=0;i<thread_number;++i){
        if(thrdid[i]==thrdaddr){
            thread_id=i;
            break;
        }
    }

    UInt64 block_number=parameter->block_num;
    
    /* Read file */
    CInFile in_file_in;
    in_file_in.Open(parameter->in_file_path,FILE_SHARE_READ,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL);
    UInt64 file_size;
    in_file_in.GetLength(file_size);

    /*out file name*/
    UString thread_tmp_file_path=parameter->out_dir_path;
    char *_buffer=new char[BLOCK];
    memset(_buffer,'\0',BLOCK);
    sprintf_s(_buffer,BLOCK,"id%.2d.jy",thread_id);
    UString string_buffer;
    Convert_AString_to_UString(AString(_buffer),string_buffer);
    thread_tmp_file_path+=string_buffer;
    delete[]_buffer;

    /*handle of temp files*/
    COutFile tmp_out_file;
    tmp_out_file.Open(thread_tmp_file_path,FILE_SHARE_WRITE,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL);

    UInt64 thread_block_num=block_number/(UInt64)thread_number;
    if(thread_id==thread_number-1) thread_block_num+=(block_number%(UInt64)thread_number);
    
    //printf("THREAD:id%d-%d\n",thread_id,(int)thread_block_num);

    UInt64 newPos;UInt32 proccessd;
    UInt64 moveDis=(UInt64)parameter->header_size+block_number/(UInt64)thread_number*(UInt64)thread_id*(UInt64)BLOCK;
    in_file_in.Seek(moveDis,FILE_BEGIN,newPos);

    Byte before_dec[BLOCK];
    Byte after_dec[BLOCK];
    for(UInt64 i=0;i<thread_block_num;++i){
        in_file_in.Read(before_dec,BLOCK,proccessd);
        decrypt_buffer(before_dec,after_dec,parameter->sha_key);
        if(i==thread_block_num-1&&thread_id==thread_number-1){
            //printf("FINAL:");print_hex(after_dec,BLOCK);
            int j=BLOCK-1;
            for(j=BLOCK-1;j>=0;--j){
                if(after_dec[j]==(uint8_t)1) break;
            }
            if(j>0){
                tmp_out_file.Write(after_dec,j,proccessd);
            }
        }
        else{
            tmp_out_file.Write(after_dec,BLOCK,proccessd);
        }

        /*
        if(thread_id==thread_number-1){
            /* Show Progress*//*
            show_progress((UInt64)header_size+i*(UInt64)BLOCK*(UInt64)thread_number,
                file_size,
                AString("Decrypting")
            );
        }
        */

        cur_proc_size[thread_id]+=(UInt64)BLOCK;
    }

    in_file_in.Close();
    tmp_out_file.Close();

    return 0;
}

/*
* 0:TRUE
* 1:Too many iterations
* 2:Can't create temporary file directories
* 3:Unexpected error
* 4:Command error
*/
int DEcrypto::crypto_process_complex()
{
    sub_path(find_pos());
    if(get_out_temp_dir_path()!=0) return 1;

    if(!CreateComplexDir(_out_file_temp_dir_path)) return 2;

    para_data_decrypt* para_array=new para_data_decrypt[THREAD_NUMBER];
    thrdid=new unsigned int[THREAD_NUMBER];
    HANDLE *handle=new HANDLE[THREAD_NUMBER+1];
    UInt64 block_number=(_in_file_size-(UInt64)all_header_size)/(UInt64)BLOCK;
    //printf("SIZE:%llu %llu %llu\n",_in_file_size, (UInt64)all_header_size, block_number);

    cur_proc_size=new UInt64[THREAD_NUMBER];
    memset(cur_proc_size,0,sizeof(UInt64)*THREAD_NUMBER);
    para_data_show *show_data=new para_data_show;
    show_data->en_flag=false;
    show_data->file_size=_in_file_size-(UInt64)all_header_size;
    show_data->thread_number=THREAD_NUMBER;

    for(int i=0;i<THREAD_NUMBER;++i){
        para_array[i].block_num=block_number;
        para_array[i].in_file_path=_in_file_path;
        para_array[i].out_dir_path=_out_file_temp_dir_path;
        para_array[i].thread_number=THREAD_NUMBER;
        para_array[i].header_size=all_header_size;
        memcpy_s(para_array[i].sha_key,sha_key_size,sha_key,sha_key_size);
    }

    /* Show Progress*/
    /*
    show_progress(all_header_size,
        _in_file_size,
        AString("Decrypting")
    );
    */

    for(int i=0;i<THREAD_NUMBER;++i){
        handle[i]=(HANDLE)_beginthreadex(nullptr,0,multi_thread_process_de,&para_array[i],0,&thrdid[i]);
    }
    handle[THREAD_NUMBER]=(HANDLE)_beginthreadex(nullptr,0,thread_show_progress,show_data,0,nullptr);

    WaitForMultipleObjects(THREAD_NUMBER+1,handle,true,INFINITE);

    /* Show Progress*/
    /*
    show_progress(_in_file_size,
        _in_file_size,
        AString("Decrypting")
    );
    */

    for(int i=0;i<THREAD_NUMBER+1;++i) CloseHandle(handle[i]);

    delete[]handle;
    delete[]para_array;
    delete[]thrdid;
    delete[]cur_proc_size;
    delete show_data;

    /*Combine*/

    UString command_string=L"copy /b ";
    command_string+=_out_file_temp_dir_path;
    command_string+=L"*.jy ";
    command_string+=_out_file_path;
    command_string+=L" > ";
    command_string+=_out_file_temp_dir_path;
    command_string+=TLOG_NAME;

    AString command_str;
    Convert_UString_to_AString(command_string,command_str);

    //printf("%s\n",command_str.Ptr());

    system(command_str.Ptr());

    bool fl=exe_system(_out_file_temp_dir_path+TLOG_NAME);

    UString _out_file_temp_dir_path2=_out_file_temp_dir_path;
    _out_file_temp_dir_path2.DeleteBack();
    if(!RemoveDirWithSubItems(_out_file_temp_dir_path2.Ptr())) return 3;

    return fl?0:4;
}

/*
* 0:TRUE
* 1:1:Too many iterations
* 2:Can't create temporary file directories
* 3:Unexpected error
* 4:Command error
*/
int DEcrypto::crypto_process()
{
    if(_in_file_size<1124){
        int result=crypto_process_simple();
        if(result>0) return result+2;
        else return 0;
    }
    else {
        return crypto_process_complex();
    }
}