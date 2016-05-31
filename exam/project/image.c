#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include "print.h"
#include "image.h"

int image_load(char *path, stu_image *image)
{
    FILE *fp = NULL;
    unsigned long size;
    unsigned short planes, bpp;
    char temp;
    int ret = 0;

    fp = fopen(path, "rb");
    if(NULL == fp)
    {
        print("File not found[%s]", path);
        return 0;
    }

    fseek(fp, 18, SEEK_SET);

    ret = fread(&image->x, 4, 1, fp);
    if(ret != 1)
    {
        print("read image x fail\n");
        return 0;
    }

    ret = fread(&image->y, 4, 1, fp);
    if(ret != 1)
    {
        print("read image y fail\n");
        return 0;
    }


    ret = fread(&planes, 2, 1, fp);
    if(ret != 1)
    {
        print("read planes fail\n");
        return 0;
    }

    if(planes != 1)
    {
        print("planes form[%s] is not 1:%u\n", path, planes);
        return 0;
    }

    ret = fread(&bpp, 2, 1, fp);
    if(ret != 1)
    {
        print("read bpp fail\n");
        return 0;
    }

    if(bpp != 24 && bpp != 32)
    {
        print("bpp form[%s] is not 24:%u\n", path, bpp);
        return 0;
    }

    fseek(fp, 24, SEEK_CUR);

    if(bpp == 24)
    {
        size = image->x * image->y * 3;
    }
    else
    {
        size = image->x * image->y * 4;
    }
    image->data = (char *)malloc(size);
    if(image->data == NULL)
    {
        print("malloc data fial\n");
        return 0;
    }

    ret = fread(image->data, 1, size, fp);
    if(ret != size)
    {
        print("read data fail\n");
        return 0;
    }

    if(bpp == 32)
    {
        for(int i=0; i<size; i+=4)
        {
            temp = image->data[i];
            image->data[i] = image->data[i+3];
            image->data[i+3] = temp;
            temp = image->data[i+1];
            image->data[i+1] = image->data[i+2];
            image->data[i+2] = temp;
        }
    }
    else
    {
        for(int i=0; i<size; i+=3)
        {
            temp = image->data[i];
            image->data[i] = image->data[i+2];
            image->data[i+2] = temp;
        }
    }

    return bpp;
}

