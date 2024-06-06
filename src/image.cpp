#include "image.h"

#include <cstdlib>
#include <string>
#include <assert.h>

image::image(uint16_t w,uint16_t h) : width(w) , height(h)
{
    data = (color *)calloc(1,sizeof(color) * w * h);
}

image::~image()
{
    if (data) free(data);
}

void image::set_color(uint16_t x,uint16_t y,const color& c)
{
    //assert(x >= 0 && x <= width);
    //assert(y >= 0 && y <= height);
    if (!(x >= 0 && x <= width)) return;
    if (!(y >= 0 && y <= height)) return;

    int index = width * y + x;
    data[index] = c;
}

color image::get_color(uint16_t x,uint16_t y) const
{
    //assert(x >= 0 && x <= width);
    //assert(y >= 0 && y <= height);
    if (!(x >= 0 && x <= width)) return color();
    if (!(y >= 0 && y <= height)) return color();
    int index = width * y + x;
    return data[index];
}

uint16_t image::get_widht() const
{
    return width;
}

uint16_t image::get_height() const
{
    return height;
}

void image::resize(uint16_t w,uint16_t h)
{
    color *new_data = (color *)calloc(1,sizeof(color) * w * h);
}

void image::clean()
{
    memset(data,0, sizeof(color) * width * height);
}