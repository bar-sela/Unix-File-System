//
// Created by shani on 6/8/22.
//
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>

#ifndef FC_UFS_H
#define FC_UFS_H

#define BLOCKSIZE 512


int Myopen_dir(const char *name);
off_t mylseek(int myfd, off_t offset, int whence);
ssize_t mywrite(int myfd, const void *buf, size_t count);
int myopen(const char *pathname, int flags);
void shorten_file(int bn);
int find_empty_block();
int find_empty_inode();
int allocate_file(int size,char name[8]);
void write_byte(int filenum, int pos, char *data,int len );
ssize_t myread(int myfd, void *buf, size_t count);
void extracting(char** t, char* last_directory , char* second_to_last_direcotry,char* s);

typedef struct SuperBlock
{
    int files_number;  // how much files I have in the system
    int blocks_number;  // each file is a chain of blocks

} superblock;

typedef struct Block
{
    char data[BLOCKSIZE];
    int nextBlock = -1;
} block;

typedef struct Inode
{
    char file_name[10] ;
    int is_dir = -1;
    int file_size = 0 ;
    int firstBlock = -1 ;
} inode;


typedef struct open_data {
    int file_d;
    int seek_pos;
}open_data;

typedef struct mydirent {
    int size;
    char name[10];
    int file_connected[10] ;
}mydIrent;

typedef struct myFILE{
    char name[8];
    int fd ;
    char mode ;
}myFILE;

size_t myfread(void* ptr, size_t size, size_t nmemb, myFILE* stream);
size_t myfwrite(const void* ptr, size_t size, size_t nmemb, myFILE* stream);
int myfscanf(myFILE* restrict_stream, const char* restrict_format, ...);
myFILE* myfopen(const char* pathname, const char* mode);
Inode* mymkfs(int blocks_num);
int myclose(int myfd);
int mymount(const char *source, const char *target,
            const char *filesystemtype, unsigned long);

SuperBlock* getSB();
Inode* getI ();
Block* getB();
open_data* get_open_files();

#endif //FC_UFS_H
