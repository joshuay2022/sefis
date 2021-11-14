## Sefis

### Description

Sefis is a file encryption tool based on Windows system, which provides file encryption and decryption.

Sefis can encrypt any file with a password and make a self-extracting binary for decrypting it.

Sefis can achieve parallel encryption and decryption using multi-thread.

The application compresses the file or file folder before encrypting it and decompress the decrypted file after decrypting it.

### Working

#### Compressing and decompressing

The application uses [7zip LZMA SDK](https://www.7-zip.org/sdk.html) based on C++ code, which can compress and decompress a file or a file folder rapidly. 

#### Encryption and decryption

The application uses AES encryption algorithm to encrypt the file based on ECB mode. The encryption key is derived from user password using PBKDF2 algorithm based on SHA-256 algorithm and a random salt whose size is 16 bytes. So the encryption (decryption) key is 32 bytes. 

#### Multi-thread

Due to AES ECB mode, the application can achieve parallel encryption and decryption. While encrypting or decrypting the file, if the file size is larger than 1024 bytes, the application will create several temporary binary file and each thread execute encryption and decryption and output the contents to its own temporary file. After encryption or decryption execution, the application uses DOS command `copy` to combine these temporary files and delete them. If the file size is no larger than 1024 bytes, the application will execute serial encryption or decryption not using multi-thread.

#### Self-extracting binary

The application stores the derived key in the header of the file in order to verify the user's password while decrypting the file.

The derived key is derived from user's password using PBKDF2 algorithm and a salt. The salt is different from the salt used to generate the encryption key and the size is 12 bytes.

 The SFX binary includes a 12-byte salt used to generate the derived key, a derived key and a 16-byte salt used to generate the encryption(decryption) key. So the SFX binary is 60 bytes.

#### File header

In order to identify encrypted files, the encrypted files all have a 8-byte file header in the front of any other component, which is a binary form of an ASCII string `LZY&&YSJ`.

### Requirements

* Windows operating system based on x86 or x86_64 architecture
* VC++ compiler
* nmake

### Installation

Enter the `src` directory, and use `nmake` command to compile the project. The `sefis.exe` is in `./src/o` directory.

### Usage

Command format:

```bash
sefis.exe [e|d] [-d] [-j [number]] input_path [output_path]
```

* Use `e` for encryption and use `d` for decryption.
* Use `-d` to delete the input file after execution.
* Use `-j [number]` to set thread number. Notice that the thread number must be positive and no larger than 40. The application will set a suitable thread number if this option is missing.
* `input_path`: The path of the input file. Notice that only paths with '\\' will be permitted. Paths with '/' are prohibited.
* `output_path`: The path of output file. This option can be missing when decrypting the file. If the output file already exists, the file will be covered.

### Future work

* A GUI interface.
* Other platforms like Linux and Mac OS.
* Other optimization. 


