#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ufs.h"





int main() {
    int m =9;
    char str[100];
    Inode *P = mymkfs(50000);
    const char status = 'w';
    myopen("root/shon", 0);
    int j =  P[0].is_dir;
    printf("%d", P[0].firstBlock);
    mylseek(1, 1600, SEEK_SET);
    open_data *cur = get_open_files();
    printf("%d\n", cur[1].seek_pos);
    mywrite(1, "leo messi", 9);
    Block *pBlock = getB();

    printf("%s\n", pBlock[4].data);
    mylseek(1, -9, SEEK_CUR);
    printf("%d\n", cur[1].seek_pos);
    myread(1, (void *) str, 9);

    printf("%s\n", str);
    printf("%d\n", cur[1].seek_pos);
    Myopen_dir("root/shani");
    inode* pInode = getI();
    int b  = pInode[2].firstBlock;

    mydirent* my = reinterpret_cast<mydirent *>(pBlock[b].data);
    printf("%s\n",my->name);
    printf("%d\n",my->size);
    printf("%d\n",my->file_connected[0]);
    //// cheaking mfseek;
    int a; float  d;
    int  c;
    char letter ;
    myFILE* file =(myfopen("shani/babihamud", &status));
    mywrite(file->fd,"13 14 i 2.2",12);
     int i = pInode[3].firstBlock;
     char buffer[11];
    //printf("%s",pBlock[i].data);
    mylseek(3,0,SEEK_SET);
    myfread(buffer,11,1,file);
    mymount(NULL,"/home/barsela/CLionProjects/fc/mason",0,0);

}

