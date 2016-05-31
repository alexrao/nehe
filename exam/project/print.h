#ifndef __PRINT_H__
#define __PRINT_H__

#define print(...) print_debug(__FILE__, __FUNCTION__,__LINE__, ## __VA_ARGS__)

void print_debug(const char *file, const char *funtion, int linenum,const char *fmt, ...);

#endif
