#include "parameters.h"
#include "ParamWidgets/sliderparam.h"
#include "argsfile.h"
#include "rman.h"
#include <QLabel>
#include <QVBoxLayout>

void albedoChanged(float value) {
    rmanSetAlbedo(value);
}

Parameters::Parameters(QWidget* parent) : QFrame(parent) {
    setMinimumWidth(500);

    setStyleSheet(R"(
        QFrame {border-color: rgb(0,0,0); border-style: solid; border-width: 1px; border-radius:4px;}
        QLabel {color: rgb(150, 150, 150); font:13px 'Inter'; border-width: 0px;}
        QLineEdit {color: rgb(220, 220, 220); font:13px 'Inter';
                    background-color: rgb(30,30,30);
                    border-style: solid; border-color: rgb(45,45,45); border-width:1px;
                    border-radius:4px;
                    padding: 2px;}
        )");

    QVBoxLayout* vbox = new QVBoxLayout;

//    SliderParam* slider = new SliderParam("param1", this);
//    connect(slider, &SliderParam::paramChanged, albedoChanged);
//    vbox->addWidget(slider);
//
//    SliderParam* slider2 = new SliderParam("longish param", this);
//    connect(slider2, &SliderParam::paramChanged, albedoChanged);
//    vbox->addWidget(slider2);

    ArgsParam* argsParams;
    int numParams = parseArgs(argsParams);

    for(int i=0; i<numParams; ++i) {
        QLabel *label = new QLabel(argsParams[i].name, this);
        vbox->addWidget(label);
    }

    vbox->addStretch();
    setLayout(vbox);
}
