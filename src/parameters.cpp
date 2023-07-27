#include "parameters.h"
#include "ParamWidgets/boolparam.h"
#include "ParamWidgets/colorparam.h"
#include "ParamWidgets/sliderparam.h"
#include "ParamWidgets/stringparam.h"
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
        QLabel {color: rgb(150, 150, 150); font:15px 'Inter'; border-width: 0px;}
        QLineEdit {color: rgb(220, 220, 220); font:15px 'Inter';
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

    ArgsInfo* argsInfo;
    int numParams = parseArgsFile(argsInfo);
    for (int i=0; i<numParams; ++i) {
        switch (argsInfo[i].type) {
            case type_float: {
                SliderParam* slider = new SliderParam(argsInfo[i].name, this);
                slider->setDefault(argsInfo[i].defaultFloat);
                vbox->addWidget(slider);
                break;
            }

            case type_normal:
            case type_color: {
                ColorParam* colorParam = new ColorParam(argsInfo[i].name, this);
                vbox->addWidget(colorParam);
                break;
            }
            case type_string: {
                StringParam* stringParam = new StringParam(argsInfo[i].name, this);
                vbox->addWidget(stringParam);
                break;
            }
            
            case type_int: {
                BoolParam* boolParam= new BoolParam(argsInfo[i].name, this);
                vbox->addWidget(boolParam);
                break;
            }
            default: {
                QLabel *label = new QLabel(argsInfo[i].name, this);
                vbox->addWidget(label);
            }
            
        }


        
        // if(argsInfo[i].type == arg_type::type_float) {
        //     SliderParam* slider = new SliderParam(argsInfo[i].name, this);
        //     vbox->addWidget(slider);
        // }
        // else if (argsInfo[i].type == arg_type::type_float) {
        //     QLabel *label = new QLabel(argsInfo[i].name, this);
        //     vbox->addWidget(label);
        // else {
        //     QLabel *label = new QLabel(argsInfo[i].name, this);
        //     vbox->addWidget(label);
        // }
    }

//    for(int i=0; i<numParams; ++i) {
//        QLabel *label = new QLabel(argsParams[i].name, this);
//        vbox->addWidget(label);
//    }

    vbox->addStretch();
    setLayout(vbox);
}
