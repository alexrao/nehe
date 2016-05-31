#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "print.h"


void print_debug(const char *file, const char *funtion, int linenum,const char *fmt, ...)
{
    char buf[512]={0};
    int offset = 0;
    int level = 0;
    if(fmt[0] > 0 && fmt[0] < 7)
    {
        offset = 1;
        if(fmt[0] <= 1)//DEBUG_HIDDEN
        {
            goto end;
        }
        else
        {
            level = fmt[0];
        }
    }
    va_list ap;
    va_start (ap, fmt);
    vsnprintf(buf,512,fmt,ap);
    va_end (ap);

    fprintf(stderr,"[%s] %d <%s>: ", file, linenum, funtion);

    if(level == 3 || level == 4 || level == 5 || level == 6)//DEBUG_FATAL DEBUG_LIGHT
    {
        //fprintf(stderr,"\033[31;46;5m");//闪烁
        if(level == 3 || level == 4)
        {
            fprintf(stderr,"\033[31;46;1m");
        }
        else
        {
            fprintf(stderr,"\033[31;42;1m");
        }
    }
    fprintf(stderr,"%s",buf + offset);

    if(level == 3 || level == 4 || level == 5 || level == 6)//DEBUG_FATAL DEBUG_LIGHT
    {
        fprintf(stderr,"\033[39;49;0m");
    }
    fprintf(stderr,"\n");
    if(level == 5)//DEBUG_FATAL
    {
        if(level == 5)
        {
            fprintf(stderr,"\n");
            fprintf(stderr,"\033[31;46;5m");//闪烁
            fprintf(stderr,"======================FATAL ERROR======================");
            fprintf(stderr,"\033[39;49;0m");
            fprintf(stderr,"\n");
        }
    }
end:
    return;
}


