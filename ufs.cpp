#include <iostream>
#include "ufs.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//https://www.youtube.com/watch?v=n2AAhiujAqs all the writing ' allocating ' and open / the basic .
//  inspired the strste part from  https://stackoverflow.com/questions/7180293/how-to-extract-filename-from-path
superblock* sB;  // the super block pointer
Inode* pInode;
block* pBlock;
open_data open_files[100000];

SuperBlock* getSB(){
    return sB;
}
Inode* getI (){
    return pInode;
}
Block* getB(){
    return pBlock;
}
open_data* get_open_files(){
    return open_files;
}



void shorten_file(int bn)
{
    int nn =  pBlock[bn].nextBlock;
    if (nn >= 0)   /// after we got out from set_filesize, if for example blocks number 3,4,5fwrite
    {              /// are the only clocks supposed to be since we want size of file = 1500 . but  the next block number of 5 is 9(n>= 0 instead of -1,-2 )
        // it a sign for us to make the file shorter
        shorten_file(nn);
    }
    pBlock[bn].nextBlock = -1;
}

void set_filesize(int filenum, int size) {
    int current = size + BLOCKSIZE - 1;
    int number = current / BLOCKSIZE;
    int bn = pInode[filenum].firstBlock;
    for (number--; number > 0; number--) {
        int next_num = pBlock[bn].nextBlock;
        if (next_num == -2) {
            int empty = find_empty_block();
            if(empty == -1){
                perror("not empty blocks now");
            }
            pBlock[bn].nextBlock = empty;
            pBlock[empty].nextBlock = -2;
        }
        bn = pBlock[bn].nextBlock;
    }
    shorten_file(bn);
    pBlock[bn].nextBlock = -2;
}

int find_empty_block()
{
    for (int i = 0; i < sB->blocks_number ; i++)
    {
        if (pBlock[i].nextBlock ==  -1 )
        {
            return i;
        }
    }
    return -1 ;

}
int find_empty_inode()
{
    for (int i = 0; i < sB->files_number ; i++)
    {
        if(pInode[i].firstBlock == -1  ) /// -1 means its clear anc can be used
        {
            return i;
        }
    }
    return -1 ;
}
void updating(mydirent* dirCur,char* name){
    for(int i = 0 ; i < 10 ; i ++){
        dirCur->file_connected[i] = -1 ;
    }
    strcpy(dirCur->name, name);
    dirCur->size = 0 ;

}

Inode* mymkfs(int s){
    int size_without_sb = s - sizeof(superblock);
    sB = static_cast<superblock *>(malloc(sizeof(superblock) ));
    int filles_number_bytes = (s/10);
    int filles_num = filles_number_bytes/ sizeof(Inode);
    int bytes_for_blocks = s - filles_number_bytes - sizeof(superblock);
    int block_num = bytes_for_blocks/ sizeof(block);

    sB->files_number = filles_num;
    sB->blocks_number =block_num;
    pInode = static_cast<Inode *>(malloc(sB->files_number * sizeof(Inode) ));
    pBlock = static_cast<block *>(malloc(sB->blocks_number * sizeof(Block) ));
    for(int i = 0 ; i < sB->files_number  ; i++){
         pInode[i].firstBlock = -1 ;
         pInode[i].file_size = 0 ;
         pInode[i].is_dir = -1 ;   /// -1 is no used yet , 0 i a file , 1 is a dir
     }
    for(int i = 0 ; i < sB->blocks_number; i ++){
        pBlock[i].nextBlock = -1 ;
    }
    for(int i = 0 ; i < sB->blocks_number; i ++){
        for(int j = 0 ; j < BLOCKSIZE; j ++)
        pBlock[i].data[j] = ' ';
    }
    // creating an INode with the data of the root directory
    int fd_of_storage_inode = allocate_file(sizeof(struct mydirent), "root");
    pInode[fd_of_storage_inode].is_dir= 1;

    struct mydirent* dirCur = static_cast<mydirent *>(malloc(sizeof(mydirent)));
    updating(dirCur,"root");

    /// coping whats pointed by 'prootdir' to the inode returned

    write_byte(fd_of_storage_inode, 0, reinterpret_cast<char *>(dirCur),sizeof(mydirent));
    free(dirCur);
    return pInode;
}

void write_byte(int filenum, int pos, char *data,int len) {

    mydirent* p = (mydirent*)data;
    //// updating the filles capacity - filles.pos stays as is !!!
    set_filesize(filenum,pos+ len);

    if(pos> pInode[filenum].file_size)
        pInode[filenum].file_size = pos;

    int cur = pInode[filenum].firstBlock;
    int pos_change = pos ;
    //// getting to correct block
    while(pos_change > BLOCKSIZE) {
        if (cur == -2) {
            int next_block = find_empty_block();
            pBlock[cur].nextBlock = next_block;
            cur = next_block;
            pBlock[cur].nextBlock = -2 ;
        }
        else if (cur == -1) {
            perror("no empty blocks anymore");
        }
        else {  /// there is actual next block and no needed finding empty blocks
            cur = pBlock[cur].nextBlock;
         }
            pos_change -= BLOCKSIZE;
         }
    ////////// writing the data to the currect file !
          int index = pos_change ;
          for(int i = 0 ; i < len; i ++){
          if(index == 511){
              cur = pBlock[cur].nextBlock;
              index = 0 ;
          }
          /// copying
          pBlock[cur].data[index] = data[i];
          /// updating
          index++;
          open_files[filenum].seek_pos +=1 ;
          if(pos + i > pInode[filenum].file_size)
              pInode[filenum].file_size++ ;
      }
 //   printf("%d\n",open_files[filenum].seek_pos);
}

int allocate_file(int size,char name[8]){    //// CREATING A FILE - ONE BLOCK - 512 BYTES
    int in = find_empty_inode();
    int block = find_empty_block();
    pInode[in].firstBlock = block;
    pBlock[block].nextBlock = -2;  //first block
    strcpy((pInode[in]).file_name, name);
    int curr_block = block;
    while(size > BLOCKSIZE) {
        int next_block = find_empty_block();
        pBlock[curr_block].nextBlock = next_block;
        curr_block = next_block;
        size -= BLOCKSIZE;
    }
    pBlock[curr_block].nextBlock = -2;
    int fd_to_be = in;
    return fd_to_be;   /// this is also can be the fd of the allocating file
}

int mymount(const char *source, const char *target,
            const char *filesystemtype, unsigned long) {
    {
        if (target != NULL) {
            FILE * file;
            file = fopen(target, "w+" );
            fwrite(sB, sizeof(SuperBlock), 1, file);
            fwrite(pInode, sizeof(Inode), sB->files_number, file);
            fwrite(pBlock, sizeof(Block), sB->blocks_number, file);
            fclose(file);
        }
        if (source != NULL) {
                FILE *file;
                file = fopen(target, "r");
                fread(sB, sizeof(SuperBlock), 1, file);
                fread(pInode, sizeof(Inode), sB->files_number, file);
                fread(pBlock, sizeof(Block), sB->blocks_number, file);
                fclose(file);
            }
        }

    }

int myopen(const char *pathname, int flags) {
    char *path = const_cast<char *>(pathname);
    char *ssc;
    int l = 0;
    ssc = strstr(path, "/");
    do {
        l = strlen(ssc) + 1;
        path = &path[strlen(path) - l + 2];
        ssc = strstr(path, "/");
    } while (ssc);
    ///path
    int file_number;
    for (file_number = 0; file_number <sB->files_number; file_number++) { /// every fd of file is equals to the order(1-10) of the correspond INode
        if (strcmp(pInode[file_number].file_name, path) == 0) {//#todo  is not a directory
            if(pInode[file_number].is_dir == 1 ) {
                printf("you want to open a diretory is a file . wrong ..");
                return -1;
            }
            else {
                open_files[file_number].file_d = file_number;
                open_files[file_number].seek_pos = 0;/// int  location of the first block of the file
                return open_files[file_number].file_d;

            }
        }
    }
    int fd = allocate_file(BLOCKSIZE , path); ///if the file does not exist - create it by alocating:
    pInode[fd].is_dir = 0;                               /// the inode represents file , not a directory !
    pInode[fd].file_size += BLOCKSIZE;
    open_files[fd].file_d =fd;
    open_files[fd].seek_pos = 0;/// int  location of the first block of the file
    return open_files[fd].file_d;


}

int myclose(int myfd){
    open_files[myfd].file_d = -1 ;
    open_files[myfd].seek_pos = - 1 ;
}

ssize_t myread(int myfd, void *buf, size_t count) {

    ((char *)buf)[count] = '\0';
    /// CALCULAING
    int current_pos = open_files[myfd].seek_pos;

    int first_block = pInode[myfd].firstBlock;
    block current = pBlock[first_block]; /// current starting at the first block of the file
    size_t left = current_pos;
    while (left >= BLOCKSIZE) {
        current = pBlock[current.nextBlock];
        if (current.nextBlock == -1) {
            perror("empty block ");
            return -1;
        }
        left = left - BLOCKSIZE;
    }
    int index = left;
    ///  taking care a "jump" between blocks
    for (int i = 0; i < count; i++) {
        if (index == 511) {
            if (current.nextBlock == -1) {
                perror("empty block ");
                return -1;
            }
            current = pBlock[current.nextBlock];
            index = 0;
        }
        /// copying

        ((char*)buf)[i] = current.data[index];
        /// updating
        index++;
        open_files[myfd].seek_pos += 1;
    }
    return open_files[myfd].seek_pos;
}

ssize_t mywrite(int myfd, const void *buf, size_t count){ ///#todo make sure it writes to the right area
    char* buff= (char*)buf;
    int pos = open_files[myfd].seek_pos;
    write_byte(myfd, pos,buff, count);
    return   open_files[myfd].seek_pos;
}

off_t mylseek(int myfd, off_t offset, int whence){
    if(whence == SEEK_SET){
       open_files[myfd].seek_pos = offset;
    }
    else if(whence == SEEK_CUR){
        open_files[myfd].seek_pos += offset;
    }
    else if(whence == SEEK_END){
        pInode[myfd].file_size += offset;
    }

}

void extracting(char** t, char* last_directory , char* second_to_last_direcotry,char* s) {
    while (*t != NULL) {
        strcpy(second_to_last_direcotry, last_directory);
        strcpy(last_directory, *t);
        *t = strtok(NULL, s);
    }
}
int updating_father_cirectory(int fd,int newfd ) {
    int second_to_last_directory_first_block = pInode[fd].firstBlock;
    mydirent *Pto_second_to_last_directory_data = reinterpret_cast<mydirent *>(pBlock[second_to_last_directory_first_block].data);
    for (int i = 0; i < sB->files_number; i++) {
        if (Pto_second_to_last_directory_data->file_connected[i] != -1) {
            Pto_second_to_last_directory_data->file_connected[i] = newfd;
            return newfd;
        }
    }
    return  -1 ;
}
    int Myopen_dir(const char *name) {   /// name is a path to the directory. returning the newfd of the new directory
        /// getting the last partion of the path
        char str[100];
        char last_directory[10] = "";
        char second_to_last_direcotry[10] = "";
        strcpy(str, name);
        char *token;
        char s[] = "/";
        token = strtok(str, s);
        ///// extracting the last and second to last directoris
        extracting(const_cast<char **>(&token), last_directory, second_to_last_direcotry, const_cast<char *>(s));
        int fd;
        for (int i = 0; i < sB->files_number; i++) {
            if (strcmp(pInode[i].file_name, last_directory) == 0) {  /// if exist
                if (pInode[i].is_dir == 0) {
                    printf("your asking to open a file as a directory ");
                    return -1;
                } else {
                    fd = i;
                    return fd;
                }
            }
        }
        /// the fd of the father firecotry
        fd = Myopen_dir(second_to_last_direcotry);
        if (fd == -1) {
            printf("second to last direcory name is actually a file ");
            return -1;
        }
        /// assigning pnode to the direcotry we want to open
        int new_fd = allocate_file(sizeof(mydirent), last_directory);
        /// creating new direcory in file system
           mydirent *p = static_cast<mydirent *>(malloc(sizeof(mydirent)));
        updating(p, last_directory);
        write_byte(new_fd, 0, reinterpret_cast<char *>(p), sizeof(mydirent));
        /// adding the new direcory in directory/file_array of the father directory
          if (updating_father_cirectory(fd, new_fd) == 1)
            printf("the arrayfilles of the parent dictionary is full!!");
        free(p);
        return new_fd;
     }
     struct mydirent *mydirent_dir(int fd ){
         if(pInode[fd].is_dir != 1 )
            perror("not a directory");
        int firstblock = pInode[fd].firstBlock;
        mydirent* ans = (mydirent*)pBlock[firstblock].data;
        return ans ;

}