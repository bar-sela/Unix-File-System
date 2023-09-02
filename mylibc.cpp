//
// Created by shani on 6/10/22.
//
#include "stdio.h"
#include <stdarg.h>
#include <stdlib.h>
#include "ufs.h"
#include <string.h>


///-------------------------------------
/// ---------------------------------------------///

myFILE* myfopen(const char* pathname, const char* m)
{
    char* mode = (char*)m;
    myFILE* file = (myFILE*)malloc(sizeof (myFILE)+1);
    if(!file)
    {
        printf("Not found enough memory.\n");
        exit(0);
    }
    file->mode = *mode;
    int fd = myopen(pathname, 0);
     file->fd = fd ;
    if(strlen(m) > 1 ){
        mylseek(fd, 0, SEEK_SET);  // todo to return a file pointer in mlseek function
        file->mode = 'K';
    }
    if(*mode == 'w')
    {
        mylseek(fd, 0, SEEK_SET);  // todo to return a file pointer in mlseek function
    }

    else if (*mode == 'a')
    {
         mylseek(fd, 0, SEEK_END);
    }

    else if (*mode == 'r')
    {
         mylseek(fd, 0, NULL);
    }
    else
        printf("The mode can be w/r/a only!");
    return file;
}

///----------------------------------------------------------------------------------///
int myfclose(myFILE* stream)
{
    int fd = stream->fd;
    myclose(fd);
    return 0;
}
///----------------------------------------------------------------------------------///
size_t myfread(void* ptr, size_t size, size_t nmemb, myFILE* stream)
{
    if(stream->mode == 'r' || stream->mode == 'K' ) {
        myread(stream->fd, ptr, size);  // read <size> bytes into ptr
        return 0;
    }
    return -1 ;
}
///----------------------------------------------------------------------------------///
size_t myfwrite(const void* ptr, size_t size, size_t nmemb, myFILE* stream) {
    if (stream->mode == 'w' || stream->mode == 'K' || stream->mode == 'a') {
        mywrite(stream->fd, ptr, size);  // write <size> bytes into the file from ptr
        return 0;
    } else
        return -1;
}
///----------------------------------------------------------------------------------///
    int myfseek(myFILE *stream, long offset, int whence) {
        mylseek(stream->fd, offset, whence);
        return 0;
    }
///----------------------------------------------------------------------------------///
    int myfscanf(myFILE *restrict_stream, const char *restrict_format, ...) {
        va_list str;
        va_start(str, restrict_format);  // initialize the list
        int i = 0;
        while (restrict_format[i + 1] != '\0') {
            //  char a = 'w';
            //  scanf("%c",&a)
            if (restrict_format[i] != '%')  // after this sign is the type of the object to be scan
            {
                i++;
                continue;
            }

            // probably the next char is the type:
            i++;  // the next char
            if (restrict_format[i] == 'c')  // char
            {
                // the argument 'str' entered char
                myfread(va_arg(str, void*), sizeof(char), 1, restrict_stream);
                i++;
                continue;
            }

            if (restrict_format[i] == 'd')  // a int
            {
                // the argument 'str' entered int
                myfread(va_arg(str, void*), sizeof(int), 1, restrict_stream);
                i++;
                continue;
            }

            if (restrict_format[i] == 'f')  // float number
            {
                myfread(va_arg(str, void*), sizeof(float), 1, restrict_stream);
                i++;
            }

        }
        return i;
    }

///----------------------------------------------------------------------------------///
int myfprintf(myFILE* restrict_stream, const char* restrict_format, ...) {
    va_list str;
    va_start(str, restrict_format);  // initialize the list
    int i = 0;

    while (restrict_format[i] != '\0')
    {
        if (restrict_format[i] != '%')  // this is a regular input
        {
            myfwrite((char *) va_arg(str, void*), sizeof(char), 1, restrict_stream);
            i++;
            continue;
        }

        // this is '%'
        if (restrict_format[i + 1] != '\0')
        {
            i++;  // we are at the type
            if (restrict_format[i] == 'c')  // char
            {
                // the argument 'str' entered char
                myfwrite((char *) va_arg(str, void*), sizeof(char), 1, restrict_stream);
                i++;
                continue;
            }

            if (restrict_format[i] == 'd')  // a int
            {
                // the argument 'str' entered int
                myfread((int *) va_arg(str, void*), sizeof(char), 1, restrict_stream);
                i++;
                continue;
            }

            if (restrict_format[i] == 'f')  // float number
            {
                myfread((float *) va_arg(str, void*), sizeof(float), 1, restrict_stream);
                i++;
                continue;
            }

            // if I got herer, so the '%' sighn in a "regular char" and need to be read!
            myfread((char*) va_arg(str, void*), sizeof(char), 1, restrict_stream);  // we read '%'
            // we don't need i++, we are already at the next char!

        }
    }
    return i;
}