#pragma once
#include <cstdint>

struct color {
    color() {};
    color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        rgba[0] = r;
        rgba[1] = g;
        rgba[2] = b;
        rgba[3] = a;
    };

    uint8_t rgba[4] = {0};
};

class image {
public:
    image(uint16_t w,uint16_t h);
    ~image();

    void set_color(uint16_t x,uint16_t y,const color& c);

    color get_color(uint16_t x,uint16_t y) const;

    uint16_t get_widht() const;

    uint16_t get_height() const;

    void resize(uint16_t w,uint16_t h);

    void clean();

    uint16_t width = 0;
    uint16_t height = 0;
    color *data = nullptr;
};