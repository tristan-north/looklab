#include "displaydriver.h"
#include "perftimer.h"
#include <ndspy.h>
#include <prmanapi.h> // for PRMANEXPORT
#include <QMutex>
#include <QImage>
#include <iostream>

QMutex g_framebufMutex;
QImage* g_pFramebuf = nullptr;

// This gets set for each image LOD in DspyImageOpen and passed in to the other
// Dspy functions by rman.
struct ImageInfo {
    int width;
    int height;
};

extern "C" {

PRMANEXPORT PtDspyError DspyImageOpen(PtDspyImageHandle* pvImage, const char* drivername,
                                      const char* filename, int width, int height, int paramCount,
                                      const UserParameter* parameters, int formatCount,
                                      PtDspyDevFormat* format, PtFlagStuff* flagstuff) {
    printf("DspyImageOpen(): \"%s\" %d %d\n", filename, width, height);

    if(g_pFramebuf) delete g_pFramebuf;
    g_pFramebuf = new QImage(width, height, QImage::Format_RGB888);
    g_pFramebuf->fill(0x333333);

    ImageInfo* pImageInfo = new ImageInfo;
    pImageInfo->width = width;
    pImageInfo->height = height;
    *pvImage = pImageInfo;

//        flagstuff->flags |=  PkDspyFlagsWantsEmptyBuckets | PkDspyFlagsWantsNullEmptyBuckets;

    // Set each channel to float format
    for (int i = 0; i < formatCount; i++)
        format[i].type = PkDspyFloat32;

    return PkDspyErrorNone;
}

PRMANEXPORT PtDspyError DspyImageQuery(PtDspyImageHandle pvImage, PtDspyQueryType querytype,
                                       int datalen, void* data) {
    PtDspyError ret = PkDspyErrorNone;

    if (datalen > 0 && nullptr != data) {
        switch (querytype) {
            case PkOverwriteQuery: {
                printf("PXR CALLBACK - DSPYIMAGEQUERY - Overwrite query\n");

                PtDspyOverwriteInfo overwriteInfo;
                if (datalen > sizeof(overwriteInfo))
                    datalen = sizeof(overwriteInfo);

                overwriteInfo.overwrite = 0;
                overwriteInfo.interactive = 0;
                memcpy(data, &overwriteInfo, datalen);

                break;
            }
            case PkRedrawQuery: {
                printf("PXR CALLBACK - DSPYIMAGEQUERY - Redraw query\n");

                PtDspyRedrawInfo redrawInfo;
                if (datalen > sizeof(redrawInfo))
                    datalen = sizeof(redrawInfo);

                redrawInfo.redraw = 1;
                memcpy(data, &redrawInfo, datalen);

                break;
            }
            case PkSizeQuery: {
                printf("PXR CALLBACK - DSPYIMAGEQUERY - Size query\n");

                PtDspySizeInfo sizeInfo;

                if (datalen > sizeof(sizeInfo))
                    datalen = sizeof(sizeInfo);

                sizeInfo.height = 512;  // Just arbitrary resolution for now
                sizeInfo.width = 512;
                sizeInfo.aspectRatio = 1.0f;

                memcpy(data, &sizeInfo, datalen);
                break;
            }
            case PkMultiResolutionQuery: {
                printf("PXR CALLBACK - DSPYIMAGEQUERY - Multires Query\n");
                // From this thread:
                //  https://renderman.pixar.com/forum/showthread.php?s=&threadid=26498
                //              // "Just before 19 shipped we found a race condition with the
                //  multires query, and rather
                //              // than fix the underlying issue, the decision was made to assume
                //  that any display driver
                //              // used with re-rendering was multires capable. But! Please
                //  continue to code as though the
                //              // query were happening. We will hopefully address the underlying
                //  issue soon."
                //
                PtDspyMultiResolutionQuery multiresInfo;
                if (datalen > sizeof(multiresInfo))
                    datalen = sizeof(multiresInfo);

                multiresInfo.supportsMultiResolution = 1;
                memcpy(data, &multiresInfo, datalen);

                break;
            }
            case PkNextDataQuery: {
                printf("PXR CALLBACK - DSPYIMAGEQUERY - Next Data Query\n");

                //              // From this thread:
                //  https://renderman.pixar.com/forum/showthread.php?s=&threadid=26498
                //              // "The PkNextDataQuery is ignored by all of our display drivers,
                //  so it is safe for yours to ignore it as well.
                //              // This looks to be a bit of legacy code that needs to be
                //  excised."
                ret = PkDspyErrorUnsupported;
                break;
            }
            case PkMemoryUsageQuery: {
                printf("PXR CALLBACK - DSPYIMAGEQUERY - Memory Usage\n");
                ret = PkDspyErrorUnsupported;
                break;
            }
            default: {
                printf("PXR CALLBACK - DSPYIMAGEQUERY - Unsupported query: %d\n", querytype);
                ret = PkDspyErrorUnsupported;
                break;
            }
        }
    } else {
        printf("PXR CALLBACK - DSPYIMAGEQUERY - Bad params query\n");
        ret = PkDspyErrorBadParams;
    }

    return ret;
}

PRMANEXPORT PtDspyError DspyImageData(PtDspyImageHandle pvImage, int xmin, int xmax_plusone,
                                      int ymin, int ymax_plusone, int entrysize,
                                      const unsigned char* data) {
//    PerfTimer timer;

    /* How many times called per second
    static int nTimesCalledPerSec {0};
    static uint64_t timingStart {0};
    static bool firstTime = true;

    uint64_t now = PerfTimer::takeTimeStamp();
    if(firstTime) {
        timingStart = now;
        firstTime = false;
    }
    else {
        double elapsedSec = double(now - timingStart) * 1e-9;
        if(elapsedSec > 1 ) {
            printf("DspyImageData times called per sec: %d\n", nTimesCalledPerSec);
            timingStart = now;
            nTimesCalledPerSec = -1;
        }
    }

    nTimesCalledPerSec++;
    */

    g_framebufMutex.lock();
    // For each pixel recieved from rman
    for (int y = ymin; y < ymax_plusone; y++) {
        ucharPixel* scanline = (ucharPixel*)g_pFramebuf->scanLine(y);
        for (int x = xmin; x < xmax_plusone; x++) {
            float r = *((float*)data);
            float g = *((float*)data + 1);
            float b = *((float*)data + 2);

            // This is slow
//            r = pow(r, 1/2.2);
//            g = pow(g, 1/2.2);
//            b = pow(b, 1/2.2);

            r *= 255;
            g *= 255;
            b *= 255;

            // Clamp to white
            r = r > 255.f ? 255.f : r;
            g = g > 255.f ? 255.f : g;
            b = b > 255.f ? 255.f : b;

            scanline[x].r = r;
            scanline[x].g = g;
            scanline[x].b = b;

            data += entrysize;
        }
    }
    g_framebufMutex.unlock();

//    static int callCount{0};
//    static double elapsedAccum{0};
//    elapsedAccum += timer.elapsedMSec();
//    if (callCount % 5000 == 0) {
//        std::cout << "DspyImageData accum time: " << elapsedAccum << " ms" << std::endl;
//        elapsedAccum = 0;
//    }
//    ++callCount;
    return PkDspyErrorNone;
}

PRMANEXPORT PtDspyError DspyImageActiveRegion(PtDspyImageHandle pvImage, int xmin, int xmax_plusone,
                                              int ymin,
                                              int ymax_plusone) { // exclusive bound
    //	printf("DspyImageActiveRegion(): xmin: %d  xmax_plusone: %d  ymin: %d
    // ymax_plusone: %d\n"); 	for(int i = 0; i < WIDTH*HEIGHT*3; i++)
    // g_renderPixels[i] = 0;

    printf("DspyImageActiveRegion\n");
    return PkDspyErrorNone;
}

PRMANEXPORT PtDspyError DspyMetadata(PtDspyImageHandle, char* metadata) {
//    printf("Dspy metadata: %s\n", metadata);
    return PkDspyErrorNone;
}

PRMANEXPORT PtDspyError DspyImageClose(PtDspyImageHandle pvImage) {
    printf("DspyImageClose\n");
    return PkDspyErrorNone;
}

}; // extern C

void registerDisplayDriver() {
    PtDspyDriverFunctionTable dt;
    dt.Version = k_PtDriverCurrentVersion;
    dt.pOpen = DspyImageOpen;
    dt.pWrite = DspyImageData;
    dt.pClose = DspyImageClose;
    dt.pQuery = DspyImageQuery;
    dt.pActiveRegion = DspyImageActiveRegion;
    dt.pMetadata = DspyMetadata;
    PtDspyError err = DspyRegisterDriverTable("looklabDisplay",
                                              &dt); // register the direct-linked display driver
    if (err)
        printf("Dspy Register error: %d\n", err);
}
