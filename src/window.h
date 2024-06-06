#pragma once

#include <cstdint>

#include "image.h"

class window {
public:
    window(uint16_t w = 1280,uint16_t h = 720);
    ~window();

    void run();

    void set_image(const image& img);

private:
    uint16_t m_width;
    uint16_t m_height;

    void *instance;
};