#include "parameters.h"
#include "ParamWidgets/boolparam.h"
#include "ParamWidgets/colorparam.h"
#include "ParamWidgets/floatparam.h"
#include "ParamWidgets/stringparam.h"
#include "argsfile.h"
#include "rman.h"
#include "common.h"
#include <QLabel>
#include <QVBoxLayout>


Parameters::Parameters(QWidget* parent) : QFrame(parent) {
    setMinimumWidth(500);

    // Can't use #define to replace inside strings so need to do it after
    QString styleSheet(R"(
        QFrame {border-color: rgb(0,0,0); border-style: solid; border-width: 1px; border-radius:4px;}
        QLabel {color: rgb(150, 150, 150); font:15px 'Inter'; border-width: 0px;}
        QLineEdit {color: rgb(220, 220, 220); font:15px 'Inter';
                    background-color: rgb(30,30,30);
                    border-style: solid; border-color: rgb(45,45,45); border-width:1px;
                    border-radius:4px;
                    padding: 2px;}

        QCheckBox::indicator {border: 1px solid rgb(45,45,45); 
                              border-radius: 3px;
                              background-color: rgb(30,30,30);
                              width: 15; height: 15;}
        QCheckBox::indicator::checked {background-color: FILLCOLOR;}
        )");

    styleSheet.replace("FILLCOLOR", QColor(PARAMETER_FILL_COLOR).name());
    setStyleSheet(styleSheet);
    
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
        ArgsInfo* arg = argsInfo+i;
        
        switch (arg->type) {
            case type_float: {
                FloatParam* slider = new FloatParam(arg->name, this);
                slider->setDefault(arg->defaultFloat);
                connect(slider, &FloatParam::paramChanged, rmanSetFloatParam);
                vbox->addWidget(slider);
                break;
            }
            case type_normal:
            case type_color: {
                ColorParam* colorParam = new ColorParam(arg->name, this);
                colorParam->setDefault(arg->defaultColor);
                connect(colorParam, &ColorParam::paramChanged, rmanSetColorParam);
                vbox->addWidget(colorParam);
                break;
            }
            case type_string: {
                StringParam* stringParam = new StringParam(arg->name, this);
                stringParam->setDefault(arg->defaultString);
                connect(stringParam, &StringParam::paramChanged, rmanSetStringParam);
                vbox->addWidget(stringParam);
                break;
            }
            case type_int: {
                BoolParam* boolParam = new BoolParam(arg->name, this);
                boolParam->setDefault(arg->defaultInt);
                connect(boolParam, &BoolParam::paramChanged, rmanSetIntParam);
                vbox->addWidget(boolParam);
                break;
            }
            default: {
                QLabel *label = new QLabel(arg->name, this);
                vbox->addWidget(label);
            }
            
        }


    }

//    for(int i=0; i<numParams; ++i) {
//        QLabel *label = new QLabel(argsParams[i].name, this);
//        vbox->addWidget(label);
//    }

    vbox->addStretch();
    setLayout(vbox);
}
