#include "parameters.h"
#include "ParamWidgets/sliderparam.h"
#include "rman.h"
#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>
#include <stdio.h>
#include <stdlib.h>

//struct ArgsParam {
//    char name[64];
//    char label[64];
//};

void albedoChanged(float value) {
    rmanSetAlbedo(value);
}

/*
int getNumParams(const char* argsBuf, const int argsBufSize) {
    const char* pBuf = argsBuf;
    int paramCount = 0;
    while (true) {
        const char* pStartOfName = strstr(pBuf, "<param name=");
        if(pStartOfName == NULL)
            break;

        paramCount++;
        pBuf = pStartOfName + sizeof("<param name=");
    }
    return paramCount;
}


int parseArgs(ArgsParam*& argsParams) {
    char pathFromTree[] = R"(\lib\plugins\Args\LamaDiffuse.args)";
    char* rmanTree = getenv("RMANTREE");
    assert(sizeof(pathFromTree) + strlen(rmanTree) < 255 && "Args filepath too long for buffer.");
    char argsFilePath[256];
    strcpy(argsFilePath, rmanTree);
    strcat(argsFilePath, pathFromTree);

    struct stat st{};
    size_t argsFileSize{0};
    if (stat(argsFilePath, &st) == 0)
        argsFileSize = st.st_size;

    char* argsBuf = (char*)malloc(argsFileSize);

    FILE* f = fopen(argsFilePath, "r");
    assert(f && "Couldn't open args file.");
    fread(argsBuf, 1, argsFileSize, f);
    fclose(f);

    int numParams = getNumParams(argsBuf, argsFileSize);

    argsParams = (ArgsParam*)calloc(numParams, sizeof(ArgsParam));

    const char* pBuf = argsBuf;
    for(int i=0; i<numParams; i++) {
        const char* pStartOfName = strstr(pBuf, "<param name=");
        pStartOfName += sizeof("<param name=");
        const char* pEndOfName = strchr(pStartOfName+1, '"');
        strncpy(argsParams[i].name, pStartOfName, pEndOfName-pStartOfName);

        pBuf = pEndOfName;
    }

    return numParams;
}
 */

Parameters::Parameters(QWidget* parent) : QFrame(parent) {
    setMinimumWidth(500);

    setStyleSheet(R"(
        QFrame {border-color: rgb(0,0,0); border-style: solid; border-width: 1px; border-radius:4px;}
        QLabel {color: rgb(150, 150, 150); font-size:12px; border-width: 0px;}
        QLineEdit {color: rgb(220, 220, 220); font-size:12px;
                    background-color: rgb(30,30,30);
                    border-style: solid; border-color: rgb(45,45,45); border-width:1px;
                    border-radius:4px;}
        )");

    QVBoxLayout* vbox = new QVBoxLayout;

    SliderParam* slider = new SliderParam("param1", this);
    connect(slider, &SliderParam::paramChanged, albedoChanged);
    vbox->addWidget(slider);

    SliderParam* slider2 = new SliderParam("longish param", this);
    connect(slider2, &SliderParam::paramChanged, albedoChanged);
    vbox->addWidget(slider2);

//    ArgsParam* argsParams;
//    int numParams = parseArgs(argsParams);
//
//    for(int i=0; i<numParams; ++i) {
//        QLabel *label = new QLabel(argsParams[i].name, this);
//        vbox->addWidget(label);
//    }

    vbox->addStretch();
    setLayout(vbox);
}
