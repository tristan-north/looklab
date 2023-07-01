#pragma once

#include <QMutex>

class QImage;

struct ucharPixel {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

extern QMutex g_framebufMutex;
extern QImage* g_pFramebuf;

void registerDisplayDriver();
