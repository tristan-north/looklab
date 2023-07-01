#include "renderview.h"
#include "displaydriver.h"
#include "common.h"
#include <QTimer>

RenderView::RenderView() {
    setFixedSize(RENDERWIDTH, RENDERHEIGHT);

    QTimer::singleShot(100, this, SLOT(updatePixmap()));
}

void RenderView::updatePixmap() {
    if (g_pFramebuf == nullptr) {
        QTimer::singleShot(33, this, SLOT(updatePixmap()));
        return;
    }

    g_framebufMutex.lock();
    setPixmap(QPixmap::fromImage(*g_pFramebuf));
    g_framebufMutex.unlock();
    //    QCoreApplication::processEvents();
    QTimer::singleShot(33, this, SLOT(updatePixmap()));
}
