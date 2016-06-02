#ifndef __IMAGE_H__
#define __IMAGE_H__

typedef struct STRUCT_IMAGE
{
    unsigned long x, y;
    char *data;
}stu_image;

int image_load(char *path, stu_image *image);
#endif
